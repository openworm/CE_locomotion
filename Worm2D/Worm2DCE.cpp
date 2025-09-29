//
//  WormCE.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "Worm2DCE.h"
//#include "../argUtils.h"
#include "../neuromlLocal/c302ForW2D.h"


Worm2DCE:: Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_)://W2Dbaseparameters1(new W2DCEpars()),
    //W2DCEpars1(dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1)),
    //W2DCEpars1(new W2DCEpars()),Worm2Dm(par1_,nullptr),Worm2D(par1_,0)
    
    Worm2Dm(par1_, n_ptr_, make_shared<W2DCEpars>()), Worm2D(par1_,0),
    W2DCEpars1(dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1))
    {

      W2DCEpars1->AVA_output = 0.0;
      W2DCEpars1->AVB_output = 0.0;

      sr.SRForm = W2DCEpars1->SRForm;
      pheno_A_gain = sr.SR_A_gain;
      pheno_B_gain = sr.SR_B_gain;

    }

   // {assert(0);}
    //Worm2Dm(par1_,(shared_ptr<W2DCEpars>) new W2DCEpars()),Worm2D(par1_,0),
    //Worm2Dm(par1_,dynamic_pointer_cast<W2Dbaseparameters>(new W2DCEpars())),Worm2D(par1_,0)
    //W2DCEpars1(dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1)){}

    //W2DCEpars1(new W2DCEpars()),Worm2Dm(par1_,n_ptr_, nullptr),Worm2D(par1_,0)
    //{W2Dbaseparameters1 = W2DCEpars1;}

    //Worm2DCE(wormIzqParams par1_, NSForW2D * n_ptr_, const W2DCEpars & W2DCEpars1_):
    //Worm2Dm(par1_,n_ptr_),Worm2D(par1_,0),W2DCEpars1(W2DCEpars1_){}





Worm2DCE::Worm2DCE(json & j):Worm2Dm(
  {j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  }, new c302ForW2D(), make_shared<W2DCEpars>()),
  Worm2D({j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  } ,0),W2DCEpars1(dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1))
{

  W2DCEpars1->setParsFromJson(j["Worm"]);
  //W2Dbaseparameters1 = W2DCEpars1;
  cout << "Worm2DCE const" << endl;

  AVA_act = 0;
  AVA_inact = 0;
  AVB_act = 0;
  AVB_inact = 0;
 
  sr.SetStretchReceptorParams(N_segments, N_stretchrec,
    j["Stretch receptor"]["SR_A_gain"]["value"] , j["Stretch receptor"]["SR_B_gain"]["value"]);

// Excitatory VNC NMJ Weight
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

pheno_A_gain = sr.SR_A_gain;
pheno_B_gain = sr.SR_B_gain;
sr.SRForm = W2DCEpars1->SRForm;

}



WormCE::WormCE()://Worm2Dm({6,24,0.1,10,60}, new NervousSystem(), new Muscles),
Worm2Dm({6,24,0.1,10,60}, new NervousSystem(), make_shared<W2DCEpars>()),
//Worm2Dm({6,24,0.1,10,60}, new NervousSystem(), nullptr),
//EvolvableS(shared_ptr<EvolparametersCE>(new EvolparametersCE())),
n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2DCE({6,24,0.1,10,60},0)
  //dynamic_cast<shared_ptr<W2DCEpars> &>(*evolvable_w2par_ptr))
  {}
  //{assert(0);}


WormCE::WormCE(int argc, const char* argv[]):
Worm2Dm({6,24,0.1,10,60}, new NervousSystem(), make_shared<W2DCEpars>(argc,argv)),
n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2DCE({6,24,0.1,10,60},0){}


WormCE::WormCE(int argc, const char* argv[], TVector<double> &geno):
WormCE(argc,argv){setParsFromGeno(geno);}


WormCE::WormCE(TVector<double> &pheno):WormCE(pheno, true){}


WormCE::WormCE(const string & filename_):WormCE()
{
    setParsFromFile(filename_);
}

// The constructor
WormCE::WormCE(TVector<double> &phengen, bool isPheno):WormCE()
{

    if (isPheno) setParsFromPheno(phengen);
    else setParsFromGeno(phengen);

}






