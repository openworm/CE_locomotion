//
//  WormCE.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "WormCE.h"
//#include "../argUtils.h"

//extern SuppliedArgs supArgs1;

/* 

NervousSystemBase* makeNervousSystem()
{  
if (supArgs1.doOrigNS) return new NervousSystem();
return new c302NervousSystem();
}
 */


WormCE::WormCE(TVector<double> &v,double output):Worm2Dm({6,24,0.1,10,60},
new NervousSystem(), new Muscles),n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2DCE({6,24,0.1,10,60},0)
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
  sr.SetStretchReceptorParams(N_segments, N_stretchrec, v(1), v(2));

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
    n.SetNeuronBias(da, v(3));
    n.SetNeuronBias(va, v(3));
    n.SetNeuronBias(db, v(4));
    n.SetNeuronBias(vb, v(4));
    n.SetNeuronBias(dd, v(5));
    n.SetNeuronBias(vd, v(5));

    // Time-constants fixed to 1.0
    for (int i = 1; i <= par1.N_neuronsperunit; i++){
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
    if (u < par1.N_units){
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

void WormCE::InitializeState(RandomState &rs)
{
  
  Worm2DCE::InitializeState(rs);
  //n.RandomizeCircuitState(-1.0, 1.0, rs);
  
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
