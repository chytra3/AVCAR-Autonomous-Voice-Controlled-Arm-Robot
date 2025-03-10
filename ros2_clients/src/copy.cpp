#include <functional>
#include <future>
#include <memory>
#include <string>
#include <sstream>

#include "std_msgs/msg/string.hpp"

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "ros2_data/action/move_xyz.hpp"
#include "ros2_data/action/move_g.hpp"
#include "ros2_data/action/move_xyzw.hpp"
#include <ros2_grasping/action/attacher.hpp>

#include "tu_int/srv/cust_srv.hpp"
#include "tu_int/msg/cust_obj.hpp"
#include "tu_int/srv/execute_actions.hpp"
#include "tu_int/srv/execute_actionsb.hpp"
#include "tu_int/srv/execute_actionsc.hpp"
#include "tu_int/srv/execute_actionsd.hpp"
#include "tu_int/srv/execute_actionse.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

bool lock = false;
bool failure = false;

std::string endeffector = "end_effector_frame";

//-------------------------------------------------------------------------------------------------------------------
//-------- MoveXYZ Client -------------------------------------------------------------------------------------------
class MoveXYZActionClient : public rclcpp::Node
{
public:
  using MoveXYZ = ros2_data::action::MoveXYZ;
  using GoalHandleMoveXYZ = rclcpp_action::ClientGoalHandle<MoveXYZ>;

  explicit MoveXYZActionClient(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
  : Node("move_xyz_action_client", options)
  {
    this->client_ptr_ = rclcpp_action::create_client<MoveXYZ>(
      this->get_node_base_interface(),
      this->get_node_graph_interface(),
      this->get_node_logging_interface(),
      this->get_node_waitables_interface(),
      "/MoveXYZ");
  }

  void send_goal(float positionx, float positiony, float positionz, float speed)
  {
    RCLCPP_ERROR(this->get_logger(), "Process XYZ waiting for lock LOCK =%d ", lock);
    while (lock==true)
    {
      continue;
    }
    lock=true;
    RCLCPP_ERROR(this->get_logger(), "Process XYZ acquired lock LOCK =%d ", lock);
    auto goal_msg = MoveXYZ::Goal();
    goal_msg.positionx = positionx;
    goal_msg.positiony = positiony;
    goal_msg.positionz = positionz;
    goal_msg.speed = speed;

    RCLCPP_INFO(this->get_logger(), "MoveXYZ Sending goal");
    
    if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10))) {
      RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting");
      failure = true;
      return;
	}
    // RCLCPP_INFO(this->get_logger(), "BEFORE XYZ LOGGER GETS CALLED");
    auto send_goal_options = rclcpp_action::Client<MoveXYZ>::SendGoalOptions();
    RCLCPP_INFO(this->get_logger(), "AFTER XYZ SEND GOAL OPTIONS");
    send_goal_options.goal_response_callback =
      std::bind(&MoveXYZActionClient::goal_response_callback, this, std::placeholders::_1);
    RCLCPP_INFO(this->get_logger(), "AFTER XYZ GOAL RESPONSE CALLBACK");
    send_goal_options.feedback_callback =
      std::bind(&MoveXYZActionClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
    RCLCPP_INFO(this->get_logger(), "AFTER XYZ FEEDBACK CALLBACK");
    send_goal_options.result_callback =
      std::bind(&MoveXYZActionClient::result_callback, this, std::placeholders::_1);
    RCLCPP_INFO(this->get_logger(), "AFTER XYZ RESULT CALLBACK");
    this->client_ptr_->async_send_goal(goal_msg, send_goal_options);
    
    while (lock == true) {
    		rclcpp::spin_some(this->get_node_base_interface());
	}
    
     
  	}

	private:
  	rclcpp_action::Client<MoveXYZ>::SharedPtr client_ptr_;

  	void goal_response_callback(const GoalHandleMoveXYZ::SharedPtr & goal_handle)
  	{
        RCLCPP_INFO(this->get_logger(), "IN START OF GOAL RESPONSE CALLBACK FUNCTION");
    		if (!goal_handle) 
    		{
      		RCLCPP_ERROR(this->get_logger(), "MoveXYZ Goal was rejected by server");
      		failure = true;
    		} 
    		else 
    		{
      		RCLCPP_INFO(this->get_logger(), "MoveXYZ Goal accepted by server, waiting for result");
    		}
        RCLCPP_INFO(this->get_logger(), "IN END OF GOAL RESPONSE CALLBACK FUNCTION");
  	}

  	void feedback_callback(GoalHandleMoveXYZ::SharedPtr, const std::shared_ptr<const MoveXYZ::Feedback> feedback)
  	{
        RCLCPP_INFO(this->get_logger(), "IN START OF FEEDBACK RESPONSE CALLBACK FUNCTION");
    		RCLCPP_INFO(this->get_logger(), "Feedback received: %s", feedback->feedback.c_str());
        RCLCPP_INFO(this->get_logger(), "IN END OF FEEDBACK RESPONSE CALLBACK FUNCTION");
  	}

  	void result_callback(const GoalHandleMoveXYZ::WrappedResult & result)
  	{
      RCLCPP_INFO(this->get_logger(), "IN START OF RESULT CALLBACK FUNCTION");
    		switch (result.code) {
      		case rclcpp_action::ResultCode::SUCCEEDED:
        			RCLCPP_INFO(this->get_logger(), "MoveXYZ Goal succeeded: %s", result.result->result.c_str());
        			lock = false;
        			break;
      		case rclcpp_action::ResultCode::ABORTED:
        			RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
        			failure = true;
        			return;
      		case rclcpp_action::ResultCode::CANCELED:
        			RCLCPP_ERROR(this->get_logger(), "Goal was canceled");
        			failure = true;
        			return;
      		default:
        			RCLCPP_ERROR(this->get_logger(), "Unknown result code");
        		return;
    		}
        
	}
};

