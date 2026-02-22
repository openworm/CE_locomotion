//
//  StretchReceptor.hpp
//  one
//
//  Created by Eduardo on 9/26/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
// Modified by Erick Olivares, Feb 2019
// Added SR for class A motorneurons
#pragma once
//#include "../VectorMatrix.h"
//#include "../random.h"
//#include <cmath>
#include "StretchReceptor.h"
using namespace std;

class StretchReceptorCE : public StretchReceptor//, public SRCE
{
public:
    
    StretchReceptorCE(int nSegs = 50, int nSR = 10, double A_SR_gain = 0.0, double B_SR_gain = 0.0);

    void SetStretchReceptorParams(int nSegs, int nSR, double A_SR_gain, double B_SR_gain);
    
    void Update();
    Params<double> getStretchReceptorParams();
    
    // Load segment deformation information
    void SetDorsalInput(int seg, double normlen){normSegLenD(seg) = normlen;};
    void SetVentralInput(int seg, double normlen){normSegLenV(seg) = normlen;};
    
    // SR activity in the units
    double A_DorsalOutput(int i){return A_D_sr(i);};
    double A_VentralOutput(int i){return A_V_sr(i);};
    double B_DorsalOutput(int i){return B_D_sr(i);};
    double B_VentralOutput(int i){return B_V_sr(i);};
    
    double NSR; // Number of stretch receptor, equal to number of units
    double NSEGS; // Number of segments in the body (50)
    double NSEGSSR; // Number of segments sensed by each stretch receptor
    
    double SR_A_gain;
    double SR_B_gain;
    
    TVector<double> normSegLenD;
    TVector<double> normSegLenV;

    TVector<double> A_D_sr;
    TVector<double> A_V_sr;
    TVector<double> B_D_sr;
    TVector<double> B_V_sr;
    int SRForm = 0;


    protected:
    SRWeights makeSRWeights_old() const;
    
};

