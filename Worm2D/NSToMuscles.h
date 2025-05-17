#include "VectorMatrix.h"
#include "random.h"
#include <iostream>
#include <math.h>
#include <vector>


struct fromWeight {int from; double weight;};

class NSToMuscles
{
    public:
    NSToMuscles(int size_, int maxconns_);

    double getWeight(int from, int to);
    void setWeight(int from, int to, double value);
    void setWeights(vector<int> from, vector<int> to, vector<double> value);


    private:
    TVector<int> numConns;
    TMatrix<fromWeight> weights;
    const int size, maxconns;
};