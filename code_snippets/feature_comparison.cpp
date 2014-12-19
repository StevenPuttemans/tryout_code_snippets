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
// Software for comparing the available feature detectors in OpenCV 2.4.10
// Example see feature_comparison.png
// --------------------------------------------------------------------------------

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

int main()
{
    initModule_nonfree();

    Mat image = imread("/home/spu/Pictures/orchidee1.png");
    Mat image_gray;
    cvtColor(image, image_gray, CV_BGR2GRAY);

    vector< vector<KeyPoint> > all_keypoint_vectors;
    string myStrings[] = {"SIFT", "SURF", "FAST", "STAR", "ORB", "BRISK", "MSER", "HARRIS", "Dense", "SimpleBlob"};
    vector<string> feature_types (myStrings, myStrings + sizeof(myStrings) / sizeof(string));


    for(int i = 0; i < (int)feature_types.size(); i ++){
        cerr << "Feature type " << feature_types[i] << " is being calculated." << endl;
        Ptr<FeatureDetector> detector = FeatureDetector::create(feature_types[i]);
        vector<KeyPoint> these_keypoints;
        detector->detect(image_gray, these_keypoints, Mat());
        all_keypoint_vectors.push_back(these_keypoints);
    }

    Mat picture_plane = Mat::zeros( 2 * image.rows, 5 * image.cols, CV_8UC3 );
    int counter = 0;
    bool quit = false;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 5; j++){
            if (counter == (int)all_keypoint_vectors.size()){
                quit = true;
                break;
            }

            Mat temp = image.clone();
            stringstream temp_text;
            drawKeypoints( image_gray, all_keypoint_vectors[counter], temp, Scalar(255,0,0), DrawMatchesFlags::DEFAULT );
            temp_text << feature_types[counter];
            putText(temp, temp_text.str(), Point(25, 25), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,255,0));
            temp.copyTo( picture_plane( Rect(j*image.cols, i*image.rows, temp.cols, temp.rows) ) );

            imshow("Complete process", picture_plane);
            waitKey(0);

            counter++;
        }

        if(quit){
            break;
        }
    }

    imshow("Complete process", picture_plane);
    waitKey(0);

    return 0;
}

