// #include "rclcpp/rclcpp.hpp"
// #include "tu_int/srv/set_goal.hpp"
// #include "std_msgs/msg/string.hpp"
// using std::placeholders::_1;
// using std::placeholders::_2;

// using namespace std::placeholders;

// std::string global_string;

// class MyServer : public rclcpp::Node
// {
// public:
//   MyServer()
//     : Node("my_goal_server")
//   {
//     server_ = this->create_service<tu_int::srv::SetGoal>(
//       "set_goal",
//       std::bind(&MyServer::handle_service_request, this, _1, _2));

//     // Create a subscriber
//     subscription_ = this->create_subscription<std_msgs::msg::String>(
//       "topic", 10, std::bind(&MyServer::topic_callback, this, _1));
//   }

// private:
//   void handle_service_request(
//     const std::shared_ptr<tu_int::srv::SetGoal::Request> request,
//     std::shared_ptr<tu_int::srv::SetGoal::Response> response)
//   {
//     if (!global_string.empty()) {
//       response->goal_res = global_string;
//     }
//   }

//   // Callback function for the subscriber
//   void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
//   {
//     RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
//     global_string = msg->data.c_str();
//   }

//   rclcpp::Service<tu_int::srv::SetGoal>::SharedPtr server_;
//   rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
// };

// int main(int argc, char** argv)
// {
//   rclcpp::init(argc, argv);
//   rclcpp::spin(std::make_shared<MyServer>());
//   rclcpp::shutdown();
//   return 0;
// }

#include "rclcpp/rclcpp.hpp"
#include "tu_int/srv/set_goal.hpp"
#include "std_msgs/msg/string.hpp"
using std::placeholders::_1;
using std::placeholders::_2;

using namespace std::placeholders;

std::string global_string;

class MyServer : public rclcpp::Node
{
public:
  MyServer()
    : Node("my_goal_server")
  {
    // Create a subscriber
    subscription_ = this->create_subscription<std_msgs::msg::String>(
      "topic", 10, std::bind(&MyServer::topic_callback, this, _1));
  }

  void start_service()
  {
    server_ = this->create_service<tu_int::srv::SetGoal>(
      "set_goal",
      std::bind(&MyServer::handle_service_request, this, _1, _2));
  }

private:
  void handle_service_request(
    const std::shared_ptr<tu_int::srv::SetGoal::Request> request,
    std::shared_ptr<tu_int::srv::SetGoal::Response> response)
  {
    response->goal_res = global_string;
  }

  // Callback function for the subscriber
  void topic_callback(const std_msgs::msg::String::SharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
    global_string = msg->data.c_str();

    // Check if global_string is not empty
    if (!global_string.empty()) {
      // Start the service if global_string is not empty
      start_service();
    }
  }

  rclcpp::Service<tu_int::srv::SetGoal>::SharedPtr server_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MyServer>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
