#include <plansys2_pddl_parser/Utils.h>

#include <memory>

#include <vector>
#include <set>
#include <string>

#include "plansys2_msgs/msg/action_execution_info.hpp"
#include "plansys2_msgs/msg/plan.hpp"

#include "plansys2_domain_expert/DomainExpertClient.hpp"
#include "plansys2_executor/ExecutorClient.hpp"
#include "plansys2_planner/PlannerClient.hpp"
#include "plansys2_problem_expert/ProblemExpertClient.hpp"

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

#include "tu_int/srv/cust_srv.hpp"
#include "tu_int/msg/cust_obj.hpp"
#include "tu_int/msg/pos_obj.hpp"
#include "tu_int/srv/set_goal.hpp"
#include "tu_int/srv/posi_obj.hpp"

class CustomObject {
public:
    std::string pos;
    double positionx;
    double positiony;
    double positionz;

    CustomObject(const std::string& pos, double positionx, double positiony, double positionz)
        : pos(pos), positionx(positionx), positiony(positiony), positionz(positionz) {}
};

using std::placeholders::_1;
using std::placeholders::_2;
std::vector<CustomObject> customObjects;
std::vector<std::string> nfpos;

class Assemble : public rclcpp::Node
{
public:
  Assemble()
  : rclcpp::Node("assembling_controller")
  {
  	std::cout << "***********************INSIDE ASSEMBLING CONTROLLER****************************************** " << std::endl;
  	std::set<std::string> all_positions_;
    service = this->create_service<tu_int::srv::PosiObj>("posi_obj",
                        std::bind(&Assemble::posObj, this, _1, _2));

  }
  
bool init()
{
  std::cout << "***********************START OF BOOL INIT -BEFORE INITIALIZATION******************************** " << std::endl;
  domain_expert_ = std::make_shared<plansys2::DomainExpertClient>();
  planner_client_ = std::make_shared<plansys2::PlannerClient>();
  problem_expert_ = std::make_shared<plansys2::ProblemExpertClient>();
  executor_client_ = std::make_shared<plansys2::ExecutorClient>();
  std::cout << "*********************** AFTER INITIALIZATION OF D,P,PL-C,EX-C *********************** " << std::endl;
  std::cout << "******************************CALLING INIT KNOWLEDGE********************************** " << std::endl;
  init_knowledge();
  std::cout << "****************************************************INIT KNOWLEDGE OVER*************************** "<< std::endl;
  std::cout << "**************************************8***NOW PLAN LOADING ******************************************************** " << std::endl;

  auto domain = domain_expert_->getDomain();
  auto problem = problem_expert_->getProblem();

  auto plan = planner_client_->getPlan(domain, problem);

  std::cout << "**********************************PLAN LOADING OVER INTO (plan) *********************************** " << std::endl;
  
  if (!plan.has_value()) {
    std::cout << "*******************************ERROR IN GETTING PLAN************************************ " << std::endl;
    std::cout << "Could not find plan to reach goal " <<
      parser::pddl::toString(problem_expert_->getGoal()) << std::endl;
    return false;
  }

  if (!executor_client_->start_plan_execution(plan.value())) {
    std::cout << "**************************************ERROR IN EXECUTING PLAN********************************** " << std::endl;
    RCLCPP_ERROR(get_logger(), "Error starting a new plan (first)");
  }

  return true;
}

  

