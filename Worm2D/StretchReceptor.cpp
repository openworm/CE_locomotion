
#include "StretchReceptor.h"
#include "Worm2DCE.h"
#include "WormRS18.h"


void SR::setFromBody(const WormBody & b)
{
    //activity of 50 segments, d and v
    for(int i = 1; i <= nsegs; ++i){
    const double ds = (b.DorsalSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
    const double vs = (b.VentralSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
   
    nslD[i-1] = transformSegs(ds);
    nslV[i-1] = transformSegs(vs);
    
    }

}

void SR::updateNS(const vector<toFromWeight> & seg_, const vector<double> & sr_, NSForW2D & ns_)
{
    for (int i=0;i<seg_.size();i++){
    const toFromWeight & tfw = seg_[i];
    ns_.IncNeuronExternalInput(tfw.to, tfw.w.weight*sr_[tfw.w.from-1]);
}

}


vector<double> SR::updateSegs1(const vector<toFromWeight> & seg_, vector<double> & nsl_)
{
    //from segs to streatch receptors
    vector<double> sr(srvars_ptr->nstretch,0.0);

    for (int i=0;i<seg_.size();i++){
    const toFromWeight & tfw = seg_[i];
    sr[tfw.to-1] += tfw.w.weight*nsl_[tfw.w.from-1];
    }

    return sr;

}

void SR::updateSegs2(const vector<toFromWeight> & seg_, vector<double> & nsl_, vector<double> & sr)
{
    //from segs to streatch receptors
    //vector<double> sr(srvars_ptr->nstretch,0.0);
    std::fill(sr.begin(), sr.end(), 0);

    for (int i=0;i<seg_.size();i++){
    const toFromWeight & tfw = seg_[i];
    sr[tfw.to-1] += tfw.w.weight*nsl_[tfw.w.from-1];
    }


}




void SRCE::incNS(NSForW2D & ns_)
{

updateNS(nssrweights.segToA_D, srvars->A_D_sr, ns_);
updateNS(nssrweights.segToA_V, srvars->A_V_sr, ns_);
updateNS(nssrweights.segToB_D, srvars->B_D_sr, ns_);
updateNS(nssrweights.segToB_V, srvars->B_V_sr, ns_);

}

void SR18::incNS(NSForW2D & ns_)
{

updateNS(nssrweights.segToD, srvars->D_sr, ns_);
updateNS(nssrweights.segToV, srvars->V_sr, ns_);


}


void SR18::updateSegs()
{   
    
    updateSegs2(srweights.segToD, nslD, srvars->D_sr);
    updateSegs2(srweights.segToV, nslV, srvars->V_sr);
    
}

void SRCE::updateSegs()
{   
    vector<double>  nslDA = multiply(nslD, SR_A_gain);
    vector<double>  nslDB = multiply(nslD, SR_B_gain);
    vector<double>  nslVA = multiply(nslV, SR_A_gain);
    vector<double>  nslVB = multiply(nslV, SR_B_gain);

    {vector<double> vec = updateSegs1(srweights.segToA_D, nslDA);
    srvars->A_D_sr.swap(vec);}
    {vector<double> vec = updateSegs1(srweights.segToA_V, nslVA);
    srvars->A_V_sr.swap(vec);}
    {vector<double> vec = updateSegs1(srweights.segToB_D, nslDB);
    srvars->B_D_sr.swap(vec);}
    {vector<double> vec = updateSegs1(srweights.segToB_V, nslVB);
    srvars->B_V_sr.swap(vec);}

}


void SR::addParsToJson(json & j) const
{
    j["stretch_receptor"]["type"]["value"] = SRType;
    j["Stretch receptor"]["Type"]["value"] = SRType;
    if (!j["stretch_receptor"].contains("set_direct"))
        j["stretch_receptor"]["set_direct"]["value"] = false;
    
}



void SR18::addParsToJson(json & j) const
{

    assert(j.contains("nervous_system"));
    //cout << j["nervous_system"] << endl;

    assert(j["nervous_system"].contains("cell_names"));
    vector<string> names = j["nervous_system"]["cell_names"]["value"].template get< vector<string> >();

    addToFromWeight(j["stretch_receptor"]["d_weights"]["value"], srweights.segToD, "to_sr", "from_seg", "weight");
    j["stretch_receptor"]["d_weights"]["message"] = "Weights from body segments to dorsal SR";
    addToFromWeight(j["stretch_receptor"]["v_weights"]["value"], srweights.segToV, "to_sr", "from_seg", "weight");
    j["stretch_receptor"]["v_weights"]["message"] = "Weights from body segments to ventral SR";

    addToFromWeight(j["stretch_receptor"]["ns_d_weights"]["value"], nssrweights.segToD, 
        "to_ns", "from_sr", "weight", names);
    j["stretch_receptor"]["ns_d_weights"]["message"] = "Weights from dorsal SR to Nervous System";
    addToFromWeight(j["stretch_receptor"]["ns_v_weights"]["value"], nssrweights.segToV, 
        "to_ns", "from_sr", "weight", names);
    j["stretch_receptor"]["ns_v_weights"]["message"] = "Weights from ventral SR to Nervous System";


    j["stretch_receptor"]["n_segs"]["value"] = nsegs;
    j["stretch_receptor"]["n_stretch"]["value"] = srvars_ptr->nstretch;

    //if (srpars!=nullptr) srpars->addParsToJson(j["Stretch receptor"]);

    j["stretch_receptor"]["sr_vnc_gain"]["value"] = SRvncgain;
    j["stretch_receptor"]["sr_head_gain"]["value"] = SRheadgain;
    j["stretch_receptor"]["sr_vnc_sr"]["value"] = vncsr;
    j["stretch_receptor"]["sr_head_sr"]["value"] = headsr; 
    j["stretch_receptor"]["plot_size"]["value"] = 2 + srvars_ptr->nstretch*3;

    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR D weights"], srweights.segToD);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR V weights"], srweights.segToV);

    j["Stretch receptor"]["SR D weights"]["message"] = "Weights from body segments to dorsal SR";
    j["Stretch receptor"]["SR V weights"]["message"] = "Weights from body segments to ventral SR";

    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR D NS weights"], nssrweights.segToD);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR V NS weights"], nssrweights.segToV);

    j["Stretch receptor"]["SR D NS weights"]["message"] = "Weights from dorsal SR to Nervous System";
    j["Stretch receptor"]["SR V NS weights"]["message"] = "Weights from ventral SR to Nervous System";

    j["Stretch receptor"]["NSegs"]["value"] = nsegs;
    j["Stretch receptor"]["NStretch"]["value"] = srvars_ptr->nstretch;

    j["Stretch receptor"]["SRvncgain"]["value"] = SRvncgain;
    j["Stretch receptor"]["SRheadgain"]["value"] = SRheadgain;
    j["Stretch receptor"]["SRvncsr"]["value"] = vncsr;
    j["Stretch receptor"]["SRheadsr"]["value"] = headsr;
    j["Stretch receptor"]["plot size"]["value"] = 2 + srvars_ptr->nstretch*3;

    SR::addParsToJson(j);
}