//-------------------------------------------------------------------------------------------------------------------
//-------- MoveG Client --------------------------------------------------------------------------------------------
class MoveGActionClient : public rclcpp::Node
{
public:
  using MoveG = ros2_data::action::MoveG;
  using GoalHandleMoveG = rclcpp_action::ClientGoalHandle<MoveG>;

  explicit MoveGActionClient(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
  : Node("move_g_action_client", options)
  {
    this->client_ptr_ = rclcpp_action::create_client<MoveG>(
      this->get_node_base_interface(),
      this->get_node_graph_interface(),
      this->get_node_logging_interface(),
      this->get_node_waitables_interface(),
      "/MoveG");
  }
  

  void send_goal(float goal)
  {
    RCLCPP_ERROR(this->get_logger(), "Process MoveG waiting for lock LOCK =%d", lock);
    while (lock==true)
    {
      continue;
    }
    lock=true;
    RCLCPP_ERROR(this->get_logger(), "Process MoveG acquired lock LOCK =%d", lock);

    auto goal_msg = MoveG::Goal();
    goal_msg.goal = goal;

    RCLCPP_INFO(this->get_logger(), "Sending goal");
    
    if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10))) {
      RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting");
      return;
    }
    auto send_goal_options = rclcpp_action::Client<MoveG>::SendGoalOptions();
    send_goal_options.goal_response_callback =
      std::bind(&MoveGActionClient::goal_response_callback, this, std::placeholders::_1);
    RCLCPP_INFO(this->get_logger(), "AFTER MoveG GOAL RESPONSE CALLBACK");
    send_goal_options.feedback_callback =
      std::bind(&MoveGActionClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
    RCLCPP_INFO(this->get_logger(), "AFTER MoveG FEEDBACK CALLBACK");
    send_goal_options.result_callback =
      std::bind(&MoveGActionClient::result_callback, this, std::placeholders::_1);
    RCLCPP_INFO(this->get_logger(), "AFTER MoveG RESULT CALLBACK");
    this->client_ptr_->async_send_goal(goal_msg, send_goal_options);
    while (lock == true) {
    		rclcpp::spin_some(this->get_node_base_interface());
	}
    
  }

