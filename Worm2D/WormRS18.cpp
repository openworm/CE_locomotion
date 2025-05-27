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

// The constructor
Worm18::Worm18(TVector<double> &v,double output):Worm2Dm({6,24,0.1,6,40}, new NervousSystem(), new Muscles),
rS18Macros(setMacros()),n(dynamic_cast<NervousSystem&>(*n_ptr)),Worm2D({6,24,0.1,6,40},0)
{
    //supArgs1.writeMessage();

    // Muscles
   // m.SetMuscleParams(par1.N_muscles, par1.T_muscle);
    

    // Nervous system // Ventral cord
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
        n.SetChemicalSynapseWeight(vba, dd, v(8)/2);
        n.SetChemicalSynapseWeight(vbp, dd, v(8)/2);
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
    NMJ_Gain_Map = 0.5;
    NMJ_Gain.SetBounds(1, par1.N_muscles);
    for (int i=1; i<=par1.N_muscles; i++)
    {
        NMJ_Gain(i) = 0.7*(1.0 - (((i-1)*NMJ_Gain_Map)/par1.N_muscles));
    }

     setUpMuscleConn();
}

void Worm18::InitializeState(RandomState &rs)
{
    
    n.RandomizeCircuitState(-0.5, 0.5, rs);
    /* for (int i = 1; i <= n.size-4; i++)
        n.SetNeuronState(i, (i-0.5)/(n.size-4));
    for (int i = 1; i <= 4; i++)
        n.SetNeuronState(i + n.size-4, (i-0.5)/4); */
    //n.RandomizeCircuitState(0.5, 0.5, rs); //fix initial conditions
    //h.RandomizeCircuitState(-0.5, 0.5, rs);
    Worm2D::InitializeState(rs);
}

/* void Worm18::HeadStep(double StepSize, double output)
{
    // Update Nervous System
    h.EulerStep(StepSize);

    // Time
    t += StepSize;
} */


vector<toFromWeight> Worm18::makeVentralMuscleConn()
{
    vector<toFromWeight> vec1;

    {vector<int> neurons({SMDV, RMDV});
    vector<double> NMJ({NMJ_SMDV, NMJ_RMDD});
    for (int i = 1; i <= HeadMotorNeuronMuscles; i++)
    makeMuscleConnHelp(vec1, neurons, NMJ, 1, i, NMJ_Gain);}

    
        vector<int> neuronsA({VDA, VBA});
        vector<int> neuronsP({VDP, VBP});
        vector<double> NMJA({NMJ_VDa, NMJ_VBa});
        vector<double> NMJP({NMJ_VDp, NMJ_VBp});
        vector<double> NMJA2({NMJ_VDa/2, NMJ_VBa/2});
        vector<double> NMJP2({NMJ_VDp/2, NMJ_VBp/2});
        for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        int mt = (i-VNCMuscleStart)%NmusclePerNU;
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

}

vector<toFromWeight> Worm18::makeDorsalMuscleConn()
{
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


    return vec1;
}


void Worm18::setMuscleInputOrig(double StepSize)
{
   double dorsalHeadInput, ventralHeadInput, ventralHeadInputA, ventralHeadInputP;

    dorsalHeadInput = NMJ_SMDD*n.NeuronOutput(SMDD) + NMJ_RMDV*n.NeuronOutput(RMDD);
    ventralHeadInput = NMJ_SMDV*n.NeuronOutput(SMDV) + NMJ_RMDD*n.NeuronOutput(RMDV);

    for (int i = 1; i <= HeadMotorNeuronMuscles; i++){
        m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalHeadInput);
        m.SetVentralMuscleInput(i, NMJ_Gain(i)*ventralHeadInput);
    }

    // Set input to Muscles from Ventral Cord
    //  Dorsal muscles (each motor neuron innervates three muscles, no overlap)

    for (int i = VNCMuscleStart; i <= par1.N_muscles; i++){
        int mi = (int) ((i-VNCMuscleStart)/NmusclePerNU)+1;
        dorsalHeadInput = NMJ_DD*n.NeuronOutput(nn(DD,mi)) + NMJ_DB*n.NeuronOutput(nn(DB,mi));
        m.SetDorsalMuscleInput(i, NMJ_Gain(i)*dorsalHeadInput);
        ventralHeadInputA = NMJ_VDa*n.NeuronOutput(nn(VDA,mi)) + NMJ_VBa*n.NeuronOutput(nn(VBA,mi));
        ventralHeadInputP = NMJ_VDp*n.NeuronOutput(nn(VDP,mi)) + NMJ_VBp*n.NeuronOutput(nn(VBP,mi));
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
    m.EulerStep(StepSize);



}


