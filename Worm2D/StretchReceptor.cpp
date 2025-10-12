
#include "StretchReceptor.h"
#include "Worm2DCE.h"

void SR::setFromBody(const WormBody & b)
{
    for(int i = 1; i <= nsegs; ++i){
    const double ds = (b.DorsalSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
    const double vs = (b.VentralSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
    nslD[i-1] = transformSegs(ds);
    nslV[i-1] = transformSegs(vs);
    }

}

void SR::incNS(NSForW2D & ns_)
{

updateNS(nssrweights.segToA_D, srvars.A_D_sr, ns_);
updateNS(nssrweights.segToA_V, srvars.A_V_sr, ns_);
updateNS(nssrweights.segToB_D, srvars.B_D_sr, ns_);
updateNS(nssrweights.segToB_V, srvars.B_V_sr, ns_);

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
    vector<double> sr(srvars.nstretch,0.0);

    for (int i=0;i<seg_.size();i++){
    const toFromWeight & tfw = seg_[i];
    sr[tfw.to-1] += tfw.w.weight*nsl_[tfw.w.from-1];
    }

    return sr;

}

void SR::updateSegs()
{
    {vector<double> vec = updateSegs1(srweights.segToA_D, nslD);
    srvars.A_D_sr.swap(vec);}
    {vector<double> vec = updateSegs1(srweights.segToA_V, nslV);
    srvars.A_V_sr.swap(vec);}
    {vector<double> vec = updateSegs1(srweights.segToB_D, nslD);
    srvars.B_D_sr.swap(vec);}
    {vector<double> vec = updateSegs1(srweights.segToB_V, nslV);
    srvars.B_V_sr.swap(vec);}

}




void SR::addParsToJson(json & j)
{

    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR A D"]["weights"], srweights.segToA_D);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR A V"]["weights"], srweights.segToA_V);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR B D"]["weights"], srweights.segToB_D);
    appendVectorToJson<toFromWeight>(j["Stretch receptor"]["SR B V"]["weights"], srweights.segToB_V);


    j["Stretch receptor"]["NSegs"]["value"] = nsegs;
    j["Stretch receptor"]["NStretch"]["value"] = srvars.nstretch;

}


void SRCE::addParsToJson(json & j)
{

    srcepars->addParsToJson(j["Stretch receptor"]);
    SR::addParsToJson(j);
    //j["Stretch receptor"]["NSegsforanSR"]["value"] = srcepars->nsegperstr;
    //j["Stretch receptor"]["SR Form"]["value"] = SRForm;

    j["Stretch receptor"]["SR_A_gain"]["value"] = SR_A_gain;
    j["Stretch receptor"]["SR_B_gain"]["value"] = SR_B_gain;
}

void SRCE::setParsFromJson(json & j)
{

    srcepars->setParsFromJson(j["Stretch receptor"]);
    //SR::addParsToJson(j);
    //srcepars->nsegperstr = j["Stretch receptor"]["NSegsforanSR"]["value"];
    //j["Stretch receptor"]["SR Form"]["value"] = SRForm;

    SR_A_gain = j["Stretch receptor"]["SR_A_gain"]["value"];
    SR_B_gain = j["Stretch receptor"]["SR_B_gain"]["value"];


}

SRWeights SRCE::makeNSSRWeights(const Worm2Dbase & w_ptr_) const
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

    //n_ptr->SetNeuronExternalInput(nn(DA,i), sr_ptr->srvars.A_D_sr[i-1] + W2DCEpars1->AVA_output*W2DCEpars1->AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(DA,i), sr_ptr->A_D_sr(i) + AVA_output);
    //n_ptr->SetNeuronExternalInput(nn(VA,i), sr_ptr->srvars.A_V_sr[i-1] + W2DCEpars1->AVA_output*W2DCEpars1->AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(VA,i), sr_ptr->A_V_sr(i) + AVA_output);
  //}
  ////   To B_class motorneurons
  //for (int i = 1; i <= par1.N_units; i++){
    //n_ptr->SetNeuronExternalInput(nn(DB,i), sr_ptr->srvars.B_D_sr[i-1] + W2DCEpars1->AVB_output*W2DCEpars1->AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(VB,i), sr_ptr->srvars.B_V_sr[i-1] + W2DCEpars1->AVB_output*W2DCEpars1->AB_output_level);
    //n_ptr->SetNeuronExternalInput(nn(DB,i), sr_ptr->B_D_sr(i) + AVB_output);
    //n_ptr->SetNeuronExternalInput(nn(VB,i), sr_ptr->B_V_sr(i) + AVB_output);
 // }

return srw;

}

SRWeights SRCE::makeSRWeights() const
{

    SRWeights srw;

    if (srcepars->SRForm == 0){
    for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j, to = 1;
        double weight = SR_A_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
    for (int i = 2; i <= 10; i++)
         for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j+(i-2)*4, to = i;
        double weight = SR_A_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }
    
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = 12+j+(i-1)*4, to = i;
        double weight = SR_B_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }

    for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j + 44, to = 10;
        double weight = SR_B_gain/srcepars->nsegperstr;
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
        double weight = SR_A_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }

//    // Unit 10 (tail), receive same input as Unit 9

    for (int j = 1; j <= srcepars->nsegperstr; j++){
    int from = j+44, to = 10;
        double weight = SR_A_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
   
    
//    //////////////////////////////
//    // B-class Stretch Receptors
//    // first unit (head) receive same input as Unit 2

    for (int j = 1; j <= srcepars->nsegperstr; j++){
        int from = j, to = 1;
        double weight = SR_B_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
    }
    

//    // Units 2 to 10 

    for (int i = 2; i <= 10; i++)
        for (int j = 1; j <= srcepars->nsegperstr; j++)
        {
        int from = j+(i-2)*4, to = i;
        double weight = SR_B_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }

}

return srw;

}


SRWeights SRReg::makeSRWeights() const
{

    double full_len = nsegs/srvars.nstretch ;//+ 1;
    //const int half_len = (int) (nsegs/(2*srvars.nstretch));


    SRWeights srw;

   for (int i = 1; i <= srvars.nstretch; i++){
 
    double midpoint = full_len*(i-0.5); 
    int start = (int) (midpoint - (srcepars->nsegperstr/2.0));
    int end = (int) (midpoint + (srcepars->nsegperstr/2.0));

    for (int j = start + 1; j< end + 1; j++)
       
   //for (int j = (i-1)*full_len - half_len + 1; j <= (i-1)*full_len + half_len + 1; j++)
    //for (int j = (i-1)*srcepars->nsegperstr + 1; j <= i*srcepars->nsegperstr; j++)
    {
        {int from = j-srregpars->offset, to = i;
        if (from>0 && from<=nsegs){
        double weight = SR_B_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }}
        {int from = j+srregpars->offset, to = i;
        if (from<=nsegs && from>0){
        double weight = SR_A_gain/srcepars->nsegperstr;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }}
    }

}

   //cout << " nsegperstr " << srcepars->nsegperstr << " " << srregpars->offset << endl;
   
return srw;

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