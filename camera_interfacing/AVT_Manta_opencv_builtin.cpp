/**********************************************************************************
* Software for accessing AVT cameras of Prosilica and Manta series
* using the builtin PvAPI interface of OpenCV
**********************************************************************************/

// --------------------------------------------------------------------------------
// Some remarks for ensuring the correct working of the interface between camera
// and the pc from which you will capture data
//
// You have to be sure that OpenCV is built with the PvAPI interface enabled.
//
// FIRST CONFIGURE IP SETTINGS
// - Change the IP address of your pc to 169.254.1.1
// - Change the subnet mask of your pc to 255.255.0.0
// - Change the gateway of your pc to 169.254.1.2
//
// CHANGE SOME NETWORK CARD SETTINGS
// - sudo ifconfig [eth0 - eth1] mtu 9000 - or 9016 ideally if your card supports that
//
// LINK THE OPENCV DEPENDENT LIBRARIES USING THE ADDITIONAL LINKER OPTION
//	`pkg-config opencv --libs`
// --------------------------------------------------------------------------------

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main()
{
    VideoCapture camera(0 + CV_CAP_PVAPI);
    Mat frame;

    while(true){
        camera >> frame;
        imshow("test", frame);
        int key = waitKey(10);
        if(key == 27){
            break;
        }
    }

    return 0;
}
