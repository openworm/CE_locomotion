#include "Worm2Dmods.h"


Worm2Dosc::Worm2Dosc():Worm2D({48,24,0.1,1,48},0),
Worm2Dm({48,24,0.1,1,48}, new NSosc(), new Muscles),
n(dynamic_cast<NSosc&>(*n_ptr)){}


Worm2Dosc::Worm2Dosc(const pfa & pfa_, const Worm2Doscpars & pars1_):Worm2D({pfa_.size,24,0.1,1,pfa_.size},0),
Worm2Dm({pfa_.size,24,0.1,1,pfa_.size}, new NSosc(pfa_), new Muscles),
n(dynamic_cast<NSosc&>(*n_ptr)),pars1(pars1_)
{
    setUpMuscleConn();
    n.setTime(t);
}

Worm2Dosc::Worm2Dosc(TVector<double> & geno_):Worm2Dosc(getPfaFromGeno(geno_), getParsFromGeno(geno_)){}

void Worm2Dosc::InitializeState(RandomState &rs)
{    
    cout << "Worm2Dosc init state" << endl;
    Worm2D::InitializeState(rs);
    return;    
}

vector<toFromWeight> Worm2Dosc::makeDVMuscleConn(int offset)
{
    vector<toFromWeight> vec1;
    for (int to_musc=1;to_musc<=24;to_musc++){
    int from_neuron = to_musc+offset;
    toFromWeight tv({from_neuron,pars1.NMJweight},to_musc);
    vec1.push_back(tv);}
    return vec1;
}


vector<toFromWeight> Worm2Dosc::makeDorsalMuscleConn()
{
    return makeDVMuscleConn(0);
}

vector<toFromWeight> Worm2Dosc::makeVentralMuscleConn()
{
    return makeDVMuscleConn(24);
}

void Worm2Dosc::Step1()
{

    n.setTime(t);
    // Update Body
    b.StepBody(settedStepSize);
    
    // Update Nervous System
    //n_ptr->EulerStep(settedStepSize);
    
    // Interneuron input  //////////////////////
    /* for (int i = 1; i <= par1.N_units; i++){
        n_ptr->SetNeuronExternalInput(nn(DB, i), wAVB_DB * AVB);
        n_ptr->SetNeuronExternalInput(nn(VB, i), wAVB_VB * AVB);
        n_ptr->SetNeuronExternalInput(nn(DA, i), wAVA_DA * AVA);
        n_ptr->SetNeuronExternalInput(nn(VA, i), wAVA_VA * AVA);
    
    } */
    
    setMuscleInput();
    //setMuscleInputOrig(StepSize);   


    // Set input to Body
    //  First two segments receive special treatment because they are only affected by a single muscle
    b.SetDorsalSegmentActivation(1, m_ptr->DorsalMuscleOutput(1)/2);
    b.SetVentralSegmentActivation(1, m_ptr->VentralMuscleOutput(1)/2);
    b.SetDorsalSegmentActivation(2, m_ptr->DorsalMuscleOutput(1)/2);
    b.SetVentralSegmentActivation(2, m_ptr->VentralMuscleOutput(1)/2);
    
    //  All other segments receive force from two muscles
    for (int i = 3; i <= N_segments-2; i++)
    {
        int mi = (int) ((i-1)/2);
        b.SetDorsalSegmentActivation(i, (m_ptr->DorsalMuscleOutput(mi) + m_ptr->DorsalMuscleOutput(mi+1))/2);
        b.SetVentralSegmentActivation(i, (m_ptr->VentralMuscleOutput(mi) + m_ptr->VentralMuscleOutput(mi+1))/2);
    }
    
    //  Last two segments receive special treatment because they are only affected by a single muscle
    b.SetDorsalSegmentActivation(N_segments-1, m_ptr->DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments-1, m_ptr->VentralMuscleOutput(par1.N_muscles)/2);
    b.SetDorsalSegmentActivation(N_segments, m_ptr->DorsalMuscleOutput(par1.N_muscles)/2);
    b.SetVentralSegmentActivation(N_segments, m_ptr->VentralMuscleOutput(par1.N_muscles)/2);
    
   
}

