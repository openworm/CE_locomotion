//
//  Worm.hpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "VectorMatrix.h"
#include "random.h"
#include "WormBody.h"
#include "NervousSystem.h"
#include "Muscles.h"

#include <cmath>

#define PI 3.14159265

using namespace std;

// Parameters
const int N_muscles = 24;           // Number of muscles alongside the body
const int N_units = 7;              // Number of neural units
const int N_neuronsperunit = 7;     // Number of neurons in a neural unit

const double T_muscle = 0.1;        // Muscle time constant

const int startingMuscleA = 1;       // XXX
const int NmusclePerNUA = 3;
const int startingMuscleB = 13;       // XXX
const int NmusclePerNUB = 4;

// Neuron name conventions
const int AS = 1;
const int DA = 2;
const int DB = 3;
const int DD = 4;
const int VD = 5;
const int VB = 6;
const int VA = 7;

// Body segment name conventions
const int Head = 1;
const int Tail = N_segments;

class Worm {
public:
    
    Worm(TVector<double> &v);
    
    void InitializeState(RandomState &rs);
    void Step(double StepSize);
    
    void DumpBodyState(ofstream &ofs, int skips);
    void DumpActState(ofstream &ofs, int skips);
    void DumpCurvature(ofstream &ofs, int skips);
    void DumpParams(ofstream &ofs);
    
    void SetAVA(double value) {AVA = value;};
    void SetAVB(double value) {AVB = value;};
    
    double CoMx();
    double CoMy();
    void Curvature(TVector<double> &c);
    double Orientation();
    
    WormBody b;
    Muscles m;
    NervousSystem n;
    
    double t; // Time
    
    // Neuromuscular junctions
    double NMJ_AS, NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD;
    double NMJ_Gain_Map;
    
    TVector<double> NMJ_Gain;
    
    // Command neuron input
    
    double wAVA_DA, wAVA_VA;
    double wAVB_DB, wAVB_VB;
    double AVA, AVB;
};
