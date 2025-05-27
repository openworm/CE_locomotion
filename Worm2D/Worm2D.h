#pragma once
//#include "../TSearch.h"
//#include "../VectorMatrix.h"
//#include "../Muscles.h"
//#include "../WormBody.h"
//#include "../NervousSystem.h"
//#include <nlohmann/json.hpp>
#include "jsonUtils.h"
#include "../neuromlLocal/NSBaseForW2D.h"


//using json = nlohmann::json;

#define PI 3.14159265

struct wormIzqParams
{
    int N_neuronsperunit;
    int N_muscles;
    double T_muscle;
    int N_units;
    int N_size;

    const doubIntParamsHead getParams() const
    {
        doubIntParamsHead var1;
        var1.parDoub.head = "Worm";
        var1.parInt.head = "Worm";
        var1.parDoub.names = {"T_muscle"};
        var1.parDoub.vals = {T_muscle};
        var1.parInt.names = {"N_neuronsperunit", "N_muscles", "N_units", "N_size"};
        var1.parInt.vals = {N_neuronsperunit, N_muscles, N_units, N_size};
        return var1;
    }

};

vector<toFromWeight> dummyVec();

class Worm2Dm 
{
    public:

    virtual void DumpActState(ofstream &ofs, int skips);
    virtual void DumpActStateState(ofstream &ofs, int skips);

    virtual void Step(double StepSize, double output) = 0;
    virtual void InitializeState(RandomState &rs) = 0;
    virtual vector<doubIntParamsHead> getWormParams() = 0;
    virtual void addParsToJson(json & j) = 0;
    virtual void DumpParams(ofstream &ofs) = 0;
    virtual void initForSimulation() =  0;
    virtual void setMuscleInput(double StepSize) {return;}
    virtual const vector<string> getCellNames() = 0;


    void Step(double StepSize) {Step(StepSize,1);}

    void DumpVal(ofstream &ofs, int skips, double val);
    void DumpBodyState(ofstream &ofs, int skips);
    void DumpCurvature(ofstream &ofs, int skips);
    
    void writeJsonFile(ofstream & json_out);
    
    double getVelocity();
    void DumpNSOrdered(ofstream &ofs, int skips);
    
    
    double CoMx();
    double CoMy();
    void Curvature(TVector<double> &c);
    double Orientation();
    void AngleCurvature(TVector<double> &c);
    NSForW2D & itsNS(){return *n_ptr;}
    virtual ~Worm2Dm(){if (m_ptr) delete m_ptr; if (n_ptr) delete n_ptr;}

    protected:
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);

    NSForW2D * const n_ptr;
    muscForW2D * m_ptr;
    WormBody b;
    const wormIzqParams par1;
    double t; // Time
    const bool muscForWDconst;
    
    

    int nn(int neuronNumber, int unitNumber);

    const int Head = 1;
    const int Tail = N_segments;

};

class Worm2D : virtual public Worm2Dm
{
    
    public:
    //virtual void InitializeState(RandomState &rs) = 0;
    //virtual void DumpBodyState(ofstream &ofs, int skips) = 0;
    //virtual void DumpCurvature(ofstream &ofs, int skips) = 0;

    
    void addParsToJson(json & j);

    //virtual ~Worm2D(){if (n_ptr) delete n_ptr;}
    //NSForW2D & itsNS(){return *n_ptr;}

    void InitializeState(RandomState &rs);
   

    protected:

    
    virtual const vector<string> getVMuscNames() = 0;
    virtual const vector<string> getDMuscNames() = 0;

    //virtual void addExtraParsToJson(json & j) = 0;
    virtual vector<toFromWeight> makeVentralMuscleConn() = 0;
    virtual vector<toFromWeight> makeDorsalMuscleConn() = 0;
    void setUpMuscleConn();

    void setMuscleInput(double StepSize);
    Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_);
    
    void setUp();
    Muscles & m;
   
    NSToMuscles vMuscConn, dMuscConn;
    
 
};



