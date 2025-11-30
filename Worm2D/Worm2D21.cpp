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

void Worm2D21m::setPhenoNames()
{

    for (int i=32; i<=38; i++) addPhenoName("NMJ", i);
    addPhenoName("NMJ_Gain_Map", 39);
}


Worm2D21m::Worm2D21m():Worm2Dm({7,24,0.1,7,49}, new c302ForW2D(), 0 , true, make_shared<W2DCEparsA>()),
W2DCEpars1(dynamic_pointer_cast<W2DCEparsA>(W2Dbaseparameters1))
{

    cout << "1 Worm2D21m const "<< muscForWDconst << endl;    
    if (muscForWDconst) {m_ptr = new c302muscForW2D(dynamic_cast<c302ForW2D&>(*n_ptr));
    cout << "muscForWDconst set"<< muscForWDconst << endl;
    }

// Interneuron inputs (AVB)
wAVB_DB = 1;
wAVB_VB = 1;
// Interneuron inputs (AVB)
wAVA_DA = 1;
wAVA_VA = 1;

//initialize these to zero, adam
W2DCEpars1->AVA_output = 0; 
W2DCEpars1->AVB_output = 0; 

}


Worm2D21::Worm2D21(TVector<double> & pheno):
Worm2Dm({7,24,0.1,7,49},new c302ForW2D(),make_shared<W2DCEparsA>()),
Worm2D({7,24,0.1,7,49},0),Worm2D21m()
{
  
   // NMJ Weight
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



Worm2D21::Worm2D21():
Worm2Dm({7,24,0.1,7,49},new c302ForW2D(),make_shared<W2DCEparsA>()),
Worm2D({7,24,0.1,7,49},0),Worm2D21m(){}


Worm2D21::Worm2D21(json & j):
Worm2Dm({7,24,0.1,7,49},new c302ForW2D(),make_shared<W2DCEparsA>()),
Worm2D({7,24,0.1,7,49},0),Worm2D21m()
{

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
    setUpMuscleConn();
}

void Worm2D21m::initForSimulation(RandomState &rs)
{
  
    SetAVB(0.0);
    SetAVA(0.0);
}

void Worm2D21m::InitializeState(RandomState &rs)
{    
    cout << "Worm2D21m init state" << endl;
    Worm2Dm::InitializeState(rs);
    for (int i = 1; i <= par1.N_size; i++)
    n_ptr->SetNeuronExternalInput(i,0); //adam set all initial inputs to zero
    return;    
}

void Worm2D21::InitializeState(RandomState &rs)
{    
    cout << "Worm2D21 init state" << endl;
    Worm2D21m::InitializeState(rs);
    Worm2D::InitializeState(rs);
    return;    
}

void Worm2D21m::setForward()
{
  W2DCEpars1->AVA_output =  0;
  W2DCEpars1->AVB_output =  W2DCEpars1->AB_output_level;
}

void Worm2D21m::setBackward()
{
  W2DCEpars1->AVA_output =  W2DCEpars1->AB_output_level;
  W2DCEpars1->AVB_output =  0;
}

vector<toFromWeight> Worm2D21::makeDorsalMuscleConn()
{
vector<int> dorsalNeurons({AS,DA,DB,DD});
vector<double> dorsalNMJ({NMJ_AS,NMJ_DA,NMJ_DB,NMJ_DD});
return makeMuscleConn(dorsalNeurons, dorsalNMJ);
}

vector<toFromWeight> Worm2D21::makeVentralMuscleConn()
{
vector<int> ventralNeurons({VD,VA,VB});
vector<double> ventralNMJ({NMJ_VD,NMJ_VA,NMJ_VB});

return makeMuscleConn(ventralNeurons, ventralNMJ);

}


vector<toFromWeight> Worm2D21::makeMuscleConn(vector<int> neurons, vector<double> NMJ)
{
    vector<toFromWeight> vec1;
    int unit = 1;
    for (int to_musc = 1; to_musc < 5; to_musc++) 
    makeMuscleConnHelp(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain);
        
    for (int unit = 2; unit <= 5; unit++)
        for (int to_musc = 5 + 3*(unit-2); to_musc < 5 + 3*(unit-1); to_musc++)
            makeMuscleConnHelp(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain);

    for (int unit = 6; unit <= 7; unit++)
        for (int to_musc = 17 + 4*(unit-6); to_musc < 17 + 4*(unit-5); to_musc++)
            makeMuscleConnHelp(vec1, neurons, NMJ, unit, to_musc, NMJ_Gain);
    
    cout << "made muscle con" << endl;
            //exit(1);
    return vec1;

}

void Worm2D21::setMuscleInputOrig()
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
    m.EulerStep(settedStepSize);
}