  int init_knowledge()
  {  
    
    
    auto node = std::make_shared<Assemble>();
    auto client = node->create_client<tu_int::srv::CustSrv>("my_custom_service");
    
    problem_expert_->addInstance(plansys2::Instance{"r2d2", "bot"}); //dummy1
    problem_expert_->addInstance(plansys2::Instance{"r2d23", "bot"}); //dummy2
    problem_expert_->addInstance(plansys2::Instance{"r2d24", "bot"}); //dummy3

 
    while (!client->wait_for_service(std::chrono::seconds(1))) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(node->get_logger(), "Interrupted while waiting for the service. Exiting.");
        return 1;
      }
      RCLCPP_INFO(node->get_logger(), "Service not available, waiting...");
    }
  
    //-----------------------------request------------------------------------------------
  
    auto request = std::make_shared<tu_int::srv::CustSrv::Request>();
    request->ipid = "id";
    
    auto future = client->async_send_request(request);

    //------------------------------------response---------------------------------------------------------
  
    if (rclcpp::spin_until_future_complete(node, future) == rclcpp::FutureReturnCode::SUCCESS){
      auto response = future.get();
      //-----------------------------------------------number of objects received-------------------------------------------------------------------

      RCLCPP_INFO(node->get_logger(), "Response received with %zu objects", response->objects.size());
      
      
      //------------------------------------------------for loop-------------------------------------------------------------------
      for (const auto& custom_object : response->objects) {

          RCLCPP_INFO(get_logger(),"Processing Object: opid=%s, opcategory=%s",custom_object.opid.c_str(),custom_object.opcategory.c_str());
          

          if(custom_object.opcategory!=""){
          problem_expert_->addInstance(plansys2::Instance{custom_object.opid.c_str(), custom_object.opcategory.c_str()});
          }

          //------------------------------------------------ arm -bot --------------------------------------------------------------

          if(custom_object.opcategory==""){

              problem_expert_->addInstance(plansys2::Instance{custom_object.pos.c_str(), "position"});

              customObjects.emplace_back(custom_object.pos.c_str(), custom_object.positionx, custom_object.positiony, custom_object.positionz);
      
          
              all_positions_.insert(custom_object.pos.c_str());
          }
          
          std::cout << "***********************  PREDICATES before if *********************** " << std::endl;
            

          if (custom_object.opcategory=="object"){
              std::cout << "***********************  PREDICATES after if *********************** " << std::endl;
              std::string predicate_str = "(located_at " + custom_object.opid + " " +custom_object.pos +")"; 
              //std::string predicate_str1 = "(not_free " +custom_object.pos+ ")"; 
              
              RCLCPP_INFO(node->get_logger(), "%s", predicate_str.c_str());
              //RCLCPP_INFO(node->get_logger(), "%s", predicate_str1.c_str()); 
                        
              problem_expert_->addPredicate(plansys2::Predicate(predicate_str));        
              //problem_expert_->addPredicate(plansys2::Predicate(predicate_str1));

              nfpos.push_back(custom_object.pos);      
              
          }
          
          RCLCPP_INFO(get_logger(),"---------------------------------over------------------------");
      }   
      
      
      for (auto it = all_positions_.begin(); it != all_positions_.end(); ++it)
      {
        for (auto jt = std::next(it); jt != all_positions_.end(); ++jt)
        {
          std::string p1 = "(path " + *it+" "+ *jt +")"; 
          std::string p2 = "(path " + *jt+" "+ *it +")"; 
          problem_expert_->addPredicate(plansys2::Predicate(p1));
          problem_expert_->addPredicate(plansys2::Predicate(p2));
          //reachable---------------------------------------------------------
        } 
      }  
      
    } 
    //--------------------------------------------------------else statement------------------------------------------------------------------------
    else{
      RCLCPP_ERROR(node->get_logger(), "Service call failed.");
    }
   
    std::cout << "***********************  PREDICATES *********************** " << std::endl;
    problem_expert_->addPredicate(plansys2::Predicate("(at arm pos0)"));    
    problem_expert_->addPredicate(plansys2::Predicate("(empty arm)"));
      
      for (auto it = all_positions_.begin(); it != all_positions_.end(); ++it)
      {

        std::string p3 = "(reachable arm " + *it +")"; 
        problem_expert_->addPredicate(plansys2::Predicate(p3));
      }
      
    //------------------------------------------free------------------------------------------------------------------------------------------ 
    for (const auto& customObject : customObjects) {
            // Check if customObject.pos is not in nfpos
            if (std::find(nfpos.begin(), nfpos.end(), customObject.pos) == nfpos.end()) {
                // Value not found in nfpos, print it
                std::string p5 = "(free " + customObject.pos+")"; 
                problem_expert_->addPredicate(plansys2::Predicate(p5));
                RCLCPP_INFO(node->get_logger(), "%s", p5.c_str());
                
            }
    }

    std::cout << "nfpos: ";
    for (const auto& value : nfpos) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    // Print customObjects.pos
    std::cout << "customObjects.pos: ";
    for (const auto& customObject : customObjects) {
        std::cout << customObject.pos << " ";
        std::cout << customObject.positionx << " ";
        std::cout << customObject.positiony << " ";
    }
    std::cout << std::endl;

	  std::cout << "***********************GOAL*********************** " << std::endl;
    
    
    auto set_goal_client = node->create_client<tu_int::srv::SetGoal>("set_goal");

    while (!set_goal_client->wait_for_service(std::chrono::seconds(1))) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(get_logger(), "Interrupted while waiting for the set_goal service. Exiting.");
        return 1;
      }
      RCLCPP_INFO(get_logger(), "set_goal service not available, waiting...");
    }

    auto set_goal_request = std::make_shared<tu_int::srv::SetGoal::Request>();
    set_goal_request->goal_req = "request";

    auto set_goal_future = set_goal_client->async_send_request(set_goal_request);

    if (rclcpp::spin_until_future_complete(node, set_goal_future) == rclcpp::FutureReturnCode::SUCCESS) {
      auto set_goal_response = set_goal_future.get();
      
      problem_expert_->setGoal(plansys2::Goal(set_goal_response->goal_res));
          
      RCLCPP_INFO(node->get_logger(), "Response: %s", set_goal_response->goal_res.c_str());
      
    } else {
      RCLCPP_ERROR(get_logger(), "Service call to set_goal failed.");
    }
      
      
      std::cout << "***********************INSIDE INIT KNOWLEDGE - END*********************** " << std::endl;
            
      return 0;
    }

  void step()
  { 
    if (!executor_client_->execute_and_check_plan()) {  // Plan finished
      auto result = executor_client_->getResult();

      if (result.value().success) {
        RCLCPP_INFO(get_logger(), "Plan succesfully finished");
      } else {
        RCLCPP_ERROR(get_logger(), "Plan finished with error");
      }
    }
  }

  void posObj(const std::shared_ptr<tu_int::srv::PosiObj::Request> request, std::shared_ptr<tu_int::srv::PosiObj::Response> response)
  {
    
        RCLCPP_INFO(get_logger(), "Positions");
        
        for (const auto& customObject : customObjects)
    {
        if (customObject.pos == request->pos)
        {
            // Access the x, y, z values directly
            response->position.x = customObject.positionx;
            response->position.y = customObject.positiony;
            response->position.z = customObject.positionz;
            return;  // No need to continue checking if we've found a matching position
        }
    }    
      
        RCLCPP_INFO(get_logger(), "Position done");
      
    return;
  }

