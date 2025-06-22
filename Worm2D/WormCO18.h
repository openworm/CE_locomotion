#include "WormRS18.h"
#include "WormAgent.h"

class WormCO18 : public Worm18
{

public:

WormCO18::WormCO18(TVector<double> &v18, TVector<double> &vw, int newsize_):
Worm18(v18,1),wa(new WormAgent(vw,newsize_)),Worm2Dm({6,24,0.1,6,40}, new NervousSystem(), new Muscles)
{}

void addParsToJson(json & j);

void writeData();

void Step1(double StepSize);

void InitializeState(RandomState &rs);

protected:

WormAgent *wa;
};