private:
  rclcpp_action::Client<MoveG>::SharedPtr client_ptr_;

  void goal_response_callback(const GoalHandleMoveG::SharedPtr & goal_handle)
  {
    if (!goal_handle) 
    {
      RCLCPP_ERROR(this->get_logger(), "Gripper Goal was rejected by server");
    } 
    else 
    {
      RCLCPP_INFO(this->get_logger(), "Gripper Goal accepted by server, waiting for result");
    }
  }

  void feedback_callback(GoalHandleMoveG::SharedPtr, const std::shared_ptr<const MoveG::Feedback> feedback)
  {
    RCLCPP_INFO(this->get_logger(), "Feedback received: %s", feedback->feedback.c_str());
  }

  void result_callback(const GoalHandleMoveG::WrappedResult & result)
  {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(this->get_logger(), "Gripper Goal succeeded: %s", result.result->result.c_str());
        lock = false;
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(this->get_logger(), "Goal was aborted");
        return;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_ERROR(this->get_logger(), "Goal was canceled");
        return;
      default:
        RCLCPP_ERROR(this->get_logger(), "Unknown result code");
        return;
    }
  }
};


//-------------------------------------------------------------------------------------------------------------------
//-------- Grasp Client ---------------------------------------------------------------------------------------------
class AttacherActionClient : public rclcpp::Node
{
public:
  using Attacher = ros2_grasping::action::Attacher;
  using GoalHandleAttacher = rclcpp_action::ClientGoalHandle<Attacher>;

  explicit AttacherActionClient(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
  : Node("attacher_action_client", options)
  {
    this->client_ptr_ = rclcpp_action::create_client<Attacher>(
      this->get_node_base_interface(),
      this->get_node_graph_interface(),
      this->get_node_logging_interface(),
      this->get_node_waitables_interface(),
      "Attacher");
  }

  void send_goal(const std::string& object, const std::string& endeffector)
  {
  
    while (lock==true)
    {
      continue;
    }
    lock=true;
    auto goal_msg = Attacher::Goal();
    goal_msg.object = object;
    goal_msg.endeffector = endeffector;

    RCLCPP_INFO(this->get_logger(), "Sending Attacher goal");
    
    if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10))) {
      RCLCPP_ERROR(this->get_logger(), "Attacher action server not available after waiting");
      return;
    }

    auto send_goal_options = rclcpp_action::Client<Attacher>::SendGoalOptions();
    send_goal_options.goal_response_callback =
      std::bind(&AttacherActionClient::goal_response_callback, this, std::placeholders::_1);
    send_goal_options.feedback_callback =
      std::bind(&AttacherActionClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
    send_goal_options.result_callback =
      std::bind(&AttacherActionClient::result_callback, this, std::placeholders::_1);

    auto future_result = this->client_ptr_->async_send_goal(goal_msg, send_goal_options);
    if(rclcpp::spin_until_future_complete(this->get_node_base_interface(), future_result)==rclcpp::FutureReturnCode::SUCCESS){
        lock = false;
        RCLCPP_INFO(this->get_logger(), "ATTACH DONE");
     }
  }

private:
  rclcpp_action::Client<Attacher>::SharedPtr client_ptr_;

  void goal_response_callback(const GoalHandleAttacher::SharedPtr & goal_handle)
  {
    if (!goal_handle) {
      RCLCPP_ERROR(this->get_logger(), "Attacher goal was rejected by server");
    } else {
      RCLCPP_INFO(this->get_logger(), "Attacher goal accepted by server");
    }
  }

  void feedback_callback(GoalHandleAttacher::SharedPtr,
                         const std::shared_ptr<const Attacher::Feedback> feedback)
  {
    RCLCPP_INFO(this->get_logger(), "Feedback received: %s", feedback->feedback.c_str());
  }

  void result_callback(const GoalHandleAttacher::WrappedResult & result)
  {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(this->get_logger(), "Attacher goal succeeded: %s", result.result->result.c_str());
        lock = false;
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(this->get_logger(), "Attacher goal was aborted");
        break;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_ERROR(this->get_logger(), "Attacher goal was canceled");
        break;
      default:
        RCLCPP_ERROR(this->get_logger(), "Unknown Attacher result code");
    }
  }
};

//-------------------------------------------------------------------------------------------------------------------
//-------- Grasp Detacher ---------------------------------------------------------------------------------------

class MyPublisher : public rclcpp::Node {
public:
    MyPublisher() : Node("my_publisher"), count_(0) {
        publisher_ = this->create_publisher<std_msgs::msg::String>("/ros2_Detach", 10);
        timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&MyPublisher::timer_callback, this));
    }
    void timer_callback() {
        auto message = std_msgs::msg::String();
        message.data = "True";
        publisher_->publish(message);
        ++count_;
        RCLCPP_INFO(this->get_logger(), "Publishing message #%d", count_);
        if (count_ == 5) {
            timer_->cancel();
        }
    }

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    int count_;
};


