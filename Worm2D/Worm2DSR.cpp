#include "Worm2DSR.h"
//#include "../neuromlLocal/c302ForW2D.h"

/* Worm2DSRm::Worm2DSRm(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
//Worm2Dm(par1_, n_ptr_, new Muscles),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}
Worm2Dm(par1_, n_ptr_),Worm2D(par1_,n_ptr_),w2dsr_ptr(sr_ptr_){}*/

Worm2DSRb::Worm2DSRb(const json & j):w2dsr_ptr(getSR(j)){if (w2dsr_ptr!=nullptr) w2dsr_ptr->setParsFromJson(j);}
Worm2DSRb::Worm2DSRb(shared_ptr<SR> sr_ptr_):w2dsr_ptr(sr_ptr_){}


Worm2DSR::Worm2DSR(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SR> sr_ptr_):
Worm2Dm(par1_, n_ptr_),Worm2D(par1_,n_ptr_),Worm2DSRb(sr_ptr_){} 


Worm2DSR::Worm2DSR(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSR(getJsonFromFile(jsonfilename_),cmd){}

Worm2DSRE::Worm2DSRE(const string & jsonfilename_, shared_ptr<const CmdArgs> cmd):
Worm2DSRE(getJsonFromFile(jsonfilename_),cmd){}

Worm2DSRE::Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2Dbaseparameters>(make_shared<W2Dbaseparameters>())),
  Worm2DSR(j,cmd){makeVals(j);}
  

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

/* void Worm2DSRE::setNSEvoFromJson(const json & j, NervousSystem & n)
{
    const json & j2 = j["Nervous system"];
    
    chem_weights_evo = j2["Chemical weights"]["evolvable"].template get< vector<toFromInt> >();
    elec_weights_evo = j2["Electrical weights"]["evolvable"].template get< vector<toFromInt> >();
    biases_evo = j2["biases"]["evolvable"].template get< vector<intPair> >();
    taus_evo = j2["taus"]["evolvable"].template get< vector<intPair> >();
    gains_evo = j2["gains"]["evolvable"].template get< vector<intPair> >();

    
} */

void Worm2DSRE::makeVals(const json & j)
{
  if (!j.contains("Evolvable")) return;

  vector<string> v1;
  v1.push_back("Evolvable");
  v1.push_back("value");
  
  genPhenLims = getEvoVecFromJ<doubDoub>(j,v1);


  for (auto it = j.begin(); it != j.end(); ++it)
    for (auto it2 = it->begin(); it2 != it->end(); ++it2)
      if (it2->contains("evolvable")){ 
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
        //TFIvec.push_back(getEvoVecFromJ<toFromInt>(j,it.key(), it2.key()));
        vector<string> v1;
        v1.push_back(it.key());
        v1.push_back(it2.key());
        TFnames.push_back(v1);
        v1.push_back("evolvable");
        TFIvec.push_back(getEvoVecFromJ<fromToInt>(j,v1));
        
        cout << "k1 " << it.key() << " k2 " << it2.key() << endl;
   
        }
        else 
        {
        //IPvec.push_back(getEvoVecFromJ<intPair>(j,it.key(), it2.key()));
        //IPnames.push_back({it.key(),it2.key()});
        vector<string> v1;
        v1.push_back(it.key());
        v1.push_back(it2.key());
        IPnames.push_back(v1);
        v1.push_back("evolvable");
        IPvec.push_back(getEvoVecFromJ<intPair>(j,v1));
        cout << "k1 " << it.key() << " k2 " << it2.key() << endl;
        }
      }
     //assert(0);
}

void Worm2DSRE::setParsFromPheno(const TVector<double> &pheno)
{


for (int i = 0; i<TFnames.size(); i++)
{

const vector<string> & s1 = TFnames[i];
const vector<fromToInt> & v1 = TFIvec[i];
if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);

if (s1[1]=="Chemical weights")
for (int j = 0; j<v1.size(); j++)
n.SetChemicalSynapseWeight(v1[j].from, v1[j].to, pheno(v1[j].val)); //unity indices
else if (s1[1]=="Electrical weights")
for (int j = 0; j<v1.size(); j++)
n.SetElectricalSynapseWeight(v1[j].from, v1[j].to, pheno(v1[j].val)); //unity indices

}
else if (s1[0]=="Dorsal NMJ")
{
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<dMuscConnvec.size(); k++)
  if ((v1[j].to == dMuscConnvec[k].to) && (v1[j].from == dMuscConnvec[k].w.from))
  {dMuscConnvec[k].w.weight = pheno(v1[j].val);break;}

}

}

for (int i = 0; i<IPnames.size(); i++)
{
const vector<string> & s1 = IPnames[i];
const vector<intPair> & v1 = IPvec[i];
if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
if (s1[1]=="biases")
for (int j = 0; j<v1.size(); j++)
n.SetNeuronBias(v1[j].ind, pheno(v1[j].val));
else if (s1[1]=="taus")
for (int j = 0; j<v1.size(); j++)
n.SetNeuronTimeConstant(v1[j].ind, pheno(v1[j].val));
else if (s1[1]=="gains")
for (int j = 0; j<v1.size(); j++)
n.SetNeuronGain(v1[j].ind, pheno(v1[j].val));
}

}

return;
}

void Worm2DSRE::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

  for (int i = 0; i<genPhenLims.size(); i++)
  {
  assert(genPhenLims[i].val1<=genPhenLims[i].val2);
  phen(i+1) = MapSearchParameter(gen(i+1), genPhenLims[i].val1, genPhenLims[i].val2);
  }

}

void Worm2DSRE::testJson(json & j)
{

  vector<doubDoub> vec;
  vec.push_back({3.0,4.0});
  vec.push_back({-1.0,2.0});
  vec.push_back({-10.0,5.0});
  j["Evolvable"]["value"] = vec; 

  {vector<fromToInt> vec;
  vec.push_back({1,3,1});
  vec.push_back({3,4,2});
  j["Dorsal NMJ"]["weights"]["evolvable"] = vec;
  j["Nervous system"]["Chemical weights"]["evolvable"] = vec;
}

  {vector<intPair> vec;
  vec.push_back({1,3});
  vec.push_back({3,3});
  j["Nervous system"]["biases"]["evolvable"] = vec;
  }

}

void Worm2DSRE::setEvolPars(W2Dparameters & w2par_, string evotype_)
{
    if (evotype_=="Evo21" || evotype_=="Evo21R"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);

    Epars1.dbunit = 10;
    Epars1.vbunit = 13;
    }

}