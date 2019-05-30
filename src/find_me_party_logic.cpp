#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <ros/ros.h>

#include <unistd.h>
#include <cstdio>
#include <iostream>

#include <boost/thread.hpp>

#include "find_me_logic.h"




#define TO_FIND_ME "AUTROBOT_to_find_me_party"
#define FROM_FIND_ME "AUTROBOT_from_find_me_party"

#define FIND_ME_INIT "inti"
#define FIND_ME_DETECT_FACE "detect face"
#define FIND_ME_DETECT_FACE_DONE "detect face done"
#define FIND_ME_LEARN_FACE_DONE "learn face done"


#define FIND_ME_FIND_AND_GO_NEAR "find and go near"
#define FIND_ME_FIND_AND_GO_NEAR_DONE "go near done"
#define FIND_ME_FIND_AND_GO_NEAR_NOT_DONE "go near failed"



void toFindMeCallback(const std_msgs::String &msg);
void findMeLogicCallback(const std_msgs::String &msg);
void logic();

ros::Publisher partyPublisher;
ros::Publisher genericLogicPublisher;
ros::Publisher odometryXPublisher;
ros::Publisher odometryYPublisher;
ros::Publisher speakPublisher;


int state = -1;
int main(int argc, char **argv) {
    ros::init(argc, argv, "find_me_logic");
    ROS_INFO("FindMe Logic Started.");

    ros::NodeHandle advertiseNodeHandle;


    partyPublisher = advertiseNodeHandle.advertise<std_msgs::String>(TO_BASIC_LOGIC, 10);

    ros::NodeHandle toFindMeNodeHandle;

    ros::Subscriber toFindMeSubscriber = toFindMeNodeHandle.subscribe(TO_FIND_ME, 10, toFindMeCallback);
    partyPublisher = advertiseNodeHandle.advertise<std_msgs::String>(FROM_FIND_ME, 10);



    ros::NodeHandle genericLogicNodeHandleToTask;
    ros::Subscriber taskLogicSubscriber = genericLogicNodeHandleToTask.subscribe(TASK_TO_LOGIC, 10, findMeLogicCallback);
    genericLogicPublisher = advertiseNodeHandle.advertise<std_msgs::String>(LOGIC_TO_TASK, 10);


    odometryXPublisher = advertiseNodeHandle.advertise<std_msgs::Int32>(TOPIC_MOVE_GLX, 10);
    odometryYPublisher = advertiseNodeHandle.advertise<std_msgs::Int32>(TOPIC_MOVE_GLY, 10);
    speakPublisher = advertiseNodeHandle.advertise<std_msgs::String>(TOPIC_SPEAK, 10);




    usleep(1000000);

    boost::thread logicThread(logic);/*start logic thread*/
    ros::spin();
    logicThread.interrupt();
    logicThread.join();

    return EXIT_SUCCESS;
}

void toFindMeCallback(const std_msgs::String &msg) { /*this method (function) just once call, when basic logic call this task*/
    ROS_INFO("toFindMeCallback: msg: %s", msg.data.c_str());
    if (msg.data == FIND_ME_INIT) {
        state = -10;
    } else if (msg.data == FIND_ME_DETECT_FACE) {
        state = 0;
    } else if (msg.data == FIND_ME_FIND_AND_GO_NEAR) {
        state = 3;
    }
}

void findMeLogicCallback(const std_msgs::String &msg) {
    ROS_INFO("findMeLogicCallback: msg: %s", msg.data.c_str());
    if (msg.data == FIND_ME_PHASE1_DONE) {
        state = 2;
    } else if (msg.data == FIND_ME_PHASE1_0_DONE) {
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
            state = -1; /*wait for learn*/
        } else if (state == 0) {/*Learn*/
            msg.data = FIND_ME_PHASE1;
            genericLogicPublisher.publish(msg);
            ROS_INFO("Start Learning.");
            state = -1; /*wait for learn*/
        } else if (state == 1) {
            msg.data = FIND_ME_DETECT_FACE_DONE;
            partyPublisher.publish(msg);
            state = -1;
            ROS_INFO("Face Detected.");
        } else if (state == 2) {
            msg.data = FIND_ME_LEARN_FACE_DONE;
            partyPublisher.publish(msg);
            state = -1;
            ROS_INFO("Learned.");
        } else if (state == 3) {
            msg.data = FIND_ME_PHASE2;
            genericLogicPublisher.publish(msg);
        } else if (state == 4) {
            msg.data = FIND_ME_FIND_AND_GO_NEAR_DONE;
            partyPublisher.publish(msg);
            state = 5;
        } else if (state == 5) {
            ROS_INFO("Find Me Done");
            ros::shutdown();
            state = -1;
            return;
        }
    }
}