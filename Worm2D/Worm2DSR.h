#include "StretchReceptor.h"


class Worm2DSR : public Worm2D
{
public:
Worm2DSR(json j);
Worm2DSR(const string & jsonfilename_);
void setWormPars(shared_ptr<const CmdArgs> cmd){
  Worm2D::setWormPars(cmd);
}

protected:

Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
shared_ptr<SR> w2dsr_ptr = nullptr;
virtual void Step1();
const string getModelName() {return "W2DSR";}
vector<doubIntParamsHead> getWormParams();

};

