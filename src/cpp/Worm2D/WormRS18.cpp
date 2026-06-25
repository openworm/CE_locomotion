//
//  Worm18.cpp
//  one
//
//  Created by Eduardo Izquierdo on 9/25/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "WormRS18.h"
//#include "../argUtils.h"

#define HEADSR
#define VNCSR

//extern SuppliedArgs2018 supArgs1;


RS18Macros Worm18::setMacros()
{
bool headsr = 0;
#ifdef HEADSR
headsr = 1;
#endif
bool vncsr = 0;
#ifdef VNCSR
vncsr = 1;
#endif
return {headsr,vncsr};
}

//Worm18::Worm18():Worm18(readPhenotype(), 0){setRs18output(1);} //for WormCO18

//Worm18::Worm18():Worm2Dm({6,24,0.1,6,40}, new NervousSystem(), new Muscles), 
Worm18::Worm18(shared_ptr<const CmdArgs> cmd):Worm2Dm({6,24,0.1,6,40}, new NervousSystem(), cmd), 
n(dynamic_cast<NervousSystem&>(*n_ptr)),sr_ptr(make_shared<SR18>()),
rS18Macros(setMacros()),Worm2D({6,24,0.1,6,40},0)
{
    //W2Dbaseparameters1->randomInitialState = 1;
    setRs18output(1);
    //initConst();
    if (true)
    n.SetCircuitSize((par1.N_units*par1.N_neuronsperunit) + 4, 4, 4);

    //doOrigSRInput18 = true;
    //getValCJ<bool>("do_orig_sr_input", doOrigSRInput18);

    //doOrigMuscInput18 = true;
    //getValCJ<bool>("do_orig_musc_input", doOrigMuscInput18); 

} //for WormCO18Full


void Worm18::initConst()
{

    
    sr_ptr->setWeights();
    sr_ptr->setNSWeights(*this);

    

    setUpMuscleConn();
    setUpBodyConn();
    makeExternalInputConn();
    

}


// The constructor

Worm18::Worm18(const TVector<double> & v1, double output, shared_ptr<const CmdArgs> cmd):Worm18(cmd)
{
     SetParameters(v1);
}

Worm18::Worm18(const TVector<double> & v1, shared_ptr<const CmdArgs> cmd):Worm18(v1,1,cmd){}

//Worm18(getVector<double>(v1), 0){}

Worm18::Worm18(vector<double> v1, double output, shared_ptr<const CmdArgs> cmd):Worm18(cmd)
{
    //supArgs1.writeMessage();

    // Muscles
   // m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
    TVector<double> v;
    getTVector<double>(v1, v);
    SetParameters(v);
    
}

Worm18::Worm18(const string & filename_,shared_ptr<const CmdArgs> cmd):Worm18(cmd)
{
    setParsFromFile(filename_);
}

void Worm18::setPhenoNames()
{


        // Bias
        //  B-
        phenoNamesNums.push_back(1);
		phenoNames.push_back("b_bias");

        phenoNamesNums.push_back(2);
		phenoNames.push_back("d_bias");

        phenoNamesNums.push_back(3);
		phenoNames.push_back("b_tau");

        phenoNamesNums.push_back(4);
		phenoNames.push_back("d_tau");

       
        phenoNamesNums.push_back(5);
		phenoNames.push_back("b_selfcons");

        phenoNamesNums.push_back(6);
		phenoNames.push_back("d_selfcons");

        phenoNamesNums.push_back(7);
		phenoNames.push_back("chemcons");
        phenoNamesNums.push_back(8);
		phenoNames.push_back("chemcons");
        phenoNamesNums.push_back(9);
		phenoNames.push_back("chemcons");
        phenoNamesNums.push_back(10);
		phenoNames.push_back("electcons");
        phenoNamesNums.push_back(11);
		phenoNames.push_back("electcons");
        phenoNamesNums.push_back(12);
		phenoNames.push_back("electcons");
        phenoNamesNums.push_back(13);
		phenoNames.push_back("electcons");
        phenoNamesNums.push_back(14);
		phenoNames.push_back("stretch_recep");
        phenoNamesNums.push_back(28);
		phenoNames.push_back("stretch_recep");
        phenoNamesNums.push_back(15);
		phenoNames.push_back("b_NMJ");
        phenoNamesNums.push_back(16);
		phenoNames.push_back("d_NMJ");
        phenoNamesNums.push_back(17);
		phenoNames.push_back("SMD_bias");
        phenoNamesNums.push_back(18);
		phenoNames.push_back("RMD_bias");
        phenoNamesNums.push_back(19);
		phenoNames.push_back("SMD_tau");
        phenoNamesNums.push_back(20);
		phenoNames.push_back("RMD_tau");
     
        phenoNamesNums.push_back(21);
		phenoNames.push_back("SMD_self_chem");
        phenoNamesNums.push_back(22);
		phenoNames.push_back("RMD_self_chem");

        phenoNamesNums.push_back(23);
		phenoNames.push_back("SMD_DV_chem");
        phenoNamesNums.push_back(24);
		phenoNames.push_back("SR_chem");
        phenoNamesNums.push_back(25);
		phenoNames.push_back("RMD_DV_chem");

        phenoNamesNums.push_back(26);
        phenoNames.push_back("SR_elec");
        phenoNamesNums.push_back(27);
        phenoNames.push_back("RR_elec");

        phenoNamesNums.push_back(29);
        phenoNames.push_back("NMJ_SMD");
        phenoNamesNums.push_back(30);
        phenoNames.push_back("NMJ_RMD");
        
}
void Worm18::SetParameters(const TVector<double> & v)
{

    setParsFromPheno(v);
}