void Worm2DCE::initForSimulation(RandomState & rs)
{

  return;

  /* cout << "initForSimulation" << endl;
  cout << sr.SR_A_gain << " " << sr.SR_B_gain 
  << " " << AVA_output << " " << AVB_output << " " << AVA_inact << " " << AVB_act << endl;

  sr.SR_A_gain = 0.0;
  AVA_output =  AVA_inact;
  AVB_output =  AVB_act;
 */
 

}




void Worm2DCE::setForward()
{
  //assert(0);
  sr.SR_A_gain = pheno_A_gain;
  sr.SR_B_gain = pheno_B_gain;

  sr.SR_A_gain = 0.0;
  //AVA_output =  1;
  //AVB_output =  0;
  W2DCEpars1->AVA_output =  0;
  W2DCEpars1->AVB_output =  W2DCEpars1->AB_output_level;
}

void Worm2DCE::setBackward()
{
  //assert(0);
  sr.SR_A_gain = pheno_A_gain;
  sr.SR_B_gain = pheno_B_gain;

  sr.SR_B_gain = 0.0;
  //AVA_output =  0;
  //AVB_output =  1;
  W2DCEpars1->AVA_output =  W2DCEpars1->AB_output_level;
  W2DCEpars1->AVB_output =  0;
}


/* void Worm2DCE::setPars(int argc, const char* argv[])
{
  
  *W2DCEpars1 = W2DCEpars(argc,argv);
  assert(W2DCEpars1->sr_type == "SR_TRANS_STRETCH" ||  W2DCEpars1->sr_type ==  "SR_TRANS_CONTRACT" 
    || W2DCEpars1->sr_type == "SR_TRANS_ABS" 
    ||  W2DCEpars1->sr_type == "SR_TRANS_NEG" || W2DCEpars1->sr_type == "None");

} */



shared_ptr<const W2Dparameters> Worm2DCE::setWormPars(shared_ptr<const CmdArgs> cmd)
{
  
  //W2DCEpars w1(argc,argv);
  W2DCEpars w1(cmd);
  *W2DCEpars1 = w1; 
  sr.SRForm = W2DCEpars1->SRForm;

 

  //W2DCEpars1->show();
  //assert(0);

  assert(W2DCEpars1->sr_type == "SR_TRANS_STRETCH" ||  W2DCEpars1->sr_type ==  "SR_TRANS_CONTRACT" 
    || W2DCEpars1->sr_type == "SR_TRANS_ABS" 
    ||  W2DCEpars1->sr_type == "SR_TRANS_NEG" || W2DCEpars1->sr_type == "None");

  return W2Dbaseparameters1;

  //evolvable_worm_pars_ptr = W2DCEpars1;
  //dynamic_cast<W2DCEpars&>(w2par_);
  //W2DCEpars1->show();
  //assert(0);
}

//shared_ptr<const W2Dparameters> WormCE::getWormPars() {return W2DCEpars1;}

void Worm2DCE::setWormPars(const W2Dparameters * w2par_)
{

//assert(0);

{ const W2DCEpars * const w1 = dynamic_cast<const W2DCEpars*>(w2par_);
  if (w1 != nullptr){
  *W2DCEpars1 = *w1;
  sr.SRForm = W2DCEpars1->SRForm;

  return;
  }
}

{ const W2DCEparsA * const w1 = dynamic_cast<const W2DCEparsA*>(w2par_);
  if (w1 != nullptr){
  W2DCEpars1->AVA_output = w1->AVA_output;
  W2DCEpars1->AVB_output = w1->AVB_output;
  W2DCEpars1->AB_output_level =  w1->AB_output_level;


  /* if (W2DCEpars1->AVB_output>0.5){
  cout << " xxx " << W2DCEpars1->AVA_output << " " << W2DCEpars1->AVB_output << " " << W2DCEpars1->AB_output_level << endl;
  //assert(0);
  }
  if (W2DCEpars1->AVB_output<0.5) {
  cout << " yyy " << W2DCEpars1->AVA_output << " " << W2DCEpars1->AVB_output << " " << W2DCEpars1->AB_output_level << endl;
  assert(0);
  }  */

  return;
  }

}

assert(0 && "not correct cast");

  //W2DCEpars1->show();
    //assert(0);
}



