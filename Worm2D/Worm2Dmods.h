#include "Worm2D.h"
//#include "Evolvable.h"

const double pi2 = 3.14159265*2.0;

//struct evoPars;

class pfa{
     public:
pfa(int size_):size(size_){}//,phase(size_,0),freq(size_,0),amp(size_,0){}
vector<double> phase, freq, amp;
const int size;
void swap_all(pfa & pfa_);
void addParsToJson(json & j);

};


class NSosc : public NSForW2D {

public:

//NSosc(const pfa & pfa_);
NSosc(int size_):pfa1(size_){}

virtual double NeuronOutput(int i) {return pfa1.amp[i-1]*sin(pi2*pfa1.freq[i-1]*t + pfa1.phase[i-1]);}
double NeuronState(int i) {return 0;}
void SetNeuronExternalInput(int i, double value) {return;}
virtual void EulerStep(double stepsize) {t+=stepsize;}
virtual ~NSosc(){};
void setTime(const double & t_){t=t_;}
const pfa & itsPfa() const {return pfa1;}
virtual void setFromPheno(TVector<double> &pheno){}
virtual void addParsToJson(json & j){pfa1.addParsToJson(j);}
//friend class Worm2DoscBase<Worm2Dosc>;
friend class Worm2Dosc;
friend class Worm2Dosc21;
friend class Worm2Dosc21all;
friend class Worm2DoscHalf;

pfa pfa1;
private:
double t;

//vector<double> phase, freq, amp;


};

class CoupledOsc : public NSosc
{
    public:
    CoupledOsc(int size_):NSosc(size_){}
    CoupledOsc(const vector<toFromWeight> & weights_, int size_):weights(weights_),NSosc(size_){}
    void EulerStep(double stepsize);
    double NeuronOutput(int i){return pfa1.amp[i-1]*sin(pfa1.phase[i-1]);}
    void setFromPheno(TVector<double> &pheno, int offset = 0);
    virtual ~CoupledOsc(){};
    friend class Worm2Dosc21Coup;
    friend class Worm2Dosc21CF;

    void addParsToJson(json & j){NSosc::addParsToJson(j);j["weights"]["value"]=weights;}
    protected:
    vector<toFromWeight> weights;

};


class W2Dparameters
{
public:
virtual ~W2Dparameters(){}
virtual void setParsFromJson(json & j){}
virtual void addParsToJson(json & j){}
};
class Worm2Doscpars1 : virtual public W2Dparameters
{
public:
double NMJweight;
void setParsFromJson(json & j){NMJweight = j["NMJWeight"]["value"];}
void addParsToJson(json & j){j["NMJWeight"]["value"] = NMJweight;}
};

class Evolparameters : virtual public W2Dparameters
{
public:
int dbunit;
int vbunit;
void setParsFromJson(json & j){dbunit = j["dbunit"]["value"]; vbunit = j["vbunit"]["value"]; }
void addParsToJson(json & j){j["dbunit"]["value"] = dbunit; j["vbunit"]["value"] = vbunit;}
};

class Worm2Doscpars : public Worm2Doscpars1, public Evolparameters
{
public:
void setParsFromJson(json & j){Worm2Doscpars1::setParsFromJson(j); Evolparameters::setParsFromJson(j);}
void addParsToJson(json & j){Worm2Doscpars1::addParsToJson(j); Evolparameters::addParsToJson(j);}

};


class Worm2Dosc21pars : public Evolparameters
{
public:

TVector<double> NMJ_Gain;
double NMJ_VN, NMJ_DN, NMJ_Gain_Map;
void setParsFromJson(json & j)
{
    NMJ_Gain_Map = j["NMJ_Gain_Map"]["value"];
    NMJ_VN =  j["NMJ_VN"]["value"];
    NMJ_DN =  j["NMJ_DN"]["value"];
    Evolparameters::setParsFromJson(j);
} 

void addParsToJson(json & j)
{
    j["NMJ_Gain_Map"]["value"] = NMJ_Gain_Map;
    j["NMJ_VN"]["value"] = NMJ_VN;
    j["NMJ_DN"]["value"] = NMJ_DN;
    Evolparameters::addParsToJson(j);
}


};