void Worm18::setParsFromPheno(const TVector<double> &v)
{
    hasVNC18 = false;

//NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);

    // Nervous system // Ventral cord
    if (false)
    n.SetCircuitSize((par1.N_units*par1.N_neuronsperunit) + 4, 4, 4);

    int db, dd, vba, vda, vbp, vdp;
    int ddNext, dbNext, vdaNext, vbaNext;

    for (int u = 1; u <= par1.N_units; u++){
        db = nn(DB,u);
        dd = nn(DD,u);
        vba = nn(VBA,u);
        vbp = nn(VBP,u);
        vda = nn(VDA,u);
        vdp = nn(VDP,u);

        ddNext = nn(DD,u+1);
        dbNext = nn(DB,u+1);
        vdaNext = nn(VDA,u+1);
        vbaNext = nn(VBA,u+1);

        // Bias
        //  B-
        n.SetNeuronBias(db, v(1));
        n.SetNeuronBias(vba, v(1));
        n.SetNeuronBias(vbp, v(1));
        //  D-
        n.SetNeuronBias(dd, v(2));
        n.SetNeuronBias(vda, v(2));
        n.SetNeuronBias(vdp, v(2));

        // Time Constant
        //  B-
        n.SetNeuronTimeConstant(db, v(3));
        n.SetNeuronTimeConstant(vba, v(3));
        n.SetNeuronTimeConstant(vbp, v(3));
        //  D-
        n.SetNeuronTimeConstant(dd, v(4));
        n.SetNeuronTimeConstant(vda, v(4));
        n.SetNeuronTimeConstant(vdp, v(4));

        // Self connections
        //  B-
        n.SetChemicalSynapseWeight(db, db, v(5));
        n.SetChemicalSynapseWeight(vba, vba, v(5));
        n.SetChemicalSynapseWeight(vbp, vbp, v(5));
        //  D-
        n.SetChemicalSynapseWeight(dd, dd, v(6));
        n.SetChemicalSynapseWeight(vda, vda, v(6));
        n.SetChemicalSynapseWeight(vdp, vdp, v(6));

        // Chemical synapses (COLORS AS IN FIGURE)
        // xB -> xD
        n.SetChemicalSynapseWeight(db, dd, v(7));       // Lighter Green
        n.SetChemicalSynapseWeight(vba, vda, v(7));
        n.SetChemicalSynapseWeight(vbp, vdp, v(7));
        // xB -> yD
        n.SetChemicalSynapseWeight(db, vda, v(8));      // Darker Green
        n.SetChemicalSynapseWeight(db, vdp, v(8));
        //n.SetChemicalSynapseWeight(vba, dd, v(8)/2);
        //n.SetChemicalSynapseWeight(vbp, dd, v(8)/2);
 
        {json jfunc;
        jfunc["f_ind"] = 1;
        jfunc["fact"] = 0.5;
        n.SetChemicalSynapseWeight(vba, dd, itsEf.eFunc(v(8), jfunc, true));
        n.SetChemicalSynapseWeight(vbp, dd, itsEf.eFunc(v(8), jfunc, true));        
        }



        // xD- -> yD
        n.SetChemicalSynapseWeight(dd, vda, v(9));     // Darker Blue

        // Gap junctions within the unit
        n.SetElectricalSynapseWeight(dd, vda, v(10));   // Light Gray
        n.SetElectricalSynapseWeight(dd, vdp, v(10));

        // D-
        n.SetElectricalSynapseWeight(vda, vdp, v(11));  // Blue
        // B-
        n.SetElectricalSynapseWeight(vba, vbp, v(12));  // Green

        // Gap junctions across units
        if (u < par1.N_units){
            //  D-
            n.SetElectricalSynapseWeight(dd, ddNext, v(11));    // Blue
            n.SetElectricalSynapseWeight(vdp, vdaNext, v(11));
            //  B-
            n.SetElectricalSynapseWeight(db, dbNext, v(12));  // Green
            n.SetElectricalSynapseWeight(vbp, vbaNext, v(12));
            //
            n.SetElectricalSynapseWeight(vbp, dbNext, v(13)); // Darker Gray
        }
    }
   // cout << "setting streatch" << endl;

    // Stretch receptor
    sr.SetStretchReceptorParams(N_segments, N_stretchrec, v(14), v(28));

    sr_ptr->SRvncgain = v(14);
    sr_ptr->SRheadgain = v(28);

    // NMJ Weight
    NMJ_DB = v(15);
    NMJ_VBa = v(15);
    NMJ_VBp = v(15);
    NMJ_DD = v(16);
    NMJ_VDa = v(16);
    NMJ_VDp = v(16);

    // Head Circuit
    //h.SetCircuitSize(4, 3, 2);

    // Bias
    n.SetNeuronBias(SMDD, v(17));
    n.SetNeuronBias(SMDV, v(17));
    n.SetNeuronBias(RMDD, v(18));
    n.SetNeuronBias(RMDV, v(18));

    // Time-Constant
    n.SetNeuronTimeConstant(SMDD, v(19));
    n.SetNeuronTimeConstant(SMDV, v(19));
    n.SetNeuronTimeConstant(RMDD, v(20));
    n.SetNeuronTimeConstant(RMDV, v(20));

    // Self-Connection
    n.SetChemicalSynapseWeight(SMDD, SMDD, v(21));
    n.SetChemicalSynapseWeight(SMDV, SMDV, v(21));
    n.SetChemicalSynapseWeight(RMDD, RMDD, v(22));
    n.SetChemicalSynapseWeight(RMDV, RMDV, v(22));

    // Chemical Synxapses (ALL)
    n.SetChemicalSynapseWeight(SMDD, SMDV, v(23));
    n.SetChemicalSynapseWeight(SMDV, SMDD, v(23));

    n.SetChemicalSynapseWeight(SMDD, RMDV, v(24));
    n.SetChemicalSynapseWeight(SMDV, RMDD, v(24));

    n.SetChemicalSynapseWeight(RMDD, RMDV, v(25));
    n.SetChemicalSynapseWeight(RMDV, RMDD, v(25));

    // Gap Junctions
    n.SetElectricalSynapseWeight(SMDD, RMDD, v(26));
    n.SetElectricalSynapseWeight(SMDV, RMDV, v(26));
    n.SetElectricalSynapseWeight(RMDV, RMDD, v(27));
    //cout << "setting nmj" << endl;
    // NMJ Weights
    NMJ_SMDD = v(29);
    NMJ_SMDV = v(29);
    NMJ_RMDD = v(30);
    NMJ_RMDV = v(30);

    // NMJ Gain
/* 
    if (false){
    NMJ_Gain_Map = 0.5;
    NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
        NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
    }

} */
     

    initConst();

    //makeExternalInputConn();
     //writeData();

}


void Worm18::InitializeState(RandomState &rs)
{
    //NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
    
    //shared_ptr<W2Dbaseparameters> w1parss = dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b);
    //assert(w1parss!=nullptr);

    //bool rIS = false;
    //getValCJ<bool>("random_initial_state", rIS);
    
    bool doLegacy;
    getValCJWorm<bool>("do_legacy",doLegacy);

    if (doLegacy){

    bool randomInitialState;
    getValCJWorm<bool>("random_initial_state",randomInitialState);

    if(randomInitialState)n.RandomizeCircuitState(-0.5, 0.5, rs);
    else 
    { for (int i = 1; i <= n.size-4; i++)
        n.SetNeuronState(i, (i-0.5)/(n.size-4));
    for (int i = 1; i <= 4; i++)
        n.SetNeuronState(i + n.size-4, (i-0.5)/4); 
    }
    }

    //n.RandomizeCircuitState(0.5, 0.5, rs); //fix initial conditions
    //h.RandomizeCircuitState(-0.5, 0.5, rs);
    Worm2D::InitializeState(rs);
    //writeData();
}



