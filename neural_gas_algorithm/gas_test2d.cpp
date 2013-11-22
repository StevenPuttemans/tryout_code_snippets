/****************************************************************************************\
* Neural Gas *
\****************************************************************************************/
#include "mlngas.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp" // 3.0
using namespace cv;
using namespace std;


    Mat draw(const Mat& dist , Mat & img, const Scalar & col,float r=2) {
        for ( int i=0; i<dist.rows; i++ ) {
            circle(img,Point2f(dist.at<Vec4f>(i,0)[0],dist.at<Vec4f>(i,0)[1]),r,col,2);
        }
        return img;
    }

    int main() 
    {
        int wsize = 400;
        Mat dist(300,1,CV_32FC4); // 300 points, 1 per row
        RNG rng(getTickCount());
        // make 4 clusters:
        int cen[4][2] = {{wsize/4,wsize/4},{3*wsize/4,wsize/4},{wsize/4,3*wsize/4},{3*wsize/4,3*wsize/4}};
        int k=0;
        for ( int i=0;i<4;i++ ) 
        {
            Scalar m(cen[i][0]-wsize/8,cen[i][1]-wsize/8);
            Scalar M(cen[i][0]+wsize/8,cen[i][1]+wsize/8);
            for ( int j=0;j<dist.rows/4;j++ ) 
            {
                dist.at<Vec4f>(k,0) = Vec4f(rng.uniform(m[0],M[0]),rng.uniform(m[1],M[1]));
                k++;
            }
        }
        dist = dist.reshape(4,50); // #%@! 2d matrix

        // larger values of lambda0 lead to some kind of sproing / congealing effect
        NeuralGas gas(dist,24,200,5,0.1,0.8,0.01);
        gas.init();        

        for ( int i=0; i<200; i++ ) 
        {
            Mat img(wsize,wsize,CV_8UC3);  
            draw(dist,img,Scalar(255));
            vector<NeuralGas::GasNode> nodes;
            gas.get_nodes(nodes);
            for ( int j=0;j<nodes.size(); j++ ) 
            {
                circle(img,Point2f(nodes[j].ref_vector[0],nodes[j].ref_vector[1]),2,Scalar(0,100,0),2);
                circle(img,Point2f(nodes[j].ref_vector[0],nodes[j].ref_vector[1]),30,Scalar(0,100,0),1);
            }
            circle(img,Point2f(gas.get_bmu().ref_vector[0],gas.get_bmu().ref_vector[1]),5,Scalar(0,200,200),2);
            circle(img,Point2f(gas.get_smu().ref_vector[0],gas.get_smu().ref_vector[1]),5,Scalar(0,0,200),2);
            circle(img,Point2f(gas.get_wmu().ref_vector[0],gas.get_wmu().ref_vector[1]),5,Scalar(200,0,200),2);
            imshow("gas",img);
            if ( waitKey(100)==27 )
                break;

            if ( ! gas.train() )
                break;
        }
        vector<NeuralGas::GasNode> nodes;
        gas.get_nodes(nodes);
        for ( int i=0;i<nodes.size(); i++ ) 
        {
            cerr << format("%3d %3d : %8.3f: %8.3f %8.3f",nodes[i].id,nodes[i].rank,nodes[i].distance,nodes[i].ref_vector[0],nodes[i].ref_vector[1]) << endl;
        }
        waitKey();
	    return 0;
    }
