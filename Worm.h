//
//  Worm.hpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "VectorMatrix.h"
#include "random.h"
#include "WormBody.h"
#include "neuroml/NervousSystem2D.h"
//#include "NervousSystem.h"
#include "Muscles.h"
#include "StretchReceptor.h"
#include "NervousSystemBase.h"

#include <cmath>

#define PI 3.14159265
#define NERVOUSSYSTEM NervousSystem2D

// Stretch-Receptor Transdusction form
// Altogether there are 8 forms this can take, depending on which of the first three are defined and then the second one.
// Note: If none of these three are defined, then the LINEAR form is the default.
//#define SR_TRANS_STRETCH
///#define SR_TRANS_CONTRACT
//#define SR_TRANS_ABS
// If NEG is not defined, then the transformation has a positive relationship.
//#define SR_TRANS_NEG

using namespace std;

// Parameters
const int N_muscles = 24;               // Number of muscles alongside the body
const int N_units = 10;                 // Number of neural units in VNC
const int N_neuronsperunit = 6;         // Number of neurons in a VNC neural unit (6 neurons)
//const int H_neuronsperunit = 3;         // Half for DV symmetry

const int N_stretchrec = 10;            // N_units // Number of stretch receptors
//
const double T_muscle = 0.1;            // Muscle time constant

const int NmusclePerNU = 4;             // All the way down to 24, in groups of 3 per unit

// Motoneuron name conventions
const int DA = 1;
const int DB = 2;
const int DD = 3;
const int VD = 4;
const int VA = 5;
const int VB = 6;

// Body segment name conventions
const int Head = 1;
const int Tail = N_segments;

int nn(int neuronNumber, int unitNumber);

/* {
  return neuronNumber+((unitNumber-1)*N_neuronsperunit);
}
 */

class wormForJson;

template<class T =  NervousSystem>
class Worm {
public:

    Worm(TVector<double> &v, double output);
    //Worm::Worm(TVector<double> &v,double output);
    void InitializeState(RandomState &rs);
    void HeadStep(double StepSize, double output);
    void Step(double StepSize, double output);

    void DumpBodyState(ofstream &ofs, int skips);
    void DumpActState(ofstream &ofs, int skips);
    void DumpVoltage(ofstream &ofs, int skips);
    void DumpParams(ofstream &ofs);
    void DumpCurvature(ofstream &ofs, int skips);
    
    operator wormForJson & () 
    {
    if (typeid(T) == typeid(NervousSystem)) //should check json interface is implemented
    return *this;
    cout << "cannot generate Json" << endl;
    exit(1);
    }

    double CoMx();
    double CoMy();
    void Curvature(TVector<double> &c);
    void AngleCurvature(TVector<double> &c);
    double Orientation();

    WormBody b;
    Muscles m;
    //NervousSystemInt<T> n;
    NervousSystemBase *n_ptr;
    T & n;
    StretchReceptor sr;

    double t; // Time

    // Neuromuscular junctions
    double NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD; //EEE
    double AVA_act, AVA_inact, AVB_act, AVB_inact;
    double AVA_output, AVB_output;

};

class wormForJson : public Worm<NervousSystem> {};