vector<toFromWeight> Worm18::makeVentralMuscleConn()
{
    //cout << "making ventral muscle con" << endl;

    vector<int> ventralNeurons({SMDV, RMDV, VDA, VBA, VDP, VBP});
    vector<double> ventralNMJ({NMJ_SMDV, NMJ_RMDD, NMJ_VDa, NMJ_VBa, NMJ_VDp, NMJ_VBp});
    hasVNC18 = true;
    //doubVars.setVal("NMJ gain map V", 0.5);
    //doubVars.setVal("NMJ gain fact", 0.7);
    namedVars["NMJ gain map V"] = 0.5;
    namedVars["NMJ gain fact"] = 0.7;
    //NMJ_gain_map_V = 0.5;
    //NMJ_gain_fact = 0.7;
    vector<weightentry> v1 = makeWeightEntry(ventralNeurons,ventralNMJ);
    ventinds.swap(v1);

    return makeVentralMuscleConn18();


/* 
    vector<toFromWeight> vec1;

        {vector<int> neurons({SMDV, RMDV});
        vector<double> NMJ({NMJ_SMDV, NMJ_RMDD});
        for (int i = 1; i <= HeadMotorNeuronMuscles; i++)
        makeMuscleConnHelp(vec1, neurons, NMJ, 1, i, NMJ_Gain);}

        vector<int> neuronsA({VDA, VBA});
        vector<int> neuronsP({VDP, VBP});
        vector<double> NMJA({NMJ_VDa, NMJ_VBa});
        vector<double> NMJP({NMJ_VDp, NMJ_VBp});
        vector<double> NMJA2({NMJ_VDa/2.0, NMJ_VBa/2.0});
        vector<double> NMJP2({NMJ_VDp/2.0, NMJ_VBp/2.0});

        for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        int mt = (i-VNCMuscleStart)%NmusclePerNU;
        //ventralHeadInputA = NMJ_VDa*n.NeuronOutput(nn(VDA,mi)) + NMJ_VBa*n.NeuronOutput(nn(VBA,mi));
        //ventralHeadInputP = NMJ_VDp*n.NeuronOutput(nn(VDP,mi)) + NMJ_VBp*n.NeuronOutput(nn(VBP,mi));

        //int mt = 1;
        switch(mt){
            case 0:
                makeMuscleConnHelp(vec1, neuronsA, NMJA, mi, i, NMJ_Gain);
                //m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputA);
                break;
            case 1:
                makeMuscleConnHelp(vec1, neuronsA, NMJA2, mi, i, NMJ_Gain);
                makeMuscleConnHelp(vec1, neuronsP, NMJP2, mi, i, NMJ_Gain);
                //m.SetVentralMuscleInput(i, NMJ_Gain(i)*((ventralHeadInputA + ventralHeadInputP)/2));
                break;
            case 2:
                makeMuscleConnHelp(vec1, neuronsP, NMJP, mi, i, NMJ_Gain);
                //m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputP);
                break;
        }

    }
    
     return vec1;
 */


}



vector<toFromWeight> Worm18::makeDorsalMuscleConn()
{

    vector<int> neurons({SMDD, RMDD, DD, DB});
    vector<double> NMJs({NMJ_SMDD, NMJ_RMDV, NMJ_DD, NMJ_DB});
    hasVNC18 = true;
    namedVars["NMJ gain map D"] = 0.5;
    namedVars["NMJ gain fact"] = 0.7;
    //doubVars.setVal("NMJ gain map D", 0.5);
    //doubVars.setVal("NMJ gain fact", 0.7);
    //NMJ_gain_map_D = 0.5;
    //NMJ_gain_fact = 0.7;
    vector<weightentry> v1 = makeWeightEntry(neurons,NMJs);
    dorsinds.swap(v1);

    return makeDorsalMuscleConn18();


/* 

    //cout << "making ventral muscle con" << endl;
    vector<toFromWeight> vec1;

    {vector<int> neurons({SMDD, RMDD});
    vector<double> NMJ({NMJ_SMDD, NMJ_RMDV});
    for (int i = 1; i <= HeadMotorNeuronMuscles; i++)
    makeMuscleConnHelp(vec1, neurons, NMJ, 1, i, NMJ_Gain);}

    {vector<int> neurons({DD,DB});
    vector<double> NMJ({NMJ_DD, NMJ_DB});
    for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        makeMuscleConnHelp(vec1, neurons, NMJ, mi, i, NMJ_Gain);
    }}

    return vec1; */

}







void Worm18::setMuscleInputOrigDorsal()
{
    
    double NMJ_Gain_Map = 0.5;
    TVector<double> NMJ_Gain;
    NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
        NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
    }

    double dorsalHeadInput = NMJ_SMDD*n_ptr->NeuronOutput(SMDD) + NMJ_RMDV*n_ptr->NeuronOutput(RMDD);

    for (int i = 1; i <= HeadMotorNeuronMuscles; i++){
        m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalHeadInput);      
        }

    for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        dorsalHeadInput = NMJ_DD*n_ptr->NeuronOutput(nn(DD,mi)) + NMJ_DB*n_ptr->NeuronOutput(nn(DB,mi));
        m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalHeadInput);
        }
}


void Worm18::setMuscleInputOrigVentral()
{
   double ventralHeadInput, ventralHeadInputA, ventralHeadInputP;

     double NMJ_Gain_Map = 0.5;
    TVector<double> NMJ_Gain;
    NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
        NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
    }
  
    ventralHeadInput = NMJ_SMDV*n_ptr->NeuronOutput(SMDV) + NMJ_RMDD*n_ptr->NeuronOutput(RMDV);

    for (int i = 1; i <= HeadMotorNeuronMuscles; i++){
        m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInput);
    }

    // Set input to Muscles from Ventral Cord
    //  Dorsal muscles (each motor neuron innervates three muscles, no overlap)

    for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        ventralHeadInputA = NMJ_VDa*n_ptr->NeuronOutput(nn(VDA,mi)) + NMJ_VBa*n_ptr->NeuronOutput(nn(VBA,mi));
        ventralHeadInputP = NMJ_VDp*n_ptr->NeuronOutput(nn(VDP,mi)) + NMJ_VBp*n_ptr->NeuronOutput(nn(VBP,mi));
        int mt = (i-VNCMuscleStart)%NmusclePerNU;
        switch(mt){
            case 0:
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputA);
                break;
            case 1:
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*((ventralHeadInputA + ventralHeadInputP)/2));
                break;
            case 2:
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputP);
                break;
        }
    }

    // Update Muscle activation
   // m.EulerStep(StepSize);

}



void Worm18::setMuscleInputOrig()
{

    double NMJ_Gain_Map = 0.5;
    TVector<double> NMJ_Gain;
    NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
        NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
    }

   double dorsalHeadInput, ventralHeadInput, ventralHeadInputA, ventralHeadInputP;

    dorsalHeadInput = NMJ_SMDD*n_ptr->NeuronOutput(SMDD) + NMJ_RMDV*n_ptr->NeuronOutput(RMDD);
    ventralHeadInput = NMJ_SMDV*n_ptr->NeuronOutput(SMDV) + NMJ_RMDD*n_ptr->NeuronOutput(RMDV);

    for (int i = 1; i <= HeadMotorNeuronMuscles; i++){
        m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalHeadInput);
        m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInput);
    }

    // Set input to Muscles from Ventral Cord
    //  Dorsal muscles (each motor neuron innervates three muscles, no overlap)

    for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        dorsalHeadInput = NMJ_DD*n_ptr->NeuronOutput(nn(DD,mi)) + NMJ_DB*n_ptr->NeuronOutput(nn(DB,mi));
        m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalHeadInput);
        ventralHeadInputA = NMJ_VDa*n_ptr->NeuronOutput(nn(VDA,mi)) + NMJ_VBa*n_ptr->NeuronOutput(nn(VBA,mi));
        ventralHeadInputP = NMJ_VDp*n_ptr->NeuronOutput(nn(VDP,mi)) + NMJ_VBp*n_ptr->NeuronOutput(nn(VBP,mi));
        int mt = (i-VNCMuscleStart)%NmusclePerNU;
        switch(mt){
            case 0:
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputA);
                break;
            case 1:
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*((ventralHeadInputA + ventralHeadInputP)/2));
                break;
            case 2:
                m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInputP);
                break;
        }
    }

    // Update Muscle activation
    m.EulerStep(settedStepSize);

}



