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
//#include "StretchReceptor.h"
//#include "NervousSystemBase.h"

#include "Worm2DSR.h"
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



class Worm2DCE: public Worm2DSR, public WormFR{
    
    public:
    //void Step(double StepSize);
    //void DumpActState(ofstream &ofs, int skips);
    void InitializeState(RandomState &rs);
    Worm2DCE(const json & j, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2DCE(const string & jsonfilename, shared_ptr<const CmdArgs> cmd_ = nullptr);

    void addParsToJson(json & j);
    void writeAct();
    const vector<string> getCellNamesUnit() {return {"DA", "DB", "DD", "VD", "VA", "VB"};}
    const vector<string> getCellNames() {return 
        getCellNamesAll(getCellNamesUnit(), par1.N_units);}
    const vector<string> getDistinctCellNames();

    const string getModelName() {return {"W2DCE"};}

    vector<doubIntParamsHead> getWormParams();
    
    shared_ptr<SRCE> makeSRCE();
    //StretchReceptorCE sr;

    shared_ptr<SRCE> sr_ptr;

    void DumpParams(ofstream &ofs);
    //virtual void initForSimulation(RandomState&);
    void setForward();
    void setBackward();
    //void randomizeNS(RandomState &rs){return;}

    //void setW2DCEpars(const W2DCEpars & W2DCEpars_);//{W2DCEpars1 = W2DCEpars_;}
    //void setW2DCEpars(const W2DCEpars & W2DCEpars_);
    //void setPars(int argc, const char* argv[]);

    //shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]);
    //void setWormPars(const W2Dparameters * w2par_);

    //shared_ptr<const W2Dparameters> setWormPars(shared_ptr<const CmdArgs> cmd);
    void setWormPars(shared_ptr<const CmdArgs> cmd);

    friend class EvolutionCE;
    friend class SRCE;

    protected:
    
    //void Step1();
    //void Step1_old();

    //Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_, 
    //shared_ptr<const CmdArgs> cmd_  = nullptr);

    //Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd = nullptr);

    Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd = nullptr);
    //Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SRCE> sr_ptr_, shared_ptr<const CmdArgs> cmd_);

    //Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_);
    //Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SRCE> sr_ptr_);
    //Worm2DCE(const json & j, shared_ptr<const CmdArgs> cmd_ = nullptr);
    
    void initConst();

    //vector<toFromWeight> makeVentralBodyConn();//{return dummyVec();}
    //vector<toFromWeight> makeDorsalBodyConn();
    vector<toFromWeight> makeVentralMuscleConn();//{return dummyVec();}
    vector<toFromWeight> makeDorsalMuscleConn();//{return dummyVec();}
    //const vector<string> getVMuscNames(){ return {"dum1"};}
    //const vector<string> getDMuscNames(){ return {"dum1"};}
    vector<toFromWeight> makeMuscleConn(const vector<int> & neurons, const vector<double> & NMJ);

    vector<intPair> makeUnitToMusc();
    

    double NMJ_DA= 0, NMJ_DB= 0, NMJ_VD= 0, NMJ_VB= 0, NMJ_VA= 0, NMJ_DD= 0; //EEE
    double AVA_output = 0, AVB_output = 0;
    double AVA_act = 0, AVA_inact = 0, AVB_act = 0, AVB_inact = 0;

    
    const int N_stretchrec = 10;            // N_units // Number of stretch receptors
    const int NmusclePerNU = 4;             // All the way down to 24, in groups of 3 per unit

   
    // Motoneuron name conventions
    const int DA = 1;
    const int DB = 2;
    const int DD = 3;
    const int VD = 4;
    const int VA = 5;
    const int VB = 6;

    double pheno_A_gain = 0, pheno_B_gain = 0; 
    //W2DCEpars W2DCEpars1;
    //shared_ptr<W2DCEpars> W2DCEpars1;

    void makeExternalInputConn();
    void assignExternalInput();
    void setInputSwitcher(const json & j);
    void setInputSwitcher();
        
    void setMuscleInputOrigA();
    void setMuscleInputOrigB();
    //void setMuscleInputOrigC();
    //void setMuscleInputOrigD();

    void setExternalInputOrig();
    void setMuscleInputOrig() {if (baseconsts.debug) setMuscleInputOrigB(); else setMuscleInputOrigA();
    
   /*  if (false){
    std::cout << std::fixed << std::showpoint;
    std::cout << std::setprecision(15);

    m.ventralMuscInputOut();
   
    } */
    
    }
    void setBodyInputOrig();
    //void setMuscleInputOrig2();
    void d11(vector<toFromWeightLD> & vent, int to_musc, int i);
    //vector<toFromWeight> v11(int to_musc, int i);
    //vector<weightentry> v12(int i);
    void v11(vector<toFromWeightLD> & vent, int to_musc, int i);

