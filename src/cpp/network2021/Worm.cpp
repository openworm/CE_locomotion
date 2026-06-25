//
//  Worm.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "Worm.h"
#include "../argUtils.h"

int nn(int neuronNumber, int unitNumber)
{
    return neuronNumber+((unitNumber-1)*N_neuronsperunit);
}

extern SuppliedArgs2021 supArgs1;

// The constructor
Worm::Worm(TVector<double> &v)
{
    // Muscles
    m.SetMuscleParams(N_muscles, T_muscle);
    
    // Nervous system // Ventral cord
    n.SetCircuitSize(N_units*N_neuronsperunit, 9, 6);
    
    int as, da, db, dd, vd, vb, va;
    int asNext, dbNext, ddNext, vdNext, vbNext, vaNext ;
    
    for (int u = 1; u <= N_units; u++){
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
        if (u < N_units){
            n.SetChemicalSynapseWeight(db, ddNext, v(40));
            n.SetChemicalSynapseWeight(vaNext, dd, v(41));
        }
//        // Electricals
        if (u < N_units){
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
    
    // Interneuron inputs (AVB)
    wAVB_DB = 1;
    wAVB_VB = 1;
    // Interneuron inputs (AVB)
    wAVA_DA = 1;
    wAVA_VA = 1;
    
    
    // NMJ Weight
    NMJ_AS = v(32);
    NMJ_DA = v(33);
    NMJ_DB = v(34);
    NMJ_DD = v(35);
    NMJ_VD = v(36);
    NMJ_VB = v(37);
    NMJ_VA = v(38);
    
    // NMJ Gain XXX
    NMJ_Gain_Map = v(39);
    NMJ_Gain.SetBounds(1, N_muscles);
    for (int i=1; i<=N_muscles; i++)
    {
        NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/N_muscles));
    }
}

void Worm::InitializeState(RandomState &rs)
{
    t = 0.0;
    n.RandomizeCircuitOutput(0.5, 0.5, rs);
    b.InitializeBodyState();
    m.InitializeMuscleState();
}

void Worm::Step(double StepSize)
{
    int mi;
    double dorsalInput, ventralInput;
    
    // Update Body
    b.StepBody(StepSize);
    
    
    // Update Nervous System
    n.EulerStep(StepSize);
    
    // Interneuron input  //////////////////////
    for (int i = 1; i <= N_units; i++){
        n.SetNeuronExternalInput(nn(DB, i), wAVB_DB * AVB);
        n.SetNeuronExternalInput(nn(VB, i), wAVB_VB * AVB);
        n.SetNeuronExternalInput(nn(DA, i), wAVA_DA * AVA);
        n.SetNeuronExternalInput(nn(VA, i), wAVA_VA * AVA);
    }
    
    // Set input to Muscles
    // Head: 4 muscles one neural unit  //////////////////////
    mi = 1;
    dorsalInput  = NMJ_AS*n.NeuronOutput(nn(AS,mi)) + NMJ_DA*n.NeuronOutput(nn(DA,mi)) + NMJ_DB*n.NeuronOutput(nn(DB,mi)) + NMJ_DD*n.NeuronOutput(nn(DD,mi));
    ventralInput = NMJ_VD*n.NeuronOutput(nn(VD,mi)) + NMJ_VA*n.NeuronOutput(nn(VA,mi)) + NMJ_VB*n.NeuronOutput(nn(VB,mi));
    for (int i = 1; i < 5; i++){
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralInput);
                m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalInput);
    }
    
    //  Body Anterior: 4 segments, 3 muscles each  //////////////////////
    for (int mi = 2; mi <= 5; mi++){
        dorsalInput  = NMJ_AS*n.NeuronOutput(nn(AS,mi)) + NMJ_DA*n.NeuronOutput(nn(DA,mi)) + NMJ_DB*n.NeuronOutput(nn(DB,mi)) + NMJ_DD*n.NeuronOutput(nn(DD,mi));
        ventralInput = NMJ_VD*n.NeuronOutput(nn(VD,mi)) + NMJ_VA*n.NeuronOutput(nn(VA,mi)) + NMJ_VB*n.NeuronOutput(nn(VB,mi));
        for (int i = 5 + 3*(mi-2); i < 5 + 3*(mi-1); i++){
            m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralInput);
            m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalInput);
        }
    }
    
    //  Posterior Body: 2 segments, 4 muscles each  //////////////////////
    for (int mi = 6; mi <= 7; mi++){
        dorsalInput  = NMJ_AS*n.NeuronOutput(nn(AS,mi)) + NMJ_DA*n.NeuronOutput(nn(DA,mi)) + NMJ_DB*n.NeuronOutput(nn(DB,mi)) + NMJ_DD*n.NeuronOutput(nn(DD,mi));
        ventralInput = NMJ_VD*n.NeuronOutput(nn(VD,mi)) + NMJ_VA*n.NeuronOutput(nn(VA,mi)) + NMJ_VB*n.NeuronOutput(nn(VB,mi));
        for (int i = 17 + 4*(mi-6); i < 17 + 4*(mi-5); i++){
            m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralInput);
            m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalInput);
        }
    }
    
    // Update Muscle activation
    m.EulerStep(StepSize);
    
    // Set input to Body
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

double Worm::CoMx()
{
    double temp = 0.0;
    for (int i = 1; i <= N_rods; i++) {
        temp += b.X(i);
    }
    return temp/N_rods;
}

double Worm::CoMy()
{
    double temp = 0.0;
    for (int i = 1; i <= N_rods; i++) {
        temp += b.Y(i);
    }
    return temp/N_rods;
}

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

double Worm::Orientation()
{
    //cout << "Worm:\tOrientation" << endl;
    return atan2(b.Y(Head)-b.Y(Tail),b.X(Head)-b.X(Tail));
}

// Dump the state to OFS if SKIPS steps have been performed

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

void Worm::DumpActState(ofstream &ofs, int skips)
{
    static int tt = skips;
    
    if (++tt >= skips) {
        tt = 0;
        //time
        ofs << t;

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


void Worm::DumpParams(ofstream &ofs)
{ofs << "Biases: \n DB: " << n.NeuronBias(DB) << "\n VB/P: " << n.NeuronBias(VB) << " / " << n.NeuronBias(VB)  << "\n VDA/P: " << n.NeuronBias(VD) <<  " / " << n.NeuronBias(VD) << endl;
}
