/*********************************************************
* Software for accessing AVT cameras of Guppy series
* using the builtin PvAPI interface of OpenCV
*
* Remove the Bayer conversion if you are using a
* monochrome variant of the camera
***********************************************************/

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int main()
{
    VideoCapture camera(0 + CV_CAP_IEEE1394);
    Mat frame;

    while(true){
        camera >> frame;
        cvtColor(frame,frame,CV_BayerBG2BGR);

        imshow("test", frame);
        int key = waitKey(10);
        if(key == 27){
            break;
        }
    }

    return 0;
}
