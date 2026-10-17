//
//  Segment.cpp
//

#include "Segment21.h"


// The constructor
Segment::Segment(TVector<double> &v)
{
    // Nervous system // Ventral cord
    n.SetCircuitSize(N_neurons, 8, 3);
    
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
}



void Segment::InitializeState(RandomState &rs)
{
    t = 0.0;
    n.RandomizeCircuitOutput(0.5, 0.5, rs);
}


void Segment::InitializeOutputs(double o)
{
    t = 0.0;
    for (int j = 1; j <= N_neurons; j++) {
        n.SetNeuronOutput(j, o);
    }
}

void Segment::Step(double StepSize)
{
    // Update Nervous System
    n.EulerStep(StepSize);
    t += StepSize;
}

void Segment::DumpActState(ofstream &ofs, int skips)
{
    static int tt = skips;
    if (++tt >= skips) {
        tt = 0;
        //time
        ofs << t;
    // Ventral Cord Motor Neurons
        for (int j = 1; j <= N_neurons; j++) {
            ofs <<  " " << n.NeuronOutput(j);
        }
        ofs << "\n";
    }
}

