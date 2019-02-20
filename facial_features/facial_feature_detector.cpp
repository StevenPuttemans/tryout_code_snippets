#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/opencv.h>
#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/tracking.hpp"

using namespace std;

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    /// Initialize the detection part
    cv::CascadeClassifier model("/data/facialfeatures/model.xml");
    cv::VideoCapture capture(0);
    cv::Mat frame;

    /// Initialize the shape detector
    dlib::image_window win;
    dlib::shape_predictor sp;
    string location = "/data/facialfeatures/shape.dat";
    dlib::deserialize(location) >> sp;

    /// Make sure there is always something to visualize
    dlib::cv_image<dlib::bgr_pixel> visualize;

    while(true){
        capture >> frame;

        std::vector< cv::Rect > dets_OpenCV;
        model.detectMultiScale(frame, dets_OpenCV, 1.2, 3);

        std::vector<dlib::full_object_detection> shapes;
        std::vector<dlib::rectangle> dets_dlib;

        /// Convert the image into the dlib required format
        dlib::cv_image<dlib::bgr_pixel> img_dlib(frame);
        visualize = img_dlib;

        for(size_t i = 0; i < dets_OpenCV.size(); i ++){
            /// Since OpenCV is using the inner face, the model fitting might go wrong, thats why we want to add a 25% padding in each direction
            /// Convert dets_OpenCV to dets_dlib format
            int x1 = dets_OpenCV[i].tl().x - (int)((double)dets_OpenCV[i].width * 0.3);
            int y1 = dets_OpenCV[i].tl().y - (int)((double)dets_OpenCV[i].height * 0.3);
            int x2 = dets_OpenCV[i].br().x + (int)((double)dets_OpenCV[i].width * 0.3);
            int y2 = dets_OpenCV[i].br().y + (int)((double)dets_OpenCV[i].height * 0.3);
            dlib::rectangle current_det(x1, y1, x2, y2);

            /// Find the facial features inside the given rectangle and store them
            dlib::full_object_detection shape = sp(img_dlib, current_det);

            dets_dlib.push_back(current_det);
            shapes.push_back(shape);

        }

        /// Retrieve the points of the shapes and
        win.clear_overlay();
        win.set_image(visualize);
        win.add_overlay(dets_dlib, dlib::rgb_pixel(255,0,0));
        win.add_overlay(dlib::render_face_detections(shapes));

    }
}

// ----------------------------------------------------------------------------------------



