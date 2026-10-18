//
//  Segment.hpp
//

#include "VectorMatrix.h"
#include "random.h"
#include "NervousSystem.h"


#include <cmath>

using namespace std;

// Parameters
const int N_neurons = 7;              // Number of neurons in units


// Neuron name conventions
const int as = 1;
const int da = 2;
const int db = 3;
const int dd = 4;
const int vd = 5;
const int vb = 6;
const int va = 7;


class Segment {
public:
    Segment(TVector<double> &v);
    
    void InitializeState(RandomState &rs);
    void InitializeOutputs(double o);
    void Step(double StepSize);


    void DumpActState(ofstream &ofs, int skips);
    
    NervousSystem n;

    double t; // Time
};