template<class T>
Worm<T>::Worm(TVector<double> &v,double output):
n_ptr(new T()),n(* dynamic_cast<T*>(n_ptr))
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
  n.SetCircuitSize(N_units*N_neuronsperunit, 3, 2);

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
    n.SetNeuronBias(da, v(3));
    n.SetNeuronBias(va, v(3));
    n.SetNeuronBias(db, v(4));
    n.SetNeuronBias(vb, v(4));
    n.SetNeuronBias(dd, v(5));
    n.SetNeuronBias(vd, v(5));

    // Time-constants fixed to 1.0
    for (int i = 1; i <= N_neuronsperunit; i++){
      n.SetNeuronTimeConstant(nn(i,u), 1.0);
    }

    // Self-connections
    n.SetChemicalSynapseWeight(da,da, v(6));
    n.SetChemicalSynapseWeight(va,va, v(6));
    n.SetChemicalSynapseWeight(db,db, v(7));
    n.SetChemicalSynapseWeight(vb,vb, v(7));
    n.SetChemicalSynapseWeight(dd,dd, v(8));
    n.SetChemicalSynapseWeight(vd,vd, v(8));

    // Cross-connections
    // Excitatory Chemical Synapses intraunit
    n.SetChemicalSynapseWeight(da, vd, v(9));
    n.SetChemicalSynapseWeight(va, dd, v(9));
    n.SetChemicalSynapseWeight(vb, dd, v(10));
    n.SetChemicalSynapseWeight(db, vd, v(10));

    // Inhibitory Chemical Synapses intraunit
    n.SetChemicalSynapseWeight(vd, va, v(11));
    n.SetChemicalSynapseWeight(dd, da, v(11));
    n.SetChemicalSynapseWeight(vd, vb, v(12));
    n.SetChemicalSynapseWeight(dd, db, v(12));

    // Electrical Synapse Intersegment connections
    if (u < N_units){
      n.SetElectricalSynapseWeight(dd, ddNext, v(13));
      n.SetElectricalSynapseWeight(vd, vdNext, v(13));
      n.SetElectricalSynapseWeight(vb, vbNext, v(14));
      n.SetElectricalSynapseWeight(db, dbNext, v(14));
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





template<class T>
void Worm<T>::InitializeState(RandomState &rs)
{
  t = 0.0;
  //n.RandomizeCircuitState(-1.0, 1.0, rs);
  for (int u = 1; u <= N_units; u++)
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
  b.InitializeBodyState();
  m.InitializeMuscleState();
}

template<class T>
void Worm<T>::Step(double StepSize, double output)
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
    n.SetNeuronExternalInput(nn(DA,i), sr.A_D_sr(i) + AVA_output);
    n.SetNeuronExternalInput(nn(VA,i), sr.A_V_sr(i) + AVA_output);
  }
  ////   To B_class motorneurons
  for (int i = 1; i <= N_units; i++){
    n.SetNeuronExternalInput(nn(DB,i), sr.B_D_sr(i) + AVB_output);
    n.SetNeuronExternalInput(nn(VB,i), sr.B_V_sr(i) + AVB_output);
  }

  // Update Nervous System
  n.EulerStep(StepSize);

  // Set input to Muscles
  //  Each motor neuron innervates four muscles, overlap in muscles 4, 6-19 and 21)
  // Load motorneuron activity
  for (int i=1; i<=N_units; i++){
    dorsalInput(i)  = NMJ_DA*n.NeuronOutput(nn(DA,i)) + NMJ_DB*n.NeuronOutput(nn(DB,i)) + NMJ_DD*n.NeuronOutput(nn(DD,i));
    ventralInput(i) = NMJ_VD*n.NeuronOutput(nn(VD,i)) + NMJ_VA*n.NeuronOutput(nn(VA,i)) + NMJ_VB*n.NeuronOutput(nn(VB,i));
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

template<class T>
double Worm<T>::CoMx()
{
  double temp = 0.0;
  for (int i = 1; i <= N_rods; i++) {
    temp += b.X(i);
  }
  return temp/N_rods;
}

template<class T>
double Worm<T>::CoMy()
{
  double temp = 0.0;
  for (int i = 1; i <= N_rods; i++) {
    temp += b.Y(i);
  }
  return temp/N_rods;
}

template<class T>
void Worm<T>::Curvature(TVector<double> &c)
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

template<class T>
void Worm<T>::AngleCurvature(TVector<double> &c)
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

template<class T>
double Worm<T>::Orientation()
{
  return atan2(b.Y(Head)-b.Y(Tail),b.X(Head)-b.X(Tail));
}

// Dump the state to OFS if SKIPS steps have been performed
template<class T>
void Worm<T>::DumpBodyState(ofstream &ofs, int skips)
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
template<class T>
void Worm<T>::DumpActState(ofstream &ofs, int skips)
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
        ofs <<  " " << n.NeuronOutput(nn(j,i));
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

template<class T>
void Worm<T>::DumpCurvature(ofstream &ofs, int skips)
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

template<class T>
void Worm<T>::DumpVoltage(ofstream &ofs, int skips)
{
  static int tt = skips;

  if (++tt >= skips) {
    tt = 0;

    ofs << t;
    // Ventral Cord Motor Neurons
    for (int i = 1; i <= N_units; i++) {
      for (int j = 1; j <= N_neuronsperunit; j++) {
        ofs <<  " " << n.NeuronState(nn(j,i));
      }
    }
    ofs << "\n";
  }
}


template<class T>
void Worm<T>::DumpParams(ofstream &ofs) {
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

  ofs << "Gap Juncs: \n DD-DD+1: " << n.ElectricalSynapseWeight(DD, DD+N_neuronsperunit) <<
  "\n VB-VB+1: " << n.ElectricalSynapseWeight(VB, VB+N_neuronsperunit) <<
  "\n VD-VD+1: " << n.ElectricalSynapseWeight(VD, VD+N_neuronsperunit) << endl;

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