void SRCE::addParsToJson(json & j) const
{
    if (
        !j.contains("stretch_receptor")
        || !j.at("stretch_receptor").is_object())
      j["stretch_receptor"] = json::object();
    j["stretch_receptor"].erase("sr_zero_gains_type_evo");
    j["stretch_receptor"].erase("SRZeroGainsTypeEvo");
    j["stretch_receptor"]["sr_zero_gains_type"]["value"] =
        zeroGainsType;
    
    if (j.contains("nervous_system")){
    //cout << j["nervous_system"] << endl;

    assert(j["nervous_system"].contains("cell_names"));
    vector<string> names = j["nervous_system"]["cell_names"]["value"].template get< vector<string> >();

    

    addToFromWeight(j["stretch_receptor"]["a_d_weights"]["value"], srweights.segToA_D, "to_sr", "from_seg", "weight");
    j["stretch_receptor"]["a_d_weights"]["message"] = "Weights from body segments to dorsal A SR";
    addToFromWeight(j["stretch_receptor"]["a_v_weights"]["value"], srweights.segToA_V, "to_sr", "from_seg", "weight");
    j["stretch_receptor"]["a_v_weights"]["message"] = "Weights from body segments to ventral A SR";
    addToFromWeight(j["stretch_receptor"]["b_d_weights"]["value"], srweights.segToB_D, "to_sr", "from_seg", "weight");
    j["stretch_receptor"]["b_d_weights"]["message"] = "Weights from body segments to dorsal B SR";
    addToFromWeight(j["stretch_receptor"]["b_v_weights"]["value"], srweights.segToB_V, "to_sr", "from_seg", "weight");
    j["stretch_receptor"]["b_v_weights"]["message"] = "Weights from body segments to ventral B SR";

    addToFromWeight(j["stretch_receptor"]["ns_a_d_weights"]["value"], nssrweights.segToA_D, 
        "to_ns", "from_sr", "weight", names);
    j["stretch_receptor"]["ns_a_d_weights"]["message"] = "Weights from dorsal A SR to Nervous System";
    addToFromWeight(j["stretch_receptor"]["ns_a_v_weights"]["value"], nssrweights.segToA_V, 
        "to_ns", "from_sr", "weight", names);
    j["stretch_receptor"]["ns_a_v_weights"]["message"] = "Weights from ventral A SR to Nervous System";
    addToFromWeight(j["stretch_receptor"]["ns_b_d_weights"]["value"], nssrweights.segToB_D, 
        "to_ns", "from_sr", "weight", names);
    j["stretch_receptor"]["ns_b_d_weights"]["message"] = "Weights from dorsal B SR to Nervous System";
    addToFromWeight(j["stretch_receptor"]["ns_b_v_weights"]["value"], nssrweights.segToB_V, 
        "to_ns", "from_sr", "weight", names);
    j["stretch_receptor"]["ns_b_v_weights"]["message"] = "Weights from ventral B SR to Nervous System";


    j["stretch_receptor"]["n_segs"]["value"] = nsegs;
    j["stretch_receptor"]["n_stretch"]["value"] = srvars_ptr->nstretch;

    //if (srpars!=nullptr) srpars->addParsToJson(j["Stretch receptor"]);

    j["stretch_receptor"]["sr_a_gain"]["value"] = SR_A_gain;
    j["stretch_receptor"]["sr_b_gain"]["value"] = SR_B_gain;

    j["stretch_receptor"]["plot_size"]["value"] = srvars_ptr->nstretch*4;

    }

    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR A D weights"], srweights.segToA_D);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR A V weights"], srweights.segToA_V);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR B D weights"], srweights.segToB_D);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR B V weights"], srweights.segToB_V);


    j["Stretch receptor"]["SR A D weights"]["message"] = "Weights from body segments to dorsal A SR";
    j["Stretch receptor"]["SR A V weights"]["message"] = "Weights from body segments to ventral A SR";
    j["Stretch receptor"]["SR B D weights"]["message"] = "Weights from body segments to dorsal B SR";
    j["Stretch receptor"]["SR B V weights"]["message"] = "Weights from body segments to ventral B SR";

    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR A D NS weights"], nssrweights.segToA_D);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR A V NS weights"], nssrweights.segToA_V);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR B D NS weights"], nssrweights.segToB_D);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR B V NS weights"], nssrweights.segToB_V);

    j["Stretch receptor"]["SR A D NS weights"]["message"] = "Weights from dorsal A SR to Nervous System";
    j["Stretch receptor"]["SR A V NS weights"]["message"] = "Weights from ventral A SR to Nervous System";
    j["Stretch receptor"]["SR B D NS weights"]["message"] = "Weights from dorsal B SR to Nervous System";
    j["Stretch receptor"]["SR B V NS weights"]["message"] = "Weights from ventral B SR to Nervous System";


    j["Stretch receptor"]["NSegs"]["value"] = nsegs;
    j["Stretch receptor"]["NStretch"]["value"] = srvars_ptr->nstretch;

    //if (srpars!=nullptr) srpars->addParsToJson(j["Stretch receptor"]);


    j["Stretch receptor"]["SR_A_gain"]["value"] = SR_A_gain;
    j["Stretch receptor"]["SR_B_gain"]["value"] = SR_B_gain;




    j["Stretch receptor"]["plot size"]["value"] = srvars_ptr->nstretch*4;

    SR::addParsToJson(j);

}


