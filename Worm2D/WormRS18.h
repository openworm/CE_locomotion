//
//  Worm.hpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//
#pragma once
//#include "VectorMatrix.h"
//#include "random.h"
//#include "WormBody.h"
//#include "NervousSystem.h"
//#include "Muscles.h"
#include "StretchReceptor18.h"
#include "Worm2D.h"

#include <cmath>

#define PI 3.14159265

using namespace std;

struct RS18Macros{
bool headsr;
bool vncsr;
};



class Worm18 : public Worm2D , public EvolvableS{
public:

    Worm18(shared_ptr<const CmdArgs> cmd);
    Worm18(vector<double> v, double output, shared_ptr<const CmdArgs> cmd);
    Worm18(const TVector<double> &v, double output, shared_ptr<const CmdArgs> cmd);
    //Worm18(bool dummy);
    Worm18(const TVector<double> &v, shared_ptr<const CmdArgs> cmd = nullptr);
    Worm18(const string & filename_,shared_ptr<const CmdArgs> cmd);
 
    void InitializeState(RandomState &rs);
    //void HeadStep(double StepSize, double output);
  
    //void writeData(){Worm2D::writeData();}
    void writeAct();
    //void DumpActState(ofstream &ofs, int skips);
    void DumpVoltage(ofstream &ofs, int skips);
    void DumpParams(ofstream &ofs);
    void initForSimulation(RandomState& rs){setRs18output(1);}
    void setRs18output(bool val_){rs18output=val_;}
    void addParsToJson(json & j);
    void SetParameters(const TVector<double> & v);

    friend class WormCO18;
    
    NervousSystem & n;


    const int DB = 1;
    const int DD = 2;
    const int VBA = 3;
    const int VDA = 4;
    const int VBP = 5;
    const int VDP = 6;
    

    // Neuron name conventions
    const int SMDD = 37;
    const int RMDD = 38;
    const int SMDV = 39;
    const int RMDV = 40;

    protected:
    void Step1(); 
    virtual void preNStep();
    void postNStep();

    //vector<toFromWeight> makeMuscleConn(vector<int> neurons, vector<double> NMJ);
    //void addParsToJson(json & j);
    
    virtual const vector<string> getSectionNames(); //{return vector<string>(par1.N_size, "vnc");}

    const vector<string> getDistinctCellNames();
    const vector<string> getCellNames();
    vector<toFromWeight> makeVentralMuscleConn();
    vector<toFromWeight> makeDorsalMuscleConn();
    const vector<string> getVMuscNames(){ return {"dum1"};}
    const vector<string> getDMuscNames(){ return {"dum1"};}
    const string getModelName() {return {"RS18"};}
    
    //Params<double> getWormParams();
    //const vector<string> getHeadCellNames() 
    //{return {"SMDD", "RMDD", "SMDV", "RMDV"};}
    
    void setMuscleInputOrig();
    void setMuscleInputOrigDorsal();
    void setMuscleInputOrigVentral(); 

    vector<doubIntParamsHead> getWormParams();

    //friend class EvolutionRS18;
    
    template<class T> friend class EvolutionFullW;
    //void setWormPars(shared_ptr<const CmdArgs> cmd_){Worm2D::setWormPars(cmd_);}

    template<class T> friend class Evolvable_ptr;
    template<class T> friend class Evolvable_ptrB;

    void initConst();
    
    void addFuncableToJson(json & j);
    void addEvolvableToJson(json & j);
    void setEvolPars(W2Dparameters & w2par_, string evotype_){}
    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
    void setParsFromPheno(const TVector<double> &pheno);
    int getVectSize(){return 30;}

    //NervousSystem & n;
    StretchReceptor18 sr;
    //NervousSystem h;
    shared_ptr<SR18> sr_ptr;

    void setPhenoNames();
   
    // Neuromuscular junctions
    double NMJ_DB, NMJ_VBa, NMJ_VBp, NMJ_DD, NMJ_VDa, NMJ_VDp;
    double NMJ_SMDD, NMJ_RMDD, NMJ_SMDV, NMJ_RMDV;
    
    //double NMJ_Gain_Map;
    
    //TVector<double> NMJ_Gain;
    
    // Head oscillator
    //double dorsalinput1, ventralinput1, dorsalinput2, ventralinput2;
    //double headFreq, headDelay, headGain, headBias;

// Parameters
     //int N_muscles = 24;           // Number of muscles alongside the body
     //int N_units = 6;              // Number of neural units
     //int N_neuronsperunit = 6;     // Number of neurons in a neural unit

    const int N_stretchrec = 6;         // N_units + 1 // Number of stretch receptors
    //
     //double T_muscle = 0.1;        // Muscle time ant

    const int HeadMotorNeuronMuscles = 6;  // Head motorneurons innervate first 8 muscles (temporarily first 6)
    const int VNCMuscleStart = 7;           // VNC motorneurons innervate starting from 7th muscle
    const int NmusclePerNU = 3;             // All the way down to 24, in groups of 3 per unit

    

    // Neuron name conventions
    

    //const int Head = 1;
    //const int Tail = N_segments;
    //bool doOrigSRInput18 = true;
    //bool doOrigMuscInput18 = true;
    bool rs18output;
    const RS18Macros rS18Macros;
    RS18Macros setMacros();

     
};


