
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

#include "linkattacher_msgs/srv/attach_link.hpp" // INCLUDE ROS2 SERVICE.
#include "linkattacher_msgs/srv/detach_link.hpp"


using std::placeholders::_1;
using std::placeholders::_2;

bool lock = false;
bool failure = false;

std::string pandalink = "end_effector_frame";
std::string pandamodel = "panda";

//-------------------------------------------------------------------------------------------------------------------
//-------- MoveXYZ Client -------------------------------------------------------------------------------------------
class MoveXYZActionClient : public rclcpp::Node
{
public:
  using MoveXYZ = ros2_data::action::MoveXYZ;
  using GoalHandleMoveXYZ = rclcpp_action::ClientGoalHandle<MoveXYZ>;

  explicit MoveXYZActionClient(const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
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
    while (lock == true)
    {
      continue;
    }
    lock = true;
    RCLCPP_ERROR(this->get_logger(), "Process XYZ acquired lock LOCK =%d ", lock);
    auto goal_msg = MoveXYZ::Goal();
    goal_msg.positionx = positionx;
    goal_msg.positiony = positiony;
    goal_msg.positionz = positionz;
    goal_msg.speed = speed;

    RCLCPP_INFO(this->get_logger(), "MoveXYZ Sending goal");

    if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10)))
    {
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

    while (lock == true)
    {
      rclcpp::spin_some(this->get_node_base_interface());
    }
  }

