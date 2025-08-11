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
 
friend class Worm2Dosc;
private:
double t;
pfa pfa1;
//vector<double> phase, freq, amp;


};


struct Worm2Doscpars
{
double NMJweight;
};



class Worm2Dosc : public Worm2D, public Evolvable
{

public:
Worm2Dosc();
Worm2Dosc(TVector<double> &v);
Worm2Dosc(const pfa & pfa_, const Worm2Doscpars & par1_);
const string getModelName() {return "Worm2Dosc";}
const vector<string> getCellNames() {return {"not implemented"};}
void InitializeState(RandomState &rs);
void initForSimulation(RandomState &) {return;}
void DumpParams(ofstream &ofs) {return;}
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen);
double EvaluationFunction(TVector<double> &v, RandomState &rs);
void writeJson(TVector<double> &);
evoPars getEvoPars();
int getVectSize();

protected:
pfa getPfaFromPheno(TVector<double> &v);
Worm2Doscpars getParsFromPheno(TVector<double> &v);
pfa getPfaFromGeno(TVector<double> &v);
Worm2Doscpars getParsFromGeno(TVector<double> &v);

vector<doubIntParamsHead> getWormParams() {
    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    parvec.push_back(var1);
    return parvec;
}

void Step1();
const vector<string> getVMuscNames() {return {"not implemented"};}
const vector<string> getDMuscNames() {return {"not implemented"};}

vector<toFromWeight> makeVentralMuscleConn();
vector<toFromWeight> makeDorsalMuscleConn();
vector<toFromWeight> makeDVMuscleConn(int offset);

NSosc & n;
Worm2Doscpars pars1;

};





//Worm2Dosc w;