#ifndef __mlngas_onboard__
#define __mlngas_onboard__

/****************************************************************************************\
*                                 Neural Gas                                             *
\****************************************************************************************/
#include "opencv2/ml/ml.hpp"

namespace cv {
	
class CV_EXPORTS NeuralGas : public CvStatModel {

public:
    struct CV_EXPORTS GasNode {
        int id;
        int rank;
        Scalar ref_vector;
        double distance;
    };

    NeuralGas();
    virtual ~NeuralGas();

    NeuralGas( const Mat _distr, int _total_nodes, int _max_iterations, float _lambda0, float lambdaT, float _epsilon0, float _epsilonT );
    bool init();

    virtual bool train( Scalar _input = Scalar( DBL_MAX ) );
    virtual bool train_auto();
    virtual void clear();

    void get_nodes(std::vector<GasNode>& nodes) const;
    int get_iteration() const;
    int get_max_iterations() const;
    GasNode get_bmu() const;
    GasNode get_smu() const;
    GasNode get_wmu() const;
    Scalar get_input() const;
    int get_total_nodes() const;

    // The networks settings are public so that should be changed dynamically depending the problem.
    float lambda0;
    float lambdaT;
    float epsilon0;
    float epsilonT;

protected:
    std::vector<GasNode> nodes;
    GasNode bmu;
    GasNode smu;
    GasNode wmu;

int total_nodes;
    int iteration, max_iterations;
    Scalar input;
    Mat distribution;

    RNG rng;

private:
    // Static method used for sorting the nodes.
    static bool Compare( const GasNode & node1, const GasNode & node2 ) {
        return ( node1.distance < node2.distance );
    }
};

}


#endif // __mlngas_onboard__

