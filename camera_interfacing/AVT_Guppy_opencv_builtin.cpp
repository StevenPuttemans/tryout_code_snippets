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
