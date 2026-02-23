//
//  WormCE.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "Worm2DCE.h"
//#include "../argUtils.h"
//#include "../neuromlLocal/c302ForW2D.h"



Worm2DCE::Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SRCE> sr_ptr_):
Worm2Dm(par1_, n_ptr_), Worm2DSR(par1_,n_ptr_,sr_ptr_),sr_ptr(sr_ptr_)
{
  sr_ptr->basePar1 = this;
}

/* Worm2DCE::Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<SRCE> sr_ptr_):
Worm2Dm(par1_, n_ptr_, make_shared<W2DCEpars>()), Worm2DSR(par1_,n_ptr_, sr_ptr_),
    W2DCEpars1(dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1b)),
    sr_ptr(sr_ptr_){} */





Worm2DCE::Worm2DCE(const json & j, shared_ptr<SRCE> sr_ptr_):Worm2Dm(
  {j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  }, new c302ForW2D()),
  Worm2DSR({j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  } ,0, sr_ptr_),//W2DCEpars1(dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1b)),
  sr_ptr(sr_ptr_)
{

  sr_ptr->basePar1 = this;

  //W2DCEpars1->setParsFromJson(j["Worm"]);
  sr_ptr->setParsFromJson(j);
 
  

  NMJ_DA = j["Worm"]["NMJ_DA"]["value"];
  NMJ_VA = j["Worm"]["NMJ_VA"]["value"];
  NMJ_DB = j["Worm"]["NMJ_DB"]["value"];
  NMJ_VB = j["Worm"]["NMJ_VB"]["value"];

  // Inhibitory VNC NMJ Weight
  NMJ_DD = j["Worm"]["NMJ_DD"]["value"];
  NMJ_VD = j["Worm"]["NMJ_VD"]["value"];

  initConst();

}


/////////////////
// W2DCE second const
//////////////////

Worm2DCE::Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, 
  shared_ptr<SRCE> sr_ptr_, shared_ptr<const CmdArgs> cmd):
  Worm2DCE(par1_,n_ptr_,sr_ptr_)
{

  setWormPars(cmd);
 // initConst();

}



Worm2DCE:: Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_):
Worm2DCE(par1_,n_ptr_,make_shared<SRCE>(N_segments,10))
{

}

Worm2DCE:: Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, shared_ptr<const CmdArgs> cmd):
Worm2DCE(par1_,n_ptr_,make_shared<SRCE>(N_segments,10),cmd){}

Worm2DCE::Worm2DCE(const string & jsonfilename_):
Worm2DCE(getJsonFromFile(jsonfilename_)){}

Worm2DCE::Worm2DCE(const json & j):Worm2DCE(j, make_shared<SRCE>(N_segments,10)){}

//////////////////////////////////
//// CE const
//////////////////////////////////

WormCE::WormCE(shared_ptr<SRCE> sr_ptr_, shared_ptr<const CmdArgs> cmd):
Worm2Dm({6,24,0.1,10,60}, new NervousSystem()),
n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2DCE({6,24,0.1,10,60},0,sr_ptr_,cmd){}


WormCE::WormCE(shared_ptr<SRCE> sr_ptr_):
Worm2Dm({6,24,0.1,10,60}, new NervousSystem()),
n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2DCE({6,24,0.1,10,60},0,sr_ptr_){}


//////////////////////////////////
//// CE second const
//////////////////////////////////


WormCE::WormCE():WormCE(make_shared<SRCE>(N_segments,10)){}

WormCE::WormCE(shared_ptr<const CmdArgs> cmd):
WormCE(make_shared<SRCE>(N_segments,10), cmd){}

WormCE::WormCE(shared_ptr<const CmdArgs> cmd, TVector<double> &phengen, bool isPheno):WormCE(cmd)
{
    if (isPheno) setParsFromPheno(phengen);
    else setParsFromGeno(phengen);
}

WormCE::WormCE(TVector<double> &phengen, bool isPheno):WormCE()
{
    if (isPheno) setParsFromPheno(phengen);
    else setParsFromGeno(phengen);
}

WormCE::WormCE(TVector<double> &pheno):WormCE(pheno, true){}

WormCE::WormCE(shared_ptr<const CmdArgs> cmd, TVector<double> &pheno):WormCE(cmd, pheno, true){}


WormCE::WormCE(shared_ptr<const CmdArgs> cmd, const string & filename_):WormCE(cmd)
{
    setParsFromFile(filename_);
}

//WormCE::WormCE(const string & filename_):WormCE(false)
//{
//    setParsFromFile(filename_);
//}

WormCE::WormCE(const json & j, const string & filename_):WormCE()
{
   
    //W2DCEpars1->setParsFromJson(j["Worm"]);
    sr_ptr->setParsFromJson(j);
    setParsFromFile(filename_);
}

WormCE::WormCE(const string & jsonfilename_, const string & filename_):
WormCE((json) getJsonFromFile(jsonfilename_),filename_){}

WormCE::WormCE(const string & jsonfilename_):
WormCE((json) getJsonFromFile(jsonfilename_)){}


