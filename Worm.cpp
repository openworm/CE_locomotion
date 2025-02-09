//
//  Worm.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "Worm.h"


int nn(int neuronNumber, int unitNumber)
{
  return neuronNumber+((unitNumber-1)*N_neuronsperunit);
}

string nervousSystemName = "NervousSystem";
NervousSystemBase* makeNervousSystem()
{
if (strcmp(nervousSystemName.c_str(),"NervousSystem") == 0) return new NervousSystem();
//if (nervousSystemName=="NervousSystem") return new NervousSystem();
cout << "that nervous system is not implemented" << endl; 
exit(0);
}

//template<class T>
Worm::Worm(TVector<double> &v,double output):n_ptr(makeNervousSystem())
{

  // PG: Setting these to zero as they were not initialised before use!
  // Note: the usage of these needs to be further investigated!
  AVA_act = 0;
  AVA_inact = 0;
  AVB_act = 0;
  AVB_inact = 0;

  // Muscles
  m.SetMuscleParams(N_muscles, T_muscle);

  // Nervous system
  n_ptr->SetCircuitSize(N_units*N_neuronsperunit, 3, 2);

  int da, db, dd, vd, vb, va;
  int ddNext, vdNext, vbNext, dbNext;

  // Stretch receptor
  sr.SetStretchReceptorParams(N_segments, N_stretchrec, v(1), v(2));

  for (int u = 1; u <= N_units; u++){
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
    n_ptr->SetNeuronBias(da, v(3));
    n_ptr->SetNeuronBias(va, v(3));
    n_ptr->SetNeuronBias(db, v(4));
    n_ptr->SetNeuronBias(vb, v(4));
    n_ptr->SetNeuronBias(dd, v(5));
    n_ptr->SetNeuronBias(vd, v(5));

    // Time-constants fixed to 1.0
    for (int i = 1; i <= N_neuronsperunit; i++){
      n_ptr->SetNeuronTimeConstant(nn(i,u), 1.0);
    }

    // Self-connections
    n_ptr->SetChemicalSynapseWeight(da,da, v(6));
    n_ptr->SetChemicalSynapseWeight(va,va, v(6));
    n_ptr->SetChemicalSynapseWeight(db,db, v(7));
    n_ptr->SetChemicalSynapseWeight(vb,vb, v(7));
    n_ptr->SetChemicalSynapseWeight(dd,dd, v(8));
    n_ptr->SetChemicalSynapseWeight(vd,vd, v(8));

    // Cross-connections
    // Excitatory Chemical Synapses intraunit
    n_ptr->SetChemicalSynapseWeight(da, vd, v(9));
    n_ptr->SetChemicalSynapseWeight(va, dd, v(9));
    n_ptr->SetChemicalSynapseWeight(vb, dd, v(10));
    n_ptr->SetChemicalSynapseWeight(db, vd, v(10));

    // Inhibitory Chemical Synapses intraunit
    n_ptr->SetChemicalSynapseWeight(vd, va, v(11));
    n_ptr->SetChemicalSynapseWeight(dd, da, v(11));
    n_ptr->SetChemicalSynapseWeight(vd, vb, v(12));
    n_ptr->SetChemicalSynapseWeight(dd, db, v(12));

    // Electrical Synapse Intersegment connections
    if (u < N_units){
      n_ptr->SetElectricalSynapseWeight(dd, ddNext, v(13));
      n_ptr->SetElectricalSynapseWeight(vd, vdNext, v(13));
      n_ptr->SetElectricalSynapseWeight(vb, vbNext, v(14));
      n_ptr->SetElectricalSynapseWeight(db, dbNext, v(14));
    }
  }

  // Excitatory VNC NMJ Weight
  NMJ_DA = v(15);
  NMJ_VA = v(15);
  NMJ_DB = v(16);
  NMJ_VB = v(16);

  // Inhibitory VNC NMJ Weight
  NMJ_DD = v(17);
  NMJ_VD = v(17);

  AVA_output = 0.0;
  AVB_output = 0.0;
}





//template<class T>
void Worm::InitializeState(RandomState &rs)
{
  t = 0.0;
  //n_ptr->RandomizeCircuitState(-1.0, 1.0, rs);
  for (int u = 1; u <= N_units; u++)
  {
    // Dorsal neurons
    n_ptr->SetNeuronOutput(nn(DA,u), 0.1);
    n_ptr->SetNeuronOutput(nn(DB,u), 0.1);
    n_ptr->SetNeuronOutput(nn(DD,u), 0.9);
    // Ventral neurons
    n_ptr->SetNeuronOutput(nn(VA,u), 0.9);
    n_ptr->SetNeuronOutput(nn(VB,u), 0.9);
    n_ptr->SetNeuronOutput(nn(VD,u), 0.1);
  }
  b.InitializeBodyState();
  m.InitializeMuscleState();
}