private:
  std::shared_ptr<plansys2::DomainExpertClient> domain_expert_;
  std::shared_ptr<plansys2::PlannerClient> planner_client_;
  std::shared_ptr<plansys2::ProblemExpertClient> problem_expert_;
  std::shared_ptr<plansys2::ExecutorClient> executor_client_;
  rclcpp::Service<tu_int::srv::PosiObj>::SharedPtr service;
  std::set<std::string> all_positions_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<Assemble>();

  if (!node->init()) {
    return 0;
  }

  rclcpp::Rate rate(5);
  while (rclcpp::ok()) {
    node->step();

    rate.sleep();
    rclcpp::spin_some(node->get_node_base_interface());
  }

  rclcpp::shutdown();

  return 0;
}

/*
bool init()
  {
    std::cout << "***********************START OF BOOL INIT -BEFORE INITIALIZATION******************************** " << std::endl;
    domain_expert_ = std::make_shared<plansys2::DomainExpertClient>();
    planner_client_ = std::make_shared<plansys2::PlannerClient>();
    problem_expert_ = std::make_shared<plansys2::ProblemExpertClient>();
    executor_client_ = std::make_shared<plansys2::ExecutorClient>();
	  std::cout << "*********************** AFTER INITIALIZATION OF D,P,PL-C,EX-C *********************** " << std::endl;
	  std::cout << "******************************CALLING INIT KNOWLEDGE********************************** " << std::endl;
    init_knowledge();
    std::cout << "****************************************************INIT KNOWLEDGE OVER*************************** "<< std::endl;
    std::cout << "**************************************8***NOW PLAN LOADING ******************************************************** " << std::endl;

    auto domain = domain_expert_->getDomain();
    auto problem = problem_expert_->getProblem();
    auto plan = planner_client_->getPlan(domain, problem);
    
    std::cout << "**********************************PLAN LOADING OVER INTO (plan) *********************************** " << std::endl;
    
    if (!plan.has_value()) {
    std::cout << "*******************************ERROR IN GETTING PLAN************************************ " << std::endl;
      std::cout << "Could not find plan to reach goal " <<
        parser::pddl::toString(problem_expert_->getGoal()) << std::endl;
      return false;
    }

    if (!executor_client_->start_plan_execution(plan.value())) {
    std::cout << "**************************************ERROR IN EXECUTING PLAN********************************** " << std::endl;
      RCLCPP_ERROR(get_logger(), "Error starting a new plan (first)");
    }

    return true;
  } */