void SRCE::setParsFromJson(const json & j) 
{
    if (j.contains("stretch_receptor")
        && j["stretch_receptor"].contains("sr_zero_gains_type"))
    {
        zeroGainsType =
            j["stretch_receptor"]["sr_zero_gains_type"]["value"].get<int>();
    }
    else if (j.contains("Stretch receptor")
        && j["Stretch receptor"].contains("SRZeroGainsType"))
    {
        zeroGainsType =
            j["Stretch receptor"]["SRZeroGainsType"]["value"].get<int>();
    }

    int commandLineValue;
    if (basePar1->getCmdVal<int>(
            "sr_zero_gains_type", commandLineValue
        )
        || basePar1->getCmdVal<int>(
            "SRZeroGainsType", commandLineValue
        ))
    {
        zeroGainsType = commandLineValue;
    }
    basePar1->setValCJ<int>(
        "sr_zero_gains_type", zeroGainsType, "stretch_receptor"
    );
    

    //double sSR_A_gain, sSR_B_gain;
    //vector<toFromWeight> segToA_D, segToA_V, segToB_D, segToB_V, nsegToA_D, nsegToA_V, nsegToB_D, nsegToB_V;

    //if (false)
    if (j.contains("stretch_receptor"))
    {

        assert(j.contains("nervous_system"));
        const json& j2 = j["nervous_system"];

        vector<string> names = j2.at("cell_names").at("value").get<vector<string> >();

        //cout << j2.at("cell_names").at("value") << endl;

        unordered_map<string, int> name_index;
        for (std::size_t i = 0; i < names.size(); ++i) name_index[names[i]] = static_cast<int>(i) + 1;
    
        //basePar1->getValCJ<double>("sr_a_gain",SR_A_gain,"stretch_receptor");
        //basePar1->getValCJ<double>("sr_b_gain",SR_B_gain,"stretch_receptor");

        SR_A_gain = j["stretch_receptor"]["sr_a_gain"]["value"].get<double>();
        SR_B_gain = j["stretch_receptor"]["sr_b_gain"]["value"].get<double>();

        if (j["stretch_receptor"].contains("set_direct") 
        && j["stretch_receptor"].at("set_direct").at("value").get<bool>()
        && j["stretch_receptor"].contains("a_d_weights")){

      
         
            SRWeights srw, nsrw;

        nsrw.segToA_D = getToFromWeightVec(j.at("stretch_receptor").at("ns_a_d_weights").at("value"),
        "to_ns", "from_sr", "weight", name_index);
        nsrw.segToA_V = getToFromWeightVec(j.at("stretch_receptor").at("ns_a_v_weights").at("value"),
        "to_ns", "from_sr", "weight", name_index);
        nsrw.segToB_D = getToFromWeightVec(j.at("stretch_receptor").at("ns_b_d_weights").at("value"),
        "to_ns", "from_sr", "weight", name_index);
        nsrw.segToB_V = getToFromWeightVec(j.at("stretch_receptor").at("ns_b_v_weights").at("value"),
        "to_ns", "from_sr", "weight", name_index);

        srw.segToA_D = getToFromWeightVec(j.at("stretch_receptor").at("a_d_weights").at("value"),
        "to_sr", "from_seg", "weight");
        srw.segToA_V = getToFromWeightVec(j.at("stretch_receptor").at("a_v_weights").at("value"),
        "to_sr", "from_seg", "weight");
        srw.segToB_D = getToFromWeightVec(j.at("stretch_receptor").at("b_d_weights").at("value"),
        "to_sr", "from_seg", "weight");
        srw.segToB_V = getToFromWeightVec(j.at("stretch_receptor").at("b_v_weights").at("value"),
        "to_sr", "from_seg", "weight");   
  
         nssrweights.swapAll(nsrw);
         srweights.swapAll(srw);

       

    }
    else{
        
    makeSRWeights(); 
    makeNSSRWeights(); 

    }

    

    }

    else
    {

        assert(0);
    
    SR_A_gain = j["Stretch receptor"]["SR_A_gain"]["value"].get<double>();
    SR_B_gain = j["Stretch receptor"]["SR_B_gain"]["value"].get<double>();


    if (j["Stretch receptor"].contains("SR A D NS weights")){

        SRWeights srw, nsrw;


    nsrw.segToA_D = 
    j["Stretch receptor"]["SR A D NS weights"]["value"].template get< vector<toFromWeight> >();
    nsrw.segToA_V = 
    j["Stretch receptor"]["SR A V NS weights"]["value"].template get< vector<toFromWeight> >();
    nsrw.segToB_D = 
    j["Stretch receptor"]["SR B D NS weights"]["value"].template get< vector<toFromWeight> >();
    nsrw.segToB_V = 
    j["Stretch receptor"]["SR B V NS weights"]["value"].template get< vector<toFromWeight> >();

    srw.segToA_D = 
    j["Stretch receptor"]["SR A D weights"]["value"].template get< vector<toFromWeight> >();
    srw.segToA_V = 
    j["Stretch receptor"]["SR A V weights"]["value"].template get< vector<toFromWeight> >();
    srw.segToB_D = 
    j["Stretch receptor"]["SR B D weights"]["value"].template get< vector<toFromWeight> >();
    srw.segToB_V = 
    j["Stretch receptor"]["SR B V weights"]["value"].template get< vector<toFromWeight> >();

    nssrweights.swapAll(nsrw);
    srweights.swapAll(srw);

    }
    
    else{
        
    makeSRWeights(); 
    makeNSSRWeights(); 

    }

    }   

    if (true){
    /* compareTFWV(srweights.segToA_D, segToA_D, "a");
    compareTFWV(srweights.segToA_V, segToA_V, "b");
    compareTFWV(srweights.segToB_D, segToB_D, "c");
    compareTFWV(srweights.segToB_V, segToB_V, "d");
    compareTFWV(nssrweights.segToA_D, nsegToA_D, "e");
    compareTFWV(nssrweights.segToA_V, nsegToA_V, "f");
    compareTFWV(nssrweights.segToB_D, nsegToB_D, "g");
    compareTFWV(nssrweights.segToB_V, nsegToB_V, "h");
    cout << sSR_A_gain << " " << SR_A_gain << endl;
    cout << sSR_B_gain << " " << SR_B_gain << endl;
    assert(sSR_A_gain==SR_A_gain);
    assert(sSR_B_gain==SR_B_gain); */
    }

    SR::setParsFromJson(j); 
    return;
    
}




