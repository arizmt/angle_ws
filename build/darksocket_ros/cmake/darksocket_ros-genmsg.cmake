# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "darksocket_ros: 1 messages, 0 services")

set(MSG_I_FLAGS "-Idarksocket_ros:/home/users/fri/Desktop/angle_ws/src/darksocket/msg;-Isensor_msgs:/opt/ros/kinetic/share/sensor_msgs/cmake/../msg;-Idarknet_ros_msgs:/home/users/fri/Desktop/angle_ws/src/darknet_ros/darknet_ros_msgs/msg;-Idarknet_ros_msgs:/home/users/fri/Desktop/angle_ws/devel/.private/darknet_ros_msgs/share/darknet_ros_msgs/msg;-Igeometry_msgs:/opt/ros/kinetic/share/geometry_msgs/cmake/../msg;-Istd_msgs:/opt/ros/kinetic/share/std_msgs/cmake/../msg;-Iactionlib_msgs:/opt/ros/kinetic/share/actionlib_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(darksocket_ros_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg" NAME_WE)
add_custom_target(_darksocket_ros_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "darksocket_ros" "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg" "sensor_msgs/Image:std_msgs/Header:darknet_ros_msgs/BoundingBox"
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(darksocket_ros
  "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/sensor_msgs/cmake/../msg/Image.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/Header.msg;/home/users/fri/Desktop/angle_ws/src/darknet_ros/darknet_ros_msgs/msg/BoundingBox.msg"
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/darksocket_ros
)

### Generating Services

### Generating Module File
_generate_module_cpp(darksocket_ros
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/darksocket_ros
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(darksocket_ros_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(darksocket_ros_generate_messages darksocket_ros_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg" NAME_WE)
add_dependencies(darksocket_ros_generate_messages_cpp _darksocket_ros_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(darksocket_ros_gencpp)
add_dependencies(darksocket_ros_gencpp darksocket_ros_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS darksocket_ros_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(darksocket_ros
  "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/sensor_msgs/cmake/../msg/Image.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/Header.msg;/home/users/fri/Desktop/angle_ws/src/darknet_ros/darknet_ros_msgs/msg/BoundingBox.msg"
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/darksocket_ros
)

### Generating Services

### Generating Module File
_generate_module_eus(darksocket_ros
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/darksocket_ros
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(darksocket_ros_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(darksocket_ros_generate_messages darksocket_ros_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg" NAME_WE)
add_dependencies(darksocket_ros_generate_messages_eus _darksocket_ros_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(darksocket_ros_geneus)
add_dependencies(darksocket_ros_geneus darksocket_ros_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS darksocket_ros_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(darksocket_ros
  "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/sensor_msgs/cmake/../msg/Image.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/Header.msg;/home/users/fri/Desktop/angle_ws/src/darknet_ros/darknet_ros_msgs/msg/BoundingBox.msg"
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/darksocket_ros
)

### Generating Services

### Generating Module File
_generate_module_lisp(darksocket_ros
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/darksocket_ros
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(darksocket_ros_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(darksocket_ros_generate_messages darksocket_ros_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg" NAME_WE)
add_dependencies(darksocket_ros_generate_messages_lisp _darksocket_ros_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(darksocket_ros_genlisp)
add_dependencies(darksocket_ros_genlisp darksocket_ros_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS darksocket_ros_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(darksocket_ros
  "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/sensor_msgs/cmake/../msg/Image.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/Header.msg;/home/users/fri/Desktop/angle_ws/src/darknet_ros/darknet_ros_msgs/msg/BoundingBox.msg"
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/darksocket_ros
)

### Generating Services

### Generating Module File
_generate_module_nodejs(darksocket_ros
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/darksocket_ros
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(darksocket_ros_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(darksocket_ros_generate_messages darksocket_ros_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg" NAME_WE)
add_dependencies(darksocket_ros_generate_messages_nodejs _darksocket_ros_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(darksocket_ros_gennodejs)
add_dependencies(darksocket_ros_gennodejs darksocket_ros_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS darksocket_ros_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(darksocket_ros
  "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/kinetic/share/sensor_msgs/cmake/../msg/Image.msg;/opt/ros/kinetic/share/std_msgs/cmake/../msg/Header.msg;/home/users/fri/Desktop/angle_ws/src/darknet_ros/darknet_ros_msgs/msg/BoundingBox.msg"
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/darksocket_ros
)

### Generating Services

### Generating Module File
_generate_module_py(darksocket_ros
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/darksocket_ros
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(darksocket_ros_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(darksocket_ros_generate_messages darksocket_ros_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/users/fri/Desktop/angle_ws/src/darksocket/msg/Detections.msg" NAME_WE)
add_dependencies(darksocket_ros_generate_messages_py _darksocket_ros_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(darksocket_ros_genpy)
add_dependencies(darksocket_ros_genpy darksocket_ros_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS darksocket_ros_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/darksocket_ros)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/darksocket_ros
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_cpp)
  add_dependencies(darksocket_ros_generate_messages_cpp sensor_msgs_generate_messages_cpp)
endif()
if(TARGET darknet_ros_msgs_generate_messages_cpp)
  add_dependencies(darksocket_ros_generate_messages_cpp darknet_ros_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/darksocket_ros)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/darksocket_ros
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_eus)
  add_dependencies(darksocket_ros_generate_messages_eus sensor_msgs_generate_messages_eus)
endif()
if(TARGET darknet_ros_msgs_generate_messages_eus)
  add_dependencies(darksocket_ros_generate_messages_eus darknet_ros_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/darksocket_ros)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/darksocket_ros
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_lisp)
  add_dependencies(darksocket_ros_generate_messages_lisp sensor_msgs_generate_messages_lisp)
endif()
if(TARGET darknet_ros_msgs_generate_messages_lisp)
  add_dependencies(darksocket_ros_generate_messages_lisp darknet_ros_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/darksocket_ros)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/darksocket_ros
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_nodejs)
  add_dependencies(darksocket_ros_generate_messages_nodejs sensor_msgs_generate_messages_nodejs)
endif()
if(TARGET darknet_ros_msgs_generate_messages_nodejs)
  add_dependencies(darksocket_ros_generate_messages_nodejs darknet_ros_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/darksocket_ros)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/darksocket_ros\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/darksocket_ros
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_py)
  add_dependencies(darksocket_ros_generate_messages_py sensor_msgs_generate_messages_py)
endif()
if(TARGET darknet_ros_msgs_generate_messages_py)
  add_dependencies(darksocket_ros_generate_messages_py darknet_ros_msgs_generate_messages_py)
endif()