WormCE::WormCE(const json & j):WormCE()
{

  //n.SetCircuitSize(par1.N_units*par1.N_neuronsperunit, 3, 2);
  setNSFromJson(j,n);
  

  assert(n.size == par1.N_units*par1.N_neuronsperunit);
  assert(n.maxchemconns == 3);
  assert(n.maxelecconns == 2);


 
 
  //W2DCEpars1->setParsFromJson(j["Worm"]);
  sr_ptr->setParsFromJson(j);
  
  
  AVA_act = 0;
  AVA_inact = 0;
  AVB_act = 0;
  AVB_inact = 0;
  
  NMJ_DA = j["Worm"]["NMJ_DA"]["value"];
  NMJ_VA = j["Worm"]["NMJ_VA"]["value"];
  NMJ_DB = j["Worm"]["NMJ_DB"]["value"];
  NMJ_VB = j["Worm"]["NMJ_VB"]["value"];

  // Inhibitory VNC NMJ Weight
  NMJ_DD = j["Worm"]["NMJ_DD"]["value"];
  NMJ_VD = j["Worm"]["NMJ_VD"]["value"];

  //W2DCEpars1->AVA_output = 0.0;
  //W2DCEpars1->AVB_output = 0.0;

  cout << "Worm2DCE const" << endl;

  pheno_A_gain = sr_ptr->SR_A_gain;
  pheno_B_gain = sr_ptr->SR_B_gain;

  //sr_ptr->setParsFromJson(j);

  //sr_ptr->setNSWeights(*this);
  //sr_ptr->setNSWeights(shared_ptr<const Worm2DCE>(this));
   
  
  setInputSwitcher(j);

  
  

  setUpMuscleConn(j);
  setUpBodyConn(j);
  makeExternalInputConnFromJson(j);

  
}

void Worm2DCE::initConst()
{
      AVA_act = 0;
      AVA_inact = 0;
      AVB_act = 0;
      AVB_inact = 0;

      //assert(0);
      //W2DCEpars1->AVA_output = 0.0;
      //W2DCEpars1->AVB_output = 0.0;

      //sr_ptr->SRForm = W2DCEpars1->SRForm;
      pheno_A_gain = sr_ptr->SR_A_gain;
      pheno_B_gain = sr_ptr->SR_B_gain;
      sr_ptr->setWeights();
      sr_ptr->setNSWeights(*this);

    
      setInputSwitcher();
      setUpMuscleConn();
      setUpBodyConn();
      makeExternalInputConn();
      //assert(0);

}


void Worm2DCE::setInputSwitcher()
{

vector<vector<int> > input_indvec;
      vector<vector<double> > input_valvec;
      input_indvec.push_back(vector<int>({0,1}));
      input_valvec.push_back(vector<double>({0,1}));
      input_indvec.push_back(vector<int>({0,1}));
      input_valvec.push_back(vector<double>({1,0}));
      input_indvec.push_back(vector<int>({0,1}));
      input_valvec.push_back(vector<double>({0,0}));

      inds.swap(input_indvec);
      vals.swap(input_valvec);

  

}


void Worm2DCE::setInputSwitcher(const json & j)
{

 if (!j.contains("input_switcher"))
  {
    
    setInputSwitcher();

  }
  else InputSwitcher::setParsFromJson(j);


}

void WormCE::addEvolvableToJson(json & j)
{
 
  {vector<doubDoub> vec; 
  
  //  shared_ptr<W2DCEpars> w1 = dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1b);


    // Genotype -> Phenotype Mapping Ranges
    const double    BiasRange               = 16.0; //15.0;
    const double    SCRange                 = 16.0; //15.0;
    const double    CSRange                 = 16.0; //15.0;
    const double    ESRange                 = 2.0;
    const double    SRmax                   = 200.0;
    const double    NMJmax                  = 0.8; //1.2;
    const double    NMJmin                  = 0.0;
    

    double SREvoBotA,  SREvoTopA, SREvoBot, SREvoTop;
    getValCJWorm<double>("SREvoBotA",SREvoBotA);
    getValCJWorm<double>("SREvoTopA",SREvoTopA);
    getValCJWorm<double>("SREvoBot",SREvoBot);
    getValCJWorm<double>("SREvoTop",SREvoTop);

    //vec.push_back({w1->SREvoBotA, w1->SREvoTopA});
    //vec.push_back({w1->SREvoBot, w1->SREvoTop});

    vec.push_back({SREvoBotA, SREvoTopA});
    vec.push_back({SREvoBot, SREvoTop});
    for (int i = 1; i <= 3; i++) vec.push_back({-BiasRange, BiasRange});
    for (int i = 1; i <= 3; i++) vec.push_back({-SCRange, SCRange});
    for (int i = 1; i <= 2; i++)  vec.push_back({0.0, CSRange});
    for (int i = 1; i <= 2; i++) vec.push_back({ -CSRange, 0.0});
    for (int i = 1; i <= 2; i++)  vec.push_back({0.0, ESRange});
     for (int i = 1; i <= 2; i++) vec.push_back({NMJmin, NMJmax});
     for (int i = 1; i <= 1; i++)  vec.push_back({-NMJmax, -NMJmin});

    j["Evolvable"]["value"] = toIntDoubDoub(vec);
  }
 
  j["Stretch receptor"]["SR_A_gain"]["evolvable"] = {{"val",1}, {"mfunc", {{"f_ind", 2}, {"cond", 0}}}};
  j["Stretch receptor"]["SR_B_gain"]["evolvable"] = {{"val",2}, {"mfunc", {{"f_ind", 2}, {"cond", 1}}}};

  //j["Stretch receptor"]["SR_A_gain"]["evolvable"] = 1;
  //j["Stretch receptor"]["SR_B_gain"]["evolvable"] = 2;
//chemvecj.push_back({{"from", vba}, {"to", dd}, {"val", 8}, {"mfunc", {{"f_ind", 1}, {"fact", 0.5}}}});

  vector<intPair> biasvec;
  vector<fromToInt> chemvec, elecvec;

  int da, db, dd, vd, vb, va;
  int ddNext, vdNext, vbNext, dbNext;


  for (int u = 1; u <= par1.N_units; u++){
    // Find the numbers that identify each neuron within a certain repeating unit

    da = nn(DA, u);
    db = nn(DB, u);
    dd = nn(DD, u);
    vd = nn(VD, u);
    va = nn(VA, u);
    vb = nn(VB, u);

    // neurons for interunit connections
    ddNext = nn(DD, u+1);
    vdNext = nn(VD, u+1);
    vbNext = nn(VB, u+1);
    dbNext = nn(DB, u+1);

            {vector<intPair> & vec = biasvec;
            vec.push_back({da,3});
            vec.push_back({va,3});
            vec.push_back({db,4});
            vec.push_back({vb,4});
            vec.push_back({dd,5});
            vec.push_back({vd,5});
            }

          {
            vector<fromToInt> & vec = chemvec;
            vec.push_back({da,da,6});
            vec.push_back({va,va,6});
            vec.push_back({db,db,7});
            vec.push_back({vb,vb,7});
            vec.push_back({dd,dd,8});
            vec.push_back({vd,vd,8});
            vec.push_back({da,vd,9});
            vec.push_back({va,dd,9});
            vec.push_back({vb,dd,10});
            vec.push_back({db,vd,10});

            vec.push_back({vd,va,11});
            vec.push_back({dd,da,11});
            vec.push_back({vd,vb,12});
            vec.push_back({dd,db,12});

          }

        {vector<fromToInt> & vec = elecvec;

          if (u < par1.N_units){
          vec.push_back({dd,ddNext,13});
          vec.push_back({vd,vdNext,13});
          vec.push_back({db,dbNext,14});
          vec.push_back({vb,vbNext,14});

        }
        }

  
  }

  j["Nervous system"]["biases"]["evolvable"] = biasvec;
  j["Nervous system"]["Chemical weights"]["evolvable"] = chemvec;
  j["Nervous system"]["Electrical weights"]["evolvable"] = elecvec;


vector<intPair> nmjvecd;
nmjvecd.push_back({DA,15});
nmjvecd.push_back({DB,16});
nmjvecd.push_back({DD,17});

vector<intPair> nmjvecv;
nmjvecv.push_back({VA,15});
nmjvecv.push_back({VB,16});
nmjvecv.push_back({VD,17});


j["VNC NMJ"]["V inds"]["evolvable"] = nmjvecv;
j["VNC NMJ"]["D inds"]["evolvable"] = nmjvecd;


}



