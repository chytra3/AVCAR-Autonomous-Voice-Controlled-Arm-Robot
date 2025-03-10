// Copyright 2019 Intelligent Robotics Lab
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <plansys2_pddl_parser/Utils.h>

#include <memory>

#include "plansys2_msgs/msg/action_execution_info.hpp"
#include "plansys2_msgs/msg/plan.hpp"

#include "plansys2_domain_expert/DomainExpertClient.hpp"
#include "plansys2_executor/ExecutorClient.hpp"
#include "plansys2_planner/PlannerClient.hpp"
#include "plansys2_problem_expert/ProblemExpertClient.hpp"

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

class Assemble : public rclcpp::Node
{
public:
  Assemble()
  : rclcpp::Node("assembling_controller")
  {
  	std::cout << "***********************INSIDE ASSEMBLING CONTROLLER****************************************** " << std::endl;
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
    std::cout << "****************************************************INIT KNOWLEDGE OVER************************************************** " << std::endl;
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
  
 /*void comb(int n){

    for (int i =0 ; i<n ; ++i){
     for( int j =0 ; j<i ; ++j){
        string p1 = "path x"+to_string(i)+" y"+to_string(i)+" x"+to_string(j)+" y"+to_string(j) ;
        string p2 = "path x"+to_string(j)+" y"+to_string(j)+" x"+to_string(i)+" y"+to_string(i) ;
        cout<<"problem_expert_->addPredicate(plansys2::Predicate(\"("<< p1 <<")\"));"<<endl; 
        cout<<"problem_expert_->addPredicate(plansys2::Predicate(\"("<< p2 <<")\"));"<<endl;
     }
    }
} */

  void init_knowledge()
  {
	std::cout << "***********************INSIDE INIT KNOWLEDGE - START*********************** " << std::endl;
    std::cout << "*********************** INSTANCES *********************** " << std::endl;
    problem_expert_->addInstance(plansys2::Instance{"r2d2", "bot"}); //dummy1
    problem_expert_->addInstance(plansys2::Instance{"r2d23", "bot"}); //dummy2
    problem_expert_->addInstance(plansys2::Instance{"r2d24", "bot"}); //dummy3
    
    problem_expert_->addInstance(plansys2::Instance{"arm", "bot"});
    
    problem_expert_->addInstance(plansys2::Instance{"box", "object"});
    problem_expert_->addInstance(plansys2::Instance{"cylinder", "object"});
    //problem_expert_->addInstance(plansys2::Instance{"cone", "object"});
    problem_expert_->addInstance(plansys2::Instance{"sphere", "object"});
    problem_expert_->addInstance(plansys2::Instance{"box2", "object"});
    //problem_expert_->addInstance(plansys2::Instance{"box3", "object"});
    
    
    problem_expert_->addInstance(plansys2::Instance{"pos0","position"});
    problem_expert_->addInstance(plansys2::Instance{"pos1","position"});
    problem_expert_->addInstance(plansys2::Instance{"pos2","position"});
    problem_expert_->addInstance(plansys2::Instance{"pos3","position"});
    problem_expert_->addInstance(plansys2::Instance{"pos4","position"});
    problem_expert_->addInstance(plansys2::Instance{"pos5","position"});
    problem_expert_->addInstance(plansys2::Instance{"pos6","position"});
    /*problem_expert_->addInstance(plansys2::Instance{"y0","positiony"});
    problem_expert_->addInstance(plansys2::Instance{"y1","positiony"});
    problem_expert_->addInstance(plansys2::Instance{"y1","positiony"});
    problem_expert_->addInstance(plansys2::Instance{"y2","positiony"});
    problem_expert_->addInstance(plansys2::Instance{"y3","positiony"});
    problem_expert_->addInstance(plansys2::Instance{"y4","positiony"});
    problem_expert_->addInstance(plansys2::Instance{"y5","positiony"});
    problem_expert_->addInstance(plansys2::Instance{"y6","positiony"});*/

    std::cout << "***********************  PREDICATES *********************** " << std::endl;
    problem_expert_->addPredicate(plansys2::Predicate("(at arm pos0)"));
    problem_expert_->addPredicate(plansys2::Predicate("(on box pos1)"));
    problem_expert_->addPredicate(plansys2::Predicate("(on cylinder pos2)"));
    //problem_expert_->addPredicate(plansys2::Predicate("(on cone pos3)"));
    problem_expert_->addPredicate(plansys2::Predicate("(on sphere pos4)"));
    problem_expert_->addPredicate(plansys2::Predicate("(on box2 pos5)"));
    
    problem_expert_->addPredicate(plansys2::Predicate("(empty arm)"));
    problem_expert_->addPredicate(plansys2::Predicate("(clear box)"));
    problem_expert_->addPredicate(plansys2::Predicate("(clear cylinder)"));
    problem_expert_->addPredicate(plansys2::Predicate("(clear box2)"));
    problem_expert_->addPredicate(plansys2::Predicate("(clear sphere)"));
    //problem_expert_->addPredicate(plansys2::Predicate("(clear cylinder)"));
    //problem_expert_->addPredicate(plansys2::Predicate("(clear cone)"));
    
    problem_expert_->addPredicate(plansys2::Predicate("(path pos1 pos0)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos0 pos1)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos2 pos0)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos0 pos2)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos2 pos1)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos1 pos2)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos3 pos0)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos0 pos3)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos3 pos1)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos1 pos3)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos3 pos2)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos2 pos3)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos4 pos0)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos0 pos4)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos4 pos1)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos1 pos4)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos4 pos2)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos2 pos4)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos4 pos3)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos3 pos4)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos5 pos0)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos0 pos5)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos5 pos1)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos1 pos5)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos5 pos2)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos2 pos5)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos5 pos3)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos3 pos5)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos5 pos4)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos4 pos5)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos6 pos0)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos0 pos6)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos6 pos1)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos1 pos6)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos6 pos2)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos2 pos6)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos6 pos3)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos3 pos6)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos6 pos4)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos4 pos6)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos6 pos5)"));
	problem_expert_->addPredicate(plansys2::Predicate("(path pos5 pos6)"));

    problem_expert_->addPredicate(plansys2::Predicate("(not_free pos0)"));
    problem_expert_->addPredicate(plansys2::Predicate("(not_free pos1)"));
    problem_expert_->addPredicate(plansys2::Predicate("(not_free pos2)"));
    problem_expert_->addPredicate(plansys2::Predicate("(free pos3)"));
    problem_expert_->addPredicate(plansys2::Predicate("(not_free pos4)"));
    problem_expert_->addPredicate(plansys2::Predicate("(not_free pos5)"));
    problem_expert_->addPredicate(plansys2::Predicate("(free pos6)"));
    
    //problem_expert_->addPredicate(plansys2::Predicate("(on box3 box2)"));
    
    problem_expert_->addPredicate(plansys2::Predicate("(reachable arm pos1)"));
    problem_expert_->addPredicate(plansys2::Predicate("(reachable arm pos2)"));
    problem_expert_->addPredicate(plansys2::Predicate("(reachable arm pos3)"));
    problem_expert_->addPredicate(plansys2::Predicate("(reachable arm pos4)"));
    problem_expert_->addPredicate(plansys2::Predicate("(reachable arm pos5)"));
    problem_expert_->addPredicate(plansys2::Predicate("(reachable arm pos6)"));

	std::cout << "***********************GOAL*********************** " << std::endl;
	
    problem_expert_->setGoal(plansys2::Goal("(and (on box pos6) (on sphere pos3) )"));
    
    std::cout << "***********************INSIDE INIT KNOWLEDGE - END*********************** " << std::endl;
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

private:
  std::shared_ptr<plansys2::DomainExpertClient> domain_expert_;
  std::shared_ptr<plansys2::PlannerClient> planner_client_;
  std::shared_ptr<plansys2::ProblemExpertClient> problem_expert_;
  std::shared_ptr<plansys2::ExecutorClient> executor_client_;
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
