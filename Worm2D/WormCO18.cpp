#include "WormCO18.h"

void WormCO18::InitializeState(RandomState &rs)
{
    InitializeState(rs);
    wa->InitializeState(rs);
}

void WormCO18::writeData(){
    
    writeData();
    dataskips;
    wa->writeData();
}

void WormCO18::addParsToJson(json & j)
{
    addParsToJson(j["RS18"]);
    wa->addParsToJson(j["CO"]);
}

void WormCO18::Step1(double StepSize)
{
  

    wa->UpdateSensors();
    wa->preNStep(StepSize);
    preNStep(StepSize);

  
    n_ptr->EulerStep(StepSize);
    wa->itsNS().EulerStep(StepSize);
    
    wa->postNStep(StepSize);
    postNStep(StepSize);
    wa->moveAgent(StepSize);
    wa->UpdateChemCon(gradSteep);

}
