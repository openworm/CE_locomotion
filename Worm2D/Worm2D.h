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



class Worm2D {
    
    public:
    //virtual void InitializeState(RandomState &rs) = 0;
    //virtual void DumpBodyState(ofstream &ofs, int skips) = 0;
    //virtual void DumpCurvature(ofstream &ofs, int skips) = 0;

    virtual void DumpActState(ofstream &ofs, int skips) = 0;
    virtual void Step(double StepSize, double output) = 0;
    virtual void InitializeState(RandomState &rs) = 0;
    virtual vector<doubIntParamsHead> getWormParams() = 0;


    void Step(double StepSize) {Step(StepSize,1);}

    void DumpVal(ofstream &ofs, int skips, double val);
    void DumpBodyState(ofstream &ofs, int skips);
    void DumpCurvature(ofstream &ofs, int skips);
    virtual void addParsToJson(json & j) = 0;
    void writeJsonFile(ofstream & json_out);
    virtual void DumpParams(ofstream &ofs) = 0;
    double getVelocity();
    void DumpNSOrdered(ofstream &ofs, int skips);
    virtual void initForSimulation() =  0;


    
    double CoMx();
    double CoMy();
    void Curvature(TVector<double> &c);
    double Orientation();
    void AngleCurvature(TVector<double> &c);

    virtual ~Worm2D(){if (n_ptr) delete n_ptr;}
    NSForW2D & itsNS(){return *n_ptr;}


    protected:

    virtual const vector<string> getCellNames() = 0;
    
    //virtual void addExtraParsToJson(json & j) = 0;
    
    
    Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_);
    int nn(int neuronNumber, int unitNumber);
    void setUp();
    Muscles m;
    WormBody b;
    NSForW2D * const n_ptr;
    const wormIzqParams par1;
    double t; // Time

    const int Head = 1;
    const int Tail = N_segments;
};




class WormIzq //: public Worm2D
{
public:
   
    
    //void addParsToJson(json & j);
    //void writeJsonFile(ofstream & json_out);
    
    
    
   // ~WormIzq(){if (n_ptr) delete n_ptr;}

    NervousSystem & n;

    protected:
    //WormIzq(wormIzqParams par1);
    WormIzq(NervousSystem & n_):n(n_){}

    
    private:
   
    
    
   
};