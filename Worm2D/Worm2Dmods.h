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
NSosc(int size_):pfa1(size_){}//,output(size_,0.0){}

virtual const double & NeuronOutput(int i) {
    output = pfa1.amp[i-1]*sin(pi2*pfa1.freq[i-1]*t + pfa1.phase[i-1]);
    return output;
}
const double & NeuronState(int i) {return zero_state;}
void SetNeuronExternalInput(int i, double value) {return;}
void IncNeuronExternalInput(int i, double value) {return;}
virtual void EulerStep(double stepsize_) {t+=stepsize_;}
virtual void EulerStep(){t+=stepsize;}
virtual ~NSosc(){};
void setTime(const double & t_){t=t_;}
void setStepSize(double val_){stepsize=val_;}
const pfa & itsPfa() const {return pfa1;}
//virtual void setFromPheno(const TVector<double> &pheno){}
virtual void addParsToJson(json & j){pfa1.addParsToJson(j);}
//friend class Worm2DoscBase<Worm2Dosc>;
friend class Worm2Dosc;
friend class Worm2Dosc21;
friend class Worm2Dosc21all;
friend class Worm2DoscHalf;
void InitializeState(RandomState &rs){setTime(0);}

pfa pfa1;
protected:
//vector<double> output;
private:
double t = 0, stepsize = 0.01;
double output = 0.0;
const double zero_state = 0.0;


//vector<double> phase, freq, amp;


};

class CoupledOsc : public NSosc
{
    public:
    CoupledOsc(int size_):NSosc(size_){}
    CoupledOsc(const vector<toFromWeight> & weights_, int size_):weights(weights_),NSosc(size_){}
    void EulerStep(double stepsize);
    const double & NeuronOutput(int i){
        output = pfa1.amp[i-1]*sin(pfa1.phase[i-1]);
        return output;
        //output[i-1] = pfa1.amp[i-1]*sin(pfa1.phase[i-1]);
        //return output[i-1];
    }
    void setFromPheno(const TVector<double> &pheno, int offset = 0);
    virtual ~CoupledOsc(){};
    friend class Worm2Dosc21Coup;
    friend class Worm2Dosc21CF;

    void addParsToJson(json & j){NSosc::addParsToJson(j);j["weights"]["value"]=weights;}
    protected:
    vector<toFromWeight> weights;
    double output = 0.0;

};

//////////////////////////////////////
////////////////////////////////////
/////////////////////////////////////


class Worm2Doscpars1 : public W2Dbaseparameters
{
public:
double NMJweight = 1;
void setParsFromJson(json & j){
    NMJweight = j["NMJWeight"]["value"];
    W2Dbaseparameters::setParsFromJson(j);
}
void addParsToJson(json & j) const {
    j["nmj_weight"]["value"] = NMJweight;
    W2Dbaseparameters::addParsToJson(j);
}
void setPars(shared_ptr<const CmdArgs> cmd){
    
    NMJweight = cmd->getArgValDoub("--NMJweight", NMJweight);
    W2Dbaseparameters::setPars(cmd);
    }
};


/* class Worm2Doscpars : public Worm2Doscpars1
{
public:
void setParsFromJson(json & j){Worm2Doscpars1::setParsFromJson(j);}
void addParsToJson(json & j) const {Worm2Doscpars1::addParsToJson(j);}

}; */


class Worm2Dosc21pars : public W2Dbaseparameters
{
public:

Worm2Dosc21pars(int N_musc_):N_musc(N_musc_){
//    NMJ_Gain.SetBounds(1,N_musc_);
    NMJ_Gain.resize(N_musc_);
}

const int N_musc;
//TVector<double> NMJ_Gain;
vector<double> NMJ_Gain;
double NMJ_VN = 1, NMJ_DN = 1, NMJ_Gain_Map = 1;

void setParsFromJson(json & j)
{
    NMJ_Gain_Map = j["NMJ_Gain_Map"]["value"];
    NMJ_VN =  j["NMJ_VN"]["value"];
    NMJ_DN =  j["NMJ_DN"]["value"];
    for (int i=1; i<=N_musc; i++)
    {
    NMJ_Gain[i-1] = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/N_musc));
    }

    W2Dbaseparameters::setParsFromJson(j);
} 

