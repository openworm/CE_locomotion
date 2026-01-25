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

Worm2DSRE::Worm2DSRE(const json & j, shared_ptr<const CmdArgs> cmd, bool callInit):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2Dbaseparameters>(make_shared<W2Dbaseparameters>())),
  Worm2DSR(j,cmd),genPhenPars(makeVals(j)){
    if (callInit) writeOrigGen(cmd);
  }
  

Worm2DSR::Worm2DSR(const json & j, shared_ptr<const CmdArgs> cmd):Worm2Dm(getIzqPars(j),
  getNS(cmd, j), shared_ptr<W2Dbaseparameters>(make_shared<W2Dbaseparameters>())),
  Worm2D(getIzqPars(j) ,nullptr),Worm2DSRb(j)
{

    bool do_nml =  cmd->getArgValInt("--donml",0);
    if (!do_nml){

    NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
    setNSFromJson(j,n);

    if (j["Nervous system"].contains("section sizes"))
      jsects = j["Nervous system"]["section sizes"];

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




void Worm2DSR::addParsToJson(json & j)
{
  NervousSystem* const n = dynamic_cast<NervousSystem*>(n_ptr);
  if (n!=nullptr){
  string nsHead = "Nervous system";
  appendAllNSJson(j[nsHead], *n);
  
  j[nsHead]["section sizes"] = jsects;
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


      writeVNC(ofs);
     
        // Muscles
        //ofs << "\nM: ";
      writeMusc(ofs);
        
      writeExtInp(ofs);

    

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

    for (int i = offset + 1; i <= par1.N_size; i++) 
        ofs <<  " " << n_ptr->NeuronOutput(i);
    
      writeVNC(ofs);
     
        // Muscles
        //ofs << "\nM: ";
      writeMusc(ofs);
        
      writeExtInp(ofs);

   
    // Muscles
    //ofs << "\nM: ";
    
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

void Worm2DSRE::addEvolvableToJson(json & j)
{
  
  j["Evolvable"]["value"] =  genPhenPars.genPhenLims;
  for (int i = 0; i<genPhenPars.TFnames.size();i++)
  {
    json j2;
    j2 = genPhenPars.TFIvec[i];
    vector<string> TFi = genPhenPars.TFnames[i];
    TFi.push_back("evolvable");
    set_nested_json(j, TFi, j2);
  }
  for (int i = 0; i<genPhenPars.IPnames.size();i++)
  {
    json j2;
    j2 = genPhenPars.IPvec[i];
    vector<string> TFi = genPhenPars.IPnames[i];
    TFi.push_back("evolvable");
    set_nested_json(j, TFi, j2);
  }
   for (int i = 0; i<genPhenPars.singValnames.size();i++)
  {
    json j2;
    j2 = genPhenPars.singVals[i];
    vector<string> TFi = genPhenPars.singValnames[i];
    TFi.push_back("evolvable");
    set_nested_json(j, TFi, j2);
  }

}

Worm2DSREpars Worm2DSRE::makeVals(const json & j)
{

  if (!j.contains("Evolvable")) return Worm2DSREpars();

  Worm2DSREpars w1pars;

  //assert(0);
  vector<string> v1;
  v1.push_back("Evolvable");
  v1.push_back("value");
  
  w1pars.genPhenLims = getEvoVecFromJ<doubDoub>(j,v1);



  for (auto it = j.begin(); it != j.end(); ++it)
    for (auto it2 = it->begin(); it2 != it->end(); ++it2)
      if (it2->contains("evolvable"))
      {
        if (it2->at("evolvable").is_number())
        {
        vector<string> v1;
        v1.push_back(it.key());
        v1.push_back(it2.key());
        w1pars.singValnames.push_back(v1);
        v1.push_back("evolvable");
        w1pars.singVals.push_back(getEvoValFromJ<int>(j,v1));
        cout << it.key() << " " << it2.key() << endl;

        }else{
        
        size_t idx = it2.key().find("weights");
        if(idx != string::npos)
        {
        //TFIvec.push_back(getEvoVecFromJ<toFromInt>(j,it.key(), it2.key()));
        vector<string> v1;
        v1.push_back(it.key());
        v1.push_back(it2.key());
        w1pars.TFnames.push_back(v1);
        v1.push_back("evolvable");
        w1pars.TFIvec.push_back(getEvoVecFromJ<fromToInt>(j,v1));
        
        //cout << "k1 " << it.key() << " k2 " << it2.key() << endl;
   
        }
        else 
        {
        //IPvec.push_back(getEvoVecFromJ<intPair>(j,it.key(), it2.key()));
        //IPnames.push_back({it.key(),it2.key()});
        vector<string> v1;
        v1.push_back(it.key());
        v1.push_back(it2.key());
        w1pars.IPnames.push_back(v1);
        v1.push_back("evolvable");
        w1pars.IPvec.push_back(getEvoVecFromJ<intPair>(j,v1));
        //cout << "k1 " << it.key() << " k2 " << it2.key() << endl;
        }
      }
      }
    // assert(0);
     return w1pars;
}




void Worm2DSRE::setParsFromPheno(const TVector<double> &pheno)
{

for (int i = 0; i<genPhenPars.TFnames.size(); i++)
{

const vector<string> & s1 = genPhenPars.TFnames[i];
const vector<fromToInt> & v1 = genPhenPars.TFIvec[i];
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
  directMuscEvo = true;
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<dMuscConnvec.size(); k++)
  if ((v1[j].to == dMuscConnvec[k].to) && (v1[j].from == dMuscConnvec[k].w.from))
  {dMuscConnvec[k].w.weight = pheno(v1[j].val);break;}

}
else if (s1[0]=="Ventral NMJ")
{
  directMuscEvo = true;
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<vMuscConnvec.size(); k++)
  if ((v1[j].to == vMuscConnvec[k].to) && (v1[j].from == vMuscConnvec[k].w.from))
  {vMuscConnvec[k].w.weight = pheno(v1[j].val);break;}

}

}

for (int i = 0; i<genPhenPars.IPnames.size(); i++)
{

  const vector<string> & s1 = genPhenPars.IPnames[i];
  const vector<intPair> & v1 = genPhenPars.IPvec[i];

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
else if (s1[0]=="VNC NMJ" || s1[0] == "VNC 18")
{
assert(!directMuscEvo);
if (s1[1]=="D inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<dorsinds.size(); k++)
if (v1[j].ind == dorsinds[k].from) {dorsinds[k].weight = pheno(v1[j].val);break;}
}
else if (s1[1]=="V inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<ventinds.size(); k++)
if (v1[j].ind == ventinds[k].from) {ventinds[k].weight = pheno(v1[j].val);break;}
}

}

}

for (int i = 0; i<genPhenPars.singValnames.size(); i++)
{
const vector<string> & s1 = genPhenPars.singValnames[i];
const int & v1 = genPhenPars.singVals[i];
if (s1[0]=="VNC NMJ" || s1[0] == "VNC 18") assert(!directMuscEvo);
namedVars[s1[1]] = pheno(v1);
//doubVars.setVal(s1[1], pheno(v1));


}

//w2dsr_ptr->setWeights();
//w2dsr_ptr->setNSWeights(*this);



setMuscBodExt();

return;
}


void Worm2DSRE::writeOrigGen(shared_ptr<const CmdArgs> cmd)
{

  string directoryName = cmd->getArgVal("--folder","HJUYGYT");
  struct stat sb;
  if (stat(directoryName.c_str(), &sb) != 0) 
  {cout << "Directory doesn't exist." << endl;exit(1);}

    ofstream BestIndividualFile;
    //bestVector = s.BestIndividual();
    BestIndividualFile.open(rename_file("EvoWJbest.gen.dat", directoryName));
    //BestIndividualFile.open(bestfilename);
    BestIndividualFile << setprecision(32);
    vector<double> initGeno = getInitGeno();
    BestIndividualFile << initGeno[0];
    for (int i=1;i<initGeno.size();i++)
    BestIndividualFile << " " << initGeno[i];
    BestIndividualFile << endl;
    BestIndividualFile.close();

}

bool check123456(double val, double val2)
{
bool b1 = (val<123456.001 && val>123455.999);
bool b2 = (val==val2);
return b1 || b2;
}

bool check123456(double val)
{
return (val<123456.001 && val>123455.999);
}

vector<double> Worm2DSRE::getInitGeno()
{

const double checkval = 123456;
vector<double> initialPheno(getVectSize(), checkval); 


for (int i = 0; i<genPhenPars.TFnames.size(); i++)
{

const vector<string> & s1 = genPhenPars.TFnames[i];
const vector<fromToInt> & v1 = genPhenPars.TFIvec[i];
if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);

if (s1[1]=="Chemical weights")
for (int j = 0; j<v1.size(); j++) 
{
  assert(check123456(initialPheno[v1[j].val-1], n.ChemicalSynapseWeight(v1[j].from, v1[j].to)));
  initialPheno[v1[j].val-1] = n.ChemicalSynapseWeight(v1[j].from, v1[j].to);

}
else if (s1[1]=="Electrical weights")
for (int j = 0; j<v1.size(); j++)
{assert(check123456(initialPheno[v1[j].val-1], n.ElectricalSynapseWeight(v1[j].from, v1[j].to)));
  initialPheno[v1[j].val-1] = n.ElectricalSynapseWeight(v1[j].from, v1[j].to);

}

}
else if (s1[0]=="Dorsal NMJ")
{
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<dMuscConnvec.size(); k++)
  if ((v1[j].to == dMuscConnvec[k].to) && (v1[j].from == dMuscConnvec[k].w.from))
  { assert(check123456(initialPheno[v1[j].val-1], dMuscConnvec[k].w.weight));
    initialPheno[v1[j].val-1] = dMuscConnvec[k].w.weight; 
    
    break;}
}
else if (s1[0]=="Ventral NMJ")
{
  if (s1[1]=="weights")
  for (int j = 0; j<v1.size(); j++) 
  for (int k = 0; k<vMuscConnvec.size(); k++)
  if ((v1[j].to == vMuscConnvec[k].to) && (v1[j].from == vMuscConnvec[k].w.from))
  { assert(check123456(initialPheno[v1[j].val-1], vMuscConnvec[k].w.weight));
    initialPheno[v1[j].val-1] = vMuscConnvec[k].w.weight; 
   
    break;}
}



}