    //W2DCEpars & W2DCEpars1;
    //string sr_type = "None";
    //bool SR_TRANS_STRETCH, SR_TRANS_CONTRACT, SR_TRANS_ABS, SR_TRANS_NEG;
   
   

};


class WormCE : public EvolvableS, public Worm2DCE
{
public:
    //WormCE(int argc, const char* argv[]);
    
    WormCE(const string & jsonfilename_, const string & filename_, shared_ptr<const CmdArgs> cmd_ = nullptr);
    WormCE(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd_ = nullptr);
    WormCE(shared_ptr<const CmdArgs> cmd_ = nullptr);
    WormCE(const json & j, shared_ptr<const CmdArgs> cmd_ = nullptr);


    WormCE(shared_ptr<const CmdArgs> cmd, const string & genfilename_);
    WormCE(shared_ptr<const CmdArgs> cmd, TVector<double> &pheno);
    WormCE(TVector<double> &pheno, shared_ptr<const CmdArgs> cmd_ = nullptr);


    //WormCE();
    //WormCE(int argc, const char* argv[], TVector<double> &geno);

    //Worm::Worm(TVector<double> &v,double output);

    void InitializeState(RandomState &rs);
    //void HeadStep(double StepSize, double output);
    //void Step(double StepSize, double output);
    //void randomizeNS(RandomState &rs);

    //void DumpBodyState(ofstream &ofs, int skips);
    //void DumpActState(ofstream &ofs, int skips);
    void DumpVoltage(ofstream &ofs, int skips);
    void DumpParams(ofstream &ofs);
    NervousSystem & n;
    void addParsToJson(json & j);
    

    void setPhenoNames();

    template<class T> friend class EvolutionFullWC;
    template<class T> friend class EvolutionFullW;
    template<class T> friend class Evolvable_ptr;
    template<class T> friend class Evolvable_ptrB;

    void setWormPars(shared_ptr<const CmdArgs> cmd)
    //shared_ptr<const W2Dparameters> setWormPars(shared_ptr<const CmdArgs> cmd)
   // shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[])
    {return Worm2DCE::setWormPars(cmd);}


    //void setWormPars(const W2Dparameters * w2par_)
    //{return Worm2DCE::setWormPars(w2par_);}

    
    protected:
    
    
    //WormCE(const string & filename_);

    


    WormCE(shared_ptr<const CmdArgs> cmd, TVector<double> &phengen, bool isPheno);
    
    //WormCE(TVector<double> &phengen, bool isPheno);
    
    WormCE(const json & j, const string & filename_, shared_ptr<const CmdArgs> cmd_ = nullptr);



    //WormCE(shared_ptr<SR> sr_ptr_, shared_ptr<const CmdArgs> cmd_ = nullptr);

    //WormCE(shared_ptr<SRCE> sr_ptr_);
    //WormCE(shared_ptr<const CmdArgs> cmd);
    //WormCE(shared_ptr<SRCE> sr_ptr_, shared_ptr<const CmdArgs> cmd = nullptr);

    //void setParsFromJson(json & j);
    void setParsFromPheno(const TVector<double> &pheno);
    void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
    int getVectSize(){return 17;}
    void setEvolPars(W2Dparameters & w2par_, string evotype_);
    //W2Dparameters & getWormPars() {return W2DCEpars1;}
  
    void addEvolvableToJson(json & j);
    void addFuncableToJson(json & j);

    //shared_ptr<const W2Dparameters> getWormPars();


    //double CoMx();
    //double CoMy();
    //void Curvature(TVector<double> &c);
    //void AngleCurvature(TVector<double> &c);
    //double Orientation();

   
};

//class wormForJson : public Worm<NervousSystem> {};

class WormCESR : public WormCE
{
public:
//WormCESR();
WormCESR(shared_ptr<const CmdArgs> cmd);
WormCESR(shared_ptr<const CmdArgs> cmd, const string & filename_);
WormCESR(const json & j, const string & filename_, shared_ptr<const CmdArgs> cmd);
WormCESR(const string & jsonfilename_, const string & filename_, shared_ptr<const CmdArgs> cmd);
const string getModelName() {return {"W2DCESR"};}

};

