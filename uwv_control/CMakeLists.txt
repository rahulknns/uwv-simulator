cmake_minimum_required(VERSION 2.8.3)
project(uwv_control)

find_package(gazebo REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)
find_package(Curses REQUIRED)
find_package(catkin REQUIRED COMPONENTS 
              roscpp 
              std_msgs
              sensor_msgs 
              geometry_msgs 
              cv_bridge
              nav_msgs
              message_generation
              # darknet_ros_msgs
              )
add_message_files( FILES
                   floatMultiArray.msg
)
generate_messages()
catkin_package(CATKIN_DEPENDS roscpp 
                              std_msgs 
                              sensor_msgs 
                              geometry_msgs
                              cv_bridge
                              # darknet_ros_msgs
                              )

link_directories(include
                 ${GAZEBO_LIBRARY_DIRS}
                 ${Boost_LIBRARIES}
                 ${PROTOBUF_LIBRARIES}
                 ${CURSES_LIBRARIES}
)

if(CATKIN_ENABLE_TESTING)
  find_package(roslaunch REQUIRED)
endif()

include_directories( include
                    ${Boost_INCLUDE_DIR}
                    ${catkin_INCLUDE_DIRS}
                    ${GAZEBO_INCLUDE_DIRS}
                    ${CURSES_INCLUDE_DIRS}
                    )

# variables to aid easy creation of ROS nodes
set(COMMON_SRC_FILE_DIR ${PROJECT_SOURCE_DIR}/src/common)
set(COMMON_SRC_FILES  ${COMMON_SRC_FILE_DIR}/pid_rotate.cpp
                      ${COMMON_SRC_FILE_DIR}/pid_translate.cpp)

add_subdirectory(src/vec6)