for (int i = 0; i<genPhenPars.IPnames.size(); i++)
{
const vector<string> & s1 = genPhenPars.IPnames[i];
const vector<intPair> & v1 = genPhenPars.IPvec[i];
if (s1[0]=="Nervous system")
{
NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
if (s1[1]=="biases")
for (int j = 0; j<v1.size(); j++) 
{assert(check123456(initialPheno[v1[j].val-1], n.NeuronBias(v1[j].ind)));
  initialPheno[v1[j].val-1] = n.NeuronBias(v1[j].ind);} 

else if (s1[1]=="taus")
for (int j = 0; j<v1.size(); j++)
{assert(check123456(initialPheno[v1[j].val-1], n.NeuronTimeConstant(v1[j].ind)));
  initialPheno[v1[j].val-1] = n.NeuronTimeConstant(v1[j].ind);
}
else if (s1[1]=="gains")
for (int j = 0; j<v1.size(); j++)
{assert(check123456(initialPheno[v1[j].val-1], n.NeuronGain(v1[j].ind)));
  initialPheno[v1[j].val-1] = n.NeuronGain(v1[j].ind);
}

}
else if (s1[0]=="VNC NMJ")
{
if (s1[1]=="D inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<dorsinds.size(); k++)
if (v1[j].ind == dorsinds[k].from)
{assert(check123456(initialPheno[v1[j].val-1], dorsinds[k].weight));
  initialPheno[v1[j].val-1] = dorsinds[k].weight;
  
  break;}
}
else if (s1[1]=="V inds")
{
for (int j = 0; j<v1.size(); j++)
for (int k = 0; k<ventinds.size(); k++)
if (v1[j].ind == ventinds[k].from)
{assert(check123456(initialPheno[v1[j].val-1], ventinds[k].weight));
  initialPheno[v1[j].val-1] = ventinds[k].weight;
  
  break;}
}

}

}

