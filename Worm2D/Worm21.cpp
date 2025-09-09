//
//  Worm21.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "Worm21.h"
//#include "../argUtils.h"


//extern SuppliedArgs2021 supArgs1;

void Worm21::setPhenoNames()
{
 
    Worm2D21::setPhenoNames();

	for (int i = 1; i <= 7; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("bias");
	}
        
    for (int i = 8; i <= 14; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("tau");
	}

    for (int i = 15; i <= 30; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("chemsyn");
	}
       
    phenoNamesNums.push_back(31);
	phenoNames.push_back("electsyn");
    
    for (int i = 40; i <= 41; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("chemsyn");
	}
    
    for (int i = 42; i <= 44; i++){	
		phenoNamesNums.push_back(i);
		phenoNames.push_back("electsyn");
	}

}

Worm21::Worm21():Worm2Dm({7,24,0.1,7,49}, new NervousSystem(), new Muscles),Worm2D21(),
n(dynamic_cast<NervousSystem&>(*n_ptr)){}

Worm21::Worm21(TVector<double> &pheno):Worm21(pheno, true){}


Worm21::Worm21(const string & filename_):Worm21()
{
    setParsFromFile(filename_);
}

// The constructor
Worm21::Worm21(TVector<double> &phengen, bool isPheno):Worm21()
{

    if (isPheno) setParsFromPheno(phengen);
    else setParsFromGeno(phengen);

}

void Worm21::setEvolPars(W2Dparameters & w2par_, string evotype_)
//void Worm21::setEvolPars(shared_ptr<W2Dparameters> w2par_ptr_, string evotype_)
{
    if (evotype_=="Evo21"){
    Evolparameters & Epars1 = dynamic_cast<Evolparameters&>(w2par_);

    Epars1.dbunit = 10;
    Epars1.vbunit = 13;
    }

}
void Worm21::setParsFromPheno(TVector<double> &pheno)
{
   
// Muscles
   // m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
    
    // Nervous system // Ventral cord
    n.SetCircuitSize(par1.N_units*par1.N_neuronsperunit, 9, 6);
    
    int as, da, db, dd, vd, vb, va;
    int asNext, dbNext, ddNext, vdNext, vbNext, vaNext ;
    
    for (int u = 1; u <= par1.N_units; u++){
        as = nn(AS, u);
        da = nn(DA, u);
        db = nn(DB, u);
        dd = nn(DD, u);
        vd = nn(VD, u);
        vb = nn(VB, u);
        va = nn(VA, u);

        asNext = nn(AS, u+1);
        dbNext = nn(DB, u+1);
        ddNext = nn(DD, u+1);
        vdNext = nn(VD, u+1);
        vbNext = nn(VB, u+1);
        vaNext = nn(VA, u+1);
        
        // Bias, Time Constant and Self Connections
        n.SetNeuronBias(as, pheno(1));
        n.SetNeuronBias(da, pheno(2));
        n.SetNeuronBias(db, pheno(3));
        n.SetNeuronBias(dd, pheno(4));
        n.SetNeuronBias(vd, pheno(5));
        n.SetNeuronBias(vb, pheno(6));
        n.SetNeuronBias(va, pheno(7));

        n.SetNeuronTimeConstant(as, pheno(8));
        n.SetNeuronTimeConstant(da, pheno(9));
        n.SetNeuronTimeConstant(db, pheno(10));
        n.SetNeuronTimeConstant(dd, pheno(11));
        n.SetNeuronTimeConstant(vd, pheno(12));
        n.SetNeuronTimeConstant(vb, pheno(13));
        n.SetNeuronTimeConstant(va, pheno(14));
        
        n.SetChemicalSynapseWeight(as, as, pheno(15));
        n.SetChemicalSynapseWeight(da, da, pheno(16));
        n.SetChemicalSynapseWeight(db, db, pheno(17));
        n.SetChemicalSynapseWeight(dd, dd, pheno(18));
        n.SetChemicalSynapseWeight(vd, vd, pheno(19));
        n.SetChemicalSynapseWeight(vb, vb, pheno(20));
        n.SetChemicalSynapseWeight(va, va, pheno(21));
        
        // --------
        // Chemical Synapses minimal network
        n.SetChemicalSynapseWeight(as, da, pheno(22));
        n.SetChemicalSynapseWeight(as, vd, pheno(23));
        n.SetChemicalSynapseWeight(da, db, pheno(24));
        n.SetChemicalSynapseWeight(db, as, pheno(25));
        n.SetChemicalSynapseWeight(vd, va, pheno(26));
        n.SetChemicalSynapseWeight(vd, vb, pheno(27));

        n.SetChemicalSynapseWeight(da, dd, pheno(28));
        n.SetChemicalSynapseWeight(vb, dd, pheno(29));
        n.SetChemicalSynapseWeight(va, dd, pheno(30));

        // Electrical Synapse minimal network
        n.SetElectricalSynapseWeight(vd, dd, pheno(31));

//        // Intersegment connections
//        // Chemicals
        if (u < par1.N_units){
            n.SetChemicalSynapseWeight(db, ddNext, pheno(40));
            n.SetChemicalSynapseWeight(vaNext, dd, pheno(41));
        }
//        // Electricals
        if (u < par1.N_units){
//        // Interclasses
            n.SetElectricalSynapseWeight(as, vaNext, pheno(42));
            n.SetElectricalSynapseWeight(da, asNext, pheno(43));
            n.SetElectricalSynapseWeight(vb, dbNext, pheno(44));
        // Intraclasses
//            n.SetElectricalSynapseWeight(db, dbNext, pheno(32));
//            n.SetElectricalSynapseWeight(vb, vbNext, pheno(32));
//            n.SetElectricalSynapseWeight(vd, vdNext, pheno(32));
//            n.SetElectricalSynapseWeight(dd, ddNext, pheno(32));
        }
    }
   
    NMJ_AS = pheno(32);
   NMJ_DA = pheno(33);
   NMJ_DB = pheno(34);
   NMJ_DD = pheno(35);
   NMJ_VD = pheno(36);
   NMJ_VB = pheno(37);
   NMJ_VA = pheno(38);
   
   // NMJ Gain XXX
   NMJ_Gain_Map = pheno(39);
   NMJ_Gain.SetBounds(1, par1.N_muscles);
   for (int i=1; i<=par1.N_muscles; i++)
   {
       NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
   }

   setUpMuscleConn();



}


