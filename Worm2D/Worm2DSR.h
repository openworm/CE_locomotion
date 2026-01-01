#include "StretchReceptor.h"
#include "../neuromlLocal/c302ForW2D.h"


class Worm2DSRb
{

//void writeAct();
protected:

vector<doubIntParamsHead> getWormParams();
static shared_ptr<SR> getSR(const json & j);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd);
static wormIzqParams getIzqPars(const json & j);
shared_ptr<SR> w2dsr_ptr = nullptr;
Worm2DSRb(const json & j);
Worm2DSRb(shared_ptr<SR> sr_ptr_);
void addParsToJson(json & j);
};

class Worm2DSRm : public Worm2Dm, public Worm2DSRb
{
public:
//Worm2DSRm(json & j, shared_ptr<const CmdArgs> cmd);
Worm2DSRm(const json & j, shared_ptr<const CmdArgs> cmd);
Worm2DSRm(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);

//void setWormPars(shared_ptr<const CmdArgs> cmd){Worm2Dm::setWormPars(cmd);}

void addParsToJson(json & j);
void writeAct();
protected:

//Worm2DSRm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
//shared_ptr<SR> w2dsr_ptr = nullptr;
void Step1();
const string getModelName() {return "W2DSRm";}
//vector<doubIntParamsHead> getWormParams();
//static shared_ptr<SR> getSR(json & j);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd);
//static wormIzqParams getIzqPars(json & j);
};


class Worm2DSR : public Worm2D, public Worm2DSRb
{
public:
Worm2DSR(const json & j, shared_ptr<const CmdArgs> cmd);
//Worm2DSR(json & j);
Worm2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);

//void setWormPars(shared_ptr<const CmdArgs> cmd){Worm2D::setWormPars(cmd);}

void addParsToJson(json & j);

void writeAct();

//void writeAct(){return Worm2DSRm::writeAct();}
protected:

Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
//shared_ptr<SR> w2dsr_ptr = nullptr;
void Step1();
const string getModelName() {return "W2DSR";}
//vector<doubIntParamsHead> getWormParams();
//static shared_ptr<SR> getSR(json & j);
//static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd, const json & j);
//static wormIzqParams getIzqPars(json & j);



};

class Worm2DSRE : public Worm2DSR, public EvolvableS
{
    public:
Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit = false);
//Worm2DSR(json & j);
Worm2DSRE(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);

void setEvolPars(W2Dparameters & w2par_, string evotype_);
void setParsFromPheno(const TVector<double> &pheno);
int getVectSize() {return genPhenLims.size();}
void GenPhenMapping(const TVector<double> &gen, TVector<double> &phen);
void PhenGenMapping(vector<double> &gen, const vector<double> &phen);
//void setNSEvoFromJson(const json & j, NervousSystem & n);
void makeVals(const json & j);
void testJson(json & j);
//void setInitGeno();
vector<double> getInitGeno();
void addEvolvableToJson(json & j);
void writeOrigGen(shared_ptr<const CmdArgs> cmd);
//vector<toFromInt> chem_weights_evo, elec_weights_evo;
//vector<intPair> biases_evo, taus_evo, gains_evo;
vector<doubDoub> genPhenLims;
vector<vector<string> > TFnames, IPnames;
vector<vector<fromToInt> > TFIvec;
vector<vector<intPair> > IPvec;
//vector<double> initialGeno;
};