void Worm18::preNStep()
{

//double ds, vs;

    b.StepBody(settedStepSize);


    //bool doOrigSRInput;
    //getValCJWorm<bool>("do_orig_sr_input",doOrigSRInput);



    if (doOrigSRInput){
   
    // Set input to Stretch Receptors from Body
    for(int i = 1; i <= N_segments; ++i){
        double ds = (b.DorsalSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
        double vs =  (b.VentralSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
        sr.SetDorsalInput(i, ds);
        sr.SetVentralInput(i, vs);
    }

    // Update Stretch Receptors
    sr.Update(); 
 



    // Set input to Nervous System (Head) from Stretch Receptors
//#ifdef HEADSR

    //StretchReceptor18 * sr_ptr = &sr;


     if (rS18Macros.headsr)
    {
     if (rs18output == 1){
        n_ptr->SetNeuronExternalInput(SMDD, sr.HeadDorsalOutput());    // Average of first
        n_ptr->SetNeuronExternalInput(SMDV, sr.HeadVentralOutput());   // to segments
        }   
    }

    //#endif

        // Set input to Nervous System (Ventral Cord) from Stretch Receptors
    //#ifdef VNCSR

    if (rS18Macros.vncsr)
    {
        for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB,i), sr.VCDorsalOutput(i));
        n_ptr->SetNeuronExternalInput(nn(VBA,i), sr.VCVentralAOutput(i));
        n_ptr->SetNeuronExternalInput(nn(VBP,i), sr.VCVentralPOutput(i));
        }
    }     


}
else
{
    sr_ptr->updateAll(b);
  

    zeroAllInputs();
    setExternalInput();
    //setExternalInputOrig();

    sr_ptr->incNS(*n_ptr);

}


//#endif



}


void Worm18::postNStep()
{

    //bool doOrigMuscInput;
    //getValCJWorm<bool>("do_orig_musc_input",doOrigMuscInput);

    if (doOrigMuscInput) setMuscleInputOrig();
    else setMuscleInput();

    
    //setMuscleInputOrig();
    //setMuscleInputVec(StepSize);
    //setMuscleInput(StepSize);

    //setMuscleInputDors();   
    //setMuscleInputVent();
    //setMuscleInputOrigDorsal();   
    //setMuscleInputOrigVentral();
    //m.EulerStep(StepSize);

    // Set input to Body
    //  First two segments receive special treatment because they are only affected by a single muscle

    setBodyInput();

 if (false){
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

}
}

void Worm18::Step1()
{
    

    preNStep();

    // Update Nervous System
    //h.EulerStep(StepSize);

    n_ptr->EulerStep(settedStepSize);

    // Set input to Muscles
    //  Input from the head circuit

    postNStep();

    // Time
    //t += StepSize;
}

const vector<string> Worm18::getSectionNames()
{
vector<string> v1(par1.N_units*par1.N_neuronsperunit, "vnc");
vector<string> v2(4, "head");
v1.insert(v1.end(),v2.begin(),v2.end());
return v1;


}

const vector<string> Worm18::getDistinctCellNames()
{
vector<string> v1 = getCellNamesUnits({"DB", "DD", "VBA", "VDA", "VBP", "VDP"}, par1.N_units);
vector<string> v2 = {"SMDD_0", "RMDD_0", "SMDV_0", "RMDV_0"};
v1.insert(v1.end(),v2.begin(),v2.end());
return v1;

}

const vector<string>  Worm18::getCellNames() 
{
    vector<string> v1 = getCellNamesAll({"DB", "DD", "VBA", "VDA", "VBP", "VDP"}, par1.N_units);
    vector<string> v2 = {"SMDD", "RMDD", "SMDV", "RMDV"};
    v1.insert(v1.end(),v2.begin(),v2.end());
    return v1;
}

void Worm18::addParsToJson(json & j)
{


    Worm2D::addParsToJson(j);

    sr_ptr->addParsToJson(j);

    if (doOrigSRInput){
    Params<double> par = sr.getStretchReceptorParams();
    appendToJson<double>(j["Stretch receptor"], par);
     }

    string nsHead = "Nervous system";

    if (false){
    appendAllNSJson(j[nsHead], n);
    }

   

   
    j[nsHead]["section sizes"]["head"]["value"] = 4;
    j[nsHead]["section sizes"]["head"]["plot order"] = 0;
    j[nsHead]["section sizes"]["VNC"]["value"] = 36;
    j[nsHead]["section sizes"]["VNC"]["plot order"] = 1;

   
    //string nsHead = "Nervous system";
    //appendCellNamesToJson(j[nsHead], getCellNames(), 1);
    //appendCellNamesToJson(j[nsHead], getHeadCellNames(), 1);
}


vector<doubIntParamsHead> Worm18::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;
    double NMJ_Gain_Map = namedVars["NMJ gain map D"];
    //namedVars["NMJ gain fact"] = 0.7;

    var1.parDoub.head = "worm";
    var1.parDoub.names = {"NMJ_DB", "NMJ_VBa", "NMJ_VBp", "NMJ_DD", "NMJ_VDa", "NMJ_VDp",
    "NMJ_SMDD", "NMJ_SMDV", "NMJ_RMDD", "NMJ_RMDV", "NMJ Gain"};
    var1.parDoub.vals = {NMJ_DB, NMJ_VBa, NMJ_VBp, NMJ_DD, NMJ_VDa, NMJ_VDp, 
    NMJ_SMDD, NMJ_SMDV, NMJ_RMDD, NMJ_RMDV, NMJ_Gain_Map};

    var1.parInt.head = "worm";
    var1.parInt.names = {"N_stretchrec", "HeadMotorNeuronMuscles", "VNCMuscleStart", "NmusclePerNU"};
    var1.parInt.vals = {N_stretchrec, HeadMotorNeuronMuscles, VNCMuscleStart, NmusclePerNU};
    var1.parInt.messages = {"Number of stretch receptors", 
        "Head motorneurons innervate first 8 muscles (temporarily first 6)",
        "VNC motorneurons innervate starting from 7th muscle",
        "All the way down to 24, in groups of 3 per unit"};
    var1.parInt.messages_inds = {0,1,2,3};
    
    parvec.push_back(var1);
    return parvec;

}