void addParsToJson(json & j) const
{
    j["nmj_gain_map"]["value"] = NMJ_Gain_Map;
    j["nmj_vn"]["value"] = NMJ_VN;
    j["nmj_dn"]["value"] = NMJ_DN;
    W2Dbaseparameters::addParsToJson(j);
}

void setPars(shared_ptr<const CmdArgs> cmd)
{
    NMJ_Gain_Map = cmd->getArgValDoub("--NMJ_Gain_Map", NMJ_Gain_Map);
    NMJ_DN = cmd->getArgValDoub("--NMJ_DN", NMJ_DN);
    NMJ_VN = cmd->getArgValDoub("--NMJ_VN", NMJ_VN);
    for (int i=1; i<=N_musc; i++)
    {
    NMJ_Gain[i-1] = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/N_musc));
    }

    W2Dbaseparameters::setPars(cmd);
}



};

/////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////


/* class Worm2DPars : public Worm2D
{
public:

protected:

//void Step1(){ Step1_old();}
void Step1_old();

Worm2DPars(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<W2Dbaseparameters> w2par_ptr);

//virtual ~Worm2DPars(){if (pars1_ptr) delete pars1_ptr;}
//W2Dparameters * const pars1_ptr = nullptr;

//shared_ptr<W2Dparameters> pars1_ptr;

//void addParsToJson(json & j){Worm2D::addParsToJson(j);pars1_ptr->addParsToJson(j["Worm"]);}


vector<doubIntParamsHead> getWormParams() {
    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    parvec.push_back(var1);
    return parvec;
}
};
 */

class Worm2DoscBase : public Worm2D, public EvolvableS
{
public:

virtual void setPfaFromPheno(const TVector<double> &v) = 0;


NSosc & n;

//virtual void setWormPars(shared_ptr<const CmdArgs> cmd) 
//{return Worm2D::setWormPars(cmd);}

void setStepSize(double val_){Worm2D::setStepSize(val_);n.setStepSize(val_);}
void InitializeState(RandomState &rs){Worm2D::InitializeState(rs);n.InitializeState(rs);}
protected:

void construct(const TVector<double> &pheno);

Worm2DoscBase(wormIzqParams par1_);
void addParsToJson(json & j);
void setTime(const double & t_){Worm2D::setTime(t_);n.setTime(t_);}


};



class Worm2Dosc1 
{
public:
Worm2Dosc1(baseParameters * pars1_):basePar1(pars1_){}//{if (pars1 == nullptr) assert(0);}
Worm2Dosc1(){}
vector<toFromWeight> makeVentralMuscleConn();
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeDVMuscleConn(int offset);

protected:
//Worm2Doscpars1 & pars1;
baseParameters * basePar1 = nullptr;
//shared_ptr<Worm2Doscpars1> pars1;
};




class Worm2Dosc : public Worm2DoscBase, public Worm2Dosc1
{
public:
//Worm2Dosc(const Worm2Dosc&);
Worm2Dosc(int size_, shared_ptr<const CmdArgs> cmd_ = nullptr);
Worm2Dosc(const string & filename_, shared_ptr<const CmdArgs> cmd_ = nullptr);
Worm2Dosc(TVector<double> & phengen, const bool & isPheno);
Worm2Dosc(shared_ptr<const CmdArgs> cmd_ = nullptr);

//Worm2Dosc(TVector<double> &v);
//Worm2Dosc(const pfa & pfa_, const Worm2Doscpars & par1_);
const string getModelName() {return "Worm2Dosc";}
vector<toFromWeight> makeVentralMuscleConn(){return Worm2Dosc1::makeVentralMuscleConn();}
vector<toFromWeight> makeDorsalMuscleConn(){return Worm2Dosc1::makeDorsalMuscleConn();}
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
void setPhenoNames(); //{return;}
int getVectSize(){return 4;}
//void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_);
void setEvolPars(W2Dparameters & w2par_, string evotype_);

template<class T> friend class Evolvable_ptrB;
template<class T> friend class EvolutionFullW;
//static inline int evoVectSize = 4;
protected:

void setPfaFromPheno(const TVector<double> &v);
//void setParsFromPheno(const TVector<double> &v);
void setParsFromPheno(const TVector<double> &v);


void addParsToJson(json & j);

//virtual ~Worm2Dosc(){if (pars1_ptr) delete pars1_ptr;}

//W2Dparameters * const pars1_ptr = nullptr;
//shared_ptr<Worm2Doscpars>  pars1;
//const int dbunit = 6;
//const int vbunit = 30;
//const int vectsize = 4;

//NSosc & n;

};