void Worm2DCE::InitializeState(RandomState &rs)
{
  Worm2D::InitializeState(rs);
}

void Worm2DCE::Step1()
{
  int mi;
  int mt = 0;
  double ds, vs;
  TVector<double> dorsalInput(1, par1.N_units);
  TVector<double> ventralInput(1, par1.N_units);

  // Update Body
  b.StepBody(settedStepSize);

  // Set input to Stretch Receptors from Body
  // Input to SR only if the segment stretch
  double d_sr,v_sr;
  for(int i = 1; i <= N_segments; ++i){
    ds = (b.DorsalSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
    vs = (b.VentralSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);

    
    if (W2DCEpars1->sr_type == "SR_TRANS_STRETCH")
    {
    ds = ds < 0.0 ? 0.0 : ds;
    vs = vs < 0.0 ? 0.0 : vs;
    }
    else if (W2DCEpars1->sr_type == "SR_TRANS_CONTRACT")
    {
    ds = ds < 0.0 ? ds : 0.0;
    vs = vs < 0.0 ? vs : 0.0;
    }
    else if (W2DCEpars1->sr_type == "SR_TRANS_ABS")
    {
    ds = ds < 0.0 ? -ds : ds;
    vs = vs < 0.0 ? -vs : vs;
    }
    else if (W2DCEpars1->sr_type == "SR_TRANS_NEG")
    {
    ds = -ds;
    vs = -vs;
    }

    sr.SetDorsalInput(i, ds);
    sr.SetVentralInput(i, vs);
  }

  // Update Stretch Receptors
  sr.Update();

  // Set input to Nervous System (Ventral Cord) from Stretch Receptors AND Command Interneurons
  ////   To A_class motorneurons
  for (int i = 1; i <= par1.N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DA,i), sr.A_D_sr(i) + W2DCEpars1->AVA_output*W2DCEpars1->AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(DA,i), sr.A_D_sr(i) + AVA_output);
    n_ptr->SetNeuronExternalInput(nn(VA,i), sr.A_V_sr(i) + W2DCEpars1->AVA_output*W2DCEpars1->AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(VA,i), sr.A_V_sr(i) + AVA_output);
  }
  ////   To B_class motorneurons
  for (int i = 1; i <= par1.N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DB,i), sr.B_D_sr(i) + W2DCEpars1->AVB_output*W2DCEpars1->AB_output_level);
    n_ptr->SetNeuronExternalInput(nn(VB,i), sr.B_V_sr(i) + W2DCEpars1->AVB_output*W2DCEpars1->AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(DB,i), sr.B_D_sr(i) + AVB_output);
    //n_ptr->SetNeuronExternalInput(nn(VB,i), sr.B_V_sr(i) + AVB_output);
  }

  // Update Nervous System
  n_ptr->EulerStep(settedStepSize);
  //cout << "step " << t << endl;
   
  // Set input to Muscles
  //  Each motor neuron innervates four muscles, overlap in muscles 4, 6-19 and 21)
  // Load motorneuron activity
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

  // Set input to Mechanical Body
  //  First two segments receive special treatment because they are only affected by a single muscle
  b.SetDorsalSegmentActivation(1, m.DorsalMuscleOutput(1)/2);
  b.SetVentralSegmentActivation(1, m.VentralMuscleOutput(1)/2);
  b.SetDorsalSegmentActivation(2, m.DorsalMuscleOutput(1)/2);
  b.SetVentralSegmentActivation(2, m.VentralMuscleOutput(1)/2);

  //  All other segments receive force from two muscles
  for (int i = 3; i <= N_segments-2; i++)
  {
    mi = (int) ((i-1)/2);
    b.SetDorsalSegmentActivation(i, (m.DorsalMuscleOutput(mi) + m.DorsalMuscleOutput(mi+1))/2);
    b.SetVentralSegmentActivation(i, (m.VentralMuscleOutput(mi) + m.VentralMuscleOutput(mi+1))/2);
  }

  //  Last two segments receive special treatment because they are only affected by a single muscle
  b.SetDorsalSegmentActivation(N_segments-1, m.DorsalMuscleOutput(par1.N_muscles)/2);
  b.SetVentralSegmentActivation(N_segments-1, m.VentralMuscleOutput(par1.N_muscles)/2);
  b.SetDorsalSegmentActivation(N_segments, m.DorsalMuscleOutput(par1.N_muscles)/2);
  b.SetVentralSegmentActivation(N_segments, m.VentralMuscleOutput(par1.N_muscles)/2);

  // Time
  //t += StepSize;
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
  append<double>(var1.parDoub.vals,{W2DCEpars1->AVA_output, W2DCEpars1->AVB_output});

  var1.parInt.head = "Worm";
  var1.parInt.vals = {N_stretchrec, NmusclePerNU};
  var1.parInt.names = {"N_stretchrec", "NmusclePerNU"};
  var1.parInt.messages = 
  {"Number of stretch receptors", "All the way down to 24, in groups of 3 per unit"};
  var1.parInt.messages_inds = {0,1};


  parvec.push_back(var1);
  return parvec;

}




