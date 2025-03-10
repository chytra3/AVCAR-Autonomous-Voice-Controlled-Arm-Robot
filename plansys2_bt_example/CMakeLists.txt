cmake_minimum_required(VERSION 3.5)
project(plansys2_bt_example)


find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(plansys2_msgs REQUIRED)
find_package(plansys2_domain_expert REQUIRED)
find_package(plansys2_executor REQUIRED)
find_package(plansys2_planner REQUIRED)
find_package(plansys2_problem_expert REQUIRED)
find_package(plansys2_pddl_parser REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(plansys2_bt_actions REQUIRED)
find_package(tu_int REQUIRED)
find_package(std_msgs REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

set(dependencies
    rclcpp
    rclcpp_action
    geometry_msgs
    tf2_geometry_msgs
    plansys2_msgs
    plansys2_domain_expert
    plansys2_executor
    plansys2_planner
    plansys2_problem_expert
    plansys2_pddl_parser
    ament_index_cpp
    tu_int
    std_msgs
)

add_executable(server src/my_server.cpp)
ament_target_dependencies(server rclcpp tu_int)

add_executable(goalserver src/my_server_goal.cpp)
ament_target_dependencies(goalserver rclcpp tu_int std_msgs)

add_executable(assemble_controller_node src/assemble_controller_node.cpp)
ament_target_dependencies(assemble_controller_node ${dependencies})

add_executable(move_action_node src/move_action_node.cpp)
ament_target_dependencies(move_action_node ${dependencies})

add_executable(pick_up_action_node src/pick_up_action_node.cpp)
ament_target_dependencies(pick_up_action_node ${dependencies})

add_executable(place_action_node src/place_action_node.cpp)
ament_target_dependencies(place_action_node ${dependencies})

add_executable(stack_action_node src/stack_action_node.cpp)
ament_target_dependencies(stack_action_node ${dependencies})

add_executable(unstack_action_node src/unstack_action_node.cpp)
ament_target_dependencies(unstack_action_node ${dependencies})


install(DIRECTORY launch pddl DESTINATION share/${PROJECT_NAME})

install(TARGETS
  goalserver
  server
  move_action_node
  stack_action_node
  unstack_action_node
  place_action_node
  pick_up_action_node
  assemble_controller_node
  ${plugin_libs}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
endif()

ament_export_dependencies(${dependencies})

ament_package()
