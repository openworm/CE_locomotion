#include "Worm2D.h"


class Worm2Dosc : public Worm2D
{

public:
Worm2Dosc();
const string getModelName() {return "Worm2Dosc";}
const vector<string> getCellNames() {return;}
void InitializeState(RandomState &rs) {return;}
void initForSimulation(RandomState &) {return;}
void DumpParams(ofstream &ofs) {return;}

protected:
vector<doubIntParamsHead> getWormParams() {return;}
void Step1() {return;}
const vector<string> getVMuscNames() {return;}
const vector<string> getDMuscNames() {return;}

vector<toFromWeight> makeVentralMuscleConn() {return;}
vector<toFromWeight> makeDorsalMuscleConn() {return;}

};

Worm2Dosc w;