/* void WormCE::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{

  if (evotype_=="EvoCE"){
  
  EvolparametersCE & Epars1 = dynamic_cast<EvolparametersCE&>(*(w2par_ptr_));

  //W2DCEpars1 = dynamic_cast<shared_ptr<W2DCEpars> &> (*w2par_ptr_);

  //W2DCEpars1 = shared_ptr<W2DCEpars>(w2par_ptr_);

  //AVA_output = Epars1.AVA_output;
  //AVB_output = Epars1.AVB_output;
  //sr_type = Epars1.sr_type;
  return;
  }
  cout << "evotype is " << evotype_ << endl;
  assert(0 && "Evotype not implemented");

} */


void WormCE::setParsFromPheno(TVector<double> &pheno)
{
// PG: Setting these to zero as they were not initialised before use!
  // Note: the usage of these needs to be further investigated!
  AVA_act = 0;
  AVA_inact = 0;
  AVB_act = 0;
  AVB_inact = 0;

  // Muscles
  //m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
  // Nervous system
  n.SetCircuitSize(par1.N_units*par1.N_neuronsperunit, 3, 2);

  int da, db, dd, vd, vb, va;
  int ddNext, vdNext, vbNext, dbNext;

  // Stretch receptor
  sr.SetStretchReceptorParams(N_segments, N_stretchrec, pheno(1), pheno(2));

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

  pheno_A_gain = sr.SR_A_gain;
  pheno_B_gain = sr.SR_B_gain;
}


void WormCE::randomizeNS(RandomState &rs)
{
 
  if (W2Dbaseparameters1->randomInitialState) {
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



  if (W2Dbaseparameters1->randomInitialState) {
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

void WormCE::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
 
  shared_ptr<W2DCEpars> w1 = dynamic_pointer_cast<W2DCEpars>(W2Dbaseparameters1);


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


     // Parameters for the Stretch Receptors
  phen(SR_A) = MapSearchParameter(gen(SR_A), w1->SREvoBot, SRmax);
  phen(SR_B) = MapSearchParameter(gen(SR_B), w1->SREvoBot, SRmax);

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
    Params<double> par = sr.getStretchReceptorParams();
    appendToJson<double>(j["Stretch receptor"], par);
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
      ofs <<  " " << sr.A_D_sr(i) << " " << sr.A_V_sr(i) << " " << sr.B_D_sr(i) << " " << sr.B_V_sr(i);
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
  
  ofs << "Stretch Receptors Gains: \n A-class SR: " << sr.SR_A_gain <<
  "\n B-class SR: " << sr.SR_B_gain <<  endl;

  ofs << "NMJ weights: \n" <<
  "\n DA: " << NMJ_DA <<
  "\n DB: " << NMJ_DB <<
  "\n DD: " << NMJ_DD <<
  "\n VD: " << NMJ_VD <<
  "\n VA: " << NMJ_VA <<
  "\n VB: " << NMJ_VB <<  endl;
}