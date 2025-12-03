#pragma once
//#include "../TSearch.h"
//#include "../VectorMatrix.h"
//#include "../Muscles.h"
//#include "../WormBody.h"
//#include "../NervousSystem.h"
//#include <nlohmann/json.hpp>
//#include "jsonUtils.h"
#include "../neuromlLocal/NSBaseForW2D.h"
#include "Evolvable.h"


//datawriter->worm2dbase (nervous system and muscle pointers)
//datawriter->worm2dbody (just body plus functions)
//worm2dbase + worm2dbody -> worm2dm (nn ptr + musc ptr + body)
//worm2dm -> worm2d (cc musc + nn ptr plus nn to musc connections )
//worm2dm -> worm2d21m (nn+musc ptrs + body + net21 specifics)
//Worm2D21m + worm2d -> worm2d21 (cc musc + nn ptr + musc connections)
//worm2d21 -> worm21 (cc musc + cc nn + musc connections)

extern string main_directoryname, main_modelname;
int nn1(int neuronNumber, int unitNumber, int N_neuronsperunit);
void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int mi, int to, TVector<double> & NMJ_Gain, int);
//string main_directoryname;
//string main_modelname;

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, int unit, int to_muscle, 
    const vector<double> & NMJ_Gain, int N_neuronsperunit);




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
        if (basename==".") {cout << "basename not set" << endl; throw std::exception();}
        writeData();
    }
    
    DataWriter()//:doFirstCall(true)
    {datatime=0;
    prefix="";
    basename=".";
    isOpen.clear();
    ofsvec.clear();
    ofnames.clear();
    tts.clear();
    }

    virtual ~DataWriter(){closeAll();}
    

    void incDatatime(double Stepsize_){datatime+=Stepsize_;}
    void setDataskips(double dataskips_){dataskips = dataskips_;}
    void setBasename(string basename_){basename=basename_;}
    void setPrefix(string prefix_){prefix=prefix_;}
    void setPrefix(){prefix=getModelName();}

    void dataReset();
    
    //void dataReset(){closeAll();}
    void closeAll();
    void InitializeData(string basename_);

    

    protected:
    virtual const string getModelName() = 0;
    size_t getPos(string name_);
    virtual void writeData() = 0; //{cout << "write data not implemented!" << endl;}

    //bool resetStats(bool & firstcall, size_t & pos, int & tt, string name_);
   
   
    string getName(string name_);

    //const bool doFirstCall;
    vector<bool> isOpen;
    vector<ofstream> ofsvec;
    vector<string> ofnames;
    vector<int> tts;

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
    double PositionX(){return b.X(Head)*100.0;} //change to cm
    double PositionY(){return b.Y(Head)*100.0;}
    void shiftX(double shiftdist_);
    void shiftY(double shiftdist_);
    void zeroX();
    void zeroY();
   
    double headDistanceToCenter();

    void rotateBody(double theta);

    virtual void addParsToJson(json & j);
    double getVelocity();
    
    //virtual ~Worm2Dbody(){}
    virtual void writeBody();
    virtual void writeCurvature();
    WormBody b;

    protected:
    void writeData();
    

    bool first_call = true;
    double xtp = 0, ytp = 0;

};

class Worm2Dbase : virtual public DataWriter
{

public:

virtual void InitializeState(RandomState &rs) = 0;
virtual void initForSimulation(RandomState &) {return;}


void Step(double StepSize_);
void Step();
virtual void setStepSize(double val_){settedStepSize=val_;}


vector<double> readPhenotype();
virtual void writeAct();
void writeState();
virtual void addParsToJson(json & j);
void writeJsonFile(ofstream & json_out);

NSForW2D & itsNS(){return *n_ptr;}
virtual void DumpParams(ofstream &ofs) {return;}
void DumpNSOrdered();
void DumpVal(string filename_, double val);
virtual double getVelocity() = 0;
const wormIzqParams par1;
int nn(int neuronNumber, int unitNumber) const;


virtual ~Worm2Dbase(){
        if (m_ptr) delete m_ptr; 
        if (n_ptr) delete n_ptr;
}

void setTime(double t_){t=t_;}
const double & itsStepSize() const {return settedStepSize;}
void incSimTimes();

//virtual shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]) {return nullptr;}
//virtual void setWormPars(const W2Dparameters * w2par_) {assert(0);}

//virtual shared_ptr<const W2Dparameters> setWormPars(shared_ptr<const CmdArgs> cmd) {return nullptr;}

virtual void setWormPars(shared_ptr<const CmdArgs> cmd) 
{W2Dbaseparameters1b->setPars(cmd);}

//shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;
shared_ptr<W2Dparameters> W2Dbaseparameters1b;

void zeroAllInputs(){
    for (int i=0;i<par1.N_size;i++)
    n_ptr->SetNeuronExternalInput(i+1, 0);
}

protected:
//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);

Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, shared_ptr<W2Dparameters>);

//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc, 
//    shared_ptr<W2Dbaseparameters> w2dpar_);

void writeData();
virtual void setPhenoNames() {return;}

virtual vector<doubIntParamsHead> getWormParams() {
    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    parvec.push_back(var1);
    return parvec;}


    virtual void Step1() = 0;
NSForW2D * const n_ptr = nullptr;
muscForW2D * m_ptr = nullptr;
    
vector<string> phenoNames;
vector<int> phenoNamesNums;

//vector<double> sjdkdsdjddssdsloe;


void addPhenoName(string name, int k);

double t; // Time


double settedStepSize;

void makeExternalInputConnFromJson(json & j);
virtual void makeExternalInputConn(){return;}
vector<toFromWeight> externalInputConn;
vector<double> externalInputs;
//vector<double> sjdkdsdjddssdsloe;
//double sjdkdsdjddssdsloe;
void setExternalInput();
virtual void assignExternalInput(){fill(externalInputs.begin(), externalInputs.end(), 0);}

vector<toFromWeight> NSInputConn, NSOutputConn;
void incInputFromNS(NSForW2D & ns_);
void incOutputToNS(Worm2Dbase & ns_);
virtual void makeNSInputConn(){return;}
virtual void makeNSOutputConn(){return;}





};



class Worm2Dm : public Worm2Dbody, public Worm2Dbase
{
    public:

    //virtual void Step(double StepSize, double output) = 0;

    virtual void InitializeState(RandomState &rs);
    //virtual vector<doubIntParamsHead> getWormParams() = 0;
    
    //virtual void initForSimulation() =  0;

    virtual const vector<string> getCellNames() {return {"not implemented"};}
    virtual void setMuscleInput() {return;}
    double getVelocity(){return Worm2Dbody::getVelocity();}
    virtual void addParsToJson(json & j);
    virtual ~Worm2Dm(){}

    void writeData();

    protected:
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, 
    //bool mfwc, shared_ptr<W2Dbaseparameters> w2dpar_);

    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_);
    //Worm2Dm(wormIzqParams par1_, shared_ptr<W2Dbaseparameters>);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
        shared_ptr<W2Dparameters> w2dpar_);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
    //muscForW2D * m_ptr_, shared_ptr<W2Dbaseparameters> w2dpar_);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
    shared_ptr<W2Dparameters> w2dpar_, bool);

    //const bool muscForWDconst;
    void setBodyInput(); //takes muscle outputs to drive body segments
    virtual vector<toFromWeight> makeBodyConn();
    virtual vector<toFromWeight> makeVentralBodyConn(); //from muscles to body
    virtual vector<toFromWeight> makeDorsalBodyConn();
    void setUpBodyConn();
    void setUpBodyConn(json & j);
    void setBodExt(json & j);
    void setBodExt();

    bool W2Dmparscalled, W2Dminitcalled;
    //shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;
     vector<toFromWeight> vBodyConnvec, dBodyConnvec;
    
     void Step1();

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
    
    virtual void preNStep(){assert(0);}
    virtual void postNStep(){assert(0);}

    protected:

    
    virtual const vector<string> getVMuscNames() {return {"not implemented"};}
    virtual const vector<string> getDMuscNames() {return {"not implemented"};}

    //virtual void addExtraParsToJson(json & j) = 0;
    virtual vector<toFromWeight> makeVentralMuscleConn() {assert(0);} //from neurons to muscles
    virtual vector<toFromWeight> makeDorsalMuscleConn() {assert(0);}  //from neurons to muscles
    void setUpMuscleConn(); //calls make dorsal and ventral musccon to set up connections. 
    void setUpMuscleConn(json & j);
    void makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int mi, int to, TVector<double> & NMJ_Gain);


    virtual void setMuscleInputOrig(){assert(0 && "setMuscleInputOrig needs overriding");}
    void setMuscleInput(); //calls setMuscleInputVec()
    void setMuscleInputVec(); //takes neuron output, inputs it to muscles using connection vector

    void setMuscBodExt();
    void setMuscBodExt(json & j);

    Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_);
    //Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_, json & j);
    //void setMuscleInputVent();
    //void setMuscleInputDors();
    //Worm2D();
    virtual void Step1();
    void setUp();
    Muscles & m;
    
    //NSToMuscles vMuscConn, dMuscConn;
    vector<toFromWeight> vMuscConnvec, dMuscConnvec;
        
    shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;
        
};






class WormFR 
{
public:
virtual void setForward() = 0;
virtual void setBackward() = 0;
virtual void randomizeNS(RandomState &rs)  = 0;

};

class WormGrad
{
public:
virtual void ResetAgentsBody()  = 0;
virtual double distanceToCenter() const = 0;


};