class Worm2DPars : public Worm2D
{
public:

protected:


void Step1();

Worm2DPars(wormIzqParams par1_, NSForW2D * n_ptr_, W2Dparameters * w2par_ptr);

virtual ~Worm2DPars(){if (pars1_ptr) delete pars1_ptr;}
W2Dparameters * const pars1_ptr = nullptr;

void addParsToJson(json & j){Worm2D::addParsToJson(j);pars1_ptr->addParsToJson(j["Worm"]);}


vector<doubIntParamsHead> getWormParams() {
    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    parvec.push_back(var1);
    return parvec;
}
};


class Worm2DoscBase : public Worm2DPars, public EvolvableS
{
public:
//void InitializeState(RandomState &rs);
//void initForSimulation(RandomState &) {return;}
//void DumpParams(ofstream &ofs) {return;}

//double EvaluationFunction(TVector<double> &v, RandomState &rs);
//void writeJson(TVector<double> &);
//evoPars getDefaultEvoPars();
virtual void setParsFromPheno(TVector<double> &v) = 0;
void setParsFromFile(const string & genofilename_);
void setParsFromGeno(TVector<double> &v);
virtual void setPfaFromPheno(TVector<double> &v) = 0;
//virtual void setParsFromPheno(TVector<double> &v) = 0;
void setPfaFromGeno(TVector<double> &v);

void setPfaFromFile(const string & genofilename_);
//void setParsFromFile(const string & genofilename_);
//const vector<string> getCellNames() {return {"not implemented"};}

NSosc & n;
//Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(*pars1_ptr);
Evolparameters & Epars1;

protected:

//void Step1();
//const vector<string> getVMuscNames() {return {"not implemented"};}
//const vector<string> getDMuscNames() {return {"not implemented"};}

void constructFromGeno(TVector<double> &geno);
void construct(TVector<double> &pheno);
void construct(const string & filename_);

Worm2DoscBase(wormIzqParams par1_, W2Dparameters * w2par_ptr);

void addParsToJson(json & j);



//W2Dparameters * const pars1_ptr;

};



class Worm2Dosc1 
{
public:
Worm2Dosc1(Worm2Doscpars1 & pars1_):pars1(pars1_){}
vector<toFromWeight> makeVentralMuscleConn();
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeDVMuscleConn(int offset);

protected:
Worm2Doscpars1 & pars1;
};




class Worm2Dosc : public Worm2DoscBase, public Worm2Dosc1
{
public:
//Worm2Dosc(const Worm2Dosc&);
Worm2Dosc(int size_);
Worm2Dosc(const string & filename_);
Worm2Dosc();
Worm2Dosc(TVector<double> & phengen, const bool & isPheno);
//Worm2Dosc(TVector<double> &v);
//Worm2Dosc(const pfa & pfa_, const Worm2Doscpars & par1_);
const string getModelName() {return "Worm2Dosc";}
vector<toFromWeight> makeVentralMuscleConn(){return Worm2Dosc1::makeVentralMuscleConn();}
vector<toFromWeight> makeDorsalMuscleConn(){return Worm2Dosc1::makeDorsalMuscleConn();}
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
void setPhenoNames(); //{return;}
int getVectSize(){return 4;}


//static inline int evoVectSize = 4;
protected:

void setPfaFromPheno(TVector<double> &v);
void setParsFromPheno(TVector<double> &v);





//virtual ~Worm2Dosc(){if (pars1_ptr) delete pars1_ptr;}

//W2Dparameters * const pars1_ptr = nullptr;
Worm2Doscpars & pars1;
//const int dbunit = 6;
//const int vbunit = 30;
//const int vectsize = 4;

//NSosc & n;

};


class Worm2DoscNML : public Worm2DPars, public Worm2Dosc1
{

