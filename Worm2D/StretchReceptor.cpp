
#include "StretchReceptor.h"

void SR::setFromBody(const WormBody & b)
{
    for(int i = 1; i <= nsegs; ++i){
    const double ds = (b.DorsalSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
    const double vs = (b.VentralSegmentLength(i) - b.RestingLength(i))/b.RestingLength(i);
    nslD[i-1] = ds;
    nslV[i-1] = vs;
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

    SR::addParsToJson(j);
    j["Stretch receptor"]["NSegsforanSR"]["value"] = NSEGSSR;
    j["Stretch receptor"]["SR Form"]["value"] = SRForm;

    j["Stretch receptor"]["SR_A_gain"]["value"] = SR_A_gain;
    j["Stretch receptor"]["SR_B_gain"]["value"] = SR_B_gain;
}


SRWeights SRCE::makeSRWeights() const
{

    SRWeights srw;

    if (SRForm == 0){
    for (int j = 1; j <= NSEGSSR; j++){
        int from = j, to = 1;
        double weight = SR_A_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
    for (int i = 2; i <= 10; i++)
         for (int j = 1; j <= NSEGSSR; j++){
        int from = j+(i-2)*4, to = i;
        double weight = SR_A_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }
    
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= NSEGSSR; j++){
        int from = 12+j+(i-1)*4, to = i;
        double weight = SR_B_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }

    for (int j = 1; j <= NSEGSSR; j++){
        int from = j + 44, to = 10;
        double weight = SR_B_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
    }

}

    if (SRForm == 1){
  
    for (int i = 1; i <= 9; i++)   
        for (int j = 1; j <= NSEGSSR; j++)
        {
        int from = 12+j+(i-1)*4, to = i;
        double weight = SR_A_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
        }

//    // Unit 10 (tail), receive same input as Unit 9

    for (int j = 1; j <= NSEGSSR; j++){
    int from = j+44, to = 10;
        double weight = SR_A_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToA_D.push_back(tfw);
        srw.segToA_V.push_back(tfw);
    }
   
    
//    //////////////////////////////
//    // B-class Stretch Receptors
//    // first unit (head) receive same input as Unit 2

    for (int j = 1; j <= NSEGSSR; j++){
        int from = j, to = 1;
        double weight = SR_B_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
    }
    

//    // Units 2 to 10 

    for (int i = 2; i <= 10; i++)
        for (int j = 1; j <= NSEGSSR; j++)
        {
        int from = j+(i-2)*4, to = i;
        double weight = SR_B_gain/NSEGSSR;
        toFromWeight tfw({from,weight},to);
        srw.segToB_D.push_back(tfw);
        srw.segToB_V.push_back(tfw);
        }

}

return srw;

}
