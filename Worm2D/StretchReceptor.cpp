
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

    j["Stretch receptor"]["Type"]["value"] = SRType;
}



void SR18::addParsToJson(json & j) const
{

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

    if (srpars!=nullptr) srpars->addParsToJson(j["Stretch receptor"]);


    j["Stretch receptor"]["SR_A_gain"]["value"] = SR_A_gain;
    j["Stretch receptor"]["SR_B_gain"]["value"] = SR_B_gain;




    j["Stretch receptor"]["plot size"]["value"] = srvars_ptr->nstretch*4;

    SR::addParsToJson(j);

}

void SR::setParsFromJson(const json & j) 
{

    SRType = j["Stretch receptor"]["Type"]["value"];
}



void SR18::setParsFromJson(const json & j) 
{
    nssrweights.segToD = 
    j["Stretch receptor"]["SR D NS weights"]["value"].template get< vector<toFromWeight> >();
    nssrweights.segToV = 
    j["Stretch receptor"]["SR V NS weights"]["value"].template get< vector<toFromWeight> >();


    srweights.segToD = 
    j["Stretch receptor"]["SR D weights"]["value"].template get< vector<toFromWeight> >();
    srweights.segToV = 
    j["Stretch receptor"]["SR V weights"]["value"].template get< vector<toFromWeight> >();

    SRvncgain = j["Stretch receptor"]["SRvncgain"]["value"];
    SRheadgain = j["Stretch receptor"]["SRheadgain"]["value"];
    vncsr = j["Stretch receptor"]["SRvncsr"]["value"];
    headsr = j["Stretch receptor"]["SRheadsr"]["value"];

    SR::setParsFromJson(j); 

}

