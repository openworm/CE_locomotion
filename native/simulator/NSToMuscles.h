#include "../VectorMatrix.h"
//#include "random.h"
//#include <iostream>
//#include <math.h>
#include <vector>
#include "../utils.h"



class NSToMuscles
{
    public:
    NSToMuscles(int size_, int maxconns_ = 100);
    NSToMuscles(int size_, const vector<toFromWeight> & vec1,  int maxconns_ = 100);

    double getWeight(int from, int to);
    void setWeight(int from, int to, double value); 
    void setWeight(const toFromWeight & w);
    void setWeights(const vector<toFromWeight> & w);
    //int getNumConns(int i) {return numConns(i);}

    friend class Worm2D;

    private:
    TVector<int> numConns;
    TMatrix<weightentry> weights;
    const int size, maxconns;
};