#include "Worm2D.h"
//#include "Evolvable.h"

const double pi2 = 3.14159265*2.0;

//struct evoPars;

class pfa{

    public:
vector<double> phase, freq, amp;
int size;
void swap_all(pfa & pfa_);
void addParsToJson(json & j);

};


class NSosc : public NSForW2D {

public:

NSosc(const pfa & pfa_);
NSosc(){}

double NeuronOutput(int i) {return pfa1.amp[i-1]*sin(pi2*pfa1.freq[i-1]*t + pfa1.phase[i-1]);}
double NeuronState(int i) {return 0;}
void SetNeuronExternalInput(int i, double value) {return;}
void EulerStep(double stepsize) {t+=stepsize;}
virtual ~NSosc(){};
void setTime(const double & t_){t=t_;}
const pfa & itsPfa() const {return pfa1;}
 
//friend class Worm2DoscBase<Worm2Dosc>;
friend class Worm2Dosc;
friend class Worm2Dosc21D;
friend class Worm2Dosc21allD;
friend class Worm2DoscHalf;

pfa pfa1;
private:
double t;

//vector<double> phase, freq, amp;


};

class CoupledOsc : public NSosc
{
    
    private:
    vector<weightentry> weights;

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

template<typename D>
class Worm2DoscBase : public Worm2DPars, public Evolvable<D>
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




class Worm2Dosc : public Worm2DoscBase<Worm2Dosc>, public Worm2Dosc1
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
static void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
void setPhenoNames(); //{return;}
static int getVectSize(){return 4;}


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


class Worm2DoscHalf : public Worm2Dosc, virtual public Evolvable<Worm2DoscHalf>
{
public:
Worm2DoscHalf();
Worm2DoscHalf(const string & filename_);
//Worm2DoscHalf(const pfa & pfa_, const Worm2Doscpars & pars1_);
Worm2DoscHalf(TVector<double> & pheno, const bool & isPheno);
static void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
//static inline int evoVectSize = 3;
static int getVectSize() {return 3;}


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

template<typename D>
class Worm2Dosc21 : virtual public Worm2DoscBase<D>, public Worm2Dosc21base
{

public:
Worm2Dosc21();
Worm2Dosc21(const string & filename_);
Worm2Dosc21(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
static void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
static int getVectSize() {return 6;}


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

class Worm2Dosc21D : public Worm2Dosc21<Worm2Dosc21D>
{
public:
Worm2Dosc21D(const string & filename_);
Worm2Dosc21D(TVector<double> & pheno, const bool & isPheno);
};


template<typename D>
class Worm2Dosc21all : public Worm2Dosc21<D>, virtual public Worm2DoscBase<D>
{
public:
Worm2Dosc21all(const string & filename_);
Worm2Dosc21all(TVector<double> & pheno, const bool & isPheno);
//static inline int evoVectSize = 6;
static void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
static int getVectSize() {return 18;}
protected:
void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);
const string getModelName() {return "Worm2Dosc21all";}
void setPhenoNames(); 
};

class Worm2Dosc21allD : public Worm2Dosc21all<Worm2Dosc21allD>
{
public:
Worm2Dosc21allD(const string & filename_);
Worm2Dosc21allD(TVector<double> & pheno, const bool & isPheno);
};


//Worm2Dosc w;


template<typename D>
void Worm2DoscBase<D>::addParsToJson(json & j)
{
    Worm2DPars::addParsToJson(j);
    n.pfa1.addParsToJson(j["Nervous system"]);

}

template<typename D>
void Worm2DoscBase<D>::construct(const string & filename_)
{
    setPfaFromFile(filename_);
    setParsFromFile(filename_);
    setUpMuscleConn();
    n.setTime(t);
}

template<typename D>
void Worm2DoscBase<D>::constructFromGeno(TVector<double> &geno)
{
    setPfaFromGeno(geno);
    setParsFromGeno(geno);
    setUpMuscleConn();
    n.setTime(t);

}

template<typename D>
void Worm2DoscBase<D>::construct(TVector<double> &pheno)
{
    setPfaFromPheno(pheno);
    setParsFromPheno(pheno);
    setUpMuscleConn();
    n.setTime(t);

}


template<typename D>
Worm2DoscBase<D>::Worm2DoscBase(wormIzqParams par1_, W2Dparameters * w2par_ptr):
Worm2DPars(par1_, 0, w2par_ptr), Worm2Dm(par1_, new NSosc()),
Epars1(dynamic_cast<Evolparameters&>(*pars1_ptr)),
n(dynamic_cast<NSosc&>(*n_ptr)){}


template<typename D>
void Worm2DoscBase<D>::setPfaFromFile(const string & genofilename_)
{
 
    ifstream ifs;
    ifs.open(genofilename_);
    TVector<double> bestVector(1, this->itsVectSize());
    ifs >> bestVector;
    ifs.close();
  
    setPfaFromGeno(bestVector);

}

template<typename D>
void Worm2DoscBase<D>::setPfaFromGeno(TVector<double> &v)
{ 
 TVector<double> phenotype(1, v.Size());
 this->itsGenPhenMapping(v, phenotype);
 setPfaFromPheno(phenotype);
}


template<typename D>
void Worm2DoscBase<D>::setParsFromFile(const string & filename_)
{
    ifstream ifs;
    ifs.open(filename_);
    TVector<double> bestVector(1, this->itsVectSize());
    //assert(0);
    ifs >> bestVector;
    ifs.close();
    setParsFromGeno(bestVector);
}



template<typename D>
void Worm2DoscBase<D>::setParsFromGeno(TVector<double> &v)
{
    
    //cout << v << endl;
    TVector<double> phenotype(1, v.Size());
    //cout << phenotype.Size() << endl;
    this->itsGenPhenMapping(v, phenotype);
 
    setParsFromPheno(phenotype);
 
}


template<typename D>
void Worm2Dosc21<D>::setPfaFromPheno(TVector<double> &phen)
{

pfa pfa1;
pfa1.size = 14;
vector<double> phase_1(14,0);

for (int unit = 1; unit<=7; unit++){
const int neuron_d = this->nn(1,unit) - 1;
phase_1[neuron_d] = phen[1]*(unit-1);
const int neuron_v = this->nn(2,unit) - 1;
phase_1[neuron_v] = phen[1]*(unit-1) + phen[2];
}
pfa1.phase.swap(phase_1);

for (int i = 1; i<=14; i++) {pfa1.freq.push_back(phen[3]);pfa1.amp.push_back(1);}
this->n.pfa1.swap_all(pfa1);

}
template<typename D>
void Worm2Dosc21all<D>::setPfaFromPheno(TVector<double> &phen)
{

pfa pfa1;
pfa1.size = 14;
vector<double> phase_1(14,0);

for (int unit = 1; unit<=7; unit++){
const int neuron_d = this->nn(1,unit) - 1;
phase_1[neuron_d] = phen[unit];
const int neuron_v = this->nn(2,unit) - 1;
phase_1[neuron_v] = phen[unit+7];
}
pfa1.phase.swap(phase_1);

for (int i = 1; i<=14; i++) {pfa1.freq.push_back(phen[15]);pfa1.amp.push_back(1);}
this->n.pfa1.swap_all(pfa1);

}
template<typename D>
void Worm2Dosc21<D>::setParsFromPheno(TVector<double> &phen, int offset)
{
    pars1.NMJ_Gain_Map = phen[offset];
    //pars1.NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=this->par1.N_muscles; i++)
    {
    pars1.NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*pars1.NMJ_Gain_Map)/this->par1.N_muscles));
    }
    
