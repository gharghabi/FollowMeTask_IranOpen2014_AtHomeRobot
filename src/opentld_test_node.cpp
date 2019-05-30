#include <ros/ros.h>
#include <stdio.h>
#include <TLD.h>

using namespace std;

int main(int argc, char** argv)
{
    tld::TLD a;
    ros::init(argc, argv, "opentld_test_node");
    cout<<"123"<<endl;

    return 0;
}