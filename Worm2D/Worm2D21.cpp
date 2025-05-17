//
//  Worm21.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "Worm2D21.h"
//#include "../argUtils.h"
#include "../neuromlLocal/c302ForW2D.h"

//extern SuppliedArgs2021 supArgs1;


Worm2D21::Worm2D21(TVector<double> &v):Worm2D({7,24,0.1,7,49}
   , new c302ForW2D("AS DA DB DD VD VB VA",7))
{
   // Interneuron inputs (AVB)
   wAVB_DB = 1;
   wAVB_VB = 1;
   // Interneuron inputs (AVB)
   wAVA_DA = 1;
   wAVA_VA = 1;
   
   AVA = 0; 
   AVB = 0; 

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
   NMJ_Gain.SetBounds(1, par1.N_muscles);
   for (int i=1; i<=par1.N_muscles; i++)
   {
       NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
   }
}


Worm2D21::Worm2D21(json & j):Worm2D(
    {j["Worm"]["N_neuronsperunit"]["value"], 
      j["Worm"]["N_muscles"]["value"], 
      j["Worm"]["T_muscle"]["value"],
      j["Worm"]["N_units"]["value"],
      j["Nervous system"]["size"]["value"]
    }, new c302ForW2D("AS DA DB DD VD VB VA",7))
{

// Interneuron inputs (AVB)
wAVB_DB = 1;
wAVB_VB = 1;
// Interneuron inputs (AVB)
wAVA_DA = 1;
wAVA_VA = 1;

//initialize these to zero, adam
AVA = 0; 
AVB = 0; 

// NMJ Weight
NMJ_AS = j["Worm"]["NMJ_AS"]["value"];
NMJ_DA = j["Worm"]["NMJ_DA"]["value"];
NMJ_DB = j["Worm"]["NMJ_DB"]["value"];
NMJ_DD = j["Worm"]["NMJ_DD"]["value"];
NMJ_VD = j["Worm"]["NMJ_VD"]["value"];
NMJ_VB = j["Worm"]["NMJ_VB"]["value"];
NMJ_VA = j["Worm"]["NMJ_VA"]["value"];

// NMJ Gain XXX
NMJ_Gain_Map = j["Worm"]["NMJ_Gain_Map"]["value"];
NMJ_Gain.SetBounds(1, par1.N_muscles);
for (int i=1; i<=par1.N_muscles; i++)
{
    NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
}

}

void Worm2D21::initForSimulation()
{

    SetAVB(0.0);
    SetAVA(0.0);
}

void Worm2D21::InitializeState(RandomState &rs)
{    
    cout << "Worm2D21 init state" << endl;
    Worm2D::InitializeState(rs);
    return;    
}

void Worm2D21::setMuscleInput()
{
// Set input to Muscles
    // Head: 4 muscles one neural unit  //////////////////////

    int mi;
    double dorsalInput, ventralInput;

    mi = 1;
    dorsalInput  = NMJ_AS*n_ptr->NeuronOutput(nn(AS,mi)) + NMJ_DA*n_ptr->NeuronOutput(nn(DA,mi)) + NMJ_DB*n_ptr->NeuronOutput(nn(DB,mi)) + NMJ_DD*n_ptr->NeuronOutput(nn(DD,mi));
    ventralInput = NMJ_VD*n_ptr->NeuronOutput(nn(VD,mi)) + NMJ_VA*n_ptr->NeuronOutput(nn(VA,mi)) + NMJ_VB*n_ptr->NeuronOutput(nn(VB,mi));
    for (int i = 1; i < 5; i++){
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralInput);
                m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalInput);
    }
    
    //  Body Anterior: 4 segments, 3 muscles each  //////////////////////
    for (int mi = 2; mi <= 5; mi++){
        dorsalInput  = NMJ_AS*n_ptr->NeuronOutput(nn(AS,mi)) + NMJ_DA*n_ptr->NeuronOutput(nn(DA,mi)) + NMJ_DB*n_ptr->NeuronOutput(nn(DB,mi)) + NMJ_DD*n_ptr->NeuronOutput(nn(DD,mi));
        ventralInput = NMJ_VD*n_ptr->NeuronOutput(nn(VD,mi)) + NMJ_VA*n_ptr->NeuronOutput(nn(VA,mi)) + NMJ_VB*n_ptr->NeuronOutput(nn(VB,mi));
        for (int i = 5 + 3*(mi-2); i < 5 + 3*(mi-1); i++){
            m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralInput);
            m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalInput);
        }
    }
    
    //  Posterior Body: 2 segments, 4 muscles each  //////////////////////
    for (int mi = 6; mi <= 7; mi++){
        dorsalInput  = NMJ_AS*n_ptr->NeuronOutput(nn(AS,mi)) + NMJ_DA*n_ptr->NeuronOutput(nn(DA,mi)) + NMJ_DB*n_ptr->NeuronOutput(nn(DB,mi)) + NMJ_DD*n_ptr->NeuronOutput(nn(DD,mi));
        ventralInput = NMJ_VD*n_ptr->NeuronOutput(nn(VD,mi)) + NMJ_VA*n_ptr->NeuronOutput(nn(VA,mi)) + NMJ_VB*n_ptr->NeuronOutput(nn(VB,mi));
        for (int i = 17 + 4*(mi-6); i < 17 + 4*(mi-5); i++){
            m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralInput);
            m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalInput);
        }
    }
    
}

