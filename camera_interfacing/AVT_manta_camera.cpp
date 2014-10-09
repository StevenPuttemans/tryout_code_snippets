/**********************************************************************************
* Copyright (C) 2008 Metron Hong Kong Limited.  All Rights Reserved.
*
* Reproduction or disclosure of this file or its contents is granted without
* prior written consent of Metron Hong Kong Limited.
**********************************************************************************
* Code name: AVT_manta_camera.cpp
* Originally written by: Antonio Yu, Chief Consultant
* Date: 1 May, 2008
* Version: 1.0
**********************************************************************************
* Adapted by Steven Puttemans on 1/08/2012
* Tested in combination with the following setup
* 1) An AVT MANTA 201 MonoChrome camera
* 2) OpenCV 2.4.8 - but compatible with anything above OpenCV 2.x
* 3) Removed the old and depricated C-API and replaced by C++ - API
**********************************************************************************/

// --------------------------------------------------------------------------------
// Some remarks for ensuring the correct working of the interface between camera
// and the pc from which you will capture data
//
// FIRST CONFIGURE IP SETTINGS
// - Change the IP address of your pc to 169.254.1.1
// - Change the subnet mask of your pc to 255.255.0.0
// - Change the gateway of your pc to 169.254.1.2
//
// CHANGE SOME NETWORK CARD SETTINGS
// - sudo ifconfig [eth0 - eth1] mtu 9000 - or 9016 ideally if your card supports that
//
// MAKE SURE THE CORRECT INCLUDES ARE MADE IN THE FOLLOWING ORDER
// - /opt/AVT_sdk/lib-pc/x64/4.5/libPvAPI.a
// - /usr/lib/x86_64-linux-gnu/librt.so
// - /usr/lib/x86_64-linux-gnu/libpthread.so
//
// ADD THE PVAPI HEADER TO YOUR PROJECT
//
// LINK THE OPENCV DEPENDENT LIBRARIES USING THE ADDITIONAL LINKER OPTION
//	`pkg-config opencv --libs`
// --------------------------------------------------------------------------------

// Tell the system that you are on a 64 bit linux based system --> needed for PvAPI
#define _x64 1
#define _LINUX 1

// Link to the AVT technologies camera library for AVT Manta
// Make sure you download the PvApi from: http://www.alliedvisiontec.com/us/products/legacy.html
#include "PvApi.h"

// Link to the necessary OpenCV libraries
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

// Functionality to provide extra output
#include <string.h>
#include <iostream>
#include <time.h>

using namespace std;
using namespace cv;

// camera's data type definition
typedef struct
{
    unsigned long   UID;
    tPvHandle       Handle;
    tPvFrame        Frame;

} tCamera;

#define CH_MONO 1	// Single channel for mono images