void Worm21::InitializeState(RandomState &rs)
{    
    Worm2D21::InitializeState(rs);
    //n.RandomizeCircuitOutput(0.2, 0.8, rs); //fix this error?? adam (should be -0.5?)
    n.RandomizeCircuitOutput(0.5, 0.5, rs); //fix this error?? adam (should be -0.5?)
    //n.RandomizeCircuitOutput(0, 0, rs); //fix this error?? adam (should be -0.5?)
    //cout << "Worm21 init state" << endl;
    return;
}

void Worm21::DumpParams(ofstream &ofs)
{   Worm2D21::DumpParams(ofs);
    ofs << "Biases: \n DB: " << n.NeuronBias(DB) << "\n VB/P: " << n.NeuronBias(VB) << " / " << n.NeuronBias(VB)  << "\n VDA/P: " << n.NeuronBias(VD) <<  " / " << n.NeuronBias(VD) << endl;
}


void Worm21::addParsToJson(json & j)
{
        string nsHead = "Nervous system";
        appendAllNSJson(j[nsHead], n);
        Worm2D21::addParsToJson(j);    
    }



void Worm21::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{

   
    const double	BiasRange				= 15.0;
     const double    SCRange                 = 15.0;
     const double    CSRange                 = 15.0;
     const double    TauMin                 = 0.1;
     const double    TauMax                 = 2.5;
     const double    ESRange                 = 2.0;
     const double    NMJmax                  = 1.2;
     //const double    IIRange                 = 15.0;    


  // Bias
  for (int i = 1; i <= 7; i++){
    phen(i) = MapSearchParameter(gen(i), -BiasRange, BiasRange);
}
// Time Constant
for (int i = 8; i <= 14; i++){
    phen(i) = MapSearchParameter(gen(i), TauMin, TauMax);
}
// Self connections
for (int i = 15; i <= 21; i++){
    phen(i) = MapSearchParameter(gen(i), -SCRange, SCRange);
}
// Chemical synapses
for (int i = 22; i <=30; i++){
    phen(i) = MapSearchParameter(gen(i), -CSRange, CSRange);
}

// Gap junctions
phen(31) = MapSearchParameter(gen(31), 0.0, ESRange);


// NMJ Weight
phen(32) = MapSearchParameter(gen(32), 0.0, NMJmax);       // AS
phen(33) = MapSearchParameter(gen(33), 0.0, NMJmax);       // DA
phen(34) = MapSearchParameter(gen(34), NMJmax, NMJmax);       // DB
phen(35) = MapSearchParameter(gen(35), -NMJmax, 0.0);      // DD
phen(36) = MapSearchParameter(gen(36), -NMJmax, 0.0);      // VD
phen(37) = MapSearchParameter(gen(37), NMJmax, NMJmax);      // VB
phen(38) = MapSearchParameter(gen(38), 0.0, NMJmax);      // VA

phen(39) = MapSearchParameter(gen(39), 0.2, 1.0);       // Used to be 0.4/0.6 XXX NMJ_Gain Mapping

// Intersegment synapse tested
phen(40) = MapSearchParameter(gen(40), -CSRange, CSRange);  // DB to DDnext
phen(41) = MapSearchParameter(gen(41), -CSRange, CSRange);  // VAnext to DD
phen(42) = MapSearchParameter(gen(42), 0.0, ESRange);       // AS -- VAnext
phen(43) = MapSearchParameter(gen(43), 0.0, ESRange);       // DA -- ASnext
phen(44) = MapSearchParameter(gen(44), 0.0, ESRange);       // VB -- DBnext


}    