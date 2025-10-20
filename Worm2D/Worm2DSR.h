#include "StretchReceptor.h"


class Worm2DSR : public Worm2D
{
public:


protected:
//Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_);
Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> w2dsr_ptr_);
shared_ptr<SR> w2dsr_ptr = nullptr;
virtual void Step1();

};