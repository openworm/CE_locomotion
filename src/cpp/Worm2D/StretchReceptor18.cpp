//
//  StretchReceptor.cpp
//  one
//
//  Created by Eduardo on 9/26/15.
//  Copyright © 2015 Eduardo Izquierdo. All rights reserved.
//

#include "StretchReceptor18.h"

StretchReceptor18::StretchReceptor18(int nSegs, int nSR, double srvncgain, double srheadgain)
{
    SetStretchReceptorParams(nSegs, nSR, srvncgain, srheadgain);
}

void StretchReceptor18::SetStretchReceptorParams(int nSegs, int nSR, double srvncgain, double srheadgain)
{
    NSEGS = nSegs;                  // Number of segments
    NSR = nSR;                      // Number of stretch receptors
    NSEGSSR = 6;                    // Number of segments that go into a stretch receptor
    SRvncgain = srvncgain;                // Stretch receptor gain
    SRheadgain = srheadgain;                // Stretch receptor gain

    NSEGSHEADSTART = 7;             // 7-12
    NSEGSHEAD = 14;                 // Number of segments for the sublateral head motorneurons
    NSEGSVNCSTART = 7;              // Segment where VNC starts

    normSegLenD.SetBounds(1, NSEGS);
    normSegLenV.SetBounds(1, NSEGS);

    all_sr_d.SetBounds(1, 1 + NSR);
    all_sr_v.SetBounds(1, 1 + 2*NSR);
    //D_sr.SetBounds(1, NSR);
    //VA_sr.SetBounds(1, NSR);
    //VP_sr.SetBounds(1, NSR);
}

Params<double> StretchReceptor18::getStretchReceptorParams()
{
Params<double> par;
par.names = {"NSR", "NSEGS", "NSEGSSR", "VNC_gain", "Head_gain", 
    "NSEGSHEADSTART", "NSEGSHEAD", "NSEGSVNCSTART"};
par.vals = {NSR, NSEGS, NSEGSSR, SRvncgain, SRheadgain, NSEGSHEADSTART, NSEGSHEAD, NSEGSVNCSTART};
par.messages = {"Number of stretch receptor",
                "Number of segments in the body",
                "Number of segments sensed by each stretch receptor"};
par.messages_inds = {0,1,2}; //must be ordered
return par;
}





void StretchReceptor18::Update()
{
    double d, v;

    // Head
    d = 0.0;
    v = 0.0;
    for (int j = NSEGSHEADSTART; j < NSEGSHEADSTART + NSEGSHEAD; j++)
    {
        d += normSegLenD(j);
        v += normSegLenV(j);
    }
    //HD_sr = SRheadgain*(d/NSEGSHEAD);
    //HV_sr = SRheadgain*(v/NSEGSHEAD);
    all_sr_d(1) = SRheadgain*(d/NSEGSHEAD);
    all_sr_v(1) = SRheadgain*(v/NSEGSHEAD);

    // First four VC Neural Units (with three muscles each)
    for (int i = 1; i <= 6; i++){
        d = 0.0;
        v = 0.0;
        for (int j = 1; j <= NSEGSSR; j++)
        {
            d += normSegLenD(j+((i-1)*NSEGSSR)+NSEGSVNCSTART-1);
            v += normSegLenV(j+((i-1)*NSEGSSR)+NSEGSVNCSTART-1);
        }

        all_sr_d(1 + i) = SRvncgain*(d/NSEGSSR);
        all_sr_v(1 + i) = SRvncgain*(v/NSEGSSR);

        d = 0.0;
        v = 0.0;
        for (int j = 1; j <= NSEGSSR; j++)
        {
            d += normSegLenD(j+((i-1)*NSEGSSR)+NSEGSVNCSTART-1+2);
            v += normSegLenV(j+((i-1)*NSEGSSR)+NSEGSVNCSTART-1+2);
        }
        all_sr_v(1 + NSR + i) = SRvncgain*(v/NSEGSSR);
    }
}
