//
//  StretchReceptor.cpp
//  one
//
//  Created by Eduardo on 9/26/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
// Modified by Erick Olivares, Feb 2019
// Added SR for class A motorneurons

#include "StretchReceptor.h"

StretchReceptor::StretchReceptor(int nSegs, int nSR, double ASRgain, double BSRgain)
{
    SetStretchReceptorParams(nSegs, nSR, ASRgain, BSRgain);
}

void StretchReceptor::SetStretchReceptorParams(int nSegs, int nSR, double ASRgain, double BSRgain)
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

void StretchReceptor::Update()
{
    double d, v;    
    //////////////////////////////
    // A-class Stretch Receptors
    // first unit (head) receive same input as Unit 2
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

//        //////////////////////////////
//    // A-class Stretch Receptors
//    // Units 1 to 9 (first segment sense by unit 1 is segment 13)
//    for (int i = 1; i <= 9; i++){
//        d = 0.0;
//        v = 0.0;
//        for (int j = 1; j <= NSEGSSR; j++)
//        {
//            d += normSegLenD(12+j+(i-1)*4);
//            v += normSegLenV(12+j+(i-1)*4);
//        }
//        A_D_sr(i) = SR_A_gain*(d/NSEGSSR);
//        A_V_sr(i) = SR_A_gain*(v/NSEGSSR);
//    }
//    // Unit 10 (tail), receive same input as Unit 9
//    d = 0.0;
//    v = 0.0;
//    for (int j = 1; j <= NSEGSSR; j++){
//        d += normSegLenD(j+44);
//        v += normSegLenV(j+44);
//    }
//    A_D_sr(10) = SR_A_gain*(d/NSEGSSR);
//    A_V_sr(10) = SR_A_gain*(v/NSEGSSR);
//    
//    //////////////////////////////
//    // B-class Stretch Receptors
//    // first unit (head) receive same input as Unit 2
//    d = 0.0;
//    v = 0.0;
//    for (int j = 1; j <= NSEGSSR; j++){
//        d += normSegLenD(j);
//        v += normSegLenV(j);
//    }
//    B_D_sr(1) = SR_B_gain*(d/NSEGSSR);
//    B_V_sr(1) = SR_B_gain*(v/NSEGSSR);

//    // Units 2 to 10 
//    for (int i = 2; i <= 10; i++){
//        d = 0.0;
//        v = 0.0;
//        for (int j = 1; j <= NSEGSSR; j++)
//        {
//            d += normSegLenD(j+(i-2)*4);
//            v += normSegLenV(j+(i-2)*4);
//        }
//        B_D_sr(i) = SR_B_gain*(d/NSEGSSR);
//        B_V_sr(i) = SR_B_gain*(v/NSEGSSR);
//    }
    

}
