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

// --------------------------------------------------------------------------------
// Some remarks for ensuring the correct working of the interface between camera
// and the pc from which you will capture data
//
// FIRST CONFIGURE IP SETTINGS
// - Change the IP address of your pc to 192.168.0.10
// - Change the subnet mask of your pc to 255.255.0.0
// - Change the gateway of your pc to 0.0.0.0
//
// INSTALL THE UEYE SDK DOWNLOADABLE FROM http://en.ids-imaging.com/ueye-software-archive.html
// - Make sure you install the latest version
// - After installing run the 'ueyecameramanager' software package
// - Hit the Manual ETH configuration button and add 192.168.0.5 as IP address
// - Also add 255.255.0.0 as subnet mask
//
// MAKE SURE THE CORRECT LINK LIBRARIES ARE MADE IN THE FOLLOWING ORDER
// - /usr/lib/libueye_api.so
// - `pkg-config opencv --libs`
//
// THE REST OF THE UEYE SOFTWARE SHOULD BE IN THE CORRECT PLACE UNIVERSALLY KNOWN
// --------------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include "opencv2/opencv.hpp"
#include "ueye.h"

using namespace std;
using namespace cv;

void initializeCameraInterface(HIDS* hCam_internal){
    // Open cam and see if it was succesfull
    INT nRet = is_InitCamera (hCam_internal, NULL);
    if (nRet == IS_SUCCESS){
        cout << "Camera initialized!" << endl;
    }

    // Setting the pixel clock to retrieve data
    UINT nPixelClockDefault = 21;
    nRet = is_PixelClock(*hCam_internal, IS_PIXELCLOCK_CMD_SET, (void*)&nPixelClockDefault, sizeof(nPixelClockDefault));

    if (nRet == IS_SUCCESS){
        cout << "Camera pixel clock succesfully set!" << endl;
    }else if(nRet == IS_NOT_SUPPORTED){
        cout << "Camera pixel clock setting is not supported!" << endl;
    }

    // Set the color mode of the camera
    INT colorMode = IS_CM_MONO8;
    nRet = is_SetColorMode(*hCam_internal,colorMode);

    if (nRet == IS_SUCCESS){
        cout << "Camera color mode succesfully set!" << endl;
    }

    // Store image in camera memory --> option to chose data capture method
    // Then access that memory to retrieve the data
    INT displayMode = IS_SET_DM_DIB;
    nRet = is_SetDisplayMode (*hCam_internal, displayMode);
}

// Capture a frame and push it in a OpenCV mat element
Mat getFrame(HIDS* hCam, int width, int height, cv::Mat& mat) {
	// Allocate memory for image
    char* pMem = NULL;
    int memID = 0;
    is_AllocImageMem(*hCam, width, height, 8, &pMem, &memID);

    // Activate the image memory for storing the frame captured
    // Grabbing the image
    // Getting the data of the frame and push it in a Mat element
    is_SetImageMem(*hCam, pMem, memID);
    is_FreezeVideo(*hCam, IS_WAIT);

	VOID* pMem_b;
	int retInt = is_GetImageMem(*hCam, &pMem_b);
	if (retInt != IS_SUCCESS) {
		cout << "Image data could not be read from memory!" << endl;
	}
	memcpy(mat.ptr(), pMem_b, mat.cols * mat.rows);
	is_FreeImageMem(*hCam, pMem, memID)
	
    	return mat;
}

int main()
{
    // ---------------------------------------------------------------------------------------------------------------
    // START BY CONFIGURING THE INTERFACE WITH THE UEYE CAMERA
    // ---------------------------------------------------------------------------------------------------------------

    // Camera initialisation
    // Index 0 means taking the first camera available
    HIDS hCam = 0;
    initializeCameraInterface(&hCam);

    // ---------------------------------------------------------------------------------------------------------------
    // CAPTURE DATA AND PROCESS
    // ---------------------------------------------------------------------------------------------------------------

    while(true){
	// Create an image and grab a frame
        Mat current_image (576, 768, CV_8UC1);
        getFrame(&hCam, 768, 576, current_image);

        // PERFORM YOUR OPENCV PROCESSING HERE!
        // Visualise the data
        imshow("test_interface", current_image);

        // Check if we need to stop processing
        if ( (int)waitKey(10) >= 0 ){
            break;
        }
    }

    // Release the camera again
    is_ExitCamera(hCam);

    return 0;
}

