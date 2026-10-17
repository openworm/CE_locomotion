//
//  StretchReceptor.cpp
//  one
//
//  Created by Eduardo on 9/26/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
// Modified by Erick Olivares, Feb 2019
// Added SR for class A motorneurons

#include "StretchReceptorCE.h"
#include <cassert>


StretchReceptorCE::StretchReceptorCE(int nSegs, int nSR, double ASRgain, double BSRgain)
//:SRCE(nSegs,nSR)
{
    SetStretchReceptorParams(nSegs, nSR, ASRgain, BSRgain);
}

void StretchReceptorCE::SetStretchReceptorParams(int nSegs, int nSR, double ASRgain, double BSRgain)
{
    NSEGS = nSegs;                  // Number of segments
    NSR = nSR;                      // Number of stretch receptors
    NSEGSSR = 6;                    // Number of segments that go into a stretch receptor
    SR_A_gain = ASRgain;                // Stretch receptor gain
    SR_B_gain = BSRgain;                // Stretch receptor gain

    normSegLenD.SetBounds(1, NSEGS);
    normSegLenV.SetBounds(1, NSEGS);
    A_D_sr.SetBounds(1, NSR);
    A_V_sr.SetBounds(1, NSR);
    B_D_sr.SetBounds(1, NSR);
    B_V_sr.SetBounds(1, NSR);
}

Params<double> StretchReceptorCE::getStretchReceptorParams()
{
Params<double> par;
par.names = {"NSR", "NSEGS", "NSEGSSR", "SR_A_gain", "SR_B_gain"};
par.vals = {NSR,NSEGS,NSEGSSR,SR_A_gain,SR_B_gain};
par.messages = {"Number of stretch receptor in DA, DB, VA and VB, equal to number of units",
                                "Number of segments in the body",
                                "Number of segments sensed by each stretch receptor"};
par.messages_inds = {0,1,2}; //must be ordered
return par;
}

SRWeights StretchReceptorCE::makeSRWeights_old() const
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


void StretchReceptorCE::Update()
{
    double d, v;    
    //////////////////////////////
    // A-class Stretch Receptors
    // first unit (head) receive same input as Unit 2

    if (SRForm == 0){
    d = 0.0;
    v = 0.0;
    for (int j = 1; j <= NSEGSSR; j++){
        d += normSegLenD(j);
        v += normSegLenV(j);
    }
    A_D_sr(1) = SR_A_gain*(d/NSEGSSR);
    A_V_sr(1) = SR_A_gain*(v/NSEGSSR);

    // Units 2 to 10 
    for (int i = 2; i <= 10; i++){
        d = 0.0;
        v = 0.0;
        for (int j = 1; j <= NSEGSSR; j++)
        {
            d += normSegLenD(j+(i-2)*4);
            v += normSegLenV(j+(i-2)*4);
        }
        A_D_sr(i) = SR_A_gain*(d/NSEGSSR);
        A_V_sr(i) = SR_A_gain*(v/NSEGSSR);
    }
    
    //////////////////////////////
    // B-class Stretch Receptors
    // Units 1 to 9 (first segment sense by unit 1 is segment 13)
    for (int i = 1; i <= 9; i++){
        d = 0.0;
        v = 0.0;
        for (int j = 1; j <= NSEGSSR; j++)
        {
            d += normSegLenD(12+j+(i-1)*4);
            v += normSegLenV(12+j+(i-1)*4);
        }
        B_D_sr(i) = SR_B_gain*(d/NSEGSSR);
        B_V_sr(i) = SR_B_gain*(v/NSEGSSR);
    }
    // Unit 10 (tail), receive same input as Unit 9
    d = 0.0;
    v = 0.0;
    for (int j = 1; j <= NSEGSSR; j++){
        d += normSegLenD(j+44);
        v += normSegLenV(j+44);
    }
    B_D_sr(10) = SR_B_gain*(d/NSEGSSR);
    B_V_sr(10) = SR_B_gain*(v/NSEGSSR);

    return;
    }

//        //////////////////////////////
//    // A-class Stretch Receptors
//    // Units 1 to 9 (first segment sense by unit 1 is segment 13)

else if (SRForm == 1)
{
    for (int i = 1; i <= 9; i++){
        d = 0.0;
        v = 0.0;
        for (int j = 1; j <= NSEGSSR; j++)
        {
            d += normSegLenD(12+j+(i-1)*4);
            v += normSegLenV(12+j+(i-1)*4);
        }
        A_D_sr(i) = SR_A_gain*(d/NSEGSSR);
        A_V_sr(i) = SR_A_gain*(v/NSEGSSR);
    }

//    // Unit 10 (tail), receive same input as Unit 9

    d = 0.0;
    v = 0.0;
    for (int j = 1; j <= NSEGSSR; j++){
        d += normSegLenD(j+44);
        v += normSegLenV(j+44);
    }
    A_D_sr(10) = SR_A_gain*(d/NSEGSSR);
    A_V_sr(10) = SR_A_gain*(v/NSEGSSR);
    
//    //////////////////////////////
//    // B-class Stretch Receptors
//    // first unit (head) receive same input as Unit 2

    d = 0.0;
    v = 0.0;
    for (int j = 1; j <= NSEGSSR; j++){
        d += normSegLenD(j);
        v += normSegLenV(j);
    }
    B_D_sr(1) = SR_B_gain*(d/NSEGSSR);
    B_V_sr(1) = SR_B_gain*(v/NSEGSSR);

//    // Units 2 to 10 

    for (int i = 2; i <= 10; i++){
        d = 0.0;
        v = 0.0;
        for (int j = 1; j <= NSEGSSR; j++)
        {
            d += normSegLenD(j+(i-2)*4);
            v += normSegLenV(j+(i-2)*4);
        }
        B_D_sr(i) = SR_B_gain*(d/NSEGSSR);
        B_V_sr(i) = SR_B_gain*(v/NSEGSSR);
    }
 
    return;
}

    cout << "SRForm " << SRForm << endl;

    assert(0 && "SRForm not set correctly");

}