void WormCE::setParsFromPheno(const TVector<double> &pheno)
{



//cout << "CE spfp " << par1.N_units << " " << par1.N_neuronsperunit << endl;
//assert(0);

//cout << "pheno " << pheno << " endpheno" << endl;
//assert(0);

// PG: Setting these to zero as they were not initialised before use!
  // Note: the usage of these needs to be further investigated!
  //AVA_act = 0;
  //AVA_inact = 0;
  //AVB_act = 0;
  //AVB_inact = 0;

  // Muscles
  //m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
  // Nervous system
  n.SetCircuitSize(par1.N_units*par1.N_neuronsperunit, 3, 2);

  int da, db, dd, vd, vb, va;
  int ddNext, vdNext, vbNext, dbNext;

  // Stretch receptor
//  sr_ptr->SetStretchReceptorParams(N_segments, N_stretchrec, pheno(1), pheno(2));
 

  if (itsEf.itsJson.contains("condval")){
  {json jevol;
  jevol["mfunc"]["f_ind"] = 2;
  jevol["mfunc"]["cond"] = 0;
  sr_ptr->SR_A_gain = itsEf.eFunc(pheno(1), jevol["mfunc"]); }
  {json jevol;
  jevol["mfunc"]["f_ind"] = 2;
  jevol["mfunc"]["cond"] = 1;
  sr_ptr->SR_B_gain = itsEf.eFunc(pheno(2), jevol["mfunc"]); }
  

  }

else{
  sr_ptr->SR_A_gain = pheno(1);
  sr_ptr->SR_B_gain = pheno(2);
}

  //cout << "psps " << pheno(1) << " "  << pheno(2) << endl;

  //assert(0);

  for (int u = 1; u <= par1.N_units; u++){
    // Find the numbers that identify each neuron within a certain repeating unit
    da = nn(DA, u);
    db = nn(DB, u);
    dd = nn(DD, u);
    vd = nn(VD, u);
    va = nn(VA, u);
    vb = nn(VB, u);

    // neurons for interunit connections
    ddNext = nn(DD, u+1);
    vdNext = nn(VD, u+1);
    vbNext = nn(VB, u+1);
    dbNext = nn(DB, u+1);

    // Biases
    n.SetNeuronBias(da, pheno(3));
    n.SetNeuronBias(va, pheno(3));
    n.SetNeuronBias(db, pheno(4));
    n.SetNeuronBias(vb, pheno(4));
    n.SetNeuronBias(dd, pheno(5));
    n.SetNeuronBias(vd, pheno(5));

    // Time-constants fixed to 1.0
    for (int i = 1; i <= par1.N_neuronsperunit; i++){
      n.SetNeuronTimeConstant(nn(i,u), 1.0);
    }

    // Self-connections
    n.SetChemicalSynapseWeight(da,da, pheno(6));
    n.SetChemicalSynapseWeight(va,va, pheno(6));
    n.SetChemicalSynapseWeight(db,db, pheno(7));
    n.SetChemicalSynapseWeight(vb,vb, pheno(7));
    n.SetChemicalSynapseWeight(dd,dd, pheno(8));
    n.SetChemicalSynapseWeight(vd,vd, pheno(8));

    // Cross-connections
    // Excitatory Chemical Synapses intraunit
    n.SetChemicalSynapseWeight(da, vd, pheno(9));
    n.SetChemicalSynapseWeight(va, dd, pheno(9));
    n.SetChemicalSynapseWeight(vb, dd, pheno(10));
    n.SetChemicalSynapseWeight(db, vd, pheno(10));

    // Inhibitory Chemical Synapses intraunit
    n.SetChemicalSynapseWeight(vd, va, pheno(11));
    n.SetChemicalSynapseWeight(dd, da, pheno(11));
    n.SetChemicalSynapseWeight(vd, vb, pheno(12));
    n.SetChemicalSynapseWeight(dd, db, pheno(12));

    // Electrical Synapse Intersegment connections
    if (u < par1.N_units){
      n.SetElectricalSynapseWeight(dd, ddNext, pheno(13));
      n.SetElectricalSynapseWeight(vd, vdNext, pheno(13));
      n.SetElectricalSynapseWeight(vb, vbNext, pheno(14));
      n.SetElectricalSynapseWeight(db, dbNext, pheno(14));
    }
  }

  // Excitatory VNC NMJ Weight
  NMJ_DA = pheno(15);
  NMJ_VA = pheno(15);
  NMJ_DB = pheno(16);
  NMJ_VB = pheno(16);

  // Inhibitory VNC NMJ Weight
  NMJ_DD = pheno(17);
  NMJ_VD = pheno(17);

  //W2DCEpars1->AVA_output = 0.0;
  //W2DCEpars1->AVB_output = 0.0;

  initConst();

  //pheno_A_gain = sr_ptr->SR_A_gain;
  //pheno_B_gain = sr_ptr->SR_B_gain;

  //sr_ptr->setWeights();
  //sr_ptr->setNSWeights(*this);

  //setUpBodyConn();
  //setUpMuscleConn();
  //makeExternalInputConn();
}






