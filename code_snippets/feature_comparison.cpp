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
    // More info: http://docs.opencv.org/modules/features2d/doc/common_interfaces_of_feature_detectors.html?highlight=featuredetector#featuredetector-create
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

