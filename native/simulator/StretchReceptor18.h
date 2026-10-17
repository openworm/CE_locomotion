//
//  StretchReceptor.hpp
//  one
//
//  Created by Eduardo on 9/26/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#pragma once
//#include <cmath>
#include "StretchReceptor.h"
using namespace std;

class StretchReceptor18 : public StretchReceptor{
public:
    
    StretchReceptor18(int nSegs = 50, int nSR = 6, double SRgain = 0.0, double SRHeadgain = 0.0);

    void SetStretchReceptorParams(int nSegs, int nSR, double SRVNCgain, double SRHeadgain);
    
    void Update();
    Params<double> getStretchReceptorParams();

    //SRWeights makeSRWeights() const;

    void SetDorsalInput(int seg, double normlen){normSegLenD(seg) = normlen;};
    void SetVentralInput(int seg, double normlen){normSegLenV(seg) = normlen;};
    

    /* double HeadDorsalOutput(){return HD_sr;};
    double HeadVentralOutput(){return HV_sr;};
    double VCDorsalOutput(int i){return D_sr(i);};
    double VCVentralAOutput(int i){return VA_sr(i);};
    double VCVentralPOutput(int i){return VP_sr(i);}; */

    double HeadDorsalOutput(){return all_sr_d(1);};
    double HeadVentralOutput(){return all_sr_v(1);};
    double VCDorsalOutput(int i){return all_sr_d(1 + i);};
    double VCVentralAOutput(int i){return all_sr_v(1 + i);};
    double VCVentralPOutput(int i){return all_sr_v(1 + NSR + i);};
    
    double NSR;
    double NSEGS;
    double NSEGSSR;
    double SRvncgain;
    double SRheadgain;
    double NSEGSHEADSTART,NSEGSHEAD, NSEGSVNCSTART;
    TVector<double> normSegLenD;
    TVector<double> normSegLenV;

    //double HD_sr;
    //double HV_sr;
    //TVector<double> D_sr;
    //TVector<double> VA_sr;
    //TVector<double> VP_sr;

    TVector<double> all_sr_d, all_sr_v;
};