///////////////////////////////
//////////////////////////////
//////////////////////////////


WormCESR::WormCESR():WormCE(make_shared<SRReg>(N_segments,10,this)),
//Worm2Dm({6,24,0.1,10,60}, new NervousSystem(), make_shared<W2DCEpars>())
Worm2Dm({6,24,0.1,10,60}, new NervousSystem())
{}



WormCESR::WormCESR(shared_ptr<const CmdArgs> cmd):WormCE(make_shared<SRReg>(N_segments,10,this),cmd),
//Worm2Dm({6,24,0.1,10,60}, new NervousSystem(), make_shared<W2DCEpars>())
Worm2Dm({6,24,0.1,10,60}, new NervousSystem())
{}


WormCESR::WormCESR(shared_ptr<const CmdArgs> cmd, const string & filename_):
WormCESR(cmd)
{
setParsFromFile(filename_);
}

WormCESR::WormCESR(json  j, const string & filename_):
WormCESR()
{
  //W2DCEpars1->setParsFromJson(j["Worm"]);
  sr_ptr->setParsFromJson(j);
  setParsFromFile(filename_);
}

WormCESR::WormCESR(const string & jsonfilename_, const string & filename_):
WormCESR((json) getJsonFromFile(jsonfilename_),filename_){}

void Worm2DCE::initForSimulation(RandomState & rs)
{

  return;

}


void Worm2DCE::setForward()
{
  //assert(0);
  sr_ptr->SR_A_gain = pheno_A_gain;
  sr_ptr->SR_B_gain = pheno_B_gain;
  //shared_ptr<SRCEpars> srcepars = dynamic_pointer_cast<SRCEpars>(sr_ptr->srpars);
  //shared_ptr<SRCEpars> srcepars = dynamic_pointer_cast<SRCEpars>(sr_ptr->)
  int zeroGainsType;
  getValCJWorm<int>("SRZeroGainsType", zeroGainsType);
  if (zeroGainsType == 0) sr_ptr->SR_A_gain = 0.0;
  AVA_output =  1;
  AVB_output =  0;
  //sr_ptr->setWeights();
  //W2DCEpars1->AVA_output =  0;
  //W2DCEpars1->AVB_output =  1; //W2DCEpars1->AB_output_level;
  
  setInputOnce(0);

  //assignExternalInputOnce(0,0);
  //assignExternalInputOnce(1,1);
}

void Worm2DCE::setBackward()
{
  //assert(0);
  sr_ptr->SR_A_gain = pheno_A_gain;
  sr_ptr->SR_B_gain = pheno_B_gain;
  //shared_ptr<SRCEpars> srcepars = dynamic_pointer_cast<SRCEpars>(sr_ptr->srpars);
  int zeroGainsType;
  getValCJWorm<int>("SRZeroGainsType", zeroGainsType);
  if (zeroGainsType  == 0) sr_ptr->SR_B_gain = 0.0;
  AVA_output =  0;
  AVB_output =  1;

  //sr_ptr->setWeights();
  //W2DCEpars1->AVA_output =  1; //W2DCEpars1->AB_output_level;
  //W2DCEpars1->AVB_output =  0;

  setInputOnce(1);

  //assignExternalInputOnce(0,1);
  //assignExternalInputOnce(1,0);
}



void Worm2DCE::setWormPars(shared_ptr<const CmdArgs> cmd)
{
  
  //W2DCEpars1->setPars(cmd);
  //Worm2DSR::setWormPars(cmd);
  sr_ptr->setPars(cmd);

}




void Worm2DCE::InitializeState(RandomState &rs)
{
  Worm2D::InitializeState(rs);
}

vector<toFromWeight> Worm2DCE::makeDorsalMuscleConn()
{
vector<int> dorsalNeurons({DA,DB,DD});
vector<double> dorsalNMJ({NMJ_DA,NMJ_DB,NMJ_DD});


//return makeMuscleConn(dorsalNeurons, dorsalNMJ);

hasVNCNMJ = true;
//doubVars.setVal("NMJ gain map D", 0);
//doubVars.setVal("NMJ gain fact", 1);

namedVars["NMJ gain map D"] = 0;
namedVars["NMJ gain fact"] = 1;
//NMJ_gain_map_D = 0;
//NMJ_gain_fact = 1;
vector<weightentry> v1 = makeWeightEntry(dorsalNeurons,dorsalNMJ);
dorsinds.swap(v1);
vector<intPair> o1 = makeUnitToMusc();
unitToMuscD.swap(o1);


return makeMuscleConnVNCD();


//return makeMuscleConn(dorsalNeurons, dorsalNMJ);
}

