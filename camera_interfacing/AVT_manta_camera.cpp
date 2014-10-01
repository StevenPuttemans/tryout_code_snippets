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
// - sudo ifconfig [eth0] mtu 9000 - or 9016 ideally if your card supports that
//
// MAKE SURE THE CORRECT INCLUDES ARE MADE IN THE FOLLOWING ORDER
// - /opt/AVT_sdk/lib-pc/x64/4.5/libPvAPI.a
// - /usr/lib/x86_64-linux-gnu/librt.a
// - /usr/lib/x86_64-linux-gnu/libpthread.a
//
// ADD THE PVAPI HEADER TO YOUR PROJECT
//
// LINK THE OPENCV DEPENDENT LIBRARIES USING THE ADDITIONAL LINKER OPTION
//	`pkg-config opencv --libs`
// --------------------------------------------------------------------------------

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

int main(int argc, char* argv[])
{
	tCamera		myCamera;
	tPvCameraInfo	cameraInfo;
	unsigned long	frameSize;
	tPvErr		Errcode;

	int counter = 0;

	if( argc == 1 ){
		cout << "This script will stream data from an AVT MANTA GigE camera using OpenCV C++ style interface." << endl;
        	cout << "AVT_manta_camera.exe <resolution width> <resolution heigth>" << endl;
		return 0;
	}

	// Be sure to move the windows to correct location
	// This is done to ensure that the window takes as much of the screen as possible, but can be commented out
	namedWindow("View window",1); 
	moveWindow("View window", 50, 50);

	// Initialize the API
	if(!PvInitialize())
	{
		// Wait for the response from a camera after the initialization of the driver
		// This is done by checking if camera's are found yet
		////////////////////////////////////////////////////////////
		while(PvCameraCount() == 0)
		{
			waitKey(15);
		}

		// Debugging output to see if camera was found or not
		cout << "Camera found, start initialisation." << endl;

		/////////////////////////////////////////////////////////////
		if ( PvCameraList(&cameraInfo, 1, NULL) == 1)
		{
			// Get the camera ID
			myCamera.UID = cameraInfo.UniqueId;

			// Open the camera
			if( !PvCameraOpen(myCamera.UID, ePvAccessMaster, &(myCamera.Handle)) )
			{
				//debug
				cout << "Camera opened succesfully." << endl;

				// Get the image size of every capture
				PvAttrUint32Get(myCamera.Handle, "TotalBytesPerFrame", &frameSize);

				// Allocate a buffer to store the image
				memset(&myCamera.Frame, 0, sizeof(tPvFrame));
				myCamera.Frame.ImageBufferSize = frameSize;
				myCamera.Frame.ImageBuffer = new char[frameSize];

				// Set maximum camera parameters - camera specific
				// Code will generate an input window from the center with the size you want
				// Therefore you need to fill in the maximal camera resolution
				int max_width = 1624;
				int max_heigth = 1234;

				int center_x = max_width / 2;
				int center_y = max_heigth / 2;

				int frame_width = atoi(argv[1]);
				int frame_heigth = atoi(argv[2]);

				// Set the manta camera parameters to get wanted frame size retrieved
				PvAttrUint32Set(myCamera.Handle, "RegionX", center_x - (frame_width / 2) );
				PvAttrUint32Set(myCamera.Handle, "RegionY", center_y - (frame_heigth / 2));
				PvAttrUint32Set(myCamera.Handle, "Width", frame_width);
				PvAttrUint32Set(myCamera.Handle, "Height", frame_heigth);

				// Start the camera
				PvCaptureStart(myCamera.Handle);

				// Set the camera to capture continuously
				PvAttrEnumSet(myCamera.Handle, "AcquisitionMode", "Continuous");
				PvCommandRun(myCamera.Handle, "AcquisitionStart");
				PvAttrEnumSet(myCamera.Handle, "FrameStartTriggerMode", "Freerun");

				// Create infinite loop - break out when condition is met
				for(;;)
				{
					if(!PvCaptureQueueFrame(myCamera.Handle, &(myCamera.Frame), NULL))
					{
						double time = (double)getTickCount();

						while(PvCaptureWaitForFrameDone(myCamera.Handle, &(myCamera.Frame), 100) == ePvErrTimeout)
						{
						}

						////////////////////////////////////////////////////////
						// Here comes the OpenCV functionality for each frame //
						////////////////////////////////////////////////////////

						// Create an image header (mono image)
						// Push ImageBuffer data into the image matrix
						Mat image = Mat(frame_heigth, frame_width, CV_8UC1);
						image.data = (uchar *)myCamera.Frame.ImageBuffer;

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

				cout << endl << "finished" << endl;
			}
			else
			cout << "open camera error" << endl;
		}
		else
		cout << "camera not found" << endl;
	}
	else
	cout << "failed to initialise the API" << endl;

    return 0;
}
