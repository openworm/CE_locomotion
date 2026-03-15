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
//#include "StretchReceptorCE.h"

//datawriter->worm2dbase (nervous system and muscle pointers)
//datawriter->worm2dbody (just body plus functions)
//worm2dbase + worm2dbody -> worm2dm (nn ptr + musc ptr + body)
//worm2dm -> worm2d (cc musc + nn ptr plus nn to musc connections )
//worm2dm -> worm2d21m (nn+musc ptrs + body + net21 specifics)
//Worm2D21m + worm2d -> worm2d21 (cc musc + nn ptr + musc connections)
//worm2d21 -> worm21 (cc musc + cc nn + musc connections)


//class SRCE;

extern string main_directoryname, main_modelname;
int nn1(int neuronNumber, int unitNumber, int N_neuronsperunit);
void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int mi, int to, TVector<double> & NMJ_Gain, int);
//string main_directoryname;
//string main_modelname;

void makeMuscleConnHelp1(vector<toFromWeight> & vec1, 
    const vector<int> & neurons, const vector<double> & NMJs, int unit, int to_muscle, 
    const vector<double> & NMJ_Gain, int N_neuronsperunit);

class baseParameters
{

    public:
    baseParameters(const json & itsJson_, shared_ptr<const CmdArgs> itsCmdArgs_)
    :BPitsJson(itsJson_),BPitsCmdArgs(itsCmdArgs_),defaultVals(setDefaultVals()){}

    baseParameters(shared_ptr<const CmdArgs> itsCmdArgs_)
    :BPitsCmdArgs(itsCmdArgs_),defaultVals(setDefaultVals()){}

    //baseParameters():defaultVals(setDefaultVals()){}


    template<class T>
    void setValCJWorm(const string & name_str, const T & val)
    {
        return setValCJ<T>(name_str,val,"Worm");
    }

    template<class T>
    void setValCJ(const string & name_str, const T & val, const string & bstr)
    {
        if (!newSetVals.contains(bstr)) newSetVals[bstr] = json::object();
        if (!newSetVals.at(bstr).contains(name_str)) newSetVals[bstr][name_str] =  json::object();
        newSetVals[bstr][name_str]["value"] = val;

        addValToJson(name_str,val,bstr);
    }


    template<class T>
    bool getValCJ(const string & name_str, T & val, const string & bstr) 
    {
        
        //cout << "nssd " << name_str << " " << bstr << endl;

        //assert(BPitsCmdArgs!=nullptr);

     

        if (newSetVals.contains(bstr) && newSetVals.at(bstr).contains(name_str))
        {
            //cout << "nesws" << endl;
            //cout << newSetVals << endl;
 
            //cout << "oosl  " << name_str << " " << bstr << endl;
 
        
            val = newSetVals[bstr][name_str].at("value").get<T>();
            return true;
        }

    
        if (BPitsCmdArgs!=nullptr) {
            // cout << "wet  " << name_str << " " << bstr << endl;
         if( BPitsCmdArgs->getArgValT<T>("--" + name_str, val)) 
        {

          //cout << "utit  " << name_str << " " << bstr << " isi " << val << endl;
 
     

            addValToJson(name_str,val,bstr);
            return true;
        }
    }

        if (!BPitsJson.empty() && BPitsJson.contains(bstr)) 
        if (getJsonValTF<T>(BPitsJson.at(bstr), name_str, val, true)) return true;

       
       
        if (defaultVals.contains(name_str)) {

            //cout << "djql  " << name_str << " " << bstr << endl;
 
            
            val = defaultVals.at(name_str).get<T>();
            addValToJson(name_str,val,bstr);
            return true;
         }


       /*  if (itsCmdArgs!=nullptr && itsCmdArgs->getArgValT<T>("--" + name_str, val))
        {
            if (!newPars.contains(bstr)) newPars[bstr] = json::object();
            newPars[bstr][name_str]["value"] = val; 
            return true;
        }
        if (itsJson!=nullptr && itsJson->contains(bstr)) 
        if (getJsonValTF<T>(itsJson->at(bstr), name_str, val, true)) return true;
        if (defaultVals.contains(name_str)) {
        val = defaultVals.at(name_str).get<T>();
        if (!newPars.contains(bstr)) newPars[bstr] = json::object();
        newPars[bstr][name_str]["value"] = val; 
        return true;
        } */

        cout << "dffd " << defaultVals << endl;
        cout << "getValCJ " << name_str << " " << bstr << endl;
        assert(0);
        return false;
    }

