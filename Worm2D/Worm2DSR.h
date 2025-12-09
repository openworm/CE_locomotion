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
};

class Worm2DSRm : public Worm2Dm, public Worm2DSRb
{
public:
//Worm2DSRm(json & j, shared_ptr<const CmdArgs> cmd);
Worm2DSRm(const json & j);
Worm2DSRm(const string & jsonfilename_);
void setWormPars(shared_ptr<const CmdArgs> cmd){
  Worm2Dm::setWormPars(cmd);
}

void writeAct();
protected:

//Worm2DSRm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
//shared_ptr<SR> w2dsr_ptr = nullptr;
virtual void Step1();
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
void setWormPars(shared_ptr<const CmdArgs> cmd){
  Worm2D::setWormPars(cmd);
}

void writeAct();

//void writeAct(){return Worm2DSRm::writeAct();}
protected:

Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
//shared_ptr<SR> w2dsr_ptr = nullptr;
virtual void Step1();
const string getModelName() {return "W2DSR";}
//vector<doubIntParamsHead> getWormParams();
//static shared_ptr<SR> getSR(json & j);
static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd);
//static wormIzqParams getIzqPars(json & j);
};