vector<toFromWeight> Worm2DCE::makeVentralMuscleConn()
{
vector<int> ventralNeurons({VD,VA,VB});
vector<double> ventralNMJ({NMJ_VD,NMJ_VA,NMJ_VB});


//return makeMuscleConn(ventralNeurons, ventralNMJ);

hasVNCNMJ = true;
namedVars["NMJ gain map V"] = 0;
namedVars["NMJ gain fact"] = 1;

//doubVars.setVal("NMJ gain map V", 0);
//doubVars.setVal("NMJ gain fact", 1);
//NMJ_gain_map_V = 0;
//NMJ_gain_fact = 1.0;
vector<weightentry> v1 = makeWeightEntry(ventralNeurons,ventralNMJ);
ventinds.swap(v1);
vector<intPair> o1 = makeUnitToMusc();
unitToMuscV.swap(o1);

return makeMuscleConnVNCV();

//return makeMuscleConn(ventralNeurons, ventralNMJ);

}


vector<intPair>  Worm2DCE::makeUnitToMusc()
{

    vector<intPair> unitToMusc;
    int unit = 1;
    for (int to_musc = 1; to_musc < 3; to_musc++) 
    unitToMusc.push_back({unit,to_musc});
    int to_musc = 4;
    unit = 1;
    unitToMusc.push_back({unit,to_musc});
    unit = 2;
    unitToMusc.push_back({unit,to_musc});
    to_musc = 5;
    unit = 2;
    unitToMusc.push_back({unit,to_musc});
    unit = 2; // Muscles 6-19
    for (int to_musc=6; to_musc<=19; to_musc++){
      unitToMusc.push_back({unit,to_musc});
      unitToMusc.push_back({unit+1,to_musc});
      unit += to_musc%2; // increment the index for the innervating unit each two muscles, starting from mi = 7
    }

  to_musc = 20;
  unit = 9;
  unitToMusc.push_back({unit,to_musc});
  to_musc = 21;
  unit = 9;
  unitToMusc.push_back({unit,to_musc});

  to_musc = 21;
  unit = 10;
  unitToMusc.push_back({unit,to_musc});
    unit = 10;
    for (int to_musc=22; to_musc<=24; to_musc++)
     unitToMusc.push_back({unit,to_musc});

 return unitToMusc;

}

vector<toFromWeight> Worm2DCE::makeMuscleConn(const vector<int> & neurons, const vector<double> & NMJ)
{
    vector<toFromWeight> vec1;
    vector<double> NMJ_Gain(par1.N_muscles,1.0);

    int unit = 1;
    for (int to_musc = 1; to_musc <= 3; to_musc++) 
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);
        
    int to_musc = 4;
    unit = 1;
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);
    unit = 2;
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);

    to_musc = 5;
    unit = 2;
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);

    unit = 2; // Muscles 6-19
    for (int to_musc=6; to_musc<=19; to_musc++){
      makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);
      makeMuscleConnHelp1(vec1, neurons, NMJ, unit+1, to_musc, NMJ_Gain, par1.N_neuronsperunit);
      unit += to_musc%2; // increment the index for the innervating unit each two muscles, starting from mi = 7
    }

    to_musc = 20;
    unit = 9;
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);

    to_musc = 21;
    unit = 9;
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);

    to_musc = 21;
    unit = 10;
    makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);

    unit = 10;
    for (int to_musc=22; to_musc<=24; to_musc++)
      makeMuscleConnHelp1(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain, par1.N_neuronsperunit);

  
    //cout << "made muscle con" << endl;
            //exit(1);
    return vec1;

}

void Worm2DCE::setMuscleInputOrig()
{

  int mi;
  int mt = 0;

TVector<double> dorsalInput(1, par1.N_units);
  TVector<double> ventralInput(1, par1.N_units);

for (int i=1; i<=par1.N_units; i++){
    dorsalInput(i)  = NMJ_DA*n_ptr->NeuronOutput(nn(DA,i)) + NMJ_DB*n_ptr->NeuronOutput(nn(DB,i)) + NMJ_DD*n_ptr->NeuronOutput(nn(DD,i));
    ventralInput(i) = NMJ_VD*n_ptr->NeuronOutput(nn(VD,i)) + NMJ_VA*n_ptr->NeuronOutput(nn(VA,i)) + NMJ_VB*n_ptr->NeuronOutput(nn(VB,i));
  }
  // Muscles 1-3
  for (int mi=1; mi<=3; mi++){
    m.SetVentralMuscleInput(mi, ventralInput(1));
    m.SetDorsalMuscleInput(mi, dorsalInput(1));
  }

  mi = 4; // 4th muscle
  m.SetVentralMuscleInput(mi, (ventralInput(1)+ventralInput(2)));
  m.SetDorsalMuscleInput(mi, (dorsalInput(1)+dorsalInput(2)));

  mi = 5; // 5th muscle
  m.SetVentralMuscleInput(mi, ventralInput(2));
  m.SetDorsalMuscleInput(mi, dorsalInput(2));

  mt = 2; // Muscles 6-19
  for (int mi=6; mi<=19; mi++){
    m.SetVentralMuscleInput(mi, (ventralInput(mt)+ventralInput(mt+1)));
    m.SetDorsalMuscleInput(mi, (dorsalInput(mt)+dorsalInput(mt+1)));
    mt += mi%2; // increment the index for the innervating unit each two muscles, starting from mi = 7
  }

  mi = 20; // 20th muscle
  m.SetVentralMuscleInput(mi, ventralInput(9));
  m.SetDorsalMuscleInput(mi, dorsalInput(9));

  mi = 21; // 21st muscle
  m.SetVentralMuscleInput(mi, (ventralInput(9)+ventralInput(10)));
  m.SetDorsalMuscleInput(mi, (dorsalInput(9)+dorsalInput(10)));

  // Muscles 22-24
  for (int mi=22; mi<=24; mi++){
    m.SetVentralMuscleInput(mi, ventralInput(10));
    m.SetDorsalMuscleInput(mi, dorsalInput(10));
  }

  // Update Muscle activation
  m.EulerStep(settedStepSize);


}




