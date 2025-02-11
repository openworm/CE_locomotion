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
#include "neuroml/NervousSystem2D.h"
//#include "NervousSystem.h"
#include "Muscles.h"
#include "StretchReceptor.h"
#include "NervousSystemBase.h"

#include <cmath>

#define PI 3.14159265
//#define NERVOUSSYSTEM NervousSystem2D

// Stretch-Receptor Transdusction form
// Altogether there are 8 forms this can take, depending on which of the first three are defined and then the second one.
// Note: If none of these three are defined, then the LINEAR form is the default.
//#define SR_TRANS_STRETCH
///#define SR_TRANS_CONTRACT
//#define SR_TRANS_ABS
// If NEG is not defined, then the transformation has a positive relationship.
//#define SR_TRANS_NEG

using namespace std;

// Parameters
const int N_muscles = 24;               // Number of muscles alongside the body
const int N_units = 10;                 // Number of neural units in VNC
const int N_neuronsperunit = 6;         // Number of neurons in a VNC neural unit (6 neurons)
//const int H_neuronsperunit = 3;         // Half for DV symmetry

const int N_stretchrec = 10;            // N_units // Number of stretch receptors
//
const double T_muscle = 0.1;            // Muscle time constant

const int NmusclePerNU = 4;             // All the way down to 24, in groups of 3 per unit

// Motoneuron name conventions
const int DA = 1;
const int DB = 2;
const int DD = 3;
const int VD = 4;
const int VA = 5;
const int VB = 6;

// Body segment name conventions
const int Head = 1;
const int Tail = N_segments;



int nn(int neuronNumber, int unitNumber);
NervousSystemBase* makeNervousSystem();
bool checkNervousSystemForJson();


class Worm {
public:

    Worm(TVector<double> &v, double output);
    //Worm::Worm(TVector<double> &v,double output);
    void InitializeState(RandomState &rs);
    void HeadStep(double StepSize, double output);
    void Step(double StepSize, double output);

    void DumpBodyState(ofstream &ofs, int skips);
    void DumpActState(ofstream &ofs, int skips);
    void DumpVoltage(ofstream &ofs, int skips);
    void DumpParams(ofstream &ofs);
    void DumpCurvature(ofstream &ofs, int skips);
    
    ~Worm(){if (n_ptr!=nullptr) delete n_ptr;}
    
    double CoMx();
    double CoMy();
    void Curvature(TVector<double> &c);
    void AngleCurvature(TVector<double> &c);
    double Orientation();

    WormBody b;
    Muscles m;
    NervousSystemBase *n_ptr;
    StretchReceptor sr;

    double t; // Time

    // Neuromuscular junctions
    double NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD; //EEE
    double AVA_act, AVA_inact, AVB_act, AVB_inact;
    double AVA_output, AVB_output;

};

//class wormForJson : public Worm<NervousSystem> {};