void SR::setParsFromJson(const json & j) 
{   
    //if (false)
    if (j.contains("stretch_receptor"))
    SRType = j["stretch_receptor"]["type"]["value"];
    else SRType = j["Stretch receptor"]["Type"]["value"];
}



void SR18::setParsFromJson(const json & j) 
{

    if (j.contains("stretch_receptor"))
    //if (false)
    {

        assert(j.contains("nervous_system"));
        const json& j3 = j["nervous_system"];

        vector<string> names = j3.at("cell_names").at("value").get<vector<string> >();

        //cout << j3.at("cell_names").at("value") << endl;

        unordered_map<string, int> name_index;
        for (std::size_t i = 0; i < names.size(); ++i) name_index[names[i]] = static_cast<int>(i) + 1;
    

        const json & j2 =  j["stretch_receptor"]; //change to at
        SRvncgain   = j2["sr_vnc_gain"]["value"].get<double>();
        SRheadgain = j2["sr_head_gain"]["value"].get<double>();
        vncsr = j2["sr_vnc_sr"]["value"].get<bool>();
        headsr = j2["sr_head_sr"]["value"].get<bool>();

        SR::setParsFromJson(j); 
    

    /* if (j2["sr_vnc_gain"].contains("evotag") || j2["sr_head_gain"].contains("evotag"))
    {

    //cout << "SR18 set from json " << SRvncgain << " " << SRheadgain << " " << vncsr << " " << headsr << endl;
    makeSRWeights(); 
    makeNSSRWeights(); 

    }else{ */

    if (j2.contains("set_direct") && j2.at("set_direct").at("value").get<bool>() && j2.contains("ns_d_weights")){
        SRWeightsSimp srw, nsrw;

        nsrw.segToD = getToFromWeightVec(j2.at("ns_d_weights").at("value"),
        "to_ns", "from_sr", "weight", name_index);
        srw.segToD = getToFromWeightVec(j2.at("d_weights").at("value"),
        "to_sr", "from_seg", "weight");
        nsrw.segToV = getToFromWeightVec(j2.at("ns_v_weights").at("value"),
        "to_ns", "from_sr", "weight", name_index);
        srw.segToV = getToFromWeightVec(j2.at("v_weights").at("value"),
        "to_sr", "from_seg", "weight");

        nssrweights.swapAll(nsrw);
        srweights.swapAll(srw);

    }else{

    makeSRWeights(); 
    makeNSSRWeights();

    }

    

    return;
    }
    
    {
    const json & j2 =  j["Stretch receptor"];

    SRvncgain = j2["SRvncgain"]["value"];
    SRheadgain = j2["SRheadgain"]["value"];
    vncsr = j2["SRvncsr"]["value"];
    headsr = j2["SRheadsr"]["value"];

    //cout << "SR18 set from json init " << SRvncgain << " " << SRheadgain << " " << vncsr << " " << headsr << endl;

    SR::setParsFromJson(j); 

    if (j2["SRvncgain"].contains("evolvable") || j2["SRheadgain"].contains("evolvable"))
    {

    //cout << "SR18 set from json " << SRvncgain << " " << SRheadgain << " " << vncsr << " " << headsr << endl;
    makeSRWeights(); 
    makeNSSRWeights(); 

    }
    else{

    if (j2.contains("SR D NS weights")){
    nssrweights.segToD = 
    j2["SR D NS weights"]["value"].template get< vector<toFromWeight> >();
    nssrweights.segToV = 
    j2["SR V NS weights"]["value"].template get< vector<toFromWeight> >();

    srweights.segToD = 
    j2["SR D weights"]["value"].template get< vector<toFromWeight> >();
    srweights.segToV = 
    j2["SR V weights"]["value"].template get< vector<toFromWeight> >();

    }else{

    makeSRWeights(); 
    makeNSSRWeights(); 

    }


    }
   
}
}







