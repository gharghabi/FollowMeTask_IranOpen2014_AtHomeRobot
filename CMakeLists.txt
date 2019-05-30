cmake_minimum_required(VERSION 2.8.3)
project(find_me)
find_package(catkin REQUIRED COMPONENTS
    genmsg
    actionlib
    actionlib_msgs
    geometry_msgs
    nav_msgs
    nodelet
    roscpp
    rospy
    sensor_msgs
    std_msgs
    image_transport
    cv_bridge
    pcl_conversions
    pcl_ros
    athomerobot
)
find_package(Boost COMPONENTS system thread date_time REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PCL REQUIRED)
pkg_check_modules(EIGEN REQUIRED eigen3)
add_definitions("-Wno-enam-compare")

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES aut_path_planner
)
include_directories(include)
include_directories(${catkin_INCLUDE_DIRS}${PCL_INCLUDE_DIRS} ${Boost_INCLUDE_DIR} ${OMPL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS})
include_directories(include/libopentld/imacq
    include/libopentld/mftracker
    include/libopentld/tld
    3rdparty/cvblobs
    3rdparty/libconfig
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})
add_subdirectory(3rdparty/cvblobs)
add_subdirectory(3rdparty/libconfig)
add_library(find_me_libopentld
    src/libopentld/imacq/ImAcq.cpp
    src/libopentld/mftracker/BB.cpp
    src/libopentld/mftracker/BBPredict.cpp
    src/libopentld/mftracker/FBTrack.cpp
    src/libopentld/mftracker/Lk.cpp
    src/libopentld/mftracker/Median.cpp
    src/libopentld/tld/Clustering.cpp
    src/libopentld/tld/DetectionResult.cpp
    src/libopentld/tld/DetectorCascade.cpp
    src/libopentld/tld/EnsembleClassifier.cpp
    src/libopentld/tld/ForegroundDetector.cpp
    src/libopentld/tld/MedianFlowTracker.cpp
    src/libopentld/tld/NNClassifier.cpp
    src/libopentld/tld/TLD.cpp
    src/libopentld/tld/TLDUtil.cpp
    src/libopentld/tld/VarianceFilter.cpp
        src/Trajectory.cpp
    src/Config.cpp
    src/FindMe.cpp
    src/Settings.cpp
        3rdparty/libconfig/grammar.c
    3rdparty/libconfig/libconfig.c
    3rdparty/libconfig/libconfigcpp.cc
    3rdparty/libconfig/scanctx.c
    3rdparty/libconfig/scanner.c
    3rdparty/libconfig/strbuf.c
    )
target_link_libraries(find_me_libopentld ${OpenCV_LIBS} ${catkin_LIBRARIES} ${OMPL_LIBRARIES} ${Boost_LIBRARIES} find_me_libopentld find_me_cvblobs)
    
add_executable(FindMe src/findme.cpp)
add_dependencies(FindMe athomerobot_generate_messages_cpp)
target_link_libraries(FindMe ${catkin_LIBRARIES} ${OMPL_LIBRARIES} ${Boost_LIBRARIES} find_me_libopentld find_me_cvblobs)

add_executable(find_me_logic src/find_me_logic.cpp)
add_dependencies(find_me_logic find_me_logic_generate_messages_cpp)
target_link_libraries(find_me_logic
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
)

add_executable(find_me_party_logic src/find_me_party_logic.cpp)
add_dependencies(find_me_party_logic find_me_party_logic_generate_messages_cpp)
target_link_libraries(find_me_party_logic
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
)
# install(TARGETS FindMe
#    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

