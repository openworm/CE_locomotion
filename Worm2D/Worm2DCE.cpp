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

 
Worm2DCE::Worm2DCE(json & j):Worm2Dm(
  {j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  }, new c302ForW2D(), new Muscles),Worm2D({j["Worm"]["N_neuronsperunit"]["value"], 
    j["Worm"]["N_muscles"]["value"], 
    j["Worm"]["T_muscle"]["value"],
    j["Worm"]["N_units"]["value"],
    j["Nervous system"]["size"]["value"]
  } ,0)
{
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

AVA_output = 0.0;
AVB_output = 0.0;

cout << "Worm2DCE const" << endl;

}

void Worm2DCE::initForSimulation(RandomState & rs)
{

  cout << "initForSimulation" << endl;
  cout << sr.SR_A_gain << " " << sr.SR_B_gain 
  << " " << AVA_output << " " << AVB_output << " " << AVA_inact << " " << AVB_act << endl;

  sr.SR_A_gain = 0.0;
  AVA_output =  AVA_inact;
  AVB_output =  AVB_act;

 
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
    #ifdef SR_TRANS_STRETCH
    ds = ds < 0.0 ? 0.0 : ds;
    vs = vs < 0.0 ? 0.0 : vs;
    #endif
    #ifdef SR_TRANS_CONTRACT
    ds = ds < 0.0 ? ds : 0.0;
    vs = vs < 0.0 ? vs : 0.0;
    #endif
    #ifdef SR_TRANS_ABS
    ds = ds < 0.0 ? -ds : ds;
    vs = vs < 0.0 ? -vs : vs;
    #endif
    #ifdef SR_TRANS_NEG
    ds = -ds;
    vs = -vs;
    #endif
    sr.SetDorsalInput(i, ds);
    sr.SetVentralInput(i, vs);
  }

  // Update Stretch Receptors
  sr.Update();

  // Set input to Nervous System (Ventral Cord) from Stretch Receptors AND Command Interneurons
  ////   To A_class motorneurons
  for (int i = 1; i <= par1.N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DA,i), sr.A_D_sr(i) + AVA_output);
    n_ptr->SetNeuronExternalInput(nn(VA,i), sr.A_V_sr(i) + AVA_output);
  }
  ////   To B_class motorneurons
  for (int i = 1; i <= par1.N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DB,i), sr.B_D_sr(i) + AVB_output);
    n_ptr->SetNeuronExternalInput(nn(VB,i), sr.B_V_sr(i) + AVB_output);
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


void Worm2DCE::addParsToJson(json & j)
{
    Params<double> par = sr.getStretchReceptorParams();
    appendToJson<double>(j["Stretch receptor"], par);
    Worm2D::addParsToJson(j);
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