    template<class T>
    bool getValCJWorm(const string & name_str, T & val)
    {

        return getValCJ<T>(name_str,val,"Worm");

    }

    template<class T>
    T getValCJWorm(const string & name_str)
    {
        T val;
        getValCJ<T>(name_str,val,"Worm");
        return val;
    }
     

    template<class T>
    bool getValCJEvo(const string & name_str, T & val)
    {

        return getValCJ<T>(name_str,val,"Evolutionary Optimization Parameters");

    }



    json setDefaultVals()
    {
        json defaultVals_;
        defaultVals_["randomInitialState"] = false;
        defaultVals_["doOrigMuscInput"] = true;
        defaultVals_["doOrigSRInput"] = true;

        defaultVals_["resetAgentBody"] = false;
        defaultVals_["rotation"] = 0.0;
        defaultVals_["orient"] = 0.0;
        defaultVals_["gradSteep"] = 0.5;
        defaultVals_["RunDuration"] = 1000;
        defaultVals_["HSStepSize"] = 0.01;
        defaultVals_["MaxDist"] = 4.5;
        defaultVals_["taxis"] = 1;
        defaultVals_["kinesis"] = 0;
        defaultVals_["SREvoBot"]=0;
        defaultVals_["SREvoTop"]=200;
        defaultVals_["SREvoBotA"]=0;
        defaultVals_["SREvoTopA"]=200;
        defaultVals_["AB_output_level"] = 1.0;
        defaultVals_["SRType"] = "None";
        defaultVals_["SRForm"] = 0;
        defaultVals_["SRSegPerSR"] = 6;
        defaultVals_["SRZeroGainsType"] = 1;
        defaultVals_["SROffset"] = 0;
        defaultVals_["NMJWeight"] = 1;
        defaultVals_["doReverse"] = 0;

        defaultVals_["OSCTbase"] = 0.25; // Cap for oscillation evaluation
        defaultVals_["agarfreq"] = 0.44;
        defaultVals_["AvgSpeed"] = 0.00022; 

        defaultVals_["NMJ_VN"] = 1; 
        defaultVals_["NMJ_DN"] = 1; 
        defaultVals_["NMJ_Gain_Map"] = 1;
        defaultVals_["fitType"] = 0;
        defaultVals_["doAngleDiff"] = 0;
        defaultVals_["doLegacy"] = true;
        defaultVals_["initNSFromJson"] = true;

       return defaultVals_;
    }

    template<class T>
    void addValToJson(const string & name_str, const T & val, const string & bstr)
    {
        if (!BPitsJson.contains(bstr)) BPitsJson[bstr] = json::object();
        if (!BPitsJson.at(bstr).contains(name_str)) BPitsJson[bstr][name_str] = json::object();
        BPitsJson[bstr][name_str]["value"] = val;
    }

   /*  void addParsToJson(json & j)
    {

    for(auto it = newPars.begin(); it != newPars.end(); ++it)
    {
        if (!j.contains(it.key())) j[it.key()] = it.value(); 
        if (j.contains(it.key()))  j[it.key()].push_back(it.value());

    }        
    }
 */
    //shared_ptr<const json itsJsonPtr()const {return &itsJson;} 

    const json & itsNewSetVals(){return newSetVals;}

    protected:
    json BPitsJson;
    //shared_ptr<const json> itsJson = nullptr;
    //shared_ptr<json> itsJson = nullptr;
    const shared_ptr<const CmdArgs> BPitsCmdArgs = nullptr;
    const json defaultVals;
    json newSetVals;

    
};




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

class InputSwitcher
{

  public:
  
  protected:

  void setInputOnce(const json & j, const int & ind, vector<double> & externalInputs);

  void setInputOnce(const int & ind, vector<double> & externalInputs);
  void construct(const json & j);
 
  void setParsFromJson(const json & j){construct(j);}
  void addParsToJson(json & j) const;