void Worm2DCE::setBodyInputOrig(){

// Set input to Mechanical Body
  //  First two segments receive special treatment because they are only affected by a single muscle
  b.SetDorsalSegmentActivation(1, m.DorsalMuscleOutput(1)/2);
  b.SetVentralSegmentActivation(1, m.VentralMuscleOutput(1)/2);
  b.SetDorsalSegmentActivation(2, m.DorsalMuscleOutput(1)/2);
  b.SetVentralSegmentActivation(2, m.VentralMuscleOutput(1)/2);

  //  All other segments receive force from two muscles
  for (int i = 3; i <= N_segments-2; i++)
  {
    int mi = (int) ((i-1)/2);
    b.SetDorsalSegmentActivation(i, (m.DorsalMuscleOutput(mi) + m.DorsalMuscleOutput(mi+1))/2);
    b.SetVentralSegmentActivation(i, (m.VentralMuscleOutput(mi) + m.VentralMuscleOutput(mi+1))/2);
  }

  //  Last two segments receive special treatment because they are only affected by a single muscle
  b.SetDorsalSegmentActivation(N_segments-1, m.DorsalMuscleOutput(par1.N_muscles)/2);
  b.SetVentralSegmentActivation(N_segments-1, m.VentralMuscleOutput(par1.N_muscles)/2);
  b.SetDorsalSegmentActivation(N_segments, m.DorsalMuscleOutput(par1.N_muscles)/2);
  b.SetVentralSegmentActivation(N_segments, m.VentralMuscleOutput(par1.N_muscles)/2);

}

void Worm2DCE::assignExternalInput(){

  return;


  //externalInputs[0] = W2DCEpars1->AVA_output*W2DCEpars1->AB_output_level;
  //externalInputs[1] = W2DCEpars1->AVB_output*W2DCEpars1->AB_output_level;
  
  //externalInputs[0] = W2DCEpars1->AVA_output;//*W2DCEpars1->AB_output_level;
  //externalInputs[1] = W2DCEpars1->AVB_output;//*W2DCEpars1->AB_output_level;
}


void Worm2DCE::makeExternalInputConn()
{


   double AB_output_level;
  getValCJWorm<double>("AB_output_level",AB_output_level);

  //const double & weightval = W2DCEpars1->AB_output_level;
  vector<toFromWeight>  vec1;
  for (int i = 1; i <= par1.N_units; i++){
    double weight = AB_output_level;
    {int from_inp = 1;
    {int to_neuron = nn(DA,i);
    toFromWeight tv({from_inp,weight},to_neuron);
    vec1.push_back(tv);}
    {int to_neuron = nn(VA,i);
    toFromWeight tv({from_inp,weight},to_neuron);
    vec1.push_back(tv);}}
    {int from_inp = 2;
    {int to_neuron = nn(DB,i);
    toFromWeight tv({from_inp,weight},to_neuron);
    vec1.push_back(tv);}
    {int to_neuron = nn(VB,i);
    toFromWeight tv({from_inp,weight},to_neuron);
    vec1.push_back(tv);}}
  }

  vector<double> exvec(2,0.0);
  externalInputs.swap(exvec);
  externalInputConn.swap(vec1);
 

  //return vec1;
}

void Worm2DCE::setExternalInputOrig()
{

  double AB_output_level;
  getValCJWorm<double>("AB_output_level",AB_output_level);

  for (int i = 1; i <= par1.N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DA,i), AVA_output*AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(DA,i), sr_ptr->A_D_sr(i) + AVA_output);
    n_ptr->SetNeuronExternalInput(nn(VA,i), AVA_output*AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(VA,i), sr_ptr->A_V_sr(i) + AVA_output);
  }
  ////   To B_class motorneurons
  for (int i = 1; i <= par1.N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DB,i), AVB_output*AB_output_level);
    n_ptr->SetNeuronExternalInput(nn(VB,i), AVB_output*AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(DB,i), sr_ptr->B_D_sr(i) + AVB_output);
    //n_ptr->SetNeuronExternalInput(nn(VB,i), sr_ptr->B_V_sr(i) + AVB_output);
  }

}


/* void Worm2DCE::Step1()
{

  
  b.StepBody(settedStepSize);

  sr_ptr->updateAll(b);

  setExternalInput();
  //setExternalInputOrig();

  sr_ptr->incNS(*n_ptr);

  n_ptr->EulerStep(settedStepSize);
  
  if (W2Dbaseparameters1->doOrigMuscInput) setMuscleInputOrig();
  else setMuscleInput();

  setBodyInput();
  
}  */

void WormCE::randomizeNS(RandomState &rs)
{
 //shared_ptr<W2Dbaseparameters> w1parss = dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b);

 //assert(w1parss!=nullptr);

   bool randomInitialState;
  getValCJWorm<bool>("randomInitialState",randomInitialState);


  if (randomInitialState) {
  n.RandomizeCircuitState(-1, 1, rs);
  n.RandomizeCircuitOutput(0.2, 0.8, rs);
  }

}

void WormCE::InitializeState(RandomState &rs)
{
  
  Worm2DCE::InitializeState(rs);

  //cout << "sss " << W2Dbaseparameters1->randomInitialState << endl;
  //cout << "sss2 " << W2DCEpars1->randomInitialState << endl;
  
  //assert(0);
 
  bool randomInitialState;
  getValCJWorm<bool>("randomInitialState",randomInitialState);


 // shared_ptr<W2Dbaseparameters> w1parss = dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b);
 // assert(w1parss!=nullptr);

  if (randomInitialState) {
    randomizeNS(rs);
  }
  else{
  for (int u = 1; u <= par1.N_units; u++)
  {
    // Dorsal neurons
    n.SetNeuronOutput(nn(DA,u), 0.1);
    n.SetNeuronOutput(nn(DB,u), 0.1);
    n.SetNeuronOutput(nn(DD,u), 0.9);
    // Ventral neurons
    n.SetNeuronOutput(nn(VA,u), 0.9);
    n.SetNeuronOutput(nn(VB,u), 0.9);
    n.SetNeuronOutput(nn(VD,u), 0.1);
  }
  }

}

