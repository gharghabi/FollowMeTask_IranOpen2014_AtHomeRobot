#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <ros/ros.h>

#include <unistd.h>
#include <cstdio>
#include <iostream>

#include <boost/thread.hpp>

#include "athomerobot/head.h"
#include "find_me_logic.h"



void basicLogicCallback(const std_msgs::String &msg);
void emergencyCallback(const std_msgs::String &msg);
void findMeLogicCallback(const std_msgs::String &msg);
void slamIsMoveCallback(const std_msgs::Int32 &msg);
void logic();

ros::Publisher basicLogicPublisher;
ros::Publisher genericLogicPublisher;
ros::Publisher odometryXPublisher;
ros::Publisher odometryYPublisher;
ros::Publisher slamPublisher;
ros::Publisher speakPublisher;
ros::Publisher zPublisher;
ros::Publisher headPublisher;


int state = -1;

bool goingToRoom = false;
bool slamGetOne = false;
int main(int argc, char **argv) {
    ros::init(argc, argv, "find_me_logic");
    ROS_INFO("FindMe Logic Started.");

    ros::NodeHandle advertiseNodeHandle;


    basicLogicPublisher = advertiseNodeHandle.advertise<std_msgs::String>(TO_BASIC_LOGIC, 10);

    ros::NodeHandle fromBasicLogicNodeHandle;
    ros::NodeHandle emergencyFromBasicLogicNodeHandle;

    ros::Subscriber basicLogicSubscriber = fromBasicLogicNodeHandle.subscribe(FROM_BASIC_LOGIC, 10, basicLogicCallback);
    basicLogicPublisher = advertiseNodeHandle.advertise<std_msgs::String>(TO_BASIC_LOGIC, 10);

    ros::Subscriber emergencyLogicSubscriber = emergencyFromBasicLogicNodeHandle.subscribe(FROM_BASIC_LOGIC_EMERGENCY, 10, emergencyCallback);



    ros::NodeHandle genericLogicNodeHandleToTask;
    ros::Subscriber taskLogicSubscriber = genericLogicNodeHandleToTask.subscribe(TASK_TO_LOGIC, 10, findMeLogicCallback);
    genericLogicPublisher = advertiseNodeHandle.advertise<std_msgs::String>(LOGIC_TO_TASK, 10);


    odometryXPublisher = advertiseNodeHandle.advertise<std_msgs::Int32>(TOPIC_MOVE_GLX, 10);
    odometryYPublisher = advertiseNodeHandle.advertise<std_msgs::Int32>(TOPIC_MOVE_GLY, 10);


    speakPublisher = advertiseNodeHandle.advertise<std_msgs::String>(TOPIC_SPEAK, 10);

    slamPublisher = advertiseNodeHandle.advertise<std_msgs::String>(SLAM_COMMAND_TOPIC, 10);

    ros::NodeHandle slamNodeHandle;
    ros::Subscriber slamSubscriber = slamNodeHandle.subscribe(SLAM_IS_MOVE_TOPIC, 10, slamIsMoveCallback);

    zPublisher = advertiseNodeHandle.advertise<std_msgs::String>(DESIREZ_TOPIC, 10);
    headPublisher = advertiseNodeHandle.advertise<athomerobot::head>(HEAD_TOPIC, 10);

    usleep(1000000);

    boost::thread logicThread(logic);/*start logic thread*/
    ros::spin();
    logicThread.interrupt();
    logicThread.join();

    return EXIT_SUCCESS;
}

void basicLogicCallback(const std_msgs::String &msg) { /*this method (function) just once call, when basic logic call this task*/
    ROS_INFO("basicLogicCallback: msg: %s", msg.data.c_str());
    if (msg.data == TASK_INIT) {
        state = -10;
    } else if (msg.data == TASK_START) {
        state = 0;
    }
}

void emergencyCallback(const std_msgs::String &msg) { /*this method (function) just once call, when general logic call this task*/
    ROS_INFO("emergency msg: %s", msg.data.c_str());
    if (msg.data == TASK_STOP) {
        ros::shutdown();
    }
}

void slamIsMoveCallback(const std_msgs::Int32 &msg) {
    if (goingToRoom && msg.data == 1) {
        slamGetOne = true;
    }
    if (slamGetOne && goingToRoom && msg.data == 0) {
        goingToRoom = false;
        state = 3;
    }
}
void findMeLogicCallback(const std_msgs::String &msg) {
    ROS_INFO("findMeLogicCallback: msg: %s", msg.data.c_str());
    if (msg.data == FIND_ME_PHASE1_DONE) {
        state = 1;
    } else if (msg.data == FIND_ME_PHASE2_DONE) {
        state = 4;
    } else if (msg.data == FIND_ME_PHASE3_DONE) {
        state = 5;
    } else if (msg.data == FIND_ME_RIGHT) {
        std_msgs::String speakMsg;
        speakMsg.data = GO_TO_RIGHT_TEXT;
        speakPublisher.publish(speakMsg);
    } else if (msg.data == FIND_ME_LEFT) {
        std_msgs::String speakMsg;
        speakMsg.data = GO_TO_LEFT_TEXT;
        speakPublisher.publish(speakMsg);
    }

}
void logic() { /*must call for every state changes*/
    ROS_INFO("State is: %d", state);
    std_msgs::String msg;
    std_msgs::Int32 msgInt;
    while (!ros::isShuttingDown()) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        if (ros::isShuttingDown()) {
            break;
        }
        if (state == -1) {
            continue;
        } else if (state == -10) {/*Learn*/
            msg.data = FIND_ME_PHASE0;
            genericLogicPublisher.publish(msg);
            ROS_INFO("Initiated.");
            msgInt.data = 2500;
            zPublisher.publish(msgInt);
            athomerobot::head msgHead;
            msgHead.pan = 0;
            msgHead.tilt = 0;
            headPublisher.publish(msgHead);

            state = -1; /*wait for learn*/
        } else if (state == 0) {/*Learn*/
            msg.data = FIND_ME_PHASE1;
            genericLogicPublisher.publish(msg);
            ROS_INFO("Start Learning.");
            state = -1; /*wait for learn*/
        } else if (state == 1) {
            msg.data = LEARNED_TEXT;
            speakPublisher.publish(msg);
            state = 2;
            ROS_INFO("Learned.");
        } else if (state == 2) {
            ROS_INFO("Going to room");
            msg.data = ROOM_LANDMARK;
            slamPublisher.publish(msg);
            state = -1;
            goingToRoom = true;
        } else if (state == 3) {
            ROS_INFO("In room");
            msg.data = FIND_ME_PHASE2;
            genericLogicPublisher.publish(msg); /*or for example odometry or any others*/
            state = -1;
        } else if (state == 4) {
            msg.data = FOUND_TEXT;
            speakPublisher.publish(msg);

            msg.data = FIND_ME_PHASE3;
            genericLogicPublisher.publish(msg);
            state = -1;
        } else if (state == 5) {
            ROS_INFO("Find Me Done");
            msg.data = TASK_DONE;
            basicLogicPublisher.publish(msg);
            ros::shutdown();
            state = -1;
            return;
        }
    }
}