#include "Worm2Dmods.h"

Worm2Dosc::Worm2Dosc(const pfa & pfa_):Worm2D({pfa_.size,24,0.1,1,pfa_.size},0),
Worm2Dm({pfa_.size,24,0.1,1,pfa_.size}, new NSosc(pfa_), new Muscles),n(dynamic_cast<NSosc&>(*n_ptr))
{
    setUpMuscleConn();
    n.setTime(t);
}

Worm2Dosc::Worm2Dosc(TVector<double> &v):Worm2Dosc(getPfaFromPheno(v)){}

void Worm2Dosc::InitializeState(RandomState &rs)
{    
    cout << "Worm2Dosc init state" << endl;
    Worm2D::InitializeState(rs);
    return;    
}

void Worm2Dosc::Step1()
{

    n.setTime(t);
    // Update Body
    b.StepBody(settedStepSize);
    
    // Update Nervous System
    //n_ptr->EulerStep(settedStepSize);
    
    // Interneuron input  //////////////////////
    /* for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB, i), wAVB_DB * AVB);
        n_ptr->SetNeuronExternalInput(nn(VB, i), wAVB_VB * AVB);
        n_ptr->SetNeuronExternalInput(nn(DA, i), wAVA_DA * AVA);
        n_ptr->SetNeuronExternalInput(nn(VA, i), wAVA_VA * AVA);
    
    } */
    
    setMuscleInput();
    //setMuscleInputOrig(StepSize);   


    // Set input to Body
    //  First two segments receive special treatment because they are only affected by a single muscle
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

NSosc::NSosc(const pfa & pfa_)
{
pfa1.size = pfa_.size;
for (int i=0;i<pfa_.size;i++)
{
pfa1.phase.push_back(pfa_.phase[i]);
pfa1.freq.push_back(pfa_.freq[i]);
pfa1.amp.push_back(pfa_.amp[i]);
}

}