class Worm2DoscNMLm : public Worm2Dm
{

    public:
    Worm2DoscNMLm(int size_, shared_ptr<const CmdArgs> cmd_, const json & j);
    Worm2DoscNMLm(const string & jsonfile, shared_ptr<const CmdArgs> cmd_ = nullptr);
    protected:
    const string getModelName() {return "Worm2DoscNMLm";}
    void addParsToJson(json & j){j=BPitsJson;Worm2Dm::addParsToJson(j);}
};


class Worm2DoscNML : public Worm2D, public Worm2Dosc1
{

    public:
   
    Worm2DoscNML(const string & jsonfile, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2DoscNML(int size_, shared_ptr<const CmdArgs> cmd_, const json & j);
    protected:
    vector<toFromWeight> makeVentralMuscleConn(){return Worm2Dosc1::makeVentralMuscleConn();}
    vector<toFromWeight> makeDorsalMuscleConn(){return Worm2Dosc1::makeDorsalMuscleConn();}
    const string getModelName() {return "Worm2DoscNML";}
    void addParsToJson(json & j){j=BPitsJson;Worm2D::addParsToJson(j);}
};




class Worm2DoscHalf : public Worm2Dosc
{
public:
Worm2DoscHalf();
Worm2DoscHalf(const string & filename_);
//Worm2DoscHalf(const pfa & pfa_, const Worm2Doscpars & pars1_);
Worm2DoscHalf(TVector<double> & pheno, const bool & isPheno);
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
//static inline int evoVectSize = 3;
int getVectSize() {return 3;}
//void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_);
//void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_);
void setEvolPars(W2Dparameters & w2par_, string evotype_);

template<class T> friend class EvolutionFullW;
template<class T> friend class Evolvable_ptrB;

protected:
vector<toFromWeight> makeDVMuscleConn(double);
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeVentralMuscleConn();


void setPfaFromPheno(const TVector<double> &phen);
void setParsFromPheno(const TVector<double> &phen);
const string getModelName() {return "Worm2DoscH";}



};

class Worm2Dosc21base
{
public:
Worm2Dosc21base(int N_neuronsperunit_, baseParameters * pars1_):basePar1(pars1_), 
N_neuronsperunit(N_neuronsperunit_){}
Worm2Dosc21base(int N_neuronsperunit_):N_neuronsperunit(N_neuronsperunit_){}
vector<toFromWeight> makeMuscleConn(vector<int> neurons, vector<double> NMJ);
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeVentralMuscleConn();

protected:
baseParameters * basePar1 = nullptr;
//shared_ptr<Worm2Dosc21pars> pars1;
const int N_neuronsperunit;
//const wormIzqParams & par1ref;
};


class Worm2Dosc21NML: public Worm2D, public Worm2Dosc21base
{
    public:
    Worm2Dosc21NML(const json & j, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2Dosc21NML(const string & jsonfile_, shared_ptr<const CmdArgs> cmd_ = nullptr);
    protected:
    vector<toFromWeight> makeVentralMuscleConn(){return Worm2Dosc21base::makeVentralMuscleConn();}
    vector<toFromWeight> makeDorsalMuscleConn(){return Worm2Dosc21base::makeDorsalMuscleConn();}
    const string getModelName() {return "Worm2Dosc21NML";}
    void addParsToJson(json & j){j=BPitsJson;Worm2D::addParsToJson(j);}
};


class Worm2Dosc21NMLm : public Worm2Dm
{

