#include "Worm2DSR.h"
//#include "../neuromlLocal/c302ForW2D.h"

/* Worm2DSRm::Worm2DSRm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
//Worm2Dm(par1_, n_ptr_, new Muscles),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}
Worm2Dm(par1_, n_ptr_),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}*/

Worm2DSRb::Worm2DSRb(const json & j):w2dsr_ptr(getSR(j)){if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);}
Worm2DSRb::Worm2DSRb(shared_ptr<SR> sr_ptr_):w2dsr_ptr(sr_ptr_){}


Worm2DSR::Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
//Worm2Dm(par1_, n_ptr_, new Muscles),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}
Worm2Dm(par1_, n_ptr_),Worm2D(par1_,n_ptr_),Worm2DSRb(sr_ptr_){} 


Worm2DSR::Worm2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSR(getJsonFromFile(jsonfilename_),cmd){}

Worm2DSR::Worm2DSR(const json & j, shared_ptr<const CmdArgs> cmd):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2Dbaseparameters>(make_shared<W2Dbaseparameters>())),
  Worm2D(getIzqPars(j) ,nullptr),Worm2DSRb(j)
{

    bool do_nml =  cmd->getArgValInt("--donml",0);
    if (!do_nml){

    NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
    setNSFromJson(j,n);

    }

    W2Dbaseparameters1b->setParsFromJson(j["Worm"]);
    setWormPars(cmd);

    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
   
    setMuscBodExt(j);
    //setUpMuscleConn(j);
    //setUpBodyConn(j);
    //makeExternalInputConnFromJson(j);

}

Worm2DSRm::Worm2DSRm(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSRm(getJsonFromFile(jsonfilename_), cmd){}

Worm2DSRm::Worm2DSRm(const json & j, shared_ptr<const CmdArgs> cmd):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2DbaseparametersNML>(make_shared<W2DbaseparametersNML>()), 0),
 Worm2DSRb(j)
{

    W2Dbaseparameters1b->setParsFromJson(j["Worm"]);
    setWormPars(cmd);
    
    //if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);
   
    setBodExt(j);
    //setUpMuscleConn(j);
    //setUpBodyConn(j);
    //makeExternalInputConnFromJson(j);

}



wormIzqParams Worm2DSRb::getIzqPars(const json & j)
{
return
  {j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  };
}

void Worm2DSR::addParsToJson(json & j)
{
  NervousSystem* const n = dynamic_cast<NervousSystem*>(n_ptr);
  if (n!=nullptr){
  string nsHead = "Nervous system";
  appendAllNSJson(j[nsHead], *n);
  }
  Worm2D::addParsToJson(j);
  Worm2DSRb::addParsToJson(j);


}
void Worm2DSRm::addParsToJson(json & j)
{
 Worm2DSRb::addParsToJson(j);
  Worm2Dm::addParsToJson(j);
}

void Worm2DSRb::addParsToJson(json & j)
{
if (w2dsr_ptr!=nullptr) w2dsr_ptr->addParsToJson(j);

}



shared_ptr<SR> Worm2DSRb::getSR(const json & j)
{

    if (j.contains("Stretch receptor")){
    
    if (j["Stretch receptor"]["Type"]["value"] == "SR18") return make_shared<SR18>();

    return make_shared<SRCE>(j["Stretch receptor"]["NSegs"]["value"],j["Stretch receptor"]["NStretch"]["value"]);

    }
    else return nullptr;

}

//json j;
//Worm2DSR w(j);

void Worm2DSR::Step1()
{
  
  zeroAllInputs();
  

  b.StepBody(settedStepSize);

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->updateAll(b);
  
  setExternalInput();
  //setExternalInputOrig();

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->incNS(*n_ptr);

  n_ptr->EulerStep(settedStepSize);
  
  //setMuscleInput();

  if (W2Dbaseparameters1->doOrigMuscInput) setMuscleInputOrig();
  else setMuscleInput();

  setBodyInput();
  
}

void Worm2DSRm::Step1()
{
  
  zeroAllInputs();

  b.StepBody(settedStepSize);

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->updateAll(b);
  
   
  setExternalInput();
  //setExternalInputOrig();

  if (w2dsr_ptr!=nullptr) w2dsr_ptr->incNS(*n_ptr);

  n_ptr->EulerStep(settedStepSize);
  
  //setMuscleInput();


  setBodyInput();
  
}

vector<doubIntParamsHead> Worm2DSRb::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;

    var1.parDoub.head = "Worm";
    var1.parDoub.names = {"variable 1"};
    var1.parDoub.vals = {1.0};

    parvec.push_back(var1);
    return parvec;

}


void Worm2DSRm::writeAct()
{
  
  size_t pos = getPos("act.dat");
  ofstream & ofs = ofsvec[pos];  
  int & tt = tts[pos];

  if (++tt >= dataskips) {
    tt = 0;

    ofs << datatime;
    //ofs << "\nSR: ";
    // Stretch receptors

     if (w2dsr_ptr!=nullptr) w2dsr_ptr->writeAct(ofs);

      
    // Head Neurons
        //ofs << "\nH: ";
        int offset = par1.N_units*par1.N_neuronsperunit;

        for (int i = offset + 1; i <= par1.N_size; i++) {
            ofs <<  " " << n_ptr->NeuronOutput(i);
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

     if (w2dsr_ptr!=nullptr) w2dsr_ptr->writeAct(ofs);

      
    // Head Neurons
        //ofs << "\nH: ";
        int offset = par1.N_units*par1.N_neuronsperunit;

        for (int i = offset + 1; i <= par1.N_size; i++) {
            ofs <<  " " << n_ptr->NeuronOutput(i);
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