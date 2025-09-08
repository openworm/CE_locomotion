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


WormCE::WormCE():Worm2Dm({6,24,0.1,10,60},new NervousSystem(), new Muscles),
n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2DCE({6,24,0.1,10,60},0),
EvolvableS(shared_ptr<EvolparametersCE>(new EvolparametersCE())){}

void WormCE::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{

  if (evotype_=="EvoCE"){
  EvolparametersCE & Epars1 = dynamic_cast<EvolparametersCE&>(*(w2par_ptr_));
  AVA_output = Epars1.AVA_output;
  AVB_output = Epars1.AVB_output;
  sr_type = Epars1.sr_type;
  return;
  }
  cout << "evotype is " << evotype_ << endl;
  assert(0 && "Evotype not implemented");

}


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

  AVA_output = 0.0;
  AVB_output = 0.0;

  pheno_A_gain = sr.SR_A_gain;
  pheno_B_gain = sr.SR_B_gain;
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

void WormCE::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{

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
  phen(SR_A) = MapSearchParameter(gen(SR_A), 0.0, SRmax);
  phen(SR_B) = MapSearchParameter(gen(SR_B), 0.0, SRmax);

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