//template<class T>
void Worm::Step(double StepSize, double output)
{
  int mi;
  int mt = 0;
  double ds, vs;
  TVector<double> dorsalInput(1, N_units);
  TVector<double> ventralInput(1, N_units);

  // Update Body
  b.StepBody(StepSize);

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
  for (int i = 1; i <= N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DA,i), sr.A_D_sr(i) + AVA_output);
    n_ptr->SetNeuronExternalInput(nn(VA,i), sr.A_V_sr(i) + AVA_output);
  }
  ////   To B_class motorneurons
  for (int i = 1; i <= N_units; i++){
    n_ptr->SetNeuronExternalInput(nn(DB,i), sr.B_D_sr(i) + AVB_output);
    n_ptr->SetNeuronExternalInput(nn(VB,i), sr.B_V_sr(i) + AVB_output);
  }

  // Update Nervous System
  n_ptr->EulerStep(StepSize);

  // Set input to Muscles
  //  Each motor neuron innervates four muscles, overlap in muscles 4, 6-19 and 21)
  // Load motorneuron activity
  for (int i=1; i<=N_units; i++){
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
  m.EulerStep(StepSize);

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
  b.SetDorsalSegmentActivation(N_segments-1, m.DorsalMuscleOutput(N_muscles)/2);
  b.SetVentralSegmentActivation(N_segments-1, m.VentralMuscleOutput(N_muscles)/2);
  b.SetDorsalSegmentActivation(N_segments, m.DorsalMuscleOutput(N_muscles)/2);
  b.SetVentralSegmentActivation(N_segments, m.VentralMuscleOutput(N_muscles)/2);

  // Time
  t += StepSize;
}

//template<class T>
double Worm::CoMx()
{
  double temp = 0.0;
  for (int i = 1; i <= N_rods; i++) {
    temp += b.X(i);
  }
  return temp/N_rods;
}

//template<class T>
double Worm::CoMy()
{
  double temp = 0.0;
  for (int i = 1; i <= N_rods; i++) {
    temp += b.Y(i);
  }
  return temp/N_rods;
}

//template<class T>
void Worm::Curvature(TVector<double> &c)
{
  double dx1,dy1,dx2,dy2,a,a1,a2,seg;
  int k=1;

  for (int i = 3; i < N_segments-1; i+=2)
  {
    dx1 = b.X(i) - b.X(i-2);
    dy1 = b.Y(i) - b.Y(i-2);
    dx2 = b.X(i+2) - b.X(i);
    dy2 = b.Y(i+2) - b.Y(i);

    a1 = atan2(dy1,dx1);
    a2 = atan2(dy2,dx2);

    if (a1 > PI/2 and a2 < -PI/2)
    a = (a1 - 2*PI) - a2;
    else
    if (a1 < -PI/2 and a2 > PI/2)
    a = a1 - (a2 - 2*PI);
    else
    a = a1-a2;

    seg = sqrt(pow(b.X(i-2)-b.X(i+2),2) + pow(b.Y(i-2)-b.Y(i+2),2));
    c(k) = (2*sin(a)/seg)/1000;
    k++;
  }
}

//template<class T>
void Worm::AngleCurvature(TVector<double> &c)
{
  double dx1,dy1,dx2,dy2,a,a1,a2,seg;
  int k=1;

  for (int i = 3; i < N_segments-1; i+=2)
  {
    dx1 = b.X(i) - b.X(i-2);
    dy1 = b.Y(i) - b.Y(i-2);
    dx2 = b.X(i+2) - b.X(i);
    dy2 = b.Y(i+2) - b.Y(i);

    a1 = atan2(dy1,dx1);
    a2 = atan2(dy2,dx2);

    if (a1 > PI/2 and a2 < -PI/2)
    a = (a1 - 2*PI) - a2;
    else
    if (a1 < -PI/2 and a2 > PI/2)
    a = a1 - (a2 - 2*PI);
    else
    a = a1-a2;
    c(k) = a;
    k++;
  }
}

//template<class T>
double Worm::Orientation()
{
  return atan2(b.Y(Head)-b.Y(Tail),b.X(Head)-b.X(Tail));
}

// Dump the state to OFS if SKIPS steps have been performed
//template<class T>
void Worm::DumpBodyState(ofstream &ofs, int skips)
{
  static int tt = skips;

  if (++tt >= skips) {
    tt = 0;

    ofs << t;
    // Body
    for (int i = 1; i <= N_rods; i++)
    {
      ofs <<  " " << b.X(i) << " " << b.Y(i) << " " << b.Phi(i);
    }
    ofs << "\n";
  }
}
//template<class T>
void Worm::DumpActState(ofstream &ofs, int skips)
{
  static int tt = skips;

  if (++tt >= skips) {
    tt = 0;

    ofs << t;
    //ofs << "\nSR: ";
    // Stretch receptors
    for (int i = 1; i <= N_stretchrec; i++) {
      ofs <<  " " << sr.A_D_sr(i) << " " << sr.A_V_sr(i) << " " << sr.B_D_sr(i) << " " << sr.B_V_sr(i);
    }
    // Ventral Cord Motor Neurons
    //ofs << "\nV: ";
    for (int i = 1; i <= N_units; i++) {
      for (int j = 1; j <= N_neuronsperunit; j++) {
        ofs <<  " " << n_ptr->NeuronOutput(nn(j,i));
      }
    }
    // Muscles
    //ofs << "\nM: ";
    for (int i = 1; i <= N_muscles; i++) {
      ofs <<  " " << m.DorsalMuscleOutput(i) << " " << m.VentralMuscleOutput(i);
    }
    ofs << "\n";
  }
}