void Worm18::Step(double StepSize, double output)
{
   
    double ds, vs;

 

    // Update Body
    b.StepBody(StepSize);

    // Set input to Stretch Receptors from Body
    for(int i = 1; i <= N_segments; ++i){
        ds = (b.DorsalSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
        vs =  (b.VentralSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
        sr.SetDorsalInput(i, ds);
        sr.SetVentralInput(i, vs);
    }

    // Update Stretch Receptors
    sr.Update();

    // Set input to Nervous System (Head) from Stretch Receptors
//#ifdef HEADSR
if (rS18Macros.headsr)
{
    if (output == 1){
        n.SetNeuronExternalInput(SMDD, sr.HeadDorsalOutput());    // Average of first
        n.SetNeuronExternalInput(SMDV, sr.HeadVentralOutput());   // to segments
    }
}
//#endif

    // Set input to Nervous System (Ventral Cord) from Stretch Receptors
//#ifdef VNCSR
if (rS18Macros.vncsr)
{
    for (int i = 1; i <= par1.N_units; i++){
        n.SetNeuronExternalInput(nn(DB,i), sr.VCDorsalOutput(i));
        n.SetNeuronExternalInput(nn(VBA,i), sr.VCVentralAOutput(i));
        n.SetNeuronExternalInput(nn(VBP,i), sr.VCVentralPOutput(i));
    }
}    
//#endif

    // Update Nervous System
    //h.EulerStep(StepSize);
    n.EulerStep(StepSize);

    // Set input to Muscles
    //  Input from the head circuit

    setMuscleInputOrig(StepSize);
    //setMuscleInput(StepSize);

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

const vector<string>  Worm18::getCellNames() 
{
    vector<string> v1 = getCellNamesAll({"DB", "DD", "VBA", "VDA", "VBP", "VDP"}, par1.N_units);
    vector<string> v2 = {"SMDD", "RMDD", "SMDV", "RMDV"};
    v1.insert(v1.end(),v2.begin(),v2.end());
    return v1;
}

void Worm18::addParsToJson(json & j)
{
    //string nsHead = "Head Nervous system";
    //appendAllNSJson(j[nsHead], h);
    //vector<string> cell_names = {"SMDD", "RMDD", "SMDV", "RMDV"};
    //appendCellNamesToJson(j[nsHead], cell_names, 1);

    Params<double> par = sr.getStretchReceptorParams();
    appendToJson<double>(j["Stretch receptor"], par);
    string nsHead = "Nervous system";
    appendAllNSJson(j[nsHead], n);
    Worm2D::addParsToJson(j);
    //string nsHead = "Nervous system";
    appendCellNamesToJson(j[nsHead], getCellNames(), 1);
    //appendCellNamesToJson(j[nsHead], getHeadCellNames(), 1);
}


vector<doubIntParamsHead> Worm18::getWormParams(){

    vector<doubIntParamsHead> parvec;
    doubIntParamsHead var1;

    var1.parDoub.head = "Worm";
    var1.parDoub.names = {"NMJ_DB", "NMJ_VBa", "NMJ_VBp", "NMJ_DD", "NMJ_VDa", "NMJ_VDp",
    "NMJ_SMDD", "NMJ_SMDV", "NMJ_RMDD", "NMJ_RMDV", "NMJ Gain"};
    var1.parDoub.vals = {NMJ_DB, NMJ_VBa, NMJ_VBp, NMJ_DD, NMJ_VDa, NMJ_VDp, 
    NMJ_SMDD, NMJ_SMDV, NMJ_RMDD, NMJ_RMDV, NMJ_Gain_Map};

    var1.parInt.head = "Worm";
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



void Worm18::DumpActState(ofstream &ofs, int skips)
{
    static int tt = skips;

    if (++tt >= skips) {
        tt = 0;

        ofs << t;
        //ofs << "\nSR: ";
        // Stretch receptors
        ofs <<  " " << sr.HeadDorsalOutput() << " " << sr.HeadVentralOutput();
        
        for (int i = 1; i <= N_stretchrec; i++) {
            ofs <<  " " << sr.VCDorsalOutput(i) << " " << sr.VCVentralAOutput(i) << " " << sr.VCVentralPOutput(i);;
        }
        // Head Neurons
        //ofs << "\nH: ";
        int offset = par1.N_units*par1.N_neuronsperunit;

        for (int i = 1; i <= 4; i++) {
            ofs <<  " " << n.NeuronOutput(offset + i);
        }
        // Ventral Cord Motor Neurons
        //ofs << "\nV: ";
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n.NeuronOutput(nn(j,i));
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

void Worm18::DumpVoltage(ofstream &ofs, int skips)
{
    static int tt = skips;

    if (++tt >= skips) {
        tt = 0;

        ofs << t;
        // Head Neurons
        int offset = par1.N_units*par1.N_neuronsperunit;
        for (int i = 1; i <= 4; i++) {
            ofs <<  " " << n.NeuronState(offset + i);
        }
        // Ventral Cord Motor Neurons
        for (int i = 1; i <= par1.N_units; i++) {
            for (int j = 1; j <= par1.N_neuronsperunit; j++) {
                ofs <<  " " << n.NeuronState(nn(j,i));
            }
        }
        ofs << "\n";
    }
}



void Worm18::DumpParams(ofstream &ofs)
{
    ofs << "Time-constants: \n DB: " << n.NeuronTimeConstant(DB) << "\n VBA/P: " << n.NeuronTimeConstant(VBA) << " / " << n.NeuronTimeConstant(VBP) << "\n DD: " << n.NeuronTimeConstant(DD) << "\n VDA/P: " << n.NeuronTimeConstant(VDA) << " / " << n.NeuronTimeConstant(VDP) << endl;
    ofs << "Biases: \n DB: " << n.NeuronBias(DB) << "\n VBA/P: " << n.NeuronBias(VBA) << " / " << n.NeuronBias(VBP)  <<  "\n DD: " << n.NeuronBias(DD) << "\n VDA/P: " << n.NeuronBias(VDA) <<  " / " << n.NeuronBias(VDP) << endl;
    ofs << "Self conns: \n DB: " << n.ChemicalSynapseWeight(DB, DB) << "\n VBA/P: " << n.ChemicalSynapseWeight(VBA, VBA) << " / " << n.ChemicalSynapseWeight(VBP, VBP) << "\n DD: " << n.ChemicalSynapseWeight(DD, DD) <<  "\n VDA/P: " << n.ChemicalSynapseWeight(VDA, VDA) <<  " / " << n.ChemicalSynapseWeight(VDP, VDP) << endl;
    ofs << "Chem Conns: \n DB->DD: " << n.ChemicalSynapseWeight(DB, DD) <<  "\n DB->VDA/VDP: " << n.ChemicalSynapseWeight(DB, VDA) << " / " << n.ChemicalSynapseWeight(DB, VDP) << "\n VBA/P->DD: " << n.ChemicalSynapseWeight(VBA, DD) << " / " << n.ChemicalSynapseWeight(VBP, DD) << "\n VBA/P->VDA/P: " << n.ChemicalSynapseWeight(VBA, VDA) << " / " << n.ChemicalSynapseWeight(VBP, VDP) << "\n VDA/P->VBA/P: " << n.ChemicalSynapseWeight(VDA, VBA) << " / " << n.ChemicalSynapseWeight(VDP, VBP) << "\n DD->VDA: " << n.ChemicalSynapseWeight(DD, VDA) <<endl;
    ofs << "Gap Juncs: \n DB-DB+1: " << n.ElectricalSynapseWeight(DB, DB+par1.N_neuronsperunit) << "\n VBA-VBP / VBP-VBP+1: " << n.ElectricalSynapseWeight(VBA, VBP) << " / " << n.ElectricalSynapseWeight(VBP, VBA+par1.N_neuronsperunit) << "\n VBP-DB+1: " << n.ElectricalSynapseWeight(VBP, DB+par1.N_neuronsperunit) << "\n DD-VDA/P: " << n.ElectricalSynapseWeight(DD, VDA) << " / " << n.ElectricalSynapseWeight(DD, VDP) << "\n DD-DD+1: " << n.ElectricalSynapseWeight(DD, DD+par1.N_neuronsperunit) << "\n VDA-VDP / VDP-VDP+1: " << n.ElectricalSynapseWeight(VDA, VDP) << " / " << n.ElectricalSynapseWeight(VDP, VDA+par1.N_neuronsperunit) <<  endl;
    ofs << "SR Gain (VNC and Head): " << sr.SRvncgain << " " << sr.SRheadgain << endl;
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