void Worm2D21::Step(double StepSize)
{
    
    
    // Update Body
    b.StepBody(StepSize);
    
    
    // Update Nervous System
    n_ptr->EulerStep(StepSize);
    
    // Interneuron input  //////////////////////
    for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB, i), wAVB_DB * AVB);
        n_ptr->SetNeuronExternalInput(nn(VB, i), wAVB_VB * AVB);
        n_ptr->SetNeuronExternalInput(nn(DA, i), wAVA_DA * AVA);
        n_ptr->SetNeuronExternalInput(nn(VA, i), wAVA_VA * AVA);
    }
    
    setMuscleInput();


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
        int mi = (int) ((i-1)/2);
        b.SetDorsalSegmentActivation(i, (m.DorsalMuscleOutput(mi) + m.DorsalMuscleOutput(mi+1))/2);
        b.SetVentralSegmentActivation(i, (m.VentralMuscleOutput(mi) + m.VentralMuscleOutput(mi+1))/2);
    }
    
    //  Last two segments receive special treatment because they are only affected by a single muscle
    b.SetDorsalSegmentActivation(N_segments-1, m.DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments-1, m.VentralMuscleOutput(par1.N_muscles)/2);
    b.SetDorsalSegmentActivation(N_segments, m.DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments, m.VentralMuscleOutput(par1.N_muscles)/2);
    
    // Time
    t += StepSize;
}




vector<doubIntParamsHead> Worm2D21::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    
    var1.parDoub.head = "Worm";
    var1.parDoub.names = {"NMJ_Gain_Map", "NMJ_AS", "NMJ_DA", "NMJ_DB", "NMJ_VD", "NMJ_VB", "NMJ_VA", "NMJ_DD"};
    var1.parDoub.vals = {NMJ_Gain_Map, NMJ_AS, NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD};

    append<string>(var1.parDoub.names,{"wAVA_DA", "wAVA_VA", "wAVB_DB", "wAVB_VB", "AVA", "AVB"});
    append<double>(var1.parDoub.vals, {wAVA_DA, wAVA_VA, wAVB_DB, wAVB_VB, AVA, AVB});
   
  
    var1.parInt.head = "Worm";
    var1.parInt.vals = {startingMuscleA,NmusclePerNUA, startingMuscleB, NmusclePerNUB};
    var1.parInt.names = {"startingMuscleA","NmusclePerNUA", "startingMuscleB","NmusclePerNUB"};
    
  
    parvec.push_back(var1);
    return parvec;
  
}




void Worm2D21::DumpActState(ofstream &ofs, int skips)
{
    static int tt = skips;
    
    if (++tt >= skips) {
        tt = 0;
        //time
        ofs << t;

        // Ventral Cord Motor Neurons
        //ofs << "\nV: ";
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n_ptr->NeuronOutput(nn(j,i));
            }
        }
        // Muscles
        //ofs << "\nM: ";
        for (int i = 1; i <= par1.N_muscles; i++) {
            ofs <<  " " << m.DorsalMuscleOutput(i) << " " << m.VentralMuscleOutput(i);
        }
        ofs << "\n";
    }
}


void Worm2D21::DumpActStateState(ofstream &ofs, int skips)
{
    static int tt = skips;
    
    if (++tt >= skips) {
        tt = 0;
        //time
        ofs << t;

        // Ventral Cord Motor Neurons
        //ofs << "\nV: ";
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n_ptr->NeuronState(nn(j,i));
            }
        }
        ofs << "\n";
    }
}


void Worm2D21::DumpCurvature(ofstream &ofs, int skips)
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


void Worm2D21::DumpParams(ofstream &ofs){ofs << "Worm2D21 parameters" << endl;}