void Worm18::writeAct()
{
   
/* 
    static bool firstcall = true;
    static size_t pos;
    static int tt;

    if (resetStats(firstcall,pos,tt,"act.dat")) return; */


   // StretchReceptor18 * sr_ptr2 = &sr;

   // bool doOrigSRInput;
   // getValCJWorm<bool>("do_orig_sr_input",doOrigSRInput);

  size_t pos = getPos("act.dat");
  ofstream & ofs = ofsvec[pos];  
    int & tt = tts[pos];


    if (++tt >= dataskips) {
        tt = 0;

        ofs << datatime;
        //ofs << "\nSR: ";
        // Stretch receptors
        if (doOrigSRInput){

        ofs <<  " " << sr.HeadDorsalOutput() << " " << sr.HeadVentralOutput();
        for (int i = 1; i <= N_stretchrec; i++) 
            ofs <<  " " << sr.VCDorsalOutput(i) << " " 
            << sr.VCVentralAOutput(i) << " " << sr.VCVentralPOutput(i);
        
        }
        else {

        ofs <<  " " << sr_ptr->HeadDorsalOutput() << " " << sr_ptr->HeadVentralOutput();
        for (int i = 1; i <= sr_ptr->srvars_ptr->nstretch; i++) 
            ofs <<  " " << sr_ptr->VCDorsalOutput(i) << " " 
            << sr_ptr->VCVentralAOutput(i) << " " << sr_ptr->VCVentralPOutput(i);


        }


        // Head Neurons
        //ofs << "\nH: ";
        int offset = par1.N_units*par1.N_neuronsperunit;

        for (int i = 1; i <= 4; i++) {
            ofs <<  " " << n_ptr->NeuronOutput(offset + i);
        }
        // Ventral Cord Motor Neurons
        //ofs << "\nV: ";
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n_ptr->NeuronOutput(nn(j,i));
            }
        }
        // Muscles
        //ofs << "\nM: ";
        if (m_ptr){
        for (int i = 1; i <= par1.N_muscles; i++) {
            ofs <<  " " << m_ptr->DorsalMuscleOutput(i) << " " << m_ptr->VentralMuscleOutput(i);
        }}
        ofs << endl;
    }
}

void Worm18::DumpVoltage(ofstream &ofs, int skips)
{
    static int tt = skips;

    if (++tt >= skips) {
        tt = 0;

        ofs << t;
        // Head Neurons
        int offset = par1.N_units*par1.N_neuronsperunit;
        for (int i = 1; i <= 4; i++) {
            ofs <<  " " << n_ptr->NeuronState(offset + i);
        }
        // Ventral Cord Motor Neurons
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n_ptr->NeuronState(nn(j,i));
            }
        }
        ofs << "\n";
    }
}



void Worm18::DumpParams(ofstream &ofs)
{
    //StretchReceptor18 * sr_ptr = &sr;
    NervousSystem & n = dynamic_cast<NervousSystem&>(*n_ptr);
    
    ofs << "Time-constants: \n DB: " << n.NeuronTimeConstant(DB) << "\n VBA/P: " << n.NeuronTimeConstant(VBA) << " / " << n.NeuronTimeConstant(VBP) << "\n DD: " << n.NeuronTimeConstant(DD) << "\n VDA/P: " << n.NeuronTimeConstant(VDA) << " / " << n.NeuronTimeConstant(VDP) << endl;
    ofs << "Biases: \n DB: " << n.NeuronBias(DB) << "\n VBA/P: " << n.NeuronBias(VBA) << " / " << n.NeuronBias(VBP)  <<  "\n DD: " << n.NeuronBias(DD) << "\n VDA/P: " << n.NeuronBias(VDA) <<  " / " << n.NeuronBias(VDP) << endl;
    ofs << "Self conns: \n DB: " << n.ChemicalSynapseWeight(DB, DB) << "\n VBA/P: " << n.ChemicalSynapseWeight(VBA, VBA) << " / " << n.ChemicalSynapseWeight(VBP, VBP) << "\n DD: " << n.ChemicalSynapseWeight(DD, DD) <<  "\n VDA/P: " << n.ChemicalSynapseWeight(VDA, VDA) <<  " / " << n.ChemicalSynapseWeight(VDP, VDP) << endl;
    ofs << "Chem Conns: \n DB->DD: " << n.ChemicalSynapseWeight(DB, DD) <<  "\n DB->VDA/VDP: " << n.ChemicalSynapseWeight(DB, VDA) << " / " << n.ChemicalSynapseWeight(DB, VDP) << "\n VBA/P->DD: " << n.ChemicalSynapseWeight(VBA, DD) << " / " << n.ChemicalSynapseWeight(VBP, DD) << "\n VBA/P->VDA/P: " << n.ChemicalSynapseWeight(VBA, VDA) << " / " << n.ChemicalSynapseWeight(VBP, VDP) << "\n VDA/P->VBA/P: " << n.ChemicalSynapseWeight(VDA, VBA) << " / " << n.ChemicalSynapseWeight(VDP, VBP) << "\n DD->VDA: " << n.ChemicalSynapseWeight(DD, VDA) <<endl;
    ofs << "Gap Juncs: \n DB-DB+1: " << n.ElectricalSynapseWeight(DB, DB+par1.N_neuronsperunit) << "\n VBA-VBP / VBP-VBP+1: " << n.ElectricalSynapseWeight(VBA, VBP) << " / " << n.ElectricalSynapseWeight(VBP, VBA+par1.N_neuronsperunit) << "\n VBP-DB+1: " << n.ElectricalSynapseWeight(VBP, DB+par1.N_neuronsperunit) << "\n DD-VDA/P: " << n.ElectricalSynapseWeight(DD, VDA) << " / " << n.ElectricalSynapseWeight(DD, VDP) << "\n DD-DD+1: " << n.ElectricalSynapseWeight(DD, DD+par1.N_neuronsperunit) << "\n VDA-VDP / VDP-VDP+1: " << n.ElectricalSynapseWeight(VDA, VDP) << " / " << n.ElectricalSynapseWeight(VDP, VDA+par1.N_neuronsperunit) <<  endl;

    //shared_ptr<W2Dbaseparameters> w1parss = dynamic_pointer_cast<W2Dbaseparameters>(W2Dbaseparameters1b);
    //assert(w1parss!=nullptr);


    //bool doOrigSRInput;
    //getValCJWorm<bool>("do_orig_sr_input",doOrigSRInput);

    if (doOrigSRInput)
    ofs << "SR Gain (VNC and Head): " << sr.SRvncgain << " " << sr.SRheadgain << endl;
    else 
    ofs << "SR Gain (VNC and Head): " << sr_ptr->SRvncgain << " " << sr_ptr->SRheadgain << endl;

   double NMJ_Gain_Map = namedVars["NMJ gain map D"];

    ofs << "NMJ weights: \n B: " << NMJ_DB << " " << NMJ_VBa << " " << NMJ_VBp << "\n D: " <<  NMJ_DD << " " << NMJ_VDa << " " << NMJ_VDp << endl;
    ofs << "Head: \nBiases: \n SMD(D/V): " << n.NeuronBias(SMDD) << " " << n.NeuronBias(SMDV) << "\n RMD(D/V): "<< n.NeuronBias(RMDD) << " "<< n.NeuronBias(RMDV) << endl;
    ofs << "Time-constants: \n SMD(D/V): " << n.NeuronTimeConstant(SMDD) << " " << n.NeuronTimeConstant(SMDV) << "\n RMD(D/V): " << n.NeuronTimeConstant(RMDD) << " " << n.NeuronTimeConstant(RMDV) << endl;
    ofs << "Self conns: \n SMD(D/V): " <<n.ChemicalSynapseWeight(SMDD,SMDD) << " " << n.ChemicalSynapseWeight(SMDV,SMDV) << "\n RMD(D/V): " << n.ChemicalSynapseWeight(RMDD,RMDD) << " "<< n.ChemicalSynapseWeight(RMDV,RMDV) << endl;
    ofs << "Chem conns: " << "\n SMDD->RMDD: " << n.ChemicalSynapseWeight(SMDD, RMDD) << "\n SMDD->SMDV: " << n.ChemicalSynapseWeight(SMDD, SMDV) << "\n SMDD->RMDV: " << n.ChemicalSynapseWeight(SMDD, RMDV) << "\n RMDD->SMDD: " << n.ChemicalSynapseWeight(RMDD, SMDD) << "\n RMDD->SMDV: " << n.ChemicalSynapseWeight(RMDD, SMDV) << "\n RMDD->RMDV: " << n.ChemicalSynapseWeight(RMDD, RMDV) << "\n SMDV->SMDD: " << n.ChemicalSynapseWeight(SMDV, SMDD) << "\n SMDV->RMDD: " << n.ChemicalSynapseWeight(SMDV, RMDD) << "\n SMDV->RMDV: " << n.ChemicalSynapseWeight(SMDV, RMDV) << "\n RMDV->SMDD: " << n.ChemicalSynapseWeight(RMDV, SMDD) << "\n RMDV->RMDD: " << n.ChemicalSynapseWeight(RMDV, RMDD) << "\n RMDV->SMDV: " << n.ChemicalSynapseWeight(RMDV, SMDV) << endl;
    ofs << "Gap Juncs: " << "\n SMD-RMD: " << n.ElectricalSynapseWeight(SMDD, RMDD)
                         << "\n RMD-RMD: " << n.ElectricalSynapseWeight(RMDD, RMDV) << endl;
    ofs << "NMJ weights: \n SMD(D/V): " << NMJ_SMDD << " " << NMJ_SMDV << "\n RMD(D/V): " <<  NMJ_RMDD << " " << NMJ_RMDV << endl;
    ofs << "NMJ Gain: " << NMJ_Gain_Map << endl;
}

