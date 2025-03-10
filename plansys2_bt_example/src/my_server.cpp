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
    
    tu_int::msg::CustObj custom_object1;
    custom_object1.opid = "";
    custom_object1.opcategory = "";
    custom_object1.pos = "pos0";
    response->objects.push_back(custom_object1);
    
    tu_int::msg::CustObj custom_object2;
    custom_object2.opid = "";
    custom_object2.opcategory = "";
    custom_object2.pos = "pos1";
    custom_object2.positionx = 0.3;
    custom_object2.positiony = -0.43; 
    custom_object2.positionz = 1.06;
    response->objects.push_back(custom_object2);
    
    tu_int::msg::CustObj custom_object3;
    custom_object3.opid = "";
    custom_object3.opcategory = "";
    custom_object3.pos = "pos2";
    custom_object3.positionx = 0.301718;
    custom_object3.positiony = 0.037076;
    custom_object3.positionz = 1.06;
    response->objects.push_back(custom_object3);
    
    tu_int::msg::CustObj custom_object7;
    custom_object7.opid = "";
    custom_object7.opcategory = "";
    custom_object7.pos = "pos3";
    custom_object7.positionx = 0.351718;
    custom_object7.positiony = 0.596804;
    custom_object7.positionz = 1.06;
    response->objects.push_back(custom_object7);
    
    tu_int::msg::CustObj custom_object8;
    custom_object8.opid = "";
    custom_object8.opcategory = "";
    custom_object8.pos = "pos4";
    custom_object8.positionx = 0.50;
    custom_object8.positiony = -0.43;
    custom_object8.positionz = 1.06;
    response->objects.push_back(custom_object8);

    tu_int::msg::CustObj custom_object9;
    custom_object9.opid = "";
    custom_object9.opcategory = "";
    custom_object9.pos = "pos5";
    custom_object9.positionx = 0.501718;
    custom_object9.positiony = 0.037076;
    custom_object9.positionz = 1.06;
    response->objects.push_back(custom_object9);

    tu_int::msg::CustObj custom_object10;
    custom_object10.opid = "";
    custom_object10.opcategory = "";
    custom_object10.pos = "pos6";
    custom_object10.positionx = 0.501718;
    custom_object10.positiony = 0.456804;
    custom_object10.positionz = 1.06;
    response->objects.push_back(custom_object10);

    tu_int::msg::CustObj custom_object11;
    custom_object11.opid = "";
    custom_object11.opcategory = "";
    custom_object11.pos = "pos7";
    custom_object11.positionx = 0.69;
    custom_object11.positiony = -0.26;
    custom_object11.positionz = 1.06;
    response->objects.push_back(custom_object11);

    tu_int::msg::CustObj custom_object12;
    custom_object12.opid = "";
    custom_object12.opcategory = "";
    custom_object12.pos = "pos8";
    custom_object12.positionx = 0.731718;
    custom_object12.positiony = 0.00;
    custom_object12.positionz = 1.06;
    response->objects.push_back(custom_object12);

    tu_int::msg::CustObj custom_object13;
    custom_object13.opid = "";
    custom_object13.opcategory = "";
    custom_object13.pos = "pos9";
    custom_object13.positionx = 0.69;
    custom_object13.positiony = 0.256804;
    custom_object13.positionz = 1.06;
    response->objects.push_back(custom_object13); 
    
    tu_int::msg::CustObj custom_object5;
    custom_object5.opid = "beer";
    custom_object5.optype = "CYLINDER";
    custom_object5.opcategory = "object";
    custom_object5.pos = "pos1";
    custom_object5.obj = "";
    custom_object5.cl = "";
    custom_object5.positionx = 0.30;
    custom_object5.positiony = -0.43; 
    custom_object5.positionz = 1.06;
    custom_object5.dimx = 0.02;
    custom_object5.dimy = 0.02;
    custom_object5.dimz = 0.09;
    response->objects.push_back(custom_object5);
    
    
    tu_int::msg::CustObj custom_object6;
    custom_object6.opid = "coke_can";
    custom_object6.optype = "CYLINDER";
    custom_object6.opcategory = "object";
    custom_object6.pos = "pos2";
    custom_object6.obj = "";
    custom_object6.cl = "";
    custom_object6.positionx = 0.301718;
    custom_object6.positiony = 0.037076;
    custom_object6.positionz = 1.06;
    custom_object6.dimx = 0.02;
    custom_object6.dimy = 0.02;
    custom_object6.dimz = 0.09;
    response->objects.push_back(custom_object6); 
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

    // Top Left 7
    // 0.69
    // -0.26

    // Top       8
    // 0.73178
    // 0.00

    // Top Right  9
    // 0.69
    // 0.256804
    
    // Left     4
    // 0.5
    // -0.43
    
    // Center    5
    // 0.501718
    // 0.00

    // Right    6
    // 0.501718
    // 0.456804
    
    //Bottom Left 1
    // 0.30
    // -0.43

    // Bottom    2
    // 0.301718
    // 0.037076

    // Bottom Right 3
    // 0.351718
    // 0.596804
  