NSosc::NSosc(const pfa & pfa_):pfa1(pfa_){}

void Worm2Dosc::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
    
    const double NMJweight_top = 10;
    const double freq_lo = 0.1;
    const double freq_hi = 10;
    //phases_lag
    int i = 1;
    phen(i) = MapSearchParameter(gen(i), 0, pi2);
    i++;
    phen(i) = MapSearchParameter(gen(i), 0, pi2);
    //dv phase_offset

    //for (i = 1; i <= 48; i++)
    //phen(i) = MapSearchParameter(gen(i), 0, pi2);
    i++;
    phen(i) = MapSearchParameter(gen(i), freq_lo, freq_hi);
    //weight
    i++;
    phen(i) = MapSearchParameter(gen(i), 0, NMJweight_top);
   

}

void Worm2Dosc::writeJson(TVector<double> &){}

evoPars Worm2Dosc::getEvoPars(){ return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 0, 0, 10, 40.0, 10.0, 0.005, 23, 4};}

int Worm2Dosc::getVectSize(){return 4;}

pfa Worm2Dosc::getPfaFromPheno(TVector<double> &phen)
{
pfa pfa1;
pfa1.size = 48;
for (int i = 1; i<=24; i++) pfa1.phase.push_back(phen[1]*i);
for (int i = 25; i<=48; i++) pfa1.phase.push_back(phen[1]*i + phen[2]);
for (int i = 1; i<=48; i++) {pfa1.freq.push_back(phen[3]);pfa1.amp.push_back(1);}
return pfa1;
}

Worm2Doscpars Worm2Dosc::getParsFromPheno(TVector<double> &phen)
{
    Worm2Doscpars w1;
    w1.NMJweight = phen[4];
    return w1;
}


pfa Worm2Dosc::getPfaFromGeno(TVector<double> &v){}
Worm2Doscpars Worm2Dosc::getParsFromGeno(TVector<double> &v){}