for (int i = 0; i<genPhenPars.singValnames.size(); i++)
{
const vector<string> & s1 = genPhenPars.singValnames[i];
const int & v1 = genPhenPars.singVals[i];
assert(check123456(initialPheno[v1-1], namedVars[s1[1]]));
initialPheno[v1 -1] = namedVars[s1[1]];
}



for (int i = 0; i< initialPheno.size(); i++)
assert(!check123456(initialPheno[i]) && "init pheno not set");

vector<double> initialGeno(getVectSize());
//initialGeno.resize(getVectSize());
PhenGenMapping(initialGeno, initialPheno);

return initialGeno;
}


void Worm2DSRE::PhenGenMapping(vector<double> &gen, const vector<double> &phen)
{

  for (int i = 0; i<genPhenPars.genPhenLims.size(); i++)
  {
  assert(genPhenPars.genPhenLims[i].val1<=genPhenPars.genPhenLims[i].val2);
  gen[i] = InverseMapSearchParameterGPT(phen[i], genPhenPars.genPhenLims[i].val1, genPhenPars.genPhenLims[i].val2);
  cout << "phengen " << phen[i] << " " << genPhenPars.genPhenLims[i].val1 << " " << genPhenPars.genPhenLims[i].val2 << endl;
  assert(!isnan(gen[i]));
  }

}


