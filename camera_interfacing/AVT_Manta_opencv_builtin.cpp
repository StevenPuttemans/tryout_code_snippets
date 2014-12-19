/*********************************************************************************************************
Copyright (c) 2013 EAVISE, KU Leuven, Campus De Nayer
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:
- A referral to the origal author of this software (Steven Puttemans)
- A notice send at the original author (steven.puttemans[at]kuleuven.be)
The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************************************************/

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