void Worm18::addFuncableToJson(json & j)
{
 
    if (true){
    if (!j.contains("Funcable")) j["Funcable"] = json::object();
    //j["Funcable"]["1"] = {{"doInverse", false}, {"fact", 1}};
    }

    json chemvecj = json::array();

  for (int u = 1; u <= par1.N_units; u++){

    int db, dd, vba, vda, vbp, vdp;
    int ddNext, dbNext, vdaNext, vbaNext;
  
        db = nn(DB,u);
        dd = nn(DD,u);
        vba = nn(VBA,u);
        vbp = nn(VBP,u);
        vda = nn(VDA,u);
        vdp = nn(VDP,u);

        ddNext = nn(DD,u+1);
        dbNext = nn(DB,u+1);
        vdaNext = nn(VDA,u+1);
        vbaNext = nn(VBA,u+1);

    chemvecj.push_back({{"from", vba}, {"to", dd}, {"mfunc", {{"f_ind", 1}, {"fact", 0.5}}}});
    chemvecj.push_back({{"from", vbp}, {"to", dd}, {"mfunc", {{"f_ind", 1}, {"fact", 0.5}}}});

  }

  j["Nervous system"]["Chemical weights"]["funcable"] = chemvecj;

}

void Worm18::addEvolvableToJson(json & j)
{
 
    {vector<doubDoub> vec; 

    const double	BiasRange			    = 15.0;
    const double    SCRange                 = 15.0;
    const double    CSRange                 = 15.0;
    const double    TauMin                  = 0.5; //
    const double    TauMax                  = 2.0;

    const double    ESRange                 = 2.0;

    const double    SRmax                   = 200.0;
    const double    NMJmax                  = 1.0;
    const double    HCSRange                = 15.0;

vec.push_back({-BiasRange, BiasRange});
vec.push_back({-BiasRange, BiasRange});
vec.push_back({ TauMin, TauMax});

vec.push_back({TauMin, TauMax});
vec.push_back({-SCRange, SCRange});
vec.push_back({-SCRange, SCRange});

vec.push_back({ -CSRange, CSRange});
vec.push_back({ -CSRange, CSRange});

//vec.push_back({ -CSRange, CSRange}); // added this - number 9

vec.push_back({ -CSRange, CSRange});

vec.push_back({0.0, ESRange});
vec.push_back({0.0, ESRange});
vec.push_back({0.0, ESRange});
vec.push_back({0.0, ESRange});
vec.push_back({-SRmax, 0.0});
vec.push_back({0.0, NMJmax});
vec.push_back({-NMJmax, 0.0});
vec.push_back({-BiasRange, BiasRange});
vec.push_back({-BiasRange, BiasRange});

vec.push_back({TauMin, TauMax});
vec.push_back({TauMin, TauMax});
vec.push_back({-SCRange, SCRange});
vec.push_back({4.0, SCRange});
vec.push_back({-HCSRange, HCSRange});
vec.push_back({-HCSRange, HCSRange});
vec.push_back({-HCSRange, HCSRange});
vec.push_back({0.0, ESRange});
vec.push_back({0.0, ESRange});
vec.push_back({-SRmax, 0.0});
vec.push_back({0.0, NMJmax});
vec.push_back({0.0, NMJmax});

j["evolvable_ranges"] = toEvolvableRangesJson(vec);
j["evolved_used"]["value"] = toEvolvedUsedJson(vec);

 }

    vector<intPair> biasvec, tauvec;
    vector<fromToInt> chemvec, elecvec;
    json chemvecj = json::array();
    json biasvecj = json::array();
    //json biasvecj = json::object();

    int db, dd, vba, vda, vbp, vdp;
    int ddNext, dbNext, vdaNext, vbaNext;

    const vector<string>  cell_names_full = getDistinctCellNames();

    for (int u = 1; u <= par1.N_units; u++){

    

        db = nn(DB,u);
        dd = nn(DD,u);
        vba = nn(VBA,u);
        vbp = nn(VBP,u);
        vda = nn(VDA,u);
        vdp = nn(VDP,u);

        ddNext = nn(DD,u+1);
        dbNext = nn(DB,u+1);
        vdaNext = nn(VDA,u+1);
        vbaNext = nn(VBA,u+1);


        //biasvecj += json::object_t::value_type({"ind", 1}, {"val", db});
        //json o2 = R"( {"ind": 1, "val": db} )"_json;
        //biasvecj += json::object_t::value_type({{"ind", db}, {"val", 1}});
        //biasvecj += json::object_t::value_type({{"ind", vba}, {"val", 1}});

        biasvecj.push_back({{"ind", db}, {"evotag", "evotag_1"}});
        biasvecj.push_back({{"ind", vba}, {"evotag", "evotag_1"}});
        biasvecj.push_back({{"ind", vbp}, {"evotag", "evotag_1"}});
        biasvecj.push_back({{"ind", dd}, {"evotag", "evotag_2"}});
        biasvecj.push_back({{"ind", vda}, {"evotag", "evotag_2"}});
        biasvecj.push_back({{"ind", vdp}, {"evotag", "evotag_2"}});


         /*    biasvecj.push_back({db,1});
            biasvecj.push_back({vba,1});
            biasvecj.push_back({vbp,1});
            biasvecj.push_back({dd,2});
            biasvecj.push_back({vda,2});
            biasvecj.push_back({vdp,2}); */

            //if (false)
            {vector<intPair> & vec = biasvec;
            vec.push_back({db,1});
            vec.push_back({vba,1});
            vec.push_back({vbp,1});
            vec.push_back({dd,2});
            vec.push_back({vda,2});
            vec.push_back({vdp,2});
            }


         {vector<intPair> & vec = tauvec;
            vec.push_back({db,3});
            vec.push_back({vba,3});
            vec.push_back({vbp,3});
            vec.push_back({dd,4});
            vec.push_back({vda,4});
            vec.push_back({vdp,4});
        }



        chemvecj.push_back({{"from", db}, {"to", db}, {"evotag", "evotag_5"}});
        chemvecj.push_back({{"from", vba}, {"to", vba}, {"evotag", "evotag_5"}});
        chemvecj.push_back({{"from", vbp}, {"to", vbp}, {"evotag", "evotag_5"}});
        chemvecj.push_back({{"from", dd}, {"to", dd}, {"evotag", "evotag_6"}});
        chemvecj.push_back({{"from", vda}, {"to", vda}, {"evotag", "evotag_6"}});
        chemvecj.push_back({{"from", vdp}, {"to", vdp}, {"evotag", "evotag_6"}});
        chemvecj.push_back({{"from", db}, {"to", dd}, {"evotag", "evotag_7"}});
        chemvecj.push_back({{"from", vba}, {"to", vda}, {"evotag", "evotag_7"}});
        chemvecj.push_back({{"from", vbp}, {"to", vdp}, {"evotag", "evotag_7"}});
        chemvecj.push_back({{"from", db}, {"to", vda}, {"evotag", "evotag_8"}});
        chemvecj.push_back({{"from", db}, {"to", vdp}, {"evotag", "evotag_8"}});
        chemvecj.push_back({{"from", vba}, {"to", dd}, {"evotag", "evotag_8"}, {"mfunc", {{"f_ind", 1}, {"fact", 0.5}}}});
        chemvecj.push_back({{"from", vbp}, {"to", dd}, {"evotag", "evotag_8"}, {"mfunc", {{"f_ind", 1}, {"fact", 0.5}}}});
        chemvecj.push_back({{"from", dd}, {"to", vda}, {"evotag", "evotag_9"}});

        addMfuncTFI(j["nervous_system"]["chemical_conns"]["value"], 
        {vba, dd,8}, cell_names_full, {{"f_ind", 1}, {"fact", 0.5}});
        addMfuncTFI(j["nervous_system"]["chemical_conns"]["value"], 
        {vbp, dd,8}, cell_names_full , {{"f_ind", 1}, {"fact", 0.5}});

//if (false)
        {
            vector<fromToInt> & vec = chemvec;
            vec.push_back({db,db,5});
            vec.push_back({vba,vba,5});
            vec.push_back({vbp,vbp,5});
            vec.push_back({dd,dd,6});
            vec.push_back({vda,vda,6});
            vec.push_back({vdp,vdp,6});
            vec.push_back({db,dd,7});
            vec.push_back({vba, vda,7});
            vec.push_back({vbp, vdp,7});
            vec.push_back({db, vda,8});
             vec.push_back({db, vdp,8});

 /*        n.SetChemicalSynapseWeight(db, vda, v(8));      // Darker Green
        n.SetChemicalSynapseWeight(db, vdp, v(8));
        n.SetChemicalSynapseWeight(vba, dd, v(8)/2);
        n.SetChemicalSynapseWeight(vbp, dd, v(8)/2); */

// reduce all by unity below here

   vec.push_back({vba, dd,8});
   vec.push_back({vbp, dd,8});
   vec.push_back({dd, vda,9});   
}


    {
    vector<fromToInt> & vec = elecvec;
    push_back_double({dd, vda, 10}, vec);
    push_back_double({dd, vdp, 10}, vec);

    push_back_double({vda, vdp, 11}, vec);
    push_back_double({vba, vbp, 12}, vec);

      if (u < par1.N_units){

    push_back_double({dd, ddNext,11}, vec);
    push_back_double({vdp, vdaNext,11}, vec);
    push_back_double({db, dbNext,12}, vec);
    push_back_double({vbp, vbaNext,12}, vec);
    push_back_double({vbp, dbNext,13}, vec);

      }


    }




    }
   
    

   // cout << "setting streatch" << endl;

    // Stretch receptor
    //sr.SetStretchReceptorParams(N_segments, N_stretchrec, v(14), v(28));

   j["stretch_receptor"]["sr_vnc_gain"]["evotag"] = "evotag_14";
   j["stretch_receptor"]["sr_head_gain"]["evotag"] = "evotag_28";

   


    j["Stretch receptor"]["SRvncgain"]["evolvable"] = 14;
    j["Stretch receptor"]["SRheadgain"]["evolvable"] = 28;

    //sr_ptr->SRvncgain = v(14);
    //sr_ptr->SRheadgain = v(28);

vector<intPair> nmjvecd;
nmjvecd.push_back({DB,15});
nmjvecd.push_back({DD,16});
nmjvecd.push_back({SMDD,29});
nmjvecd.push_back({RMDD,30});

vector<intPair> nmjvecv;
nmjvecv.push_back({VBA,15});
nmjvecv.push_back({VBP,15});
nmjvecv.push_back({VDA,16});
nmjvecv.push_back({VDP,16});
nmjvecv.push_back({SMDV,29});
nmjvecv.push_back({RMDV,30});

addEvolvableIP(j["vnc_18"]["dorsal_conns"], nmjvecd , "weight", getCellNames());
addEvolvableIP(j["vnc_18"]["ventral_conns"], nmjvecv , "weight", getCellNames());


j["VNC 18"]["V inds"]["evolvable"] = to_evo_json(nmjvecv);
j["VNC 18"]["D inds"]["evolvable"] = to_evo_json(nmjvecd);

 

    // Head Circuit
    //h.SetCircuitSize(4, 3, 2);

    // Bias

/* 
    biasvecj.push_back({SMDD,17});
    biasvecj.push_back({SMDV,17});
    biasvecj.push_back({RMDD,18});
    biasvecj.push_back({RMDV,18}); */
    biasvecj.push_back({{"ind", SMDD}, {"evotag", "evotag_17"}});
    biasvecj.push_back({{"ind", SMDV}, {"evotag", "evotag_17"}});
    biasvecj.push_back({{"ind", RMDD}, {"evotag", "evotag_18"}});
    biasvecj.push_back({{"ind", RMDV}, {"evotag", "evotag_18"}});

    //if (false)
    {vector<intPair> & vec = biasvec;
    vec.push_back({SMDD,17});
    vec.push_back({SMDV,17});
    vec.push_back({RMDD,18});
    vec.push_back({RMDV,18});
    }

  {vector<intPair> & vec = tauvec;
    vec.push_back({SMDD,19});
    vec.push_back({SMDV,19});
    vec.push_back({RMDD,20});
    vec.push_back({RMDV,20});
    }


    chemvecj.push_back({{"from", SMDD}, {"to", SMDD}, {"evotag", "evotag_21"}});
    chemvecj.push_back({{"from", SMDV}, {"to", SMDV}, {"evotag", "evotag_21"}});
    chemvecj.push_back({{"from", RMDD}, {"to", RMDD}, {"evotag", "evotag_22"}});
    chemvecj.push_back({{"from", RMDV}, {"to", RMDV}, {"evotag", "evotag_22"}});
    chemvecj.push_back({{"from", SMDD}, {"to", SMDV}, {"evotag", "evotag_23"}});
    chemvecj.push_back({{"from", SMDV}, {"to", SMDD}, {"evotag", "evotag_23"}});
    chemvecj.push_back({{"from", SMDD}, {"to", RMDV}, {"evotag", "evotag_24"}});
    chemvecj.push_back({{"from", SMDV}, {"to", RMDD}, {"evotag", "evotag_24"}});
    chemvecj.push_back({{"from", RMDD}, {"to", RMDV}, {"evotag", "evotag_25"}});
    chemvecj.push_back({{"from", RMDV}, {"to", RMDD}, {"evotag", "evotag_25"}});


//if (false)
  {
            vector<fromToInt> & vec = chemvec;
            vec.push_back({SMDD, SMDD,21});
            vec.push_back({SMDV, SMDV,21});
            vec.push_back({RMDD, RMDD,22});
            vec.push_back({RMDV, RMDV,22});
             vec.push_back({SMDD, SMDV,23});
            vec.push_back({SMDV, SMDD,23});
            vec.push_back({SMDD, RMDV,24});
            vec.push_back({SMDV, RMDD,24});
            vec.push_back({RMDD, RMDV,25});
            vec.push_back({RMDV, RMDD,25});
  }

   {vector<fromToInt> & vec = elecvec;
        push_back_double({SMDD, RMDD,26}, vec);
        push_back_double({SMDV, RMDV,26}, vec);
        push_back_double({RMDV, RMDD,27}, vec);    
        }

   
   

    addEvolvableTFI(j["nervous_system"]["chemical_conns"]["value"], chemvec, cell_names_full);
    addEvolvableTFI(j["nervous_system"]["electrical_conns"]["value"], elecvec, cell_names_full, true);
    addEvolvableIP(j["nervous_system"]["cells"], biasvec, "bias", cell_names_full);
    addEvolvableIP(j["nervous_system"]["cells"], tauvec, "tau", cell_names_full);

  



 j["Nervous system"]["taus"]["evolvable"] = to_evo_json(tauvec);
    j["Nervous system"]["biases"]["evolvable"] = biasvecj;
  j["Nervous system"]["Chemical weights"]["evolvable"] = chemvecj;
  j["Nervous system"]["Electrical weights"]["evolvable"] = elecvec;

  //json & j2 = j["Nervous system"]["Chemical weights"]["evolvable"];

    /* for(auto it = j2.begin(); it != j2.end(); ++it)
        {

            if (it->at("from")==vba && it->at("to")==dd);

        }   
 */

    addEvoNames(j);

    }