void Worm2DSRE::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{

  for (int i = 0; i<genPhenPars.genPhenLims.size(); i++)
  {
  assert(genPhenPars.genPhenLims[i].val1<=genPhenPars.genPhenLims[i].val2);
  phen(i+1) = MapSearchParameter(gen(i+1), genPhenPars.genPhenLims[i].val1, genPhenPars.genPhenLims[i].val2);
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

void Sensor::InitializeSensors(RandomState &rs_)
{

  InitialiseAgent();
	
	ResetChemCon();
	//InitializeState(rs_);

	//ResetAgentIntState(rs_);
	UpdateChemCon();

}


void WormCO2DSR::initForSimulation(RandomState &rs_)
//void WormAgent::InitializeSimulation(RandomState &rs_)
{

  //return;
  Worm2DSRE::initForSimulation(rs_);
	//rs = &rs_;
  Sensor::ResetAgentsBody();
  InitializeSensors(rs_);
  
	
}



/* void WormCO2DSR::ResetAgentIntState(RandomState &rs)
{
	NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
	n.RandomizeCircuitState(0.0, 0.0, rs);
	//n.RandomizeCircuitState(0.0, 0.5, rs);
}
 */


void WormCO2DSR::InitializeState(RandomState &rs)
{

  Worm2DSRE::InitializeState(rs);

  //return;
	NervousSystem * n = dynamic_cast<NervousSystem*>(n_ptr);

	if (n!=nullptr){
	if (W2Dbaseparameters1->randomInitialState)
    {
        n->RandomizeCircuitState(-1, 1, rs);
        n->RandomizeCircuitOutput(0.2, 0.8, rs);
    }
	//else n->RandomizeCircuitState(0.0, 0.0, rs);
}

}




void Sensor::InitialiseAgent()
{
	//VelDelta = (int) (HST/gradPars->HSStepSize);

  if (spvec.size()>0){
  spvec[0].HSStepSize = CO2DSRpars->HSStepSize;
  spvec[0].gradSteep = CO2DSRpars->gradSteep;

  }

  for (int i = 0; i<spvec.size(); i++){

  SensorPars & sp1 = spvec[i];
	sp1.iSensorN = (int) (sp1.sensorN/sp1.HSStepSize);
	//dSensorN = (double) iSensorN;
	sp1.iSensorM = (int) (sp1.sensorM/sp1.HSStepSize);
  }


	//dSensorM = (double) iSensorM;
	//int upperbound = ((int) (((2*CO2DSRpars->RunDuration) + sensorN + sensorM) / CO2DSRpars->HSStepSize)) + 1;

	//cout << "uppervel " << upperbound << " " << VelDelta << endl;
 	
	//chemConHistory.SetBounds(1, upperbound);
	//chemConHistory.FillContents(0.0);

	//histCurv.SetBounds(1, VelDelta);
	//histCurv.FillContents(0.0);
	//histTheta.SetBounds(1, VelDelta);
	//histTheta.FillContents(0.0);
	
}


void WormCO2DSR::Step1()
{
   
    //UpdateSensors();
	Worm2DSRE::Step1();
  UpdateChemCon();
   
}


void SensorPars::writeParsToJson(json & j) const
{

addParsToJson1<double>(j,{"sensorN","sensorM","gradSteep", 
  "HSStepSize", "x_center", "y_center"},
    {sensorN,sensorM,gradSteep,HSStepSize,x_center,y_center});
addParsToJson1<int>(j,{"extInp1", "extInp2"}, {extInp1, extInp2});

}

void SensorPars::setParsFromJson(const json & j)
{

  sensorN = j["sensorN"]["value"];
  sensorM = j["sensorM"]["value"];
  gradSteep = j["gradSteep"]["value"];
  HSStepSize = j["HSStepSize"]["value"];
  x_center  = j["x_center"]["value"];
  y_center = j["y_center"]["value"];
  extInp1 = j["extInp1"]["value"];
  extInp2 = j["extInp2"]["value"];

//double sensorN, sensorM;
//double dSensorN, dSensorM;
//int iSensorN, iSensorM;
//double chemCon, presentAvgCon, pastAvgCon;
//double presentAvgCon, pastAvgCon;
//int extInp1, extInp2;
//double gradSteep, HSStepSize, x_center, y_center;

}

void Sensor::setParsFromJson(const json & j, shared_ptr<gradParameters> CO2DSRpars_)
{

  if (j.contains("Sensors"))
 {
  json j2 = j["Sensors"];
  int ind = 1;
  while(j2.contains("Sensor_" + to_string(ind))){

  SensorPars sp1;
  sp1.setParsFromJson(j2["Sensor_" + to_string(ind)]);
  spvec.push_back(sp1);

  }
  
 }else if (j["Worm"].contains("sensorM"))
 {


  SensorPars sp1;
  sp1.gradSteep = CO2DSRpars_->gradSteep;
  sp1.HSStepSize = CO2DSRpars_->HSStepSize;
  sp1.extInp1 = 0;
  sp1.extInp2 = 1;
  sp1.sensorM = j["Worm"]["sensorM"]["value"];
  sp1.sensorN = j["Worm"]["sensorN"]["value"];
  sp1.x_center = 0, sp1.y_center = 0;
  spvec.push_back(sp1);

  
 }

 
}

void  Sensor::writeParsToJson(json & j) const
{

json j2 = j["Sensors"];

for (int i =0; i<spvec.size(); i++)
{

const SensorPars & sp1 = spvec[i];
sp1.writeParsToJson(j2["Sensor_" + to_string(i+1)]);



}
if (spvec.size()>0)
{
const SensorPars & sp1 = spvec[0];
sp1.writeParsToJson(j["Worm"]);

}



}




void Sensor::ResetChemCon()
{ 
  for (int i = 0; i<spvec.size(); i++){
    
  SensorPars & sp1 = spvec[i];
	double chemCon = -headDistanceToLocation(sp1.x_center,sp1.y_center) * sp1.gradSteep;

	//double dist = distanceToCenter();
	//chemCon = -dist * CO2DSRpars->gradSteep;

	//pastCon = chemCon;
  sp1.chemConHistory.clear();
	//timer = iSensorN + iSensorM + 1;
	for (int i = 1; i <= sp1.iSensorN + sp1.iSensorM + 1; i++) sp1.chemConHistory.push_back(chemCon);
		//chemConHistory(i) = chemCon;
	sp1.presentAvgCon = chemCon * sp1.iSensorN;
	sp1.pastAvgCon = chemCon * sp1.iSensorM;

  }
}


void Sensor::UpdateChemCon()
{
	for (int i = 0; i<spvec.size(); i++){
    
  SensorPars & sp1 = spvec[i];
	//pastCon = chemCon;
	double chemCon = -headDistanceToLocation(sp1.x_center,sp1.y_center) * sp1.gradSteep;
  sp1.chemConHistory.push_back(chemCon);

  }
	//chemConHistory(timer) = chemCon;
	//timer += 1;
}

void Sensor::assignExternalInput(vector<double> & externalInputs)
{
  for (int i = 0; i<spvec.size(); i++){
    
  SensorPars & sp1 = spvec[i];

  double dSensorN = (double) sp1.iSensorN;
  double dSensorM = (double) sp1.iSensorM;
  sp1.presentAvgCon += sp1.chemConHistory[sp1.chemConHistory.size()-1] - 
  sp1.chemConHistory[sp1.chemConHistory.size()- sp1.iSensorN -1];
  sp1.pastAvgCon += sp1.chemConHistory[sp1.chemConHistory.size() - sp1.iSensorN - 1] 
  -  sp1.chemConHistory[sp1.chemConHistory.size()- sp1.iSensorN - sp1.iSensorM - 1];


	//presentAvgCon += chemConHistory(timer - 1) - chemConHistory(timer - iSensorN - 1);
	//pastAvgCon += chemConHistory(timer - iSensorN - 1) - chemConHistory(timer - iSensorN - iSensorM - 1);
	double tempDiff = (sp1.presentAvgCon/dSensorN) - (sp1.pastAvgCon/dSensorM);
	externalInputs[sp1.extInp1] = tempDiff > 0.0 ? tempDiff: 0.0;
	externalInputs[sp1.extInp2] =  tempDiff < 0.0 ? fabs(tempDiff): 0.0;

  }
}


void WormCO2DSR::assignExternalInput()
{
  Worm2DSRE::assignExternalInput();
  Sensor::assignExternalInput(externalInputs);
}