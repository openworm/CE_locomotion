#include "WormCO18.h"

void WormCO18::InitializeState(RandomState &rs)
{
    Worm18::InitializeState(rs);
    WormAgent::InitializeState(rs);
}

void WormCO18::writeData(){
    
    Worm18::writeData();
    dataskips;
    WormAgent::writeData();
}

void WormCO18::addParsToJson(json & j)
{
    Worm18::addParsToJson(j["RS18"]);
    WormAgent::addParsToJson(j["CO"]);
}

void WormCO18::Step1(double StepSize)
{
  

    UpdateSensors();
    WormAgent::preNStep(StepSize, HStimestep);
    Worm18::preNStep(StepSize);

  
    Worm18::n.EulerStep(StepSize);
    WormAgent::n.EulerStep(StepSize);
    
    WormAgent::postNStep(StepSize,rs,taxis,kinesis);
    Worm18::postNStep(StepSize);
    moveAgent(StepSize);
    UpdateChemCon(gradSteep);

}
