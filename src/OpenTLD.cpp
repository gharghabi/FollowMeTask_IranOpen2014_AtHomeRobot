
#include <ros/ros.h>
#include "std_msgs/Int32.h"

#include "Main.h"
#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include <stdio.h>

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netdb.h>
// #include <exception>
// #include <boost/thread.hpp>
// #include <math.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#include <iostream>
#include <pcl/console/parse.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/sample_consensus/sac_model_sphere.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <boost/thread/thread.hpp>
#include <pcl/visualization/cloud_viewer.h>

#include <math.h>
#include <pcl/common/common.h>
#include <pcl/common/transforms.h>
// #include <flann/flann.h>
// #include <flann/io/hdf5.h>
#include <boost/filesystem.hpp>

#include <pcl/conversions.h>

//ROS includes
#include<stdexcept>
// ROS core
#include <ros/ros.h>
#include <boost/thread/mutex.hpp>
#include <pcl_ros/point_cloud.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/Image.h>
#include <stereo_msgs/DisparityImage.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/image_encodings.h>

// #include "athomerobot/arm.h"
// #include "athomerobot/omnidata.h"
// #include "athomerobot/head.h"
// #include "athomerobot/irsensor.h"
// #include "std_msgs/Int32.h"
// PCL includes
// #include <pcl/io/io.h>
// #include <pcl/io/pcd_io.h>
// #include <pcl/point_types.h>
// #include <pcl/range_image/range_image_planar.h>
// #include <pcl/filters/passthrough.h>
// #include <pcl/common/common_headers.h>
// #include <pcl/visualization/cloud_viewer.h>
//openCV
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
// cv_bridge
#include <cv_bridge/cv_bridge.h>
// messages
// timer
#include <cstdio>
#include <ctime>

#include <sensor_msgs/Image.h>
#include <stereo_msgs/DisparityImage.h>
#include <sensor_msgs/CameraInfo.h>


//void Omnidrive(int x,int y,int w);

using namespace cv;
using namespace std;
using namespace pcl;

using tld::Config;
using tld::Gui;
using tld::Settings;

ros::Publisher robotMovex;
ros::Publisher robotMovey;

char buffer[1843236];
cv::Mat image_rgb;
cv::Mat image_depth;
cv::Mat image_main;

ImAcq *imAcq;
Main *thmain;
typedef std_msgs::Int32 int_msg;

ros::Publisher chatter_pub[3];

void error(const char *msg) {
    cout << msg << endl;
    //exit(0);
}


class ImageConverter {
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber image_sub_;

public:
    ImageConverter()
        : it_(nh_) {
        // Subscrive to input video feed and publish output video feed
        image_sub_ = it_.subscribe("/camera/rgb/image_color", 1,
                                   &ImageConverter::imageCb, this);

        //cv::namedWindow(OPENCV_WINDOW);
    }

    ~ImageConverter() {
        //cv::destroyWindow(OPENCV_WINDOW);
    }
    void imageCb(const sensor_msgs::ImageConstPtr &msg) {
        cv_bridge::CvImagePtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception &e) {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        };
        int_msg chapmsg;//chap
        chapmsg.data = -10;
        int_msg rastmsg;
        rastmsg.data = 10; //rast
        int_msg jolomsg;//jolo
        jolomsg.data = 10;
        if (thmain->picready == true) {
            imAcq->khrgb_input = &cv_ptr->image; //tasvir ba background vabi background !
            imAcq->khpicture_flag = true;
            thmain->doWork();
            if (thmain->move == 0) {
                std::cout << "left" << std::endl;
                //                Omnidrive(0,0,50);
            } else if (thmain->move == 1) {
                //      std::cout<<"right"<<std::endl;
                //      Omnidrive(0,0,-50);
            } else if (thmain->move == 2) {
                std::cout << "mostaghim" << std::endl;
                //            Omnidrive(0,0,0);
            } else {
                std::cout << "hich kodam" << std::endl;
                //          Omnidrive(0,0,0);
            }
            thmain->move == 3;
        }
    }
};


// void Cloud_CallBack(const sensor_msgs::PointCloud2ConstPtr& cloud_m)
// {

// //  boost::shared_ptr<const pcl::PointCloud<pcl::PointXYZRGBA> > pointer(cloud_m);

