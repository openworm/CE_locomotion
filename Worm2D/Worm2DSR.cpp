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
  } ,0),w2dsr_ptr(getSR(j))
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
  
    if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
   

  //sr_ptr->setNSWeights(*this);
  //sr_ptr->setNSWeights(shared_ptr<const Worm2DCE>(this));

    setUpMuscleConn(j);
    setUpBodyConn(j);
    makeExternalInputConnFromJson(j);

  
}


shared_ptr<SR> Worm2DSR::getSR(json & j)
{

    if (j.contains("Stretch receptor"))
    return make_shared<SR>(j["Stretch receptor"]["NSegs"]["value"],j["Stretch receptor"]["NStretch"]["value"]);
    else return nullptr;

}

//json j;
//Worm2DSR w(j);

void Worm2DSR::Step1()
{
  
    
  b.StepBody(settedStepSize);

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->updateAll(b);
  
   
  setExternalInput();
  //setExternalInputOrig();

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->incNS(*n_ptr);

  n_ptr->EulerStep(settedStepSize);
  
  setMuscleInput();

  //if (W2Dbaseparameters1->doOrigMuscInput) setMuscleInputOrig();
  //else setMuscleInput();

  setBodyInput();
  
}



vector<doubIntParamsHead> Worm2DSR::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;

    var1.parDoub.head = "Worm";
    var1.parDoub.names = {"variable 1"};
    var1.parDoub.vals = {1.0};

    parvec.push_back(var1);
    return parvec;

}


void Worm2DSR::writeAct()
{
  
  size_t pos = getPos("act.dat");
  ofstream & ofs = ofsvec[pos];  
  int & tt = tts[pos];

  if (++tt >= dataskips) {
    tt = 0;

    ofs << datatime;
    //ofs << "\nSR: ";
    // Stretch receptors

     if (w2dsr_ptr!=nullptr){

    for (int i = 1; i <= w2dsr_ptr->srvars.nstretch; i++) {
      //ofs <<  " " << sr_ptr->A_D_sr(i) << " " << sr_ptr->A_V_sr(i) << " " << sr_ptr->B_D_sr(i) << " " << sr_ptr->B_V_sr(i);
      ofs <<  " " << w2dsr_ptr->srvars.A_D_sr[i-1] << " " << w2dsr_ptr->srvars.A_V_sr[i-1] << " " 
      << w2dsr_ptr->srvars.B_D_sr[i-1] << " " << w2dsr_ptr->srvars.B_V_sr[i-1];
    }

    }

    // Ventral Cord Motor Neurons
    //ofs << "\nV: ";
    for (int i = 1; i <= par1.N_units; i++) {
      for (int j = 1; j <= par1.N_neuronsperunit; j++) {
        ofs <<  " " << n_ptr->NeuronOutput(nn(j,i));
      }
    }

    // Muscles
    //ofs << "\nM: ";
    if (m_ptr){
    for (int i = 1; i <= par1.N_muscles; i++) {
      ofs <<  " " << m_ptr->DorsalMuscleOutput(i) << " " << m_ptr->VentralMuscleOutput(i);
    }}
    ofs << endl;
  }
}