    pars1.NMJ_VN = phen[offset+1];
    pars1.NMJ_DN = phen[offset+2];
    pars1.dbunit = this->nn(1,3);
    pars1.vbunit = this->nn(2,3);
   
}

template<typename D>
void Worm2Dosc21<D>::setParsFromPheno(TVector<double> &phen)
{
    setParsFromPheno(phen,4);

}
template<typename D>
void Worm2Dosc21all<D>::setParsFromPheno(TVector<double> &phen)
{
    Worm2Dosc21<D>::setParsFromPheno(phen,16);
   
}
template<typename D>
void Worm2Dosc21<D>::setPhenoNames(){
    this->addPhenoName("phase offset", 1);
    this->addPhenoName("DV phase offset", 2);
    this->addPhenoName("freq", 3);
    this->addPhenoName("NMJ Gain map", 4);
    this->addPhenoName("D_NMJ weight", 5);
    this->addPhenoName("V_NMJ weight", 6);
}

template<typename D>
void Worm2Dosc21<D>::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{

    cout << "GenPhenMapping" << endl;
    //assert(0);
    const double NMJweight_top = 10;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    
    phen(1) = MapSearchParameter(gen(1), 0, pi2);
    
    phen(2) = MapSearchParameter(gen(2), 0, pi2);
    
    phen(3) = MapSearchParameter(gen(3), freq_lo, freq_hi);
    //weight
    
    phen(4) = MapSearchParameter(gen(4), 0.2, 1.0); //from Net21
    //phen(4) = MapSearchParameter(gen(4), 0.0, 0.1);

    //phen(5) = MapSearchParameter(gen(5), NMJweight_top*-1, NMJweight_top);
    phen(5) = MapSearchParameter(gen(5), 0, NMJweight_top);


    //phen(6) = MapSearchParameter(gen(6), NMJweight_top*-1, NMJweight_top);
    phen(6) = MapSearchParameter(gen(6), 0, NMJweight_top);
    
   cout << "GenPhenMapping" << endl;

}
template<typename D>
void Worm2Dosc21all<D>::setPhenoNames(){
    for (int unit=1; unit<=7; unit++)
    this->addPhenoName("D phase offset", unit);
    for (int unit=8; unit<=14; unit++)
    this->addPhenoName("V phase offset", unit);
    
    this->addPhenoName("freq", 15);
    this->addPhenoName("NMJ Gain map", 16);
    this->addPhenoName("D_NMJ weight", 17);
    this->addPhenoName("V_NMJ weight", 18);
}