//template<class T>
void Worm::DumpCurvature(ofstream &ofs, int skips)
{

  double dx1,dy1,dx2,dy2,a,a1,a2,seg;
  static int tt = skips;

  if (++tt >= skips) {
    tt = 0;
    //time
    ofs << t;

    for (int i = 3; i < N_segments-1; i+=2)
    {
      dx1 = b.X(i) - b.X(i-2);
      dy1 = b.Y(i) - b.Y(i-2);
      dx2 = b.X(i+2) - b.X(i);
      dy2 = b.Y(i+2) - b.Y(i);

      a1 = atan2(dy1,dx1);
      a2 = atan2(dy2,dx2);

      if (a1 > PI/2 and a2 < -PI/2)
      a = (a1 - 2*PI) - a2;
      else
      if (a1 < -PI/2 and a2 > PI/2)
      a = a1 - (a2 - 2*PI);
      else
      a = a1-a2;

      seg = sqrt(pow(b.X(i-2)-b.X(i+2),2) + pow(b.Y(i-2)-b.Y(i+2),2));
      ofs <<  " " << (2*sin(a)/seg)/1000;
    }
    ofs << "\n";
  }
}

//template<class T>
void Worm::DumpVoltage(ofstream &ofs, int skips)
{
  static int tt = skips;

  if (++tt >= skips) {
    tt = 0;

    ofs << t;
    // Ventral Cord Motor Neurons
    for (int i = 1; i <= N_units; i++) {
      for (int j = 1; j <= N_neuronsperunit; j++) {
        ofs <<  " " << n_ptr->NeuronState(nn(j,i));
      }
    }
    ofs << "\n";
  }
}


//template<class T>
void Worm::DumpParams(ofstream &ofs) {
  ofs << "Time-constants: \n" <<
  "\n DA: " << n_ptr->NeuronTimeConstant(DA) <<
  "\n DB: " << n_ptr->NeuronTimeConstant(DB) <<
  "\n DD: " << n_ptr->NeuronTimeConstant(DD) <<
  "\n VD: " << n_ptr->NeuronTimeConstant(VD) <<
  "\n VA: " << n_ptr->NeuronTimeConstant(VA) <<
  "\n VB: " << n_ptr->NeuronTimeConstant(VB) << endl;

  ofs << "Biases: \n" <<
  "\n DA: " << n_ptr->NeuronBias(DA) <<
  "\n DB: " << n_ptr->NeuronBias(DB) <<
  "\n DD: " << n_ptr->NeuronBias(DD) <<
  "\n VD: " << n_ptr->NeuronBias(VD) <<
  "\n VA: " << n_ptr->NeuronBias(VA) <<
  "\n VB: " << n_ptr->NeuronBias(VB) << endl;

  ofs << "Self conns: \n" <<
  "\n DA: " << n_ptr->ChemicalSynapseWeight(DA, DA) <<
  "\n DB: " << n_ptr->ChemicalSynapseWeight(DB, DB) <<
  "\n DD: " << n_ptr->ChemicalSynapseWeight(DD, DD) <<
  "\n VD: " << n_ptr->ChemicalSynapseWeight(VD, VD) <<
  "\n VA: " << n_ptr->ChemicalSynapseWeight(VA, VA) <<
  "\n VB: " << n_ptr->ChemicalSynapseWeight(VB, VB) << endl;

  ofs << "Interneuron propierties: \n AVA active state: " << AVA_act <<
  "\n AVB active state: " << AVB_act <<
  "\n AVA inactive state: " << AVA_inact <<
  "\n AVB inactive state: " << AVB_inact << endl;

  ofs << "Chem Conns: \n" <<
  "\n DA->VD: " << n_ptr->ChemicalSynapseWeight(DA, VD) <<
  "\n DB->VD: " << n_ptr->ChemicalSynapseWeight(DB, VD) <<
  "\n VD->VA: " << n_ptr->ChemicalSynapseWeight(VD, VA) <<
  "\n VD->VB: " << n_ptr->ChemicalSynapseWeight(VD, VB) <<
  "\n VA->DD: " << n_ptr->ChemicalSynapseWeight(VA, DD) <<
  "\n VA->VD: " << n_ptr->ChemicalSynapseWeight(VA, VD) <<
  "\n VB->DD: " << n_ptr->ChemicalSynapseWeight(VB, DD) <<
  "\n VB->VD: " << n_ptr->ChemicalSynapseWeight(VB, VD) <<  endl;

  ofs << "Gap Juncs: \n DD-DD+1: " << n_ptr->ElectricalSynapseWeight(DD, DD+N_neuronsperunit) <<
  "\n VB-VB+1: " << n_ptr->ElectricalSynapseWeight(VB, VB+N_neuronsperunit) <<
  "\n VD-VD+1: " << n_ptr->ElectricalSynapseWeight(VD, VD+N_neuronsperunit) << endl;

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
