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



// The constructor
Worm21::Worm21(TVector<double> &v)
:Worm2Dm({7,24,0.1,7,49}, new NervousSystem(), new Muscles), Worm2D21(v),
n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2Dbase({7,24,0.1,7,49}, new NervousSystem(), new Muscles)
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
        n.SetNeuronBias(as, v(1));
        n.SetNeuronBias(da, v(2));
        n.SetNeuronBias(db, v(3));
        n.SetNeuronBias(dd, v(4));
        n.SetNeuronBias(vd, v(5));
        n.SetNeuronBias(vb, v(6));
        n.SetNeuronBias(va, v(7));

        n.SetNeuronTimeConstant(as, v(8));
        n.SetNeuronTimeConstant(da, v(9));
        n.SetNeuronTimeConstant(db, v(10));
        n.SetNeuronTimeConstant(dd, v(11));
        n.SetNeuronTimeConstant(vd, v(12));
        n.SetNeuronTimeConstant(vb, v(13));
        n.SetNeuronTimeConstant(va, v(14));
        
        n.SetChemicalSynapseWeight(as, as, v(15));
        n.SetChemicalSynapseWeight(da, da, v(16));
        n.SetChemicalSynapseWeight(db, db, v(17));
        n.SetChemicalSynapseWeight(dd, dd, v(18));
        n.SetChemicalSynapseWeight(vd, vd, v(19));
        n.SetChemicalSynapseWeight(vb, vb, v(20));
        n.SetChemicalSynapseWeight(va, va, v(21));
        
        // --------
        // Chemical Synapses minimal network
        n.SetChemicalSynapseWeight(as, da, v(22));
        n.SetChemicalSynapseWeight(as, vd, v(23));
        n.SetChemicalSynapseWeight(da, db, v(24));
        n.SetChemicalSynapseWeight(db, as, v(25));
        n.SetChemicalSynapseWeight(vd, va, v(26));
        n.SetChemicalSynapseWeight(vd, vb, v(27));

        n.SetChemicalSynapseWeight(da, dd, v(28));
        n.SetChemicalSynapseWeight(vb, dd, v(29));
        n.SetChemicalSynapseWeight(va, dd, v(30));

        // Electrical Synapse minimal network
        n.SetElectricalSynapseWeight(vd, dd, v(31));

//        // Intersegment connections
//        // Chemicals
        if (u < par1.N_units){
            n.SetChemicalSynapseWeight(db, ddNext, v(40));
            n.SetChemicalSynapseWeight(vaNext, dd, v(41));
        }
//        // Electricals
        if (u < par1.N_units){
//        // Interclasses
            n.SetElectricalSynapseWeight(as, vaNext, v(42));
            n.SetElectricalSynapseWeight(da, asNext, v(43));
            n.SetElectricalSynapseWeight(vb, dbNext, v(44));
        // Intraclasses
//            n.SetElectricalSynapseWeight(db, dbNext, v(32));
//            n.SetElectricalSynapseWeight(vb, vbNext, v(32));
//            n.SetElectricalSynapseWeight(vd, vdNext, v(32));
//            n.SetElectricalSynapseWeight(dd, ddNext, v(32));
        }
    }
   
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


void Worm21::addParsToJson(json & j){
        string nsHead = "Nervous system";
        appendAllNSJson(j[nsHead], n);
        Worm2D21::addParsToJson(j);}