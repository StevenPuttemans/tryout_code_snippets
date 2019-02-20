#include <iostream>
#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/opencv.h>
#include <dlib/image_processing/render_face_detections.h>

#include "opencv2/opencv.hpp"

using namespace std;

// ----------------------------------------------------------------------------------------

template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv){
    cv::VideoCapture capture(0);
    cv::Mat frame;

    net_type net;
    dlib::deserialize("/data/facialfeatures/mmod_human_face_detector.dat") >> net;

    dlib::shape_predictor sp;
    dlib::deserialize("/data/facialfeatures/shape.dat") >> sp;

    dlib::image_window win;
    while(true)
    {
        /// Grab an image from the webcam
        capture >> frame;

        /// Convert the image into the dlib required format
        dlib::cv_image<dlib::bgr_pixel> img(frame);
        dlib::matrix<dlib::rgb_pixel> img2;
        dlib::assign_image(img2,img);

		std::vector<dlib::mmod_rect> dets;
        dets = net(img2);
        win.clear_overlay();
        win.set_image(img2);

        for (auto &d : dets){
            /// For each detection calculate the facial features
            int x1 = d.rect.left() - (int)((double)d.rect.width() * 0.15);
            int y1 = d.rect.top() - (int)((double)d.rect.height() * 0.15);
            int x2 = d.rect.right() + (int)((double)d.rect.width() * 0.15);
            int y2 = d.rect.bottom() + (int)((double)d.rect.height() * 0.15);
            dlib::rectangle larger(x1, y1, x2, y2);
            dlib::full_object_detection shape = sp(img, larger);
            win.add_overlay(larger);
            win.add_overlay(dlib::render_face_detections(shape));
        }
    }
}