//-------------------------------------------------------------------------------------------------------------------
//-------- Gazebo Controller ---------------------------------------------------------------------------------------
class GoalSender : public rclcpp::Node
{
	public:
  		GoalSender() : Node("goal_sender")
  		{
    			moveXYZ_client_ = std::make_shared<MoveXYZActionClient>();
    			moveG_client_ = std::make_shared<MoveGActionClient>();
    			attach_client_ = std::make_shared<AttacherActionClient>();
    			
    			
    			send_goals();
    			
    			
    			service = this->create_service<tu_int::srv::ExecuteActions>("execute_actions",
                        std::bind(&GoalSender::moveRobot, this, _1, _2));

        		service2 = this->create_service<tu_int::srv::ExecuteActionsb>("execute_actionsb",
                        std::bind(&GoalSender::pickRobot, this, _1, _2));
        
        		service3 = this->create_service<tu_int::srv::ExecuteActionsc>("execute_actionsc",
                        std::bind(&GoalSender::placeRobot, this, _1, _2));
    				
  		}

	private:
  		std::shared_ptr<MoveXYZActionClient> moveXYZ_client_;
  		std::shared_ptr<MoveGActionClient> moveG_client_;
  		std::shared_ptr<AttacherActionClient> attach_client_;
  		
  		rclcpp::Service<tu_int::srv::ExecuteActions>::SharedPtr service;
    		rclcpp::Service<tu_int::srv::ExecuteActionsb>::SharedPtr service2;
    		rclcpp::Service<tu_int::srv::ExecuteActionsc>::SharedPtr service3;
    		
    		const rclcpp::Logger LOGGER = rclcpp::get_logger("move_group_demo_logger");

  		void send_goals()
  		{
  		moveG_client_->send_goal(0.04);
     		//moveXYZ_client_->send_goal(0.69, -0.26, 1.5, 0.5);
     		//attach_client_->send_goal("box" , endeffector);
     		//moveG_client_->send_goal(0.03);
     		//moveXYZ_client_->send_goal(0.601718, 0.037076, 1.5, 0.5);
  		}
  	public:
//-------------------------------------------------------------------------------------------------------------------
//-------- Move Server ---------------------------------------------------------------------------------------------
		void moveRobot(const std::shared_ptr<tu_int::srv::ExecuteActions::Request> request,
                        std::shared_ptr<tu_int::srv::ExecuteActions::Response> response)
          {
          	RCLCPP_INFO(LOGGER, "[GAZEBO ACTOR SAYS:]: MOVE ACTION HAS STARTED\n");
          	moveXYZ_client_->send_goal(request->posit.x, request->posit.y, 2.0, 1.0);
          	if(failure == false)
          	{
          		response->success = true;
          	}
          	else
          	{
          		response->success = false;
          	}
   			return;
 		}


//-------------------------------------------------------------------------------------------------------------------
//-------- Pick Server ---------------------------------------------------------------------------------------------
		void pickRobot(const std::shared_ptr<tu_int::srv::ExecuteActionsb::Request> request,
                        std::shared_ptr<tu_int::srv::ExecuteActionsb::Response> response)
          {
    			RCLCPP_INFO(LOGGER, "[GAZEBO ACTOR SAYS:]: PICK ACTION HAS STARTED\n");
			
  			if(failure == false)
          	{
          		response->success = true;
          	}
          	else
          	{
          		response->success = false;
          	}
    			return;
		}

//-------------------------------------------------------------------------------------------------------------------
//-------- Place Server --------------------------------------------------------------------------------------------
		void placeRobot(const std::shared_ptr<tu_int::srv::ExecuteActionsc::Request> request,
                        std::shared_ptr<tu_int::srv::ExecuteActionsc::Response> response)
          {
    			RCLCPP_INFO(LOGGER, "[GAZEBO ACTOR SAYS:]: PLACE ACTION HAS STARTED\n");
            
           	if(failure == false)
          	{
          		response->success = true;
          	}
          	else
          	{
          		response->success = false;
          	}
    			return;
  		}


//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
};
//-------------------------------------------------------------------------------------------------------------------
//-------- Main Function -------------------------------------------------------------------------------------------
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<GoalSender>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}