void WormCE::setEvolPars(W2Dparameters & w2par_, string evotype_)
//void Worm21::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{
    if (evotype_=="Evo21" || evotype_=="Evo21R"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);

    Epars1.dbunit = nn(DB,3);
    Epars1.vbunit = nn(VB,3);
    }

}


void WormCE::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
 
  //shared_ptr<W2DCEpars> w1 = dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1b);


    // Genotype -> Phenotype Mapping Ranges
    const double    BiasRange               = 16.0; //15.0;
    const double    SCRange                 = 16.0; //15.0;
    const double    CSRange                 = 16.0; //15.0;
    const double    ESRange                 = 2.0;
    const double    SRmax                   = 200.0;
    const double    NMJmax                  = 0.8; //1.2;
    const double    NMJmin                  = 0.0;
    
    const int SR_A = 1;
    const int SR_B = 2;


    //cout <<  "gpevobot " << gen(1) << " " << gen(2) << " " << w1->SREvoBotA << " " 
    //<< w1->SREvoTopA << " " << w1->SREvoBot << " " << w1->SREvoTop << endl;

    //assert(0);

    double SREvoBotA,  SREvoTopA, SREvoBot, SREvoTop;
    getValCJWorm<double>("SREvoBotA",SREvoBotA);
    getValCJWorm<double>("SREvoTopA",SREvoTopA);
    getValCJWorm<double>("SREvoBot",SREvoBot);
    getValCJWorm<double>("SREvoTop",SREvoTop);

     // Parameters for the Stretch Receptors
  phen(SR_A) = MapSearchParameter(gen(SR_A), SREvoBotA, SREvoTopA);
  phen(SR_B) = MapSearchParameter(gen(SR_B), SREvoBot, SREvoTop);

  //cout << "mms " << MapSearchParameter(-1.0, w1->SREvoBot, SRmax) << endl;
  //assert(0);

  // Bias
  int k=3;
  for (int i = 1; i <= 3; i++){
    phen(k) = MapSearchParameter(gen(k), -BiasRange, BiasRange);k++;
  }
  // Self connections
  for (int i = 1; i <= 3; i++){
    phen(k) = MapSearchParameter(gen(k), -SCRange, SCRange);k++;
  }
  // DA, DB, VA, VB Chemical synapses (excitatory)
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), 0.0, CSRange);k++;
  }
  // VD Chemical synapses (Inhibitory)
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), -CSRange, 0.0);k++;
  }
  // Interunits Gap junctions
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), 0.0, ESRange);k++;
  }
  // Excitatory NMJ Weight
  for (int i = 1; i <= 2; i++){
    phen(k) = MapSearchParameter(gen(k), NMJmin, NMJmax);k++;
  }
  // Inhibitory NMJ Weight
  for (int i = 1; i <= 1; i++){
    phen(k) = MapSearchParameter(gen(k), -NMJmax, -NMJmin);k++;
  }


}

void WormCE::setPhenoNames()
{
 
  int k = 1;
  phenoNamesNums.push_back(k);k++;
  phenoNames.push_back("SR_A");
  phenoNamesNums.push_back(k);k++;
  phenoNames.push_back("SR_B");


  for (int i = 1; i <= 3; i++){
    phenoNamesNums.push_back(k);k++;
    phenoNames.push_back("Bias");
  }

  for (int i = 1; i <= 3; i++){
    phenoNamesNums.push_back(k);k++;
    phenoNames.push_back("Self_cons");
  }


  // DA, DB, VA, VB Chemical synapses (excitatory)
  for (int i = 1; i <= 2; i++){
    phenoNamesNums.push_back(k);k++;
    phenoNames.push_back("Syn_E");
    
  }

  for (int i = 1; i <= 2; i++){
    phenoNamesNums.push_back(k);k++;
    phenoNames.push_back("Syn_I");
    
  }

  for (int i = 1; i <= 2; i++){
    phenoNamesNums.push_back(k);k++;
    phenoNames.push_back("Gap");
    
  }

  for (int i = 1; i <= 2; i++){
    phenoNamesNums.push_back(k);k++;
    phenoNames.push_back("NMJ_E");
    
  }

  for (int i = 1; i <= 1; i++){
    phenoNamesNums.push_back(k);k++;
    phenoNames.push_back("NMJ_I");
    
  }
  
}


void Worm2DCE::addParsToJson(json & j)
{
    //Params<double> par = sr_ptr->getStretchReceptorParams();
    //appendToJson<double>(j["Stretch receptor"], par);

    sr_ptr->addParsToJson(j);
    Worm2D::addParsToJson(j);
    //W2DCEpars1->addParsToJson(j);
    //string nsHead = "Nervous system";
    //appendCellNamesToJson(j[nsHead], getCellNames(), par1.N_units);
}

