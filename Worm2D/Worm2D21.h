//
//  Worm.hpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

//#include "VectorMatrix.h"
//#include "random.h"
//#include "WormBody21.h"
//#include "NervousSystem.h"
//#include "Muscles.h"
#include "Worm2D.h"

#include <cmath>

#define PI 3.14159265

using namespace std;

// Parameters
//const int N_muscles = 24;           // Number of muscles alongside the body
//const int N_units = 7;              // Number of neural units
//const int N_neuronsperunit = 7;     // Number of neurons in a neural unit

//const double T_muscle = 0.1;        // Muscle time constant



// Body segment name conventions
//const int Head = 1;
//const int Tail = N_segments;

class Worm2D21m : virtual public Worm2Dm {
public:
    
    //Worm21(TVector<double> &v);
   
    Worm2D21m();
    void SetAVA(double value) {AVA = value;};
    void SetAVB(double value) {AVB = value;};
    void initForSimulation();
    void InitializeState(RandomState &rs);
    //void DumpActState(ofstream &ofs, int skips);
    //void DumpActStateState(ofstream &ofs, int skips);
    void DumpParams(ofstream &ofs);

    //void Step();
    //void Step(double StepSize, double output){return Step(StepSize);}
    

protected:

    void Step1(double StepSize);
    void addParsToJson(json & j);

    
    const vector<string> getCellNames() {return {"AS", "DA", "DB", "DD", "VD", "VB", "VA" };}

    vector<doubIntParamsHead> getWormParams();
    double wAVA_DA, wAVA_VA;
    double wAVB_DB, wAVB_VB;
    double AVA, AVB;

    const int AS = 1;
    const int DA = 2;
    const int DB = 3;
    const int DD = 4;
    const int VD = 5;
    const int VB = 6;
    const int VA = 7;

};

class Worm2D21 : public Worm2D21m, public Worm2D {
public:
    
    //Worm21(TVector<double> &v);
    Worm2D21(TVector<double> &v);
    Worm2D21(json & j);
    
    void InitializeState(RandomState &rs);
   
   
  
    //void DumpCurvature(ofstream &ofs, int skips);
    
    
    
    
    
    //NervousSystem & n;

    protected:

    Worm2D21(wormIzqParams par1_, NSForW2D * n_ptr_)
    :Worm2Dm(par1_, n_ptr_, new Muscles),Worm2D(par1_,0),Worm2D21m(){}

    void addParsToJson(json & j);

    

    void setMuscleInputOrig(double);
    vector<toFromWeight> makeVentralMuscleConn();
    vector<toFromWeight> makeDorsalMuscleConn();
    //void makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    //vector<int> neurons, vector<double> NMJs, int mi, int to);
    vector<toFromWeight> makeMuscleConn(vector<int> dorsalNeurons, vector<double> dorsalNMJ);

    

    const vector<string> getVMuscNames() {return {"MV1","MV2","MV3","MV4", 
        "MV1","MV2","MV3",  "MV1","MV2","MV3", "MV1","MV2","MV3", "MV1","MV2","MV3", "MV1","MV2","MV3", "MV4",
    "MV1","MV2","MV3", "MV4"};}
    const vector<string> getDMuscNames() {return {"MD1","MD2","MD3","MD4", 
        "MD1","MD2","MD3",  "MD1","MD2","MD3", "MD1","MD2","MD3", "MD1","MD2","MD3", "MD1","MD2","MD3", "MD4",
    "MD1","MD2","MD3", "MD4"};}
    
        
    vector<doubIntParamsHead> getWormParams();

    friend class Evolution21;
    
    protected:

    // Neuromuscular junctions
    double NMJ_AS, NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD;
    double NMJ_Gain_Map;
    
    TVector<double> NMJ_Gain;
    
    // Command neuron input
    

    const int startingMuscleA = 1;       // XXX
    const int NmusclePerNUA = 3;
    const int startingMuscleB = 13;       // XXX
    const int NmusclePerNUB = 4;
    
    
    // Neuron name conventions
    

    
    //const int Head = 1;
    //const int Tail = N_segments;
};
