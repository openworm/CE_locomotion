#pragma once
//#include "../TSearch.h"
//#include "../VectorMatrix.h"
//#include "../Muscles.h"
//#include "../WormBody.h"
//#include "../NervousSystem.h"
//#include <nlohmann/json.hpp>
#include "jsonUtils.h"
#include "../neuromlLocal/NSBaseForW2D.h"

//datawriter->worm2dbase (nervous system and muscle pointers)
//datawriter->worm2dbody (just body plus functions)
//worm2dbase + worm2dbody -> worm2dm (nn ptr + musc ptr + body)
//worm2dm -> worm2d (cc musc + nn ptr plus nn to musc connections )
//worm2dm -> worm2d21m (nn+musc ptrs + body + net21 specifics)
//Worm2D21m + worm2d -> worm2d21 (cc musc + nn ptr + musc connections)
//worm2d21 -> worm21 (cc musc + cc nn + musc connections)


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



class DataWriter{

    public:
   
    
    void writeDataCheck(){ 
        if (basename==".") {cout << "basename not set" << endl; exit(1);}
        writeData();
    }
    
    DataWriter(){datatime=0;prefix="";basename=".";}
    virtual ~DataWriter(){closeAll();}
    

    void incDatatime(double Stepsize_){datatime+=Stepsize_;}
    void setDataskips(double dataskips_){dataskips = dataskips_;}
    void setBasename(string basename_){basename=basename_;}
    void setPrefix(string prefix_){prefix=prefix_;}
   
    void dataReset(){closeAll();writeDataCheck();}
    //void dataReset(){closeAll();}
    void closeAll();
    void InitializeData(string basename_);


    protected:

    bool resetStats(bool & firstcall, size_t & pos, int & tt, string name_);
    virtual void writeData() = 0; //{cout << "write data not implemented!" << endl;}
   

    string getName(string name_);

    vector<bool> isOpen;
    vector<ofstream> ofsvec;
    vector<string> ofnames;
    int dataskips;
    double datatime;
    string basename;
    string prefix;
};



class Worm2Dbody : virtual public DataWriter
{

    public:

    //Worm2Dbody():DataWriter(){}
    double CoMx();
    double CoMy();
    void Curvature(TVector<double> &c);
    double Orientation();
    void AngleCurvature(TVector<double> &c);
    //void DumpBodyState(ofstream &ofs, int skips);
    virtual void InitializeState(RandomState &rs) = 0;
    
    virtual void addParsToJson(json & j);
    double getVelocity();
    
    //virtual ~Worm2Dbody(){}
    virtual void writeBody();
    virtual void writeCurvature();
    
    protected:
    void writeData();
    WormBody b;

};

class Worm2Dbase : virtual public DataWriter
{

public:

virtual void InitializeState(RandomState &rs) = 0;
virtual void initForSimulation() =  0;

void Step(double StepSize_);

virtual void writeAct();
void writeState();
virtual void addParsToJson(json & j);
void writeJsonFile(ofstream & json_out);

NSForW2D & itsNS(){return *n_ptr;}
virtual void DumpParams(ofstream &ofs) = 0;
void DumpNSOrdered();
void DumpVal(string filename_, double val);
virtual double getVelocity() = 0;

virtual ~Worm2Dbase(){
        if (m_ptr) delete m_ptr; 
        if (n_ptr) delete n_ptr;
}

protected:
Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);
Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
void writeData();

virtual const string getModelName() = 0;
virtual vector<doubIntParamsHead> getWormParams() = 0;
virtual void Step1(double StepSize_) = 0;
NSForW2D * const n_ptr;
muscForW2D * m_ptr;
    


double t; // Time
const bool muscForWDconst;
const wormIzqParams par1;
int nn(int neuronNumber, int unitNumber);


};



class Worm2Dm : public Worm2Dbody, public Worm2Dbase
{
    public:

    //virtual void Step(double StepSize, double output) = 0;

    virtual void InitializeState(RandomState &rs) = 0;
    //virtual vector<doubIntParamsHead> getWormParams() = 0;
    
    //virtual void initForSimulation() =  0;
    virtual const vector<string> getCellNames() = 0;
    //virtual const string getModelName() = 0;

    
    //void Step(double StepSize_);
    //void DumpBodyState(ofstream &ofs, int skips);
    //void DumpCurvature(ofstream &ofs, int skips);
    virtual void setMuscleInput(double StepSize) {return;}
    //virtual void DumpActState(ofstream &ofs, int skips);
    //virtual void DumpActStateState(ofstream &ofs, int skips);
    //void DumpVal(ofstream &ofs, int skips, double val);
    //void writeData();
    //virtual void writeAct();
    //void writeState();
    double getVelocity(){return Worm2Dbody::getVelocity();}
    virtual void addParsToJson(json & j);
    //virtual void DumpParams(ofstream &ofs){return;}

    //void writeJsonFile(ofstream & json_out);
    
   
    //void DumpNSOrdered(ofstream &ofs, int skips);
    
    
    //NSForW2D & itsNS(){return *n_ptr;}

   /*  virtual ~Worm2Dm(){
        if (m_ptr) delete m_ptr; 
        if (n_ptr) delete n_ptr;
    } */

    protected:
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
    void writeData();

    //virtual void Step1(double StepSize_) = 0;
    //NSForW2D * const n_ptr;
    //muscForW2D * m_ptr;
    


    //const wormIzqParams par1;
    //double t; // Time
    //const bool muscForWDconst;
    //double StepSize;
    

    //int nn(int neuronNumber, int unitNumber);


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
    //void writeData(){Worm2Dm::writeData();}

    protected:

    
    virtual const vector<string> getVMuscNames() = 0;
    virtual const vector<string> getDMuscNames() = 0;

    //virtual void addExtraParsToJson(json & j) = 0;
    virtual vector<toFromWeight> makeVentralMuscleConn() = 0;
    virtual vector<toFromWeight> makeDorsalMuscleConn() = 0;
    void setUpMuscleConn();
    void makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int mi, int to, TVector<double> & NMJ_Gain);
    void setMuscleInput(double StepSize);
    void setMuscleInputVec(double StepSize);
    Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_);
    void setMuscleInputVent();
    void setMuscleInputDors();

    void setUp();
    Muscles & m;
   
    NSToMuscles vMuscConn, dMuscConn;
    vector<toFromWeight> vMuscConnvec, dMuscConnvec;
    
 
};