void Worm2DCE::writeAct()
{

/* 
  static bool firstcall = true;
  static size_t pos;
  static int tt;

  if (resetStats(firstcall,pos,tt,"act.dat")) return;
 */
  /* if (firstcall || !isOpen[pos]){
      ofsvec.push_back(ofstream(getName("act.dat")));
      pos = ofsvec.size() - 1;
      isOpen.push_back(true);
      firstcall = false;
      tt = dataskips;
  } */
  
  size_t pos = getPos("act.dat");
  ofstream & ofs = ofsvec[pos];  
  int & tt = tts[pos];

  if (++tt >= dataskips) {
    tt = 0;

    ofs << datatime;
    //ofs << "\nSR: ";
    // Stretch receptors
    for (int i = 1; i <= N_stretchrec; i++) {
      //ofs <<  " " << sr_ptr->A_D_sr(i) << " " << sr_ptr->A_V_sr(i) << " " << sr_ptr->B_D_sr(i) << " " << sr_ptr->B_V_sr(i);
      ofs <<  " " << sr_ptr->srvars->A_D_sr[i-1] << " " << sr_ptr->srvars->A_V_sr[i-1] << " " 
      << sr_ptr->srvars->B_D_sr[i-1] << " " << sr_ptr->srvars->B_V_sr[i-1];
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


void Worm2DCE::DumpParams(ofstream &ofs) {
  
  ofs << "Stretch Receptors Gains: \n A-class SR: " << sr_ptr->SR_A_gain <<
  "\n B-class SR: " << sr_ptr->SR_B_gain <<  endl;

  ofs << "NMJ weights: \n" <<
  "\n DA: " << NMJ_DA <<
  "\n DB: " << NMJ_DB <<
  "\n DD: " << NMJ_DD <<
  "\n VD: " << NMJ_VD <<
  "\n VA: " << NMJ_VA <<
  "\n VB: " << NMJ_VB <<  endl;
}


vector<doubIntParamsHead> Worm2DCE::getWormParams(){

  vector<doubIntParamsHead> parvec;
  doubIntParamsHead var1;

  var1.parDoub.head = "Worm";
  var1.parDoub.names = {"NMJ_DA", "NMJ_DB", "NMJ_VD", "NMJ_VB", "NMJ_VA", "NMJ_DD"};
  var1.parDoub.vals = {NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD};
  append<string>(var1.parDoub.names,{"AVA_act", "AVA_inact", "AVB_act", "AVB_inact"});
  append<string>(var1.parDoub.names,{"AVA_output", "AVB_output"});
  append<double>(var1.parDoub.vals,{AVA_act, AVA_inact, AVB_act, AVB_inact});
  append<double>(var1.parDoub.vals,{AVA_output, AVB_output});

  var1.parInt.head = "Worm";
  var1.parInt.vals = {N_stretchrec, NmusclePerNU};
  var1.parInt.names = {"N_stretchrec", "NmusclePerNU"};
  var1.parInt.messages = 
  {"Number of stretch receptors", "All the way down to 24, in groups of 3 per unit"};
  var1.parInt.messages_inds = {0,1};


  parvec.push_back(var1);
  return parvec;

}


void WormCE::addParsToJson(json & j)
{

  string nsHead = "Nervous system";
  appendAllNSJson(j[nsHead], n);
  Worm2DCE::addParsToJson(j);
}

void WormCE::DumpVoltage(ofstream &ofs, int skips)
{
  static int tt = skips;

  if (++tt >= skips) {
    tt = 0;

    ofs << t;
    // Ventral Cord Motor Neurons
    for (int i = 1; i <= par1.N_units; i++) {
      for (int j = 1; j <= par1.N_neuronsperunit; j++) {
        ofs <<  " " << n.NeuronState(nn(j,i));
      }
    }
    ofs << "\n";
  }
}


void WormCE::DumpParams(ofstream &ofs) {
  ofs << "Time-constants: \n" <<
  "\n DA: " << n.NeuronTimeConstant(DA) <<
  "\n DB: " << n.NeuronTimeConstant(DB) <<
  "\n DD: " << n.NeuronTimeConstant(DD) <<
  "\n VD: " << n.NeuronTimeConstant(VD) <<
  "\n VA: " << n.NeuronTimeConstant(VA) <<
  "\n VB: " << n.NeuronTimeConstant(VB) << endl;

  ofs << "Biases: \n" <<
  "\n DA: " << n.NeuronBias(DA) <<
  "\n DB: " << n.NeuronBias(DB) <<
  "\n DD: " << n.NeuronBias(DD) <<
  "\n VD: " << n.NeuronBias(VD) <<
  "\n VA: " << n.NeuronBias(VA) <<
  "\n VB: " << n.NeuronBias(VB) << endl;

  ofs << "Self conns: \n" <<
  "\n DA: " << n.ChemicalSynapseWeight(DA, DA) <<
  "\n DB: " << n.ChemicalSynapseWeight(DB, DB) <<
  "\n DD: " << n.ChemicalSynapseWeight(DD, DD) <<
  "\n VD: " << n.ChemicalSynapseWeight(VD, VD) <<
  "\n VA: " << n.ChemicalSynapseWeight(VA, VA) <<
  "\n VB: " << n.ChemicalSynapseWeight(VB, VB) << endl;

  ofs << "Interneuron propierties: \n AVA active state: " << AVA_act <<
  "\n AVB active state: " << AVB_act <<
  "\n AVA inactive state: " << AVA_inact <<
  "\n AVB inactive state: " << AVB_inact << endl;

  ofs << "Chem Conns: \n" <<
  "\n DA->VD: " << n.ChemicalSynapseWeight(DA, VD) <<
  "\n DB->VD: " << n.ChemicalSynapseWeight(DB, VD) <<
  "\n VD->VA: " << n.ChemicalSynapseWeight(VD, VA) <<
  "\n VD->VB: " << n.ChemicalSynapseWeight(VD, VB) <<
  "\n VA->DD: " << n.ChemicalSynapseWeight(VA, DD) <<
  "\n VA->VD: " << n.ChemicalSynapseWeight(VA, VD) <<
  "\n VB->DD: " << n.ChemicalSynapseWeight(VB, DD) <<
  "\n VB->VD: " << n.ChemicalSynapseWeight(VB, VD) <<  endl;

  ofs << "Gap Juncs: \n DD-DD+1: " << n.ElectricalSynapseWeight(DD, DD+par1.N_neuronsperunit) <<
  "\n VB-VB+1: " << n.ElectricalSynapseWeight(VB, VB+par1.N_neuronsperunit) <<
  "\n VD-VD+1: " << n.ElectricalSynapseWeight(VD, VD+par1.N_neuronsperunit) << endl;


  Worm2DCE::DumpParams(ofs);
 
}