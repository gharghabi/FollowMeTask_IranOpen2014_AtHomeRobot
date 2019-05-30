
#include "FindMe.h"
#include "Config.h"



using namespace tld;
using namespace cv;


ros::Publisher robotMovex;
ros::Publisher robotMovey;

void scaleRect(Rect &rect, float factor) {
    int w = rect.width;
    int h = rect.height;

    rect.width *= factor;
    rect.height *= factor;
    rect.x += (w - rect.width) / 2;
    rect.y += (h - rect.height) / 2;
}

void FindMe::Omnidrive(int x, int y, int w) {
    //send  message via ROS to autrobot node to controll motors
    //TOCOMPILE:     athomerobot::omnidata msg;
    //TOCOMPILE:     msg.d0 = x;
    //TOCOMPILE:     msg.d1 = y;
    //TOCOMPILE:     msg.d2 = w;

    //TOCOMPILE:     chatter_pub[0].publish(msg);
}

void FindMe::doWork() {
    if (inDoWork) {
        ROS_INFO("Early in dowork.");
        return;
    }
    inDoWork = true;
    double tic = getTickCount();
    tld->processImage((image));
    double toc = (cvGetTickCount() - tic) / cvGetTickFrequency();
    toc = toc / 1000000;
    fps = 1 / toc;
    inDoWork = false;


    showImage();
    cv::waitKey(1);

}
void FindMe::showImage() {

    char string[128];
    char learningString[10] = "";
    if (tld->learning) {
        strcpy(learningString, "Learning");
    }
    sprintf(string, "Posterior %.2f; fps: %.2f, #numwindows:%d, %s", tld->currConf, fps, tld->detectorCascade->numWindows, learningString);


    if (tld->currBB != NULL) {
        CvScalar rectangleColor = (tld->currConf > threshold) ? blue : yellow;
        rectangle(image, tld->currBB->tl(), tld->currBB->br(), rectangleColor, 8, 8, 0);
    }


    rectangle(image, cvPoint(0, 0), cvPoint(image.size().width, 50), black, CV_FILLED, 8, 0);
    putText(image, string, Point(25, 25), FONT_HERSHEY_SIMPLEX, .5, white);

    imshow("TLD", image);
    // waitKey(10);
}
void FindMe::getImageFromCam(Mat &image) {
    if (!videoCapture.isOpened() && videoCapture.open(0) && !videoCapture.isOpened() ) {
        ROS_ERROR("Cant open VideoCapture.");
        exit(-1);
    }

    videoCapture >> image;
}
void FindMe::getImageFromRos(Mat &image) {
    if (camImage.size().width != 0) {
        camImage.copyTo(image);
    }
}

void FindMe::updateImage() {
    Mat img;
    getImageFromRos(img);
    updateImage(img);
}

void FindMe::updateImage(Mat img) {
    if (inDoWork) {
        return;
    }
    if (img.size().width != 0) {
        img.copyTo(image);
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        if (!inited) {
            init();
        }
    }
}

int FindMe::findMaximumFace() {
    if (faces.empty()) {
        return -1;
    }
    int maximumIndex = 0;
    float maximum;
    maximum = faces[0].width * faces[0].height ;
    float temp;
    for (int i = 1; i < faces.size(); i++) {
        if ((temp = faces[i].width * faces[i].height) > maximum) {
            maximumIndex = i;
            maximum = temp;
        }
    }
    return maximumIndex;
}
bool FindMe::detectFace() {

    int faceIndex;


    if (image.size().width == 0 ) {
        return false;
    }
    detectFaces();/*
    for (size_t i = 0; i < faces.size(); i++) {
        rectangle(gray, faces[i], 5);
    }
    imshow("faces", gray);
    waitKey(5);*/
    faceIndex = findMaximumFace();
    if (faceIndex != -1) {
        scaleRect( faces[faceIndex], .8);
        setTrackObjectBoundingBox(&faces[faceIndex]);
        return true;
    }
    return false;
}


void FindMe::detectFaces() {

    faces.clear();
    CascadeClassifier face_cascade;

    if (!face_cascade.load(FACE_CASCADE_NAME)) {
        ROS_ERROR("Error in loading Cascade File.");
        // exit(1);
        ros::shutdown();
    };
    face_cascade.detectMultiScale(gray, faces, 1.2, 12, 0 | CV_HAAR_SCALE_IMAGE , cv::Size(50, 50), cv::Size(400, 400));
}

void FindMe::setTrackObjectBoundingBox(Rect *boundingBox) {

    ROS_INFO("Start Tracking Object {x:%d, y:%d, width:%d, height:%d}.", boundingBox->x, boundingBox->y, boundingBox->width, boundingBox->height);
    tld->selectObject(gray, boundingBox);
    doWork();
}

void FindMe::stopLearning() {
    setLearning(false);
}

void FindMe::startLearning() {
    setLearning(true);
}

void FindMe::setLearning(bool learning) {
    ROS_INFO("Learning %s.", learning ? "started" : "stopped");
    tld->learningEnabled = learning;
}

void FindMe::init() {
    tld->detectorCascade->imgWidth = gray.cols;
    tld->detectorCascade->imgHeight = gray.rows;
    tld->detectorCascade->imgWidthStep = gray.step;
    ROS_INFO("TLD Initiated");
    inited = true;
}
void FindMe::mainLoop() {
    detectFace();
    while (waitKey(5) != 'q') {
        updateImage();
        doWork();
    }
}

float FindMe::getRect(cv::Rect &boundingBox) {
    if (tld->currBB != NULL) {
        boundingBox.x =  tld->currBB->x;
        boundingBox.y =  tld->currBB->y;
        boundingBox.width =  tld->currBB->width;
        boundingBox.height =  tld->currBB->height;
        // ROS_INFO("Object {Posterior: %f ,x:%d, y:%d, width:%d, height:%d}.", tld->currConf, tld->currBB->x, tld->currBB->y, tld->currBB->width, tld->currBB->height);
        return tld->currConf;
    } else {
        // ROS_INFO("Not found");
        return -1;
    }
}

void FindMe::controlRobot() {
    if (tld->currConf > threshold) {
        // int deltaX = 320 - (tld->currBB->x + tld->currBB->width / 2);
        // Omnidrive(0, 0, -deltaX / 3);
        ROS_INFO("threshold: %f, Posterior: %f.", threshold, tld->currConf );
        // found = true;
    }

    /*TOCOMPILE: if (!found) {
        Omnidrive(0, 0, 50);
    } else {
        if (incenter) {
            Omnidrive(0, 0, 0);
        } else {
            int deltaX = 320 - (tld->currBB->x + tld->currBB->width / 2);
            if (deltaX > -50 && deltaX < 50) {
                incenter = true;
            } else {
                Omnidrive(0, 0, -deltaX / 3);
            }
        }
    }*/

}


// void FindMe::rosImageCallBack(const sensor_msgs::ImageConstPtr &msg) {
//     cv_bridge::CvImagePtr cv_ptr;
//     try {
//         cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
//     } catch (cv_bridge::Exception &e) {
//         ROS_ERROR("cv_bridge exception: %s", e.what());
//         return;
//     };
//     camImage = cv_ptr->image;

//     updateImage();
//     if (!inited) {
//         init();
//         ROS_INFO("Init: done.");
//         inited = true;


//         // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//         //TOCOMPILE: ROS_INFO("Waiting for action server to start.");
//         // wait for the action server to start
//         //TOCOMPILE: followPathAction.waitForServer(); //will wait for infinite time
//         //TOCOMPILE: ROS_INFO("server start.");
//         // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

//         return;
//     }
//     if (!inDoWork) {
//         if (!faceDetected) { /*Step 1*/
//             detectFace();
//         }
//         if (!faceDetected) return;

//         if (frameCounter++ == 200) {
//             stopLearning();
//             ROS_INFO("Learned.");
//             found = false;
//             incenter = false;


//             imshow("ha", image);
//             waitKey(10);
//             waitForRoom = true;
//             return;
//         } else if ( frameCounter < 200) {

//             doWork();
//             inDoWork = false;
//             return;
//         }


//         /*SLAM*/
//         if (waitForRoom) {
//             waitForRoom = false;

//             ROS_INFO("Action server started, sending goal.");
//             // send a goal to the action
//             //TOCOMPILE:             movements::FollowPathGoal goal;
//             //TOCOMPILE:             geometry_msgs::Pose p;
//             //TOCOMPILE:             goal.path.push_back(p);

//             //TOCOMPILE:             p.position.x = 20;

//             //TOCOMPILE:             goal.move_speed = 100;
//             //TOCOMPILE:             goal.path.push_back(p);
//             //TOCOMPILE:             followPathAction.sendGoal(goal);


//             //wait for the action to return
//             //TOCOMPILE:             bool finished_before_timeout = followPathAction.waitForResult(ros::Duration(180.0/*in secconds*/));

//             //TOCOMPILE:             if (finished_before_timeout) {
//             inRoom = true;
//             //TOCOMPILE:                 ROS_INFO("Action finished");
//             return;
//             //TOCOMPILE:             } else
//             //TOCOMPILE:                 ROS_INFO("Action did not finish before the time out.");

//         }

//         /*step 3*/
//         if (inRoom) {
//             doWork();
//             controlRobot();

//             inDoWork = false;
//         }

//     }
// }
