// tu_int/src/my_server.cpp
#include "rclcpp/rclcpp.hpp"
#include "tu_int/srv/cust_srv.hpp"
#include "tu_int/msg/cust_obj.hpp" 

using namespace std::placeholders;

class MyServer : public rclcpp::Node
{
public:
  MyServer()
    : Node("my_server")
  {
    server_ = this->create_service<tu_int::srv::CustSrv>(
      "my_custom_service",
      std::bind(&MyServer::handle_service_request, this, _1, _2));
  }

private:
  void handle_service_request(
    const std::shared_ptr<tu_int::srv::CustSrv::Request> request,
    std::shared_ptr<tu_int::srv::CustSrv::Response> response)
  {
      
    
    /*tu_int::msg::CustObj custom_object1;
    custom_object1.opid = "dummy1";
    custom_object1.opcategory = "object";
    custom_object1.posx = "x9";
    custom_object1.posy = "y9";
    response->objects.push_back(custom_object1);
    
    tu_int::msg::CustObj custom_object2;
    custom_object2.opid = "dummy2";
    custom_object2.opcategory = "object";
    custom_object2.posx = "x10";
    custom_object2.posy = "y10";
    response->objects.push_back(custom_object2);
    
    tu_int::msg::CustObj custom_object3;
    custom_object3.opid = "dummy3";
    custom_object3.opcategory = "object";
    custom_object3.posx = "x11";
    custom_object3.posy = "y11";
    response->objects.push_back(custom_object3);*/
    
    tu_int::msg::CustObj custom_object4;
    custom_object4.opid = "arm";
    custom_object4.opcategory = "bot";
    custom_object4.pos = "pos0";
    response->objects.push_back(custom_object4);
    
    
    tu_int::msg::CustObj custom_object5;
    custom_object5.opid = "box1";
    custom_object5.optype = "BOX";
    custom_object5.opcategory = "object";
    custom_object5.pos = "pos1";
    custom_object5.positionx = 0.0;
    custom_object5.positiony = -0.5;
    custom_object5.positionz = 0.01;
    custom_object5.dimx = 0.1;
    custom_object5.dimy = 0.05;
    custom_object5.dimz = 0.02;
    response->objects.push_back(custom_object5);
    
    tu_int::msg::CustObj custom_object6;
    custom_object6.opid = "cylinder";
    custom_object6.optype = "CYLINDER";
    custom_object6.opcategory = "object";
    custom_object6.pos = "pos2";
    custom_object6.positionx = 0.5;
    custom_object6.positiony = 0;
    custom_object6.positionz = 0;
    custom_object6.dimx = 0.1;
    custom_object6.dimy = 0.02;
    custom_object6.dimz = 0.02;
    response->objects.push_back(custom_object6);
    
    tu_int::msg::CustObj custom_object7;
    custom_object7.opid = "";
    custom_object7.opcategory = "";
    custom_object7.pos = "pos3";
    custom_object7.positionx = -0.3;
    custom_object7.positiony = 0.5;
    custom_object7.positionz = 0.00;
    response->objects.push_back(custom_object7);
    
    tu_int::msg::CustObj custom_object8;
    custom_object8.opid = "sphere";
    custom_object8.optype = "SPHERE";
    custom_object8.opcategory = "object";
    custom_object8.pos = "pos4";
    custom_object8.positionx = 0.4;
    custom_object8.positiony = -0.5;
    custom_object8.positionz = 0.00;
    custom_object8.dimx = 0.02;
    custom_object8.dimy = 0.05;
    custom_object8.dimz = 0.02;
    response->objects.push_back(custom_object8);
    
    tu_int::msg::CustObj custom_object9;
    custom_object9.opid = "box2";
    custom_object9.optype = "BOX";
    custom_object9.opcategory = "object";
    custom_object9.pos = "pos5";
    custom_object9.positionx = 0.0;
    custom_object9.positiony = -0.8;
    custom_object9.positionz = 0.01;
    custom_object9.dimx = 0.1;
    custom_object9.dimy = 0.1;
    custom_object9.dimz = 0.02;
    response->objects.push_back(custom_object9);
    
    tu_int::msg::CustObj custom_object10;
    custom_object10.opid = "";
    custom_object10.opcategory = "";
    custom_object10.pos = "pos6";
    custom_object10.positionx = -0.5;
    custom_object10.positiony = 0;
    custom_object10.positionz = 0.0;
    response->objects.push_back(custom_object10);
    
    
    
    
  }

  rclcpp::Service<tu_int::srv::CustSrv>::SharedPtr server_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MyServer>());
  rclcpp::shutdown();
  return 0;
}

