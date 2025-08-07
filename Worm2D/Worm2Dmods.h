#include "Worm2D.h"


class Worm2Dosc : public Worm2D
{

public:
Worm2Dosc(TVector<double> &v);
Worm2Dosc::Worm2Dosc(const pfa & pfa_);
const string getModelName() {return "Worm2Dosc";}
const vector<string> getCellNames() {return;}
void InitializeState(RandomState &rs);
void initForSimulation(RandomState &) {return;}
void DumpParams(ofstream &ofs) {return;}
pfa getPfaFromPheno(TVector<double> &v){}

protected:
vector<doubIntParamsHead> getWormParams() {return;}
void Step1();
const vector<string> getVMuscNames() {return;}
const vector<string> getDMuscNames() {return;}

vector<toFromWeight> makeVentralMuscleConn() {return;}
vector<toFromWeight> makeDorsalMuscleConn() {return;}


NSosc & n;

};

struct pfa{
vector<double> phase, freq, amp;
int size;
};

class NSosc : public NSForW2D {

public:

NSosc(const pfa & pfa_);

double NeuronOutput(int i) {return pfa1.amp[i]*sin(pi2*pfa1.freq[i]*t + pfa1.phase[i]);}
double NeuronState(int i) {return 0;}
void SetNeuronExternalInput(int i, double value) {return;}
void EulerStep(double stepsize) {return;}
virtual ~NSosc(){};
void setTime(const double & t_){t=t_;}

private:
double t;
pfa pfa1;
//vector<double> phase, freq, amp;
const double pi2 = 3.14159265*2.0;

};