  vector<double> timeperiods;
  double time_offset = 0, total_period = 0;
  vector<vector<int> > inds;
  vector<vector<double> > vals;
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
    double PositionX() const {return b.X(Head)*100.0;} //change to cm
    double PositionY() const {return b.Y(Head)*100.0;}
    void shiftX(double shiftdist_);
    void shiftY(double shiftdist_);
    void zeroX();
    void zeroY();
    //void ResetAgentsBody(shared_ptr<gradParameters> CO2DSRpars);
    void ResetAgentsBody(baseParameters & basePar_);

    double headDistanceToCenter() const;
    double headDistanceToLocation(const double & x, const double & y) const;
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





class Worm2Dbase : public baseParameters, virtual public DataWriter, public InputSwitcher
{

public:

virtual void InitializeState(RandomState &rs) = 0;
virtual void initForSimulation(RandomState &) {return;}


void Step(double StepSize_);
void Step();
virtual void setStepSize(double val_){settedStepSize=val_;}

virtual void randomizeNS(RandomState &rs);
vector<double> readPhenotype();
virtual void writeAct();
void writeExtInp(ofstream & ofs);
void writeVNC(ofstream & ofs);
void writeMusc(ofstream & ofs);
void writeState();
virtual void addParsToJson(json & j);
void writeJsonFile(ofstream & json_out);
virtual void addEvolvableToJson(json & j) {return;}
void addParsToJson();

const NSForW2D & itsNS() const {return *n_ptr;}
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

virtual void setTime(double t_){t=t_;datatime=t_;}
const double & itsStepSize() const {return settedStepSize;}
void incSimTimes();

//virtual shared_ptr<const W2Dparameters> setWormPars(int argc, const char* argv[]) {return nullptr;}
//virtual void setWormPars(const W2Dparameters * w2par_) {assert(0);}

//virtual shared_ptr<const W2Dparameters> setWormPars(shared_ptr<const CmdArgs> cmd) {return nullptr;}

virtual void setWormPars(shared_ptr<const CmdArgs> cmd_) 
{
    //BPitsCmdArgs = cmd_;
    //W2Dbaseparameters1b->setPars(cmd);

}

// not this, shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;

//shared_ptr<baseParameters> basePar1 = nullptr;

//baseParameters basePar1;

//void setBasePar(shared_ptr<baseParameters> basePar1_){basePar1=basePar1_;}

//shared_ptr<W2Dparameters> W2Dbaseparameters1b = nullptr;

void zeroAllInputs(){
    for (int i=0;i<par1.N_size;i++)
    n_ptr->SetNeuronExternalInput(i+1, 0);
}

template<class T> friend class Evolvable_ptrB;

void setInputOnce(const int & ind) {InputSwitcher::setInputOnce(ind,externalInputs);}



protected:
//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);

Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_,
    shared_ptr<const CmdArgs> cmd_);
Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_,
    shared_ptr<const CmdArgs> cmd_, const json & j);

//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, shared_ptr<W2Dparameters>);

//Worm2Dbase(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc, 
//    shared_ptr<W2Dbaseparameters> w2dpar_);

static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd, const json & j);

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

double t = 0; // Time


double settedStepSize = 0.01;

void makeExternalInputConnFromJson(const json & j);
virtual void makeExternalInputConn(){return;}
vector<toFromWeight> externalInputConn;
vector<double> externalInputs;
//vector<double> sjdkdsdjddssdsloe;
//double sjdkdsdjddssdsloe;
void setExternalInput();
virtual void assignExternalInput(){fill(externalInputs.begin(), externalInputs.end(), 0);}

void assignExternalInputOnce(const int & ind, const double & val){externalInputs[ind]=val;}

vector<toFromWeight> NSInputConn, NSOutputConn;
void incInputFromNS(NSForW2D & ns_);
void incOutputToNS(Worm2Dbase & ns_);
virtual void makeNSInputConn(){return;}
virtual void makeNSOutputConn(){return;}
//namedValVec<double> doubVars;
json namedVars;
static wormIzqParams getIzqPars(const json & j);




