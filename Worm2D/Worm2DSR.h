#include "StretchReceptor.h"
#include "../neuromlLocal/c302ForW2D.h"

class Worm2DSR : public Worm2D
{
public:
Worm2DSR(json j, shared_ptr<const CmdArgs> cmd);
Worm2DSR(json j);
Worm2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd);
void setWormPars(shared_ptr<const CmdArgs> cmd){
  Worm2D::setWormPars(cmd);
}

void writeAct();
protected:

Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
shared_ptr<SR> w2dsr_ptr = nullptr;
virtual void Step1();
const string getModelName() {return "W2DSR";}
vector<doubIntParamsHead> getWormParams();
static shared_ptr<SR> getSR(json & j);
static NSForW2D * getNS(shared_ptr<const CmdArgs> cmd);

};

