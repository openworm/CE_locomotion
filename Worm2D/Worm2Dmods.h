#include "Worm2D.h"
//#include "Evolvable.h"

const double pi2 = 3.14159265*2.0;

//struct evoPars;

struct pfa{
vector<double> phase, freq, amp;
int size;
};


class NSosc : public NSForW2D {

public:

NSosc(const pfa & pfa_);
NSosc(){}

double NeuronOutput(int i) {return pfa1.amp[i-1]*sin(pi2*pfa1.freq[i-1]*t + pfa1.phase[i-1]);}
double NeuronState(int i) {return 0;}
void SetNeuronExternalInput(int i, double value) {return;}
void EulerStep(double stepsize) {return;}
virtual ~NSosc(){};
void setTime(const double & t_){t=t_;}
const pfa & itsPfa() const {return pfa1;}
 
friend class Worm2DoscBase;
friend class Worm2Dosc;
friend class Worm2DoscHalf;
private:
double t;
pfa pfa1;
//vector<double> phase, freq, amp;


};

class W2Dparameters
{
public:
double NMJweight;
int dbunit;
int vbunit;
virtual ~W2Dparameters(){}
};
class Worm2Doscpars : public W2Dparameters
{
public:

};

class Worm2DoscBase : public Worm2D, public Evolvable
{
public:
void InitializeState(RandomState &rs);
void initForSimulation(RandomState &) {return;}
void DumpParams(ofstream &ofs) {return;}

double EvaluationFunction(TVector<double> &v, RandomState &rs);
void writeJson(TVector<double> &);
evoPars getDefaultEvoPars();

virtual void setPfaFromPheno(TVector<double> &v) = 0;
virtual void setParsFromPheno(TVector<double> &v) = 0;
void setPfaFromGeno(TVector<double> &v);
void setParsFromGeno(TVector<double> &v);
void setPfaFromFile(const string & filename_);
void setParsFromFile(const string & filename_);

protected:

void Step1();
const vector<string> getVMuscNames() {return {"not implemented"};}
const vector<string> getDMuscNames() {return {"not implemented"};}





virtual ~Worm2DoscBase(){if (pars1_ptr) delete pars1_ptr;}
Worm2DoscBase(W2Dparameters * w2par_ptr, int size_):Worm2D({size_,24,0.1,1,size_},0),
n(dynamic_cast<NSosc&>(*n_ptr)),pars1_ptr(w2par_ptr){}

NSosc & n;
W2Dparameters * const pars1_ptr = nullptr;



};

class Worm2Dosc : public Worm2DoscBase
{
public:
Worm2Dosc(const Worm2Dosc&);
Worm2Dosc(int size_);
Worm2Dosc(const string & filename_);
Worm2Dosc();
//Worm2Dosc(TVector<double> &v);
Worm2Dosc(const pfa & pfa_, const Worm2Doscpars & par1_);
const string getModelName() {return "Worm2Dosc";}
const vector<string> getCellNames() {return {"not implemented"};}
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);

int getVectSize(){return 4;}

protected:

void setPfaFromPheno(TVector<double> &v);
void setParsFromPheno(TVector<double> &v);

vector<doubIntParamsHead> getWormParams() {
    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    parvec.push_back(var1);
    return parvec;
}

vector<toFromWeight> makeVentralMuscleConn();
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeDVMuscleConn(int offset);



//virtual ~Worm2Dosc(){if (pars1_ptr) delete pars1_ptr;}

//W2Dparameters * const pars1_ptr = nullptr;
Worm2Doscpars & pars1;
//const int dbunit = 6;
//const int vbunit = 30;
//const int vectsize = 4;

//NSosc & n;

};


class Worm2DoscHalf : public Worm2Dosc
{
public:
Worm2DoscHalf();
Worm2DoscHalf(const string & filename_);
Worm2DoscHalf(const pfa & pfa_, const Worm2Doscpars & pars1_);
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);

protected:
vector<toFromWeight> makeDVMuscleConn(double);
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeVentralMuscleConn();
int getVectSize() {return 3;}

void setPfaFromPheno(TVector<double> &phen);
void setParsFromPheno(TVector<double> &phen);




};

class Worm2Dosc21 : public Worm2Dosc
{

public:
vector<toFromWeight> makeMuscleConn(double);
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeVentralMuscleConn();

};


//Worm2Dosc w;