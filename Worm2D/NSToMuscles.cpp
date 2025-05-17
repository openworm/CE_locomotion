#include "VectorMatrix.h"
#include "random.h"
#include <iostream>
#include <math.h>
#include "NSToMuscles.h"


NSToMuscles::NSToMuscles(int size_, int maxconns_):size(size_),maxconns(maxconns_)
{
    numConns.SetBounds(1,size);
    for (int i = 1; i <= size; i++) numConns[i] = 0;
    weights.SetBounds(1, size,1, maxconns);
}


double NSToMuscles::getWeight(int from, int to)
{
    for (int i = 1; i <= numConns(to); i++) {
        if (weights[to][i].from == from)
            return weights[to][i].weight;
    }
    return 0.0;
}

void NSToMuscles::setWeights(vector<int> from, vector<int> to, vector<double> value)
{
    

}

void NSToMuscles::setWeight(int from, int to, double value)
{
    // If the connection is already stored, just change its value
    for (int i = 1; i <= numConns[to]; i++)
        if (weights[to][i].from == from) {
            weights[to][i].weight = value;
            return;
        };
    // Otherwise, make sure we have room for an additional connection ...
    if (numConns[to] == maxconns) {
        cerr << "Maximum synapses (" << maxconns << ") exceeded for muscle " << to << endl;
        exit(EXIT_FAILURE);
    }
    // ... and store it
    int i = ++numConns[to];
    weights[to][i].from = from;
    weights[to][i].weight = value;
}