//     pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZRGBA>);
//     pcl::PointCloud<pcl::PointXYZRGBA> pointer;
//     pcl::fromROSMsg(*cloud_m, pointer);
//     //*global_cloud = pointer;

//     //  for (size_t i = 0; i < pointer.points.size (); ++i)
//     //  {
//     //     std::cout << "    " << pointer.points[i].x
//     //           << " "    << pointer.points[i].y
//     //           << " "    << pointer.points[i].z << std::endl;
//     // }

//     for (int i = 0; i < pointer.width; ++i)
//       {
//         for (int j = 0; j < pointer.height; ++j)
//         {
//           std::cout << "    " << pointer.points[i,j].x
//                << " "    << pointer.points[i,j].y
//                << " "    << pointer.points[i,j].z << std::endl;
//         }
//      }
//     waitKey(10);
//     bool s=false;
//   if (pointer.width !=0)
//   {

//     s=true;

//   }
//  // std::cout<<" "<<pointer.points[1].x<<" "<<pointer.points[1].y<<" "<<pointer.points[1].z<<" "<<pointer.width<<" "<<pointer.height<<std::endl;

// // std::cout<<pointer.points[10,10].x<<std::endl;
//   // if(s)
//   // {

//   // //cout<< "start" <<endl;
//   //   pcd_read = true;
//   //   //start = clock();
//   // }
// }

// void Omnidrive(int x,int y,int w)
// {
//    //send  message via ROS to autrobot node to controll motors
//  athomerobot::omnidata msg;
//  msg.d0 = x;
//  msg.d1 = y;
//  msg.d2 = w;
//
//  chatter_pub[0].publish(msg);
// }

int main(int argc, char **argv) {
    ros::init(argc, argv, "ROSOpenTLD");
    cout << "App start" << endl;

    //boost::thread client_Process(&server_getpicture);

    // ros::NodeHandle n_listenDepth;
    // ros::Subscriber subDepth = n_listenDepth.subscribe("/camera/depth_registered/points", 1, Cloud_CallBack);

    // ros::NodeHandle robotMove_nh;
    //robotMovex=robotMove_nh.advertise<int_msg>("AUTROBOTIN_moveglx",10);
    //robotMovey=robotMove_nh.advertise<int_msg>("AUTROBOTIN_movegly",10);

    //?ros::NodeHandle n;
    //?chatter_pub[0] = n.advertise<athomerobot::omnidata>("/AUTROBOTIN_omnidrive", 1000);

    imAcq = imAcqAlloc();
    Mat mypic = cv::Mat::zeros(480, 640, CV_8UC3);
    imAcq->khrgb_input = &mypic;


    thmain = new Main();
    Config config;

    Gui *gui = new Gui();

    thmain->gui = gui;
    thmain->imAcq = imAcq;
    if (config.init(argc, argv) == PROGRAM_EXIT) {
        return EXIT_FAILURE;
    }

    config.configure(thmain);

    srand(thmain->seed);

    imAcqInit(imAcq);

    if (thmain->showOutput) {
        gui->init();
    }


    //main->doWork();


    ros::init(argc, argv, "image_converter");
    ImageConverter ic;
    ros::spin();

    //  image_pub_ = it_.advertise("/image_converter/output_video", 1);

    //server_getpicture();

    delete thmain;
    thmain = NULL;
    delete gui;
    gui = NULL;

    //client_Process.interrupt();
    //client_Process.join();

    return EXIT_SUCCESS;
}

// min_win: 15
// patch_size: 15
// ncc_thesame: 0.95
// valid: 0.5
// num_trees: 10
// num_features: 13
// thr_fern: 0.5
// thr_nn: 0.65
// thr_nn_valid: 0.7
// num_closest_init: 10
// num_warps_init: 20
// noise_init: 5
// angle_init: 20
// shift_init: 0.02
// scale_init: 0.02
// num_closest_update: 10
// num_warps_update: 10
// noise_update: 5
// angle_update: 10
// shift_update: 0.02
// scale_update: 0.02
// overlap: 0.2
// num_patches: 100
// bb_x: 288
// bb_y: 36
// bb_w: 25
// bb_h: 42