private:
  rclcpp_action::Client<MoveXYZ>::SharedPtr client_ptr_;

  void goal_response_callback(const GoalHandleMoveXYZ::SharedPtr &goal_handle)
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

  void result_callback(const GoalHandleMoveXYZ::WrappedResult &result)
  {
    RCLCPP_INFO(this->get_logger(), "IN START OF RESULT CALLBACK FUNCTION");
    switch (result.code)
    {
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
//-------- MoveXYZW Client ------------------------------------------------------------------------------------------
class MoveXYZWActionClient : public rclcpp::Node
{
public:
  using MoveXYZW = ros2_data::action::MoveXYZW;
  using GoalHandleMoveXYZW = rclcpp_action::ClientGoalHandle<MoveXYZW>;

  explicit MoveXYZWActionClient(const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
      : Node("move_xyzw_action_client", options)
  {
    this->client_ptr_ = rclcpp_action::create_client<MoveXYZW>(
        this->get_node_base_interface(),
        this->get_node_graph_interface(),
        this->get_node_logging_interface(),
        this->get_node_waitables_interface(),
        "/MoveXYZW");
  }

  void send_goal(float positionx, float positiony, float positionz, float yaw, float pitch, float roll, float speed)
  {
    RCLCPP_ERROR(this->get_logger(), "Process XYZW waiting for lock LOCK =%d ", lock);

    while (lock == true)
    {
      continue;
    }
    lock = true;
    RCLCPP_ERROR(this->get_logger(), "Process XYZW acquired lock LOCK =%d", lock);

    auto goal_msg = MoveXYZW::Goal();
    goal_msg.positionx = positionx;
    goal_msg.positiony = positiony;
    goal_msg.positionz = positionz;
    goal_msg.yaw = yaw;
    goal_msg.pitch = pitch;
    goal_msg.roll = roll;
    goal_msg.speed = speed;

    RCLCPP_INFO(this->get_logger(), "Sending goal");

    if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10)))
    {
      RCLCPP_ERROR(this->get_logger(), "Action server not available after waiting");
      return;
    }
    auto send_goal_options = rclcpp_action::Client<MoveXYZW>::SendGoalOptions();
    send_goal_options.goal_response_callback =
        std::bind(&MoveXYZWActionClient::goal_response_callback, this, std::placeholders::_1);
    RCLCPP_INFO(this->get_logger(), "AFTER XYZW GOAL RESPONSE CALLBACK");
    send_goal_options.feedback_callback =
        std::bind(&MoveXYZWActionClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
    RCLCPP_INFO(this->get_logger(), "AFTER XYZW FEEDBACK CALLBACK");
    send_goal_options.result_callback =
        std::bind(&MoveXYZWActionClient::result_callback, this, std::placeholders::_1);
    RCLCPP_INFO(this->get_logger(), "AFTER XYZW RESULT CALLBACK");
    this->client_ptr_->async_send_goal(goal_msg, send_goal_options);
    while (lock == true)
    {
      rclcpp::spin_some(this->get_node_base_interface());
    }
  }

private:
  rclcpp_action::Client<MoveXYZW>::SharedPtr client_ptr_;

  void goal_response_callback(const GoalHandleMoveXYZW::SharedPtr &goal_handle)
  {
    if (!goal_handle)
    {
      RCLCPP_ERROR(this->get_logger(), "MoveXYZW Goal was rejected by server");
    }
    else
    {
      RCLCPP_INFO(this->get_logger(), "MoveXYZW Goal accepted by server, waiting for result");
    }
  }

  void feedback_callback(GoalHandleMoveXYZW::SharedPtr, const std::shared_ptr<const MoveXYZW::Feedback> feedback)
  {
    RCLCPP_INFO(this->get_logger(), "Feedback received: %s", feedback->feedback.c_str());
  }

  void result_callback(const GoalHandleMoveXYZW::WrappedResult &result)
  {
    switch (result.code)
    {
    case rclcpp_action::ResultCode::SUCCEEDED:
      RCLCPP_INFO(this->get_logger(), "MoveXYZW Goal succeeded: %s", result.result->result.c_str());
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
//-------- MoveG Client --------------------------------------------------------------------------------------------
class MoveGActionClient : public rclcpp::Node
{
public:
  using MoveG = ros2_data::action::MoveG;
  using GoalHandleMoveG = rclcpp_action::ClientGoalHandle<MoveG>;

  explicit MoveGActionClient(const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
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
    while (lock == true)
    {
      continue;
    }
    lock = true;
    RCLCPP_ERROR(this->get_logger(), "Process MoveG acquired lock LOCK =%d", lock);

    auto goal_msg = MoveG::Goal();
    goal_msg.goal = goal;

    RCLCPP_INFO(this->get_logger(), "Sending goal");

    if (!this->client_ptr_->wait_for_action_server(std::chrono::seconds(10)))
    {
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
    while (lock == true)
    {
      rclcpp::spin_some(this->get_node_base_interface());
    }
  }

private:
  rclcpp_action::Client<MoveG>::SharedPtr client_ptr_;

  void goal_response_callback(const GoalHandleMoveG::SharedPtr &goal_handle)
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

  void result_callback(const GoalHandleMoveG::WrappedResult &result)
  {
    switch (result.code)
    {
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
//-------- Linker Client ----------------------------------------------------------------------------------------
// class LinkAttacherController : public rclcpp::Node
// {
// public:
//   LinkAttacherController() : Node("link_attacher_controller")
//   {
//   }

// private:
// };
//-------------------------------------------------------------------------------------------------------------------
//-------- Gazebo Controller ---------------------------------------------------------------------------------------

class GoalSender : public rclcpp::Node
{
public:
  GoalSender() : Node("goal_sender")
  {
    moveXYZ_client_ = std::make_shared<MoveXYZActionClient>();
    moveXYZW_client_ = std::make_shared<MoveXYZWActionClient>();
    moveG_client_ = std::make_shared<MoveGActionClient>();

    // attachClient_ = this->create_client<linkattacher_msgs::srv::AttachLink>("attach_link");
    // detachClient_ = this->create_client<linkattacher_msgs::srv::DetachLink>("detach_link");
    attachClient_ = this->create_client<linkattacher_msgs::srv::AttachLink>("ATTACHLINK");
    detachClient_ = this->create_client<linkattacher_msgs::srv::DetachLink>("DETACHLINK");

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
  std::shared_ptr<MoveXYZWActionClient> moveXYZW_client_;
  std::shared_ptr<MoveGActionClient> moveG_client_;

  rclcpp::Client<linkattacher_msgs::srv::AttachLink>::SharedPtr attachClient_;
  rclcpp::Client<linkattacher_msgs::srv::DetachLink>::SharedPtr detachClient_;

  rclcpp::Service<tu_int::srv::ExecuteActions>::SharedPtr service;
  rclcpp::Service<tu_int::srv::ExecuteActionsb>::SharedPtr service2;
  rclcpp::Service<tu_int::srv::ExecuteActionsc>::SharedPtr service3;

  const rclcpp::Logger LOGGER = rclcpp::get_logger("move_group_demo_logger");

  void send_goals()
  {
    //moveG_client_->send_goal(0.04);
    //moveXYZW_client_->send_goal(0.3, 0, 1.2, 0, 0, 0, 1.0);
    //attachLink("coke");
    RCLCPP_INFO(LOGGER, "INSIDE SEND GOALS\n");
    //moveG_client_->send_goal(0.02);
    //moveXYZW_client_->send_goal(0.2, 0, 1.5, 0, 0, 0, 1.0);
    //detachLink("coke");
  }

public:
  //-------------------------------------------------------------------------------------------------------------------
  //-------- Move Server ---------------------------------------------------------------------------------------------
  void moveRobot(const std::shared_ptr<tu_int::srv::ExecuteActions::Request> request,
                 std::shared_ptr<tu_int::srv::ExecuteActions::Response> response)
  {
    RCLCPP_INFO(LOGGER, "[GAZEBO ACTOR SAYS:]: MOVE ACTION HAS STARTED\n");
    moveXYZ_client_->send_goal(request->posit.x, request->posit.y, 1.4, 1.0);
    if (failure == false)
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
    moveG_client_->send_goal(0.04);
    moveXYZ_client_->send_goal(request->posit.x, request->posit.y, 1.23, 1.0);
    attachLink(request->id.c_str());
    moveG_client_->send_goal(0.035);
    moveXYZ_client_->send_goal(request->posit.x, request->posit.y, 1.4, 1.0);

    if (failure == false)
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
    moveXYZ_client_->send_goal(request->posit.x, request->posit.y, 1.25, 1.0);
    detachLink(request->id.c_str());
    moveG_client_->send_goal(0.04);
    moveXYZ_client_->send_goal(request->posit.x, request->posit.y, 1.4, 1.0);
    if (failure == false)
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
  void attachLink(const std::string& link)
{
    auto request = std::make_shared<linkattacher_msgs::srv::AttachLink::Request>();
    request->model1_name = "panda";
    request->link1_name = "end_effector_frame";
    request->model2_name = link;
    request->link2_name = link;

    auto future = attachClient_->async_send_request(request);

    if (future.wait_for(std::chrono::seconds(5)) == std::future_status::timeout)
    {
        RCLCPP_ERROR(this->get_logger(), "Service call timed out");
        return;
    }

    auto response = future.get();
    if (response->success)
    {
        RCLCPP_INFO(this->get_logger(), "Attach request succeeded");
    }
    else
    {
        RCLCPP_ERROR(this->get_logger(), "Attach request failed");
    }
}

  //-------------------------------------------------------------------------------------------------------------------
  void detachLink(const std::string &link)
  {
    auto request = std::make_shared<linkattacher_msgs::srv::DetachLink::Request>();
    request->model1_name = "panda";
    request->link1_name = "end_effector_frame";
    request->model2_name = link;
    request->link2_name = link;

    auto future = detachClient_->async_send_request(request);
    
    if (future.wait_for(std::chrono::seconds(5)) == std::future_status::timeout)
    {
        RCLCPP_ERROR(this->get_logger(), "Service call timed out Detach");
        return;
    }

    auto response = future.get();
    
    if (response->success)
    {
        RCLCPP_INFO(this->get_logger(), "Detach request succeeded");
    }
    else
    {
        RCLCPP_ERROR(this->get_logger(), "Detach request failed");
    }
  }
  //-------------------------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------------------------
};
//-------------------------------------------------------------------------------------------------------------------
//-------- Main Function -------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<GoalSender>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