template<typename D>
void Worm2Dosc21all<D>::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
   
    cout << "GenPhenMapping" << endl;
    //assert(0);
    const double NMJweight_top = 10;
    const double freq_lo = 0.01;
    const double freq_hi = 4;
    //phases_lag
    
    for (int unit=1;unit<=14;unit++) 
    phen(unit) = MapSearchParameter(gen(unit), 0, pi2);
    
    phen(15) = MapSearchParameter(gen(15), freq_lo, freq_hi);
    //weight
    

    phen(16) = MapSearchParameter(gen(16), 0.2, 1.0); //from Net21
    phen(17) = MapSearchParameter(gen(17), 0, NMJweight_top);
    phen(18) = MapSearchParameter(gen(18), 0, NMJweight_top);
    
   cout << "GenPhenMapping" << endl;

}


template<typename D>
Worm2Dosc21<D>::Worm2Dosc21():
Worm2DoscBase<D>({2,24,0.1,7,14}, new Worm2Dosc21pars()),
Worm2Dosc21base(this->par1, dynamic_cast<Worm2Dosc21pars&>(*(this->pars1_ptr))),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    pars1.NMJ_Gain.SetBounds(1, this->par1.N_muscles);
}




template<typename D>
Worm2Dosc21<D>::Worm2Dosc21(const string & filename_):
Worm2DoscBase<D>({2,24,0.1,7,14}, new Worm2Dosc21pars()),
Worm2Dosc21base(this->par1, dynamic_cast<Worm2Dosc21pars&>(*(this->pars1_ptr))),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    this->pars1.NMJ_Gain.SetBounds(1, this->par1.N_muscles);
    this->construct(filename_);
}
template<typename D>
Worm2Dosc21<D>::Worm2Dosc21(TVector<double> & pheno, const bool & isPheno):
Worm2DoscBase<D>({2,24,0.1,7,14}, new Worm2Dosc21pars()),
Worm2Dosc21base(this->par1, dynamic_cast<Worm2Dosc21pars&>(*(this->pars1_ptr))),Worm2Dm({2,24,0.1,7,14},new NSosc())
{
    this->pars1.NMJ_Gain.SetBounds(1, this->par1.N_muscles);
    if (isPheno) this->construct(pheno);
    else this->constructFromGeno(pheno);
}

template<typename D>
Worm2Dosc21all<D>::Worm2Dosc21all(const string & filename_):
Worm2Dosc21<D>(),Worm2Dm({2,24,0.1,7,14},new NSosc()),
Worm2DoscBase<D>({2,24,0.1,7,14}, new Worm2Dosc21pars())
{
    this->pars1.NMJ_Gain.SetBounds(1, this->par1.N_muscles);
    this->construct(filename_);

}

template<typename D>
Worm2Dosc21all<D>::Worm2Dosc21all(TVector<double> & pheno, const bool & isPheno):
Worm2Dosc21<D>(),Worm2Dm({2,24,0.1,7,14},new NSosc()),
Worm2DoscBase<D>({2,24,0.1,7,14}, new Worm2Dosc21pars())
{
    this->pars1.NMJ_Gain.SetBounds(1, this->par1.N_muscles);
    if (isPheno) this->construct(pheno);
    else this->constructFromGeno(pheno);

}