//Efunctor itsEf;
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
    const vector<toFromWeight> &  itsvBodyConnvec() const {return vBodyConnvec;}

    void writeData();

    protected:
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, bool mfwc);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_, 
    //bool mfwc, shared_ptr<W2Dbaseparameters> w2dpar_);

    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, muscForW2D * m_ptr_);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd_, const json & j);

    //Worm2Dm(wormIzqParams par1_, shared_ptr<W2Dbaseparameters>);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
    //    shared_ptr<W2Dparameters> w2dpar_);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, 
    //muscForW2D * m_ptr_, shared_ptr<W2Dbaseparameters> w2dpar_);
    //Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2Dm(wormIzqParams par1_, NSForW2D * n_ptr_, bool, shared_ptr<const CmdArgs> cmd_, const json & j);

    //const bool muscForWDconst;
    void setBodyInput(); //takes muscle outputs to drive body segments
    virtual vector<toFromWeight> makeBodyConn();
    virtual vector<toFromWeight> makeVentralBodyConn(); //from muscles to body
    virtual vector<toFromWeight> makeDorsalBodyConn();
    void setUpBodyConn();
    void setUpBodyConn(const json & j);
    void setBodExt(const json & j);
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

    const vector<toFromWeight> & itsvMuscConnvec() const {return vMuscConnvec;}
 
    const vector<toFromWeight> & itsdMuscConnvec() const {return dMuscConnvec;}

    protected:

    
    virtual const vector<string> getVMuscNames() {return {"not implemented"};}
    virtual const vector<string> getDMuscNames() {return {"not implemented"};}

    //virtual void addExtraParsToJson(json & j) = 0;
    virtual vector<toFromWeight> makeVentralMuscleConn() {assert(0);} //from neurons to muscles
    virtual vector<toFromWeight> makeDorsalMuscleConn() {assert(0);}  //from neurons to muscles



    void setUpMuscleConn(); //calls make dorsal and ventral musccon to set up connections. 
    void setUpMuscleConn(const json & j);
    void makeMuscleConnHelp(vector<toFromWeight> & vec1, 
    vector<int> neurons, vector<double> NMJs, int mi, int to, TVector<double> & NMJ_Gain);
    //vector<toFromWeight> makeMuscleConn(vector<int> dorsalNeurons, vector<double> dorsalNMJ);
    vector<toFromWeight> makeMuscleConnW2D(vector<int> neurons, vector<double> NMJ,
    TVector<double> & NMJ_Gain, vector<intPair> & unitToMusc);

    virtual void setMuscleInputOrig(){assert(0 && "setMuscleInputOrig needs overriding");}
    void setMuscleInput(); //calls setMuscleInputVec()
    void setMuscleInputVec(); //takes neuron output, inputs it to muscles using connection vector

    void setMuscBodExt();
    void setMuscBodExt(const json & j);

    Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_);
    //Worm2D(wormIzqParams par1_, NSForW2D * n_ptr_, json & j);
    //void setMuscleInputVent();
    //void setMuscleInputDors();
    //Worm2D();
    void Step1();
    void setUp();
    Muscles & m;
    
    //NSToMuscles vMuscConn, dMuscConn;
    vector<toFromWeight> vMuscConnvec, dMuscConnvec;
        
    //shared_ptr<W2Dbaseparameters> W2Dbaseparameters1;  //change this back

    vector<weightentry> ventinds, dorsinds;
    vector<intPair> unitToMuscV, unitToMuscD;
    //double NMJ_gain_map_V, NMJ_gain_map_D, NMJ_gain_fact = 0.7;
    
    const bool doOrigMuscInput, doOrigSRInput;
    bool hasVNCNMJ = false, hasVNC18 = false;
    vector<toFromWeight> makeMuscleConnVNCV();
    vector<toFromWeight> makeMuscleConnVNCD();
    vector<toFromWeight> makeVentralMuscleConn18();
    vector<toFromWeight> makeDorsalMuscleConn18();

};






class WormFR 
{
public:
virtual void setForward() = 0;
virtual void setBackward() = 0;
//virtual void randomizeNS(RandomState &rs)  = 0;

};

class WormGrad
{
public:
virtual void ResetAgentsBody()  = 0;
virtual double distanceToCenter() const = 0;
virtual void InitializeSensors(RandomState& rs) = 0;

};


