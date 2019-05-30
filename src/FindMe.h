/*
 * File:   FindMe.h
 * Author: ramin
 *
 * Created on March 26, 2014, 5:11 PM
 */

#ifndef FINDME_H
#define FINDME_H

#include "TLD.h"
#include "ImAcq.h"
#include <stdio.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <ros/ros.h>




//todo: copy
#define FACE_CASCADE_NAME  "/usr/haarcascade_frontalface_default.xml"

enum Retval {
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

class FindMe {

    cv::VideoCapture videoCapture;

    cv::Mat image;
    cv::Mat gray;
    cv::Mat camImage;

    CvScalar yellow;
    CvScalar blue;
    CvScalar black;
    CvScalar white;


    std::vector<cv::Rect> faces;


    bool inDoWork;
    bool inited;

    float fps;

public:
    tld::TLD *tld;
    double threshold;

    void getImageFromCam(cv::Mat &image);
    void getImageFromRos(cv::Mat &image);

    void updateImage();
    void updateImage(cv::Mat img);

    void detectFaces();
    bool detectFace();

    void setTrackObjectBoundingBox(cv::Rect *boundingBox);

    void stopLearning();
    void startLearning();
    void setLearning(bool learning);

    void init();
    void doWork();
    void mainLoop();
    int findMaximumFace();
    void Omnidrive(int x, int y, int w) ;
    void controlRobot();

    void showImage();

    float getRect(cv::Rect &boundingBox); /*return posterior, set boundingBox x,y,w,h*/
    FindMe() {



        yellow = CV_RGB(255, 255, 0);
        blue = CV_RGB(0, 0, 255);
        black = CV_RGB(0, 0, 0);
        white = CV_RGB(255, 255, 255);

        tld = new tld::TLD();

        threshold = 0.6;

        inDoWork = false;

        inited = false;

    }

    ~FindMe() {
        videoCapture.release();

        delete tld;
    }

};


#endif  /* FINDME_H */