void initialize_camera(tCamera* current_cam, int max_capture_width, int max_capture_height, int desired_width, int desired_height){
    // Initialize the PvAPI interface so that we can look for cameras
    if(!PvInitialize()){
        // Wait for the response from a camera after the initialization of the driver
        // This is done by checking if camera's are found yet
        while(PvCameraCount() == 0)
        {
            waitKey(15);
        }

        // If there is a camera connecte to the camera 1 interface, grab it!
        tPvCameraInfo cameraInfo;
        if ( PvCameraList(&cameraInfo, 1, NULL) == 1)
	{
            unsigned long frameSize;

            // Get the camera ID
	    current_cam->UID = cameraInfo.UniqueId;
            // Open the camera
	    if( !PvCameraOpen(current_cam->UID, ePvAccessMaster, &(current_cam->Handle)) )
	    {
            // Debug
            cout << "Camera opened succesfully" << endl;

            // Get the image size of every capture
            PvAttrUint32Get(current_cam->Handle, "TotalBytesPerFrame", &frameSize);

            // Allocate a buffer to store the image
            memset(&current_cam->Frame, 0, sizeof(tPvFrame));
            current_cam->Frame.ImageBufferSize = frameSize;
            current_cam->Frame.ImageBuffer = new char[frameSize];

            // Set maximum camera parameters - camera specific
	    // Code will generate an input window from the center with the size you want
	    int center_x = max_capture_width / 2;
      	    int center_y = max_capture_height / 2;

	    // Set the manta camera parameters to get wanted frame size retrieved
   	    PvAttrUint32Set(current_cam->Handle, "RegionX", center_x - (desired_width / 2) );
	    PvAttrUint32Set(current_cam->Handle, "RegionY", center_y - (desired_height / 2));
	    PvAttrUint32Set(current_cam->Handle, "Width", desired_width);
	    PvAttrUint32Set(current_cam->Handle, "Height", desired_height);

	    // Start the camera
	    PvCaptureStart(current_cam->Handle);

    	    // Set the camera to capture continuously
	    PvAttrEnumSet(current_cam->Handle, "AcquisitionMode", "Continuous");
   	    PvCommandRun(current_cam->Handle, "AcquisitionStart");
	    PvAttrEnumSet(current_cam->Handle, "FrameStartTriggerMode", "Freerun");
	}else{
            cout << "Opening camera error" << endl;
	}
	}else{
    	    cout << "Camera not found or opened unsuccesfully" << endl;
	}
    }else{
        // State that we did not succeed in initializing the API
        cout << "Failed to initialise the camera API" << endl;
    }
}

int main(int argc, char* argv[])
{
	tCamera		myCamera;
	tPvErr		Errcode;

	if( argc == 1 ){
		cout << "This script will stream data from an AVT MANTA GigE camera using OpenCV C++ style interface." << endl;
        	cout << "AVT_manta_camera.exe <resolution width> <resolution heigth>" << endl;
		return 0;
	}

	// Be sure to move the windows to correct location
	// This is done to ensure that the window takes as much of the screen as possible, but can be commented out
	namedWindow("View window",1);
	moveWindow("View window", 50, 50);

	// Initialize the camera API and perform some checks
	int max_capture_width = 1624;
	int max_capture_height = 1234;
	int desired_width = atoi(argv[1]);
	int desired_height = atoi(argv[2]);
	initialize_camera(&myCamera, max_capture_width, max_capture_height, desired_width, desired_height);

    // Create infinite loop - break out when condition is met
    // This is done for trigering the camera capture
    for(;;)
    {
        if(!PvCaptureQueueFrame(myCamera.Handle, &(myCamera.Frame), NULL))
        {
            while(PvCaptureWaitForFrameDone(myCamera.Handle, &(myCamera.Frame), 100) == ePvErrTimeout)
            {
            }

            ////////////////////////////////////////////////////////
            // Here comes the OpenCV functionality for each frame //
            ////////////////////////////////////////////////////////

            // Create an image header (mono image)
            // Push ImageBuffer data into the image matrix
            Mat image = Mat(desired_height, desired_width, CV_8UC1);
            image.data = (uchar *)myCamera.Frame.ImageBuffer;

            // This code should be enabled on a the AVT Proscilla series for color images
            // This works because of the way the frame memory is stored and grabbed and
            // Due to the fact that OpenCV only uses smart pointers to the start of the data
            // REMARK: avoid problems with the correct input grabbing
            //Mat image = Mat(frame_heigth, frame_width, CV_16UC1);
            //image.data = (uchar *)myCamera.Frame.ImageBuffer;
            //Mat color;
            //cvtColor(image,color,CV_BayerBG2BGR);

            // Show the actual frame
            // Wait 10 ms to have an actual window visualisation
            imshow("View window", image);
            waitKey(10);

            // Release the image data
            image.release();
        }
    }

    // Stop the acquisition & free the camera
    Errcode = PvCommandRun(myCamera.Handle, "AcquisitionStop");
    if (Errcode != ePvErrSuccess)
        throw Errcode;

    PvCaptureEnd(myCamera.Handle);
    PvCameraClose(myCamera.Handle);

    return 0;
}