void SRCE::setParsFromJson(const json & j) 
{
    
    SR_A_gain = j["Stretch receptor"]["SR_A_gain"]["value"];
    SR_B_gain = j["Stretch receptor"]["SR_B_gain"]["value"];


    nssrweights.segToA_D = 
    j["Stretch receptor"]["SR A D NS weights"]["value"].template get< vector<toFromWeight> >();
    nssrweights.segToA_V = 
    j["Stretch receptor"]["SR A V NS weights"]["value"].template get< vector<toFromWeight> >();
    nssrweights.segToB_D = 
    j["Stretch receptor"]["SR B D NS weights"]["value"].template get< vector<toFromWeight> >();
    nssrweights.segToB_V = 
    j["Stretch receptor"]["SR B V NS weights"]["value"].template get< vector<toFromWeight> >();

    srweights.segToA_D = 
    j["Stretch receptor"]["SR A D weights"]["value"].template get< vector<toFromWeight> >();
    srweights.segToA_V = 
    j["Stretch receptor"]["SR A V weights"]["value"].template get< vector<toFromWeight> >();
    srweights.segToB_D = 
    j["Stretch receptor"]["SR B D weights"]["value"].template get< vector<toFromWeight> >();
    srweights.segToB_V = 
    j["Stretch receptor"]["SR B V weights"]["value"].template get< vector<toFromWeight> >();

    if (srpars!=nullptr) srpars->setParsFromJson(j["Stretch receptor"]);

   
    SR::setParsFromJson(j); 
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


void SR18::makeNSSRWeights(const Worm2Dbase & w_ptr_)
{
/* 
     for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB,i), sr.VCDorsalOutput(i));
        n_ptr->SetNeuronExternalInput(nn(VBA,i), sr.VCVentralAOutput(i));
        n_ptr->SetNeuronExternalInput(nn(VBP,i), sr.VCVentralPOutput(i));
to = 1 + srvars_ptr->nstretch + i */

    SRWeightsSimp srw;

    const Worm18 & w_ptr = dynamic_cast<const Worm18&>(w_ptr_);

    if (vncsr){
    for (int i = 1; i <= w_ptr.par1.N_units; i++){
    {int from = i + 1;
    {int to = w_ptr.nn(w_ptr.DB,i);
    toFromWeight tfw({from,1.0},to);
    srw.segToD.push_back(tfw);}
    {int to = w_ptr.nn(w_ptr.VBA,i);
    toFromWeight tfw({from,1.0},to);
    srw.segToV.push_back(tfw);}
    }
    int from = 1 + srvars_ptr->nstretch + i;
    {int to = w_ptr.nn(w_ptr.VBP,i);
    toFromWeight tfw({from,1.0},to);
    srw.segToV.push_back(tfw);}
    }
    }


    if (headsr){

        //n_ptr->SetNeuronExternalInput(SMDD, sr.HeadDorsalOutput());    // Average of first
        //n_ptr->SetNeuronExternalInput(SMDV, sr.HeadVentralOutput()); 
        int from = 1;
        {int to = w_ptr.SMDD;
        toFromWeight tfw({from,1.0},to);
        srw.segToD.push_back(tfw);}
        {int to = w_ptr.SMDV;
        toFromWeight tfw({from,1.0},to);
        srw.segToV.push_back(tfw);}


    }
    nssrweights.swapAll(srw);


}




void SRCE::makeNSSRWeights(const Worm2Dbase & w_ptr_) 
{
    SRWeights srw;
    const Worm2DCE & w_ptr = dynamic_cast<const Worm2DCE&>(w_ptr_);

    //shared_ptr<const Worm2DCE> w_ptr = dynamic_pointer_cast<const Worm2DCE>(w_ptr_);

    //cout << "ssd " << w_ptr->par1.N_units << " ds " << w_ptr->DA << endl;
    //assert(0);


for (int i = 1; i <= w_ptr.par1.N_units; i++){
    int from = i;
    {
    int to = w_ptr.nn(w_ptr.DA,i);
    toFromWeight tfw({from,1.0},to);
    srw.segToA_D.push_back(tfw);
    }
    {
    int to = w_ptr.nn(w_ptr.VA,i);
    toFromWeight tfw({from,1.0},to);
    srw.segToA_V.push_back(tfw);
    }
    {
    int to = w_ptr.nn(w_ptr.DB,i);
    toFromWeight tfw({from,1.0},to);
    srw.segToB_D.push_back(tfw);
    }
    {
    int to = w_ptr.nn(w_ptr.VB,i);
    toFromWeight tfw({from,1.0},to);
    srw.segToB_V.push_back(tfw);
    }
    
}
    nssrweights.swapAll(srw);
    
    


//return srw;

}


void SR18::makeSRWeights()
{
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

    if (srcepars->SRForm == 0){
    for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j, to = 1;
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
    for (int i = 2; i <= 10; i++)
         for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j+(i-2)*4, to = i;
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }
    
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = 12+j+(i-1)*4, to = i;
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }

    for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j + 44, to = 10;
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
    }

}

    if (srcepars->SRForm == 1){
  
    for (int i = 1; i <= 9; i++)   
        for (int j = 1; j <= srcepars->nsegperstr; j++)
        {
        int from = 12+j+(i-1)*4, to = i;
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }

//    // Unit 10 (tail), receive same input as Unit 9

    for (int j = 1; j <= srcepars->nsegperstr; j++){
    int from = j+44, to = 10;
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
   
    
//    //////////////////////////////
//    // B-class Stretch Receptors
//    // first unit (head) receive same input as Unit 2

    for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j, to = 1;
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
    }
    

//    // Units 2 to 10 

    for (int i = 2; i <= 10; i++)
        for (int j = 1; j <= srcepars->nsegperstr; j++)
        {
        int from = j+(i-2)*4, to = i;
        double weight = 1.0/srcepars->nsegperstr;
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


    SRWeights srw;

   for (int i = 1; i <= srvars_ptr->nstretch; i++){
 
    double midpoint = full_len*(i-0.5); 
    int start = (int) (midpoint - (srcepars->nsegperstr/2.0));
    int end = (int) (midpoint + (srcepars->nsegperstr/2.0));

    for (int j = start + 1; j< end + 1; j++)
       
   //for (int j = (i-1)*full_len - half_len + 1; j <= (i-1)*full_len + half_len + 1; j++)
    //for (int j = (i-1)*srcepars->nsegperstr + 1; j <= i*srcepars->nsegperstr; j++)
    {
        {int from = j-srregpars->offset, to = i;
        if (from>0 && from<=nsegs){
        double weight = 1.0/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }}
        {int from = j+srregpars->offset, to = i;
        if (from<=nsegs && from>0){
        double weight = 1.0/srcepars->nsegperstr;
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

    if (srcepars->sr_type == "SR_TRANS_STRETCH")
    {
    val1 = val < 0.0 ? 0.0 : val;
    }
    else if (srcepars->sr_type == "SR_TRANS_CONTRACT")
    {
    val1 = val < 0.0 ? val : 0.0;
    }
    else if (srcepars->sr_type == "SR_TRANS_ABS")
    {
    val1 = val < 0.0 ? -val : val;
    }
    else if (srcepars->sr_type == "SR_TRANS_NEG")
    {
    val1 = -val;
    }

return val1;

}