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

#include "Worm2D.h"
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


class Worm2DCE: public  Worm2D{
    
    public:
    //void Step(double StepSize);
    //void DumpActState(ofstream &ofs, int skips);
    void InitializeState(RandomState &rs);
    Worm2DCE(json & j);
    void addParsToJson(json & j);
    void writeAct();

    const vector<string> getCellNames() {return 
        getCellNamesAll( {"DA", "DB", "DD", "VD", "VA", "VB"}, par1.N_units);}
    
    const string getModelName() {return {"CE"};}

    vector<doubIntParamsHead> getWormParams();
    StretchReceptorCE sr;
    void DumpParams(ofstream &ofs);
    virtual void initForSimulation(RandomState&);

    friend class EvolutionCE;

    protected:
    //void Step1();
    void Step1();
    Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_)
    :Worm2Dm(par1_, n_ptr_, new Muscles),Worm2D(par1_,0){}

    vector<toFromWeight> makeVentralMuscleConn(){return dummyVec();}
    vector<toFromWeight> makeDorsalMuscleConn(){return dummyVec();}
    const vector<string> getVMuscNames(){ return {"dum1"};}
    const vector<string> getDMuscNames(){ return {"dum1"};}

   

    double NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD; //EEE
    double AVA_output, AVB_output;
    double AVA_act, AVA_inact, AVB_act, AVB_inact;

    
    const int N_stretchrec = 10;            // N_units // Number of stretch receptors
    const int NmusclePerNU = 4;             // All the way down to 24, in groups of 3 per unit

   
    // Motoneuron name conventions
    const int DA = 1;
    const int DB = 2;
    const int DD = 3;
    const int VD = 4;
    const int VA = 5;
    const int VB = 6;


   
};