void SRCE::writeAct(ofstream & ofs)
{

    for (int i = 1; i <= srvars_ptr->nstretch; i++) 
      //ofs <<  " " << sr_ptr->A_D_sr(i) << " " << sr_ptr->A_V_sr(i) << " " << sr_ptr->B_D_sr(i) << " " << sr_ptr->B_V_sr(i);
      ofs <<  " " << srvars->A_D_sr[i-1] << " " << srvars->A_V_sr[i-1] << " " 
      << srvars->B_D_sr[i-1] << " " << srvars->B_V_sr[i-1];

}

void SR18::writeAct(ofstream & ofs)
{
 
    ofs <<  " " << HeadDorsalOutput() << " " << HeadVentralOutput();
        for (int i = 1; i <= srvars_ptr->nstretch; i++) 
            ofs <<  " " << VCDorsalOutput(i) << " " 
            << VCVentralAOutput(i) << " " << VCVentralPOutput(i);

}


void SR18::makeNSSRWeights()
{
/* 
     for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB,i), sr.VCDorsalOutput(i));
        n_ptr->SetNeuronExternalInput(nn(VBA,i), sr.VCVentralAOutput(i));
        n_ptr->SetNeuronExternalInput(nn(VBP,i), sr.VCVentralPOutput(i));
to = 1 + srvars_ptr->nstretch + i */

    SRWeightsSimp srw;

    //cout << "SR18 sr" << vncsr << " " << headsr << endl;

    //const Worm18 & w_ptr = dynamic_cast<const Worm18&>(w_ptr_);

    if (vncsr){
    for (int i = 1; i <= N_units; i++){
    {int from = i + 1;
    {int to = nn1(DB,i,N_neuronsperunit);
    toFromWeight tfw({from,1.0},to);
    srw.segToD.push_back(tfw);}
    {int to = nn1(VBA,i,N_neuronsperunit);
    toFromWeight tfw({from,1.0},to);
    srw.segToV.push_back(tfw);}
    }
    int from = 1 + srvars_ptr->nstretch + i;
    {int to = nn1(VBP,i,N_neuronsperunit);
    toFromWeight tfw({from,1.0},to);
    srw.segToV.push_back(tfw);}
    }
    }


    if (headsr){

        //n_ptr->SetNeuronExternalInput(SMDD, sr.HeadDorsalOutput());    // Average of first
        //n_ptr->SetNeuronExternalInput(SMDV, sr.HeadVentralOutput()); 
        int from = 1;
        {int to = SMDD;
        toFromWeight tfw({from,1.0},to);
        srw.segToD.push_back(tfw);}
        {int to = SMDV;
        toFromWeight tfw({from,1.0},to);
        srw.segToV.push_back(tfw);}


    }
    nssrweights.swapAll(srw);


}




void SRCE::makeNSSRWeights() 
{

    

    SRWeights srw;
    //const Worm2DCE & w_ptr = dynamic_cast<const Worm2DCE&>(w_ptr_);

    //shared_ptr<const Worm2DCE> w_ptr = dynamic_pointer_cast<const Worm2DCE>(w_ptr_);

    //cout << "ssd " << w_ptr->par1.N_units << " ds " << w_ptr->DA << endl;
    //assert(0);


for (int i = 1; i <= N_units; i++){
    int from = i;
    {
    int to = nn1(DA,i,N_neuronsperunit);
    toFromWeight tfw({from,1.0},to);
    srw.segToA_D.push_back(tfw);
    }
    {
    int to = nn1(VA,i,N_neuronsperunit);
    toFromWeight tfw({from,1.0},to);
    srw.segToA_V.push_back(tfw);
    }
    {
    int to = nn1(DB,i,N_neuronsperunit);
    toFromWeight tfw({from,1.0},to);
    srw.segToB_D.push_back(tfw);
    }
    {
    int to = nn1(VB,i,N_neuronsperunit);
    toFromWeight tfw({from,1.0},to);
    srw.segToB_V.push_back(tfw);
    }
    
}
    nssrweights.swapAll(srw);
    
    


//return srw;

}


void SR18::makeSRWeights()
{


    //cout << "SR18 gain" << SRheadgain << " " << SRvncgain << endl;


    //weights from 50 segs to stretch receptors
    SRWeightsSimp srw;

    for (int j = NSEGSHEADSTART; j < NSEGSHEADSTART + NSEGSHEAD; j++){
        int from = j, to = 1;
        double weight = SRheadgain/NSEGSHEAD;
        toFromWeight tfw({from,weight},to);
        srw.segToD.push_back(tfw);
        srw.segToV.push_back(tfw);
    }

    for (int i = 1; i <= 6; i++){

        for (int j = 1; j <= NSEGSSR; j++){
            
            {int from = j+((i-1)*NSEGSSR)+NSEGSVNCSTART-1, to = 1 + i;
            double weight = SRvncgain/NSEGSSR;
            toFromWeight tfw({from,weight},to);
            srw.segToD.push_back(tfw);
            srw.segToV.push_back(tfw);}

            {double weight = SRvncgain/NSEGSSR;
            int from = j+((i-1)*NSEGSSR)+NSEGSVNCSTART-1+2, to = 1 + srvars_ptr->nstretch + i;
            toFromWeight tfw({from,weight},to);
            srw.segToV.push_back(tfw);}

            }


        }

    srweights.swapAll(srw);

}

void SRCE::makeSRWeights()
{

    SRWeights srw;

    int SRForm, nsegperstr;
    basePar1->getValCJ<int>("sr_form",SRForm,"Stretch Receptor");
    basePar1->getValCJ<int>("sr_seg_per_sr",nsegperstr,"Stretch Receptor");

    if (SRForm == 0){
    for (int j = 1; j <= nsegperstr; j++){
        int from = j, to = 1;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
    for (int i = 2; i <= 10; i++)
         for (int j = 1; j <= nsegperstr; j++){
        int from = j+(i-2)*4, to = i;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }
    
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= nsegperstr; j++){
        int from = 12+j+(i-1)*4, to = i;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }

    for (int j = 1; j <= nsegperstr; j++){
        int from = j + 44, to = 10;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
    }

}

    if (SRForm == 1){
  
    for (int i = 1; i <= 9; i++)   
        for (int j = 1; j <= nsegperstr; j++)
        {
        int from = 12+j+(i-1)*4, to = i;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }

//    // Unit 10 (tail), receive same input as Unit 9

    for (int j = 1; j <= nsegperstr; j++){
    int from = j+44, to = 10;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
   
    
//    //////////////////////////////
//    // B-class Stretch Receptors
//    // first unit (head) receive same input as Unit 2

    for (int j = 1; j <= nsegperstr; j++){
        int from = j, to = 1;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
    }
    

//    // Units 2 to 10 

    for (int i = 2; i <= 10; i++)
        for (int j = 1; j <= nsegperstr; j++)
        {
        int from = j+(i-2)*4, to = i;
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }

}
    srweights.swapAll(srw);
    

//return srw;

}


void SRReg::makeSRWeights()
{

    double full_len = nsegs/srvars_ptr->nstretch ;//+ 1;
    //const int half_len = (int) (nsegs/(2*srvars.nstretch));
    
    
    int SRForm, nsegperstr;
    basePar1->getValCJ<int>("sr_form",SRForm,"Stretch Receptor");
    basePar1->getValCJ<int>("sr_seg_per_sr",nsegperstr,"Stretch Receptor");
    int offset;
    basePar1->getValCJ<int>("sr_offset",offset,"Stretch Receptor");

    SRWeights srw;

   for (int i = 1; i <= srvars_ptr->nstretch; i++){
 
    double midpoint = full_len*(i-0.5); 
    int start = (int) (midpoint - (nsegperstr/2.0));
    int end = (int) (midpoint + (nsegperstr/2.0));

    for (int j = start + 1; j< end + 1; j++)
       
   //for (int j = (i-1)*full_len - half_len + 1; j <= (i-1)*full_len + half_len + 1; j++)
    //for (int j = (i-1)*srcepars->nsegperstr + 1; j <= i*srcepars->nsegperstr; j++)
    {
        {int from = j-offset, to = i;
        if (from>0 && from<=nsegs){
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }}
        {int from = j+offset, to = i;
        if (from<=nsegs && from>0){
        double weight = 1.0/nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }}
    }

}

   //cout << " nsegperstr " << srcepars->nsegperstr << " " << srregpars->offset << endl;
   
//return srw;
 
srweights.swapAll(srw);

}


double SRCE::transformSegs(const double & val){

    double val1 = val;

    


    if (sr_type == "SR_TRANS_STRETCH")
    {
    val1 = val < 0.0 ? 0.0 : val;
    }
    else if (sr_type == "SR_TRANS_CONTRACT")
    {
    val1 = val < 0.0 ? val : 0.0;
    }
    else if (sr_type == "SR_TRANS_ABS")
    {
    val1 = val < 0.0 ? -val : val;
    }
    else if (sr_type == "SR_TRANS_NEG")
    {
    val1 = -val;
    }

return val1;

}
