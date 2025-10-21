#include "Worm2DSR.h"


Worm2DSR::Worm2DSR(const string & jsonfilename_):
Worm2DSR(getJsonFromFile(jsonfilename_)){}

Worm2DSR::Worm2DSR(json j):Worm2Dm(
  {j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  }, new NervousSystem()),
  Worm2D({j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  } ,0),w2dsr_ptr(make_shared<SR>(j["Stretch receptor"]["NSegs"]["value"],
j["Stretch receptor"]["NStretch"]["value"]))
{
    NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);

    
    W2Dbaseparameters1->setParsFromJson(j["Worm"]);
    
  //W2DCEpars1->setParsFromJson(j["Worm"]);
  //w2dsr_ptr->setParsFromJson(j);

  //NMJ_DA = j["Worm"]["NMJ_DA"]["value"];
  //NMJ_VA = j["Worm"]["NMJ_VA"]["value"];
  //NMJ_DB = j["Worm"]["NMJ_DB"]["value"];
  //NMJ_VB = j["Worm"]["NMJ_VB"]["value"];

  // Inhibitory VNC NMJ Weight
  //NMJ_DD = j["Worm"]["NMJ_DD"]["value"];
  //NMJ_VD = j["Worm"]["NMJ_VD"]["value"];

    setNSFromJson(j,n);
  
   
    assert(n.size == par1.N_units*par1.N_neuronsperunit);
    assert(n.maxchemconns == 3);
    assert(n.maxelecconns == 2);
  
    w2dsr_ptr->makeWeightsFromJson(j);
   

  //sr_ptr->setNSWeights(*this);
  //sr_ptr->setNSWeights(shared_ptr<const Worm2DCE>(this));

    setUpMuscleConn(j);
    setUpBodyConn(j);
    makeExternalInputConnFromJson(j);

  
}

//json j;
//Worm2DSR w(j);


vector<doubIntParamsHead> Worm2DSR::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;

    var1.parDoub.head = "Worm";
    var1.parDoub.names = {"variable 1"};
    var1.parDoub.vals = {1.0};

    parvec.push_back(var1);
    return parvec;

}