double Worm2Dosc::EvaluationFunction(TVector<double> &geno, RandomState &rs){

  
    const double OSCT = 0.25 * ep_ptr->Duration; // Cap for oscillation evaluation
    const double agarfreq = 0.44;
    const double    AvgSpeed = 0.00022;             // Average speed of the worm in meters per seconds
    const double    BBCfit = AvgSpeed*ep_ptr->Duration;

    const double & Duration = ep_ptr->Duration;
    const int & VectSize = ep_ptr->VectSize;
    const double & StepSize = ep_ptr->StepSize;
    const int & N_curvs = ep_ptr->N_curvs;
    const double & Transient = ep_ptr->Transient;
    const int & skip_steps = ep_ptr->skip_steps;

    int dbunit = 1;
    int vbunit = 25;
    TVector<double> phenotype(1, VectSize);
    GenPhenMapping(geno, phenotype);
    n.pfa1 = getPfaFromPheno(phenotype);
    pars1 = getParsFromPheno(phenotype);

    setUpMuscleConn();

        // Fitness
        double fitness_tr = 0.0;
        double bodyorientation, anglediff;
        double movementorientation, distancetravelled = 0, displacement, temp;
        TVector<double> curvature(1, N_curvs);
        TVector<double> antpostcurv(1, 2);
        antpostcurv.FillContents(0.0);
    
        // Evaluation of B-class neuron oscillation,and frequency in segment 2.
        // The index of B class in this segment correspond to DBs2 = 10; VBs2 = 13
        double DBp, VBp, dDB, dVB;
        double oscDB = 0, oscVB = 0;
        double FoDB, FoVB, FfDB, FfVB;
    
        double freqDB=0, freqVB=0;
        int pDB = 0, pVB = 0, signtagDB, signtagVB, signDB, signVB;
        TVector<double> peaksDB(1, 2*Duration);
        TVector<double> peaksVB(1, 2*Duration);// longer vector if you want frequencies higer than 2 Hz.
    
        
        // Genotype-Phenotype Mapping
        
        
        //Worm21 w(phenotype);
        
        InitializeState(rs);
        
        // Transient XXX
        //SetAVB(0.0);
        //SetAVA(0.0);
        
        for (double t = 0.0; t <= Transient; t += StepSize){
            Step(StepSize);
        }    
        
        DBp = n.NeuronOutput(dbunit);
        VBp = n.NeuronOutput(vbunit);
    
        Step(StepSize); // determine sign of derivative
    
        dDB = n.NeuronOutput(dbunit) - DBp;
        dVB = n.NeuronOutput(vbunit) - VBp;
        signtagDB = (dDB  > 0) ? 1 : -1;
        signtagVB = (dVB  > 0) ? 1 : -1;
        DBp = n.NeuronOutput(dbunit);
        VBp = n.NeuronOutput(vbunit);
        
        double xt = CoMx(), xtp;
        double yt = CoMy(), ytp;
    
        // Time loop
        for (double t = 0.0; t <= Duration; t += StepSize) {
            // Step simulation
            Step(StepSize);
            
            ///// Oscilation
            // check changes in sign of derivative
            dDB = n.NeuronOutput(dbunit) - DBp;
            dVB = n.NeuronOutput(vbunit) - VBp;
            signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
            signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);
    
            oscDB += abs(DBp - n.NeuronOutput(dbunit));
            oscVB += abs(VBp - n.NeuronOutput(vbunit));
    
            if ((signDB == -1) and (signtagDB >= 0)){
                pDB +=1;
                peaksDB[pDB] = t;
                if (pDB >= 2*Duration){return 0;};
            }
            if ((signVB == -1) and (signtagVB >= 0)){
                pVB +=1;
                peaksVB[pVB] = t;
                if (pVB >= 2*Duration){return 0;};
            }
    
            signtagDB = signDB;
            signtagVB = signVB;
            DBp = n.NeuronOutput(dbunit);
            VBp = n.NeuronOutput(vbunit);
            
            //// Locomotion
            // Current and past centroid position
            xtp = xt; ytp = yt;
            xt = CoMx(); yt = CoMy();
            
            // Integration error check
            if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize){
                return 0.0;
            }
            
            // Fitness
            bodyorientation = Orientation();                  // Orientation of the body position
            movementorientation = atan2(yt-ytp,xt-xtp);         // Orientation of the movement
            anglediff = movementorientation - bodyorientation;  // Check how orientations align
            temp = cos(anglediff) > 0.0 ? 1.0 : -1.0;           // Add to fitness only movement forward
            distancetravelled += temp * sqrt(pow(xt-xtp,2)+pow(yt-ytp,2));
    
        }
        // B Oscillation evaluation
        if ((pDB < 2) or (pVB < 2)){return 0;};
        for (int i = 1; i<pDB; i+=1){freqDB += (1./(pDB-1))*(1./(peaksDB[i+1]- peaksDB[i]));} 
        for (int i = 1; i<pVB; i+=1){freqVB += (1./(pVB-1))*(1./(peaksVB[i+1]- peaksVB[i]));} 
    
        FfDB = fabs(freqDB - agarfreq)/agarfreq < 1 ? fabs(freqDB - agarfreq)/agarfreq : 1;
        FfVB = fabs(freqVB - agarfreq)/agarfreq < 1 ? fabs(freqVB - agarfreq)/agarfreq : 1;
    
        FoDB = oscDB > OSCT ? 1 : oscDB / OSCT;
        FoVB = oscVB > OSCT ? 1 : oscVB / OSCT;
    
        // Locomotion evaluation
        fitness_tr = (1 - (fabs(BBCfit-distancetravelled)/BBCfit));
    
   
        return fitness_tr * FoDB * FoVB * (1 - FfDB) * (1 - FfVB);
    }