void Worm2D21m::Step1()
{
    
    
    // Update Body
    b.StepBody(settedStepSize);
    
    
    // Update Nervous System
    n_ptr->EulerStep(settedStepSize);
    
    // Interneuron input  //////////////////////
    for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB, i), wAVB_DB * W2DCEpars1->AVB_output);
        n_ptr->SetNeuronExternalInput(nn(VB, i), wAVB_VB * W2DCEpars1->AVB_output);
        n_ptr->SetNeuronExternalInput(nn(DA, i), wAVA_DA * W2DCEpars1->AVA_output);
        n_ptr->SetNeuronExternalInput(nn(VA, i), wAVA_VA * W2DCEpars1->AVA_output);
        //n_ptr->SetNeuronExternalInput(nn(DB, i), 1);
        //n_ptr->SetNeuronExternalInput(nn(VB, i), 1);
        //n_ptr->SetNeuronExternalInput(nn(DA, i), 1);
        //n_ptr->SetNeuronExternalInput(nn(VA, i), 1);
    }
    
    setMuscleInput();
    //setMuscleInputOrig(StepSize);   

    // Update Muscle activation
    //m.EulerStep(StepSize);
    
    setBodyInput();

    // Set input to Body
    //  First two segments receive special treatment because they are only affected by a single muscle

    if (false){
    b.SetDorsalSegmentActivation(1, m_ptr->DorsalMuscleOutput(1)/2);
    b.SetVentralSegmentActivation(1, m_ptr->VentralMuscleOutput(1)/2);
    b.SetDorsalSegmentActivation(2, m_ptr->DorsalMuscleOutput(1)/2);
    b.SetVentralSegmentActivation(2, m_ptr->VentralMuscleOutput(1)/2);
    
    //  All other segments receive force from two muscles
    for (int i = 3; i <= N_segments-2; i++)
    {
        int mi = (int) ((i-1)/2);
        b.SetDorsalSegmentActivation(i, (m_ptr->DorsalMuscleOutput(mi) + m_ptr->DorsalMuscleOutput(mi+1))/2);
        b.SetVentralSegmentActivation(i, (m_ptr->VentralMuscleOutput(mi) + m_ptr->VentralMuscleOutput(mi+1))/2);
    }
    
    //  Last two segments receive special treatment because they are only affected by a single muscle
    b.SetDorsalSegmentActivation(N_segments-1, m_ptr->DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments-1, m_ptr->VentralMuscleOutput(par1.N_muscles)/2);
    b.SetDorsalSegmentActivation(N_segments, m_ptr->DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments, m_ptr->VentralMuscleOutput(par1.N_muscles)/2);
    
}
    // Time
    //t += StepSize;
}

vector<doubIntParamsHead> Worm2D21m::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;

    var1.parDoub.head = "Worm";
    var1.parDoub.names = {"wAVA_DA", "wAVA_VA", "wAVB_DB", "wAVB_VB", "AVA", "AVB"};
    var1.parDoub.vals = {wAVA_DA, wAVA_VA, wAVB_DB, wAVB_VB, 
        W2DCEpars1->AVA_output, W2DCEpars1->AVB_output};

    parvec.push_back(var1);
    return parvec;

}


vector<doubIntParamsHead> Worm2D21::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    
    var1.parDoub.head = "Worm";
    var1.parDoub.names = {"NMJ_Gain_Map", "NMJ_AS", "NMJ_DA", "NMJ_DB", "NMJ_VD", "NMJ_VB", "NMJ_VA", "NMJ_DD"};
    var1.parDoub.vals = {NMJ_Gain_Map, NMJ_AS, NMJ_DA, NMJ_DB, NMJ_VD, NMJ_VB, NMJ_VA, NMJ_DD};


    append<string>(var1.parDoub.names,{"wAVA_DA", "wAVA_VA", "wAVB_DB", "wAVB_VB", "AVA", "AVB"});
    append<double>(var1.parDoub.vals, {wAVA_DA, wAVA_VA, wAVB_DB, wAVB_VB, W2DCEpars1->AVA_output, W2DCEpars1->AVB_output});
   
  
    var1.parInt.head = "Worm";
    var1.parInt.vals = {startingMuscleA,NmusclePerNUA, startingMuscleB, NmusclePerNUB};
    var1.parInt.names = {"startingMuscleA","NmusclePerNUA", "startingMuscleB","NmusclePerNUB"};
    
  
    parvec.push_back(var1);
    return parvec;
  
}



/* void Worm2D21::DumpCurvature(ofstream &ofs, int skips)
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
} */


void Worm2D21m::addParsToJson(json & j){
        Worm2Dm::addParsToJson(j);
       // W2DCEpars1->addParsToJson(j);
    }


void Worm2D21::addParsToJson(json & j){
        Worm2D21m::addParsToJson(j);
        Worm2D::addParsToJson(j); 
    }

//// 2Dm -> 2D  -> 2D21
///  2Dm -> 2D21m -> 2D21


void Worm2D21m::DumpParams(ofstream &ofs){ofs << "Worm2D21m parameters" << endl;}