    public:
    Worm2Dosc21NMLm(const json & j, shared_ptr<const CmdArgs> cmd_);
    Worm2Dosc21NMLm(const string & jsonfile, shared_ptr<const CmdArgs> cmd_ = nullptr);
    protected:
    const string getModelName() {return "Worm2Dosc21NMLm";}
};

class Worm2Dosc21 : public Worm2DoscBase, public Worm2Dosc21base
{

public:
Worm2Dosc21(shared_ptr<const CmdArgs> cmd_ = nullptr);
Worm2Dosc21(const string & filename_, shared_ptr<const CmdArgs> cmd_ = nullptr);
Worm2Dosc21(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return 6;}
//void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_);
void setEvolPars(W2Dparameters & w2par_, string evotype_);

template<class T> friend class EvolutionFullW;
template<class T> friend class Evolvable_ptrB;
protected:
void setPhenoNames(); 
//vector<toFromWeight> makeMuscleConn(vector<int> neurons, vector<double> NMJ);
vector<toFromWeight> makeDorsalMuscleConn() {return Worm2Dosc21base::makeDorsalMuscleConn();}
vector<toFromWeight> makeVentralMuscleConn() {return Worm2Dosc21base::makeVentralMuscleConn();}

void setPfaFromPheno(const TVector<double> &phen);
void setParsFromPheno(const TVector<double> &phen);
void setParsFromPheno(const TVector<double> &phen, int offset);

void addParsToJson(json & j);

const string getModelName() {return "Worm2Dosc21";}



//Worm2Dosc21pars & pars1;

};



class Worm2Dosc21all : public Worm2Dosc21
{
public:
Worm2Dosc21all(shared_ptr<const CmdArgs> cmd_ = nullptr);
Worm2Dosc21all(const string & filename_, const bool & isGenJson = true, 
    shared_ptr<const CmdArgs> cmd_ = nullptr);
Worm2Dosc21all(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return 18;}

template<class T> friend class EvolutionFullW;
template<class T> friend class Evolvable_ptrB;
protected:
void setPfaFromPheno(const TVector<double> &phen);
void setParsFromPheno(const TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21all";}
void setPhenoNames(); 
//void setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_);

};


class Worm2Dosc21allNML: public Worm2D, public Worm2Dosc21base
{
    public:
    Worm2Dosc21allNML(const json & j, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2Dosc21allNML(const string & jsonfile_, shared_ptr<const CmdArgs> cmd_ = nullptr);
    protected:
    vector<toFromWeight> makeVentralMuscleConn(){return Worm2Dosc21base::makeVentralMuscleConn();}
    vector<toFromWeight> makeDorsalMuscleConn(){return Worm2Dosc21base::makeDorsalMuscleConn();}
    const string getModelName() {return "Worm2Dosc21allNML";}
    void addParsToJson(json & j){j=BPitsJson;Worm2D::addParsToJson(j);}

};

class Worm2Dosc21allNMLm : public Worm2Dm
{

    public:
    Worm2Dosc21allNMLm(const json & j, shared_ptr<const CmdArgs> cmd_ = nullptr);
    Worm2Dosc21allNMLm(const string & jsonfile, shared_ptr<const CmdArgs> cmd_ = nullptr);
    protected:
    const string getModelName() {return "Worm2Dosc21allNMLm";}
};

class Worm2Dosc21S : public Worm2Dosc21
{
public:
Worm2Dosc21S();
Worm2Dosc21S(const string & filename_);
Worm2Dosc21S(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return 5;}

template<class T> friend class EvolutionFullW;
template<class T> friend class Evolvable_ptrB;
protected:
//void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(const TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21S";}
void setPhenoNames(); 
};


class Worm2Dosc21Coup : public Worm2Dosc21
{
public:
Worm2Dosc21Coup();
Worm2Dosc21Coup(const string & filename_);
Worm2Dosc21Coup(TVector<double> & pheno, const bool & isPheno);

void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return cn.weights.size()+4;}

template<class T> friend class EvolutionFullW;
template<class T> friend class Evolvable_ptrB;

protected:
void setPfaFromPheno(const TVector<double> &phen);
void setParsFromPheno(const TVector<double> &phen);
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
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
int getVectSize() {return cn.weights.size() + 14 + 3;}

template<class T> friend class EvolutionFullW;
template<class T> friend class Evolvable_ptrB;
protected:
void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(const TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21CF";}
void setPhenoNames();
};




//Worm2Dosc w;
