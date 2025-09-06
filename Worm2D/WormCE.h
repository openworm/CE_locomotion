//
//  Worm.hpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//
//#pragma once
//#include "VectorMatrix.h"
//#include "random.h"
//#include "WormBody.h"
//#include "../neuromlLocal/c302NervousSystem.h"
//#include "NervousSystem.h"
//#include "Muscles.h"
#include "StretchReceptorCE.h"
//#include "NervousSystemBase.h"

#include "Worm2DCE.h"
#include "jsonUtils.h"


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
//const int N_muscles = 24;               // Number of muscles alongside the body
//const int N_units = 10;                 // Number of neural units in VNC
//const int N_neuronsperunit = 6;         // Number of neurons in a VNC neural unit (6 neurons)
//const int H_neuronsperunit = 3;         // Half for DV symmetry
//const double T_muscle = 0.1;            // Muscle time constant




// Body segment name conventions
//const int Head = 1;
//const int Tail = N_segments;



//int nn(int neuronNumber, int unitNumber);


//NervousSystemBase* makeNervousSystem();

//bool checkNervousSystemForJson();

using json = nlohmann::json;


class WormCE : public Worm2DCE , public EvolvableS
{
public:

    WormCE(TVector<double> &pheno);
    WormCE(const string & filename_);
    WormCE(TVector<double> &phengen, bool isPheno);
    WormCE();

    //Worm::Worm(TVector<double> &v,double output);

    void InitializeState(RandomState &rs);
    //void HeadStep(double StepSize, double output);
    //void Step(double StepSize, double output);

    //void DumpBodyState(ofstream &ofs, int skips);
    //void DumpActState(ofstream &ofs, int skips);
    void DumpVoltage(ofstream &ofs, int skips);
    void DumpParams(ofstream &ofs);
    NervousSystem & n;
    void addParsToJson(json & j);

    protected:
    void setParsFromPheno(TVector<double> &pheno);
    void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
    int getVectSize(){return 17;}
    void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_){}

    //double CoMx();
    //double CoMy();
    //void Curvature(TVector<double> &c);
    //void AngleCurvature(TVector<double> &c);
    //double Orientation();

    

   
};

//class wormForJson : public Worm<NervousSystem> {};