    public:
    Worm2DoscNML(int size);
    Worm2DoscNML(const string & jsonfile);
    protected:
    vector<toFromWeight> makeVentralMuscleConn(){return Worm2Dosc1::makeVentralMuscleConn();}
    vector<toFromWeight> makeDorsalMuscleConn(){return Worm2Dosc1::makeDorsalMuscleConn();}
    const string getModelName() {return "Worm2DoscNML";}
};


class Worm2DoscHalf : public Worm2Dosc
{
public:
Worm2DoscHalf();
Worm2DoscHalf(const string & filename_);
//Worm2DoscHalf(const pfa & pfa_, const Worm2Doscpars & pars1_);
Worm2DoscHalf(TVector<double> & pheno, const bool & isPheno);
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
//static inline int evoVectSize = 3;
int getVectSize() {return 3;}


protected:
vector<toFromWeight> makeDVMuscleConn(double);
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeVentralMuscleConn();


void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);
const string getModelName() {return "Worm2DoscH";}



};

class Worm2Dosc21base
{
public:
Worm2Dosc21base(const wormIzqParams & par1_, Worm2Dosc21pars & pars1_):pars1(pars1_), par1ref(par1_){}
vector<toFromWeight> makeMuscleConn(vector<int> neurons, vector<double> NMJ);
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeVentralMuscleConn();

protected:
Worm2Dosc21pars & pars1;
const wormIzqParams & par1ref;
};


class Worm2Dosc21NML: public Worm2DPars, public Worm2Dosc21base
{
    public:
    Worm2Dosc21NML();
    Worm2Dosc21NML(const string & jsonfile_);
    protected:
    vector<toFromWeight> makeVentralMuscleConn(){return Worm2Dosc21base::makeVentralMuscleConn();}
    vector<toFromWeight> makeDorsalMuscleConn(){return Worm2Dosc21base::makeDorsalMuscleConn();}
    const string getModelName() {return "Worm2Dosc21NML";}

};


class Worm2Dosc21 : public Worm2DoscBase, public Worm2Dosc21base
{

public:
Worm2Dosc21();
Worm2Dosc21(const string & filename_);
Worm2Dosc21(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return 6;}


protected:
void setPhenoNames(); 
//vector<toFromWeight> makeMuscleConn(vector<int> neurons, vector<double> NMJ);
vector<toFromWeight> makeDorsalMuscleConn() {return Worm2Dosc21base::makeDorsalMuscleConn();}
vector<toFromWeight> makeVentralMuscleConn() {return Worm2Dosc21base::makeVentralMuscleConn();}

void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen, int offset);

const string getModelName() {return "Worm2Dosc21";}

//Worm2Dosc21pars & pars1;

};



class Worm2Dosc21all : public Worm2Dosc21
{
public:
Worm2Dosc21all();
Worm2Dosc21all(const string & filename_);
Worm2Dosc21all(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return 18;}
protected:
void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21all";}
void setPhenoNames(); 
};

class Worm2Dosc21S : public Worm2Dosc21
{
public:
Worm2Dosc21S();
Worm2Dosc21S(const string & filename_);
Worm2Dosc21S(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return 5;}
protected:
//void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21S";}
void setPhenoNames(); 
};


class Worm2Dosc21Coup : public Worm2Dosc21
{
public:
Worm2Dosc21Coup();
Worm2Dosc21Coup(const string & filename_);
Worm2Dosc21Coup(TVector<double> & pheno, const bool & isPheno);

void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return cn.weights.size()+4;}


protected:
void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21Coup";}
void setPhenoNames();
vector<toFromWeight> getWeightVec();

CoupledOsc & cn;

};

class Worm2Dosc21CF : public Worm2Dosc21Coup
{
public:
Worm2Dosc21CF();
Worm2Dosc21CF(const string & filename_);
Worm2Dosc21CF(TVector<double> & pheno, const bool & isPheno);
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return cn.weights.size() + 14 + 3;}
protected:
void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21CF";}
void setPhenoNames();
};




//Worm2Dosc w;