void Worm18::GenPhenMapping(const TVector<double> &gen, TVector<double> &phen)
{
    // --------------------------------
    // Parameters for the Ventral Nerve Cord Unit
    // --------------------------------
    // Bias


    const double	  BiasRange			         	= 15.0;
    const double    SCRange                 = 15.0;
    const double    CSRange                 = 15.0;
    const double    TauMin                  = 0.5; //
    const double    TauMax                  = 2.0;

    const double    ESRange                 = 2.0;

    const double    SRmax                   = 200.0;
    const double    NMJmax                  = 1.0;
    const double    HCSRange                = 15.0;


    phen(1) = MapSearchParameter(gen(1), -BiasRange, BiasRange);        // DB, VBa, VBp
    phen(2) = MapSearchParameter(gen(2), -BiasRange, BiasRange);        // DD, VDa, VDp

    // Time Constant
    phen(3) = MapSearchParameter(gen(3), TauMin, TauMax);               // DB, VBa, VBp
    phen(4) = MapSearchParameter(gen(4), TauMin, TauMax);               // DD, VDa, VDp

    // Self connections
    phen(5) = MapSearchParameter(gen(5), -SCRange, SCRange);            // DB, VBa, VBp
    phen(6) = MapSearchParameter(gen(6), -SCRange, SCRange);            // DD, VDa, VDp

    // Chemical synapses
    phen(7) = MapSearchParameter(gen(7), -CSRange, CSRange);            // DB -> DD, VBa -> VDa, VBp -> VDp

    phen(8) = MapSearchParameter(gen(8), -CSRange, CSRange);          // DB -> VDa, DB -> VDp, VBa -> DD /2, VBp -> DD /2

    phen(9) = MapSearchParameter(gen(9), -CSRange, CSRange);          // DD -> VDa

    // Gap junctions across class within unit
    phen(10) = MapSearchParameter(gen(10), 0.0, ESRange);      // DD - VDa, DD - VDp

    // Gap junctions per class
    phen(11) = MapSearchParameter(gen(11), 0.0, ESRange);      // VD - VD, DD - DD
    phen(12) = MapSearchParameter(gen(12), 0.0, ESRange);      // VB - VB, DB - DB

    // Gap junctions across class, across neural unit
    phen(13) = MapSearchParameter(gen(13), 0.0, ESRange);      // VB -> DB+1

    // Stretch receptor
    phen(14) = MapSearchParameter(gen(14), -SRmax, 0.0);        // B- class SR weight

    // NMJ Weight
    phen(15) = MapSearchParameter(gen(15), 0.0, NMJmax);       // DB, VBa, VBp
    phen(16) = MapSearchParameter(gen(16), -NMJmax, 0.0);      // DD, VDa, VDp

    // --------------------------------
    // Parameters for the Head circuit
    // --------------------------------
    // Bias
    phen(17) = MapSearchParameter(gen(17), -BiasRange, BiasRange);    // SMDD, SMDV
    phen(18) = MapSearchParameter(gen(18), -BiasRange, BiasRange);    // RMDD, RMDV

    // Time Constant
    phen(19) = MapSearchParameter(gen(19), TauMin, TauMax);           // SMDD, SMDV
    phen(20) = MapSearchParameter(gen(20), TauMin, TauMax);           // RMDD, RMDV

    // Self connections
    phen(21) = MapSearchParameter(gen(21), -SCRange, SCRange);      // SMDD, SMDV
    phen(22) = MapSearchParameter(gen(22), 4.0, SCRange);           // RMDD, RMDV

    // Chemical synapses
    phen(23) = MapSearchParameter(gen(23), -HCSRange, HCSRange);      // SMDD -> SMDV, SMDV -> SMDD
    phen(24) = MapSearchParameter(gen(24), -HCSRange, HCSRange);      // SMDD -> RMDV, SMDV -> RMDD
    phen(25) = MapSearchParameter(gen(25), -HCSRange, HCSRange);      // RMDD -> RMDV, RMDV -> RMDD

    // Gap junctions across class within unit
    phen(26) = MapSearchParameter(gen(26), 0.0, ESRange);      // SMDD - RMDD, SMDV - RMDV
    phen(27) = MapSearchParameter(gen(27), 0.0, ESRange);      // RMDV - RMDD

    // SMD Stretch Receptor
    phen(28) = MapSearchParameter(gen(28), -SRmax, 0.0);        // SMD- class SR weight

    // NMJ Weight
    phen(29) = MapSearchParameter(gen(29), 0.0, NMJmax);    // SMDD, SMDV
    phen(30) = MapSearchParameter(gen(30), 0.0, NMJmax);    // RMDD, RMDV
}
