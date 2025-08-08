#include "Worm2Dmods.h"

Worm2Dosc::Worm2Dosc(const pfa & pfa_, const Worm2Doscpars & pars1_):Worm2D({pfa_.size,24,0.1,1,pfa_.size},0),
Worm2Dm({pfa_.size,24,0.1,1,pfa_.size}, new NSosc(pfa_), new Muscles),
n(dynamic_cast<NSosc&>(*n_ptr)),pars1(pars1_)
{
    setUpMuscleConn();
    n.setTime(t);
}

Worm2Dosc::Worm2Dosc(TVector<double> &v):Worm2Dosc(getPfaFromPheno(v), getParsFromPheno(v)){}

void Worm2Dosc::InitializeState(RandomState &rs)
{    
    cout << "Worm2Dosc init state" << endl;
    Worm2D::InitializeState(rs);
    return;    
}

vector<toFromWeight> Worm2Dosc::makeDVMuscleConn()
{
    const int nsize = n.itsPfa().size;
    assert(nsize == 24);
    vector<toFromWeight> vec1;
    for (int to=1;to<=nsize;to++){
    toFromWeight tv({to,pars1.NMJweight},to);
    vec1.push_back(tv);}
    return vec1;
}


vector<toFromWeight> Worm2Dosc::makeDorsalMuscleConn()
{
    return makeDVMuscleConn();
}

vector<toFromWeight> Worm2Dosc::makeVentralMuscleConn()
{
    return makeDVMuscleConn();
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

NSosc::NSosc(const pfa & pfa_)
{
pfa1.size = pfa_.size;
for (int i=0;i<pfa_.size;i++)
{
pfa1.phase.push_back(pfa_.phase[i]);
pfa1.freq.push_back(pfa_.freq[i]);
pfa1.amp.push_back(pfa_.amp[i]);
}

}

void Worm2Dosc::GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
  // Bias
  for (int i = 1; i <= 7; i++){
    phen(i) = MapSearchParameter(gen(i), -BiasRange, BiasRange);
}
// Time Constant
for (int i = 8; i <= 14; i++){
    phen(i) = MapSearchParameter(gen(i), TauMin, TauMax);
}
// Self connections
for (int i = 15; i <= 21; i++){
    phen(i) = MapSearchParameter(gen(i), -SCRange, SCRange);
}
// Chemical synapses
for (int i = 22; i <=30; i++){
    phen(i) = MapSearchParameter(gen(i), -CSRange, CSRange);
}

// Gap junctions
phen(31) = MapSearchParameter(gen(31), 0.0, ESRange);

// Intersegment synapse tested
phen(40) = MapSearchParameter(gen(40), -CSRange, CSRange);  // DB to DDnext
phen(41) = MapSearchParameter(gen(41), -CSRange, CSRange);  // VAnext to DD
phen(42) = MapSearchParameter(gen(42), 0.0, ESRange);       // AS -- VAnext
phen(43) = MapSearchParameter(gen(43), 0.0, ESRange);       // DA -- ASnext
phen(44) = MapSearchParameter(gen(44), 0.0, ESRange);       // VB -- DBnext

// NMJ Weight
phen(32) = MapSearchParameter(gen(32), 0.0, NMJmax);       // AS
phen(33) = MapSearchParameter(gen(33), 0.0, NMJmax);       // DA
phen(34) = MapSearchParameter(gen(34), NMJmax, NMJmax);       // DB
phen(35) = MapSearchParameter(gen(35), -NMJmax, 0.0);      // DD
phen(36) = MapSearchParameter(gen(36), -NMJmax, 0.0);      // VD
phen(37) = MapSearchParameter(gen(37), NMJmax, NMJmax);      // VB
phen(38) = MapSearchParameter(gen(38), 0.0, NMJmax);      // VA

phen(39) = MapSearchParameter(gen(39), 0.2, 1.0);       // Used to be 0.4/0.6 XXX NMJ_Gain Mapping

}


double Worm2Dosc::EvaluationFunction(TVector<double> &v, RandomState &rs){

    const double & Duration = evoPars1.Duration;
    const int & VectSize = evoPars1.VectSize;
    const double & StepSize = evoPars1.StepSize;
    const int & N_curvs = evoPars1.N_curvs;
    const double & Transient = evoPars1.Transient;
    const int & skip_steps = evoPars1.skip_steps;

    
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
        TVector<double> phenotype(1, VectSize);
        GenPhenMapping(v, phenotype);
        
        Worm21 w(phenotype);
        
        w.InitializeState(rs);
        
        // Transient XXX
        w.SetAVB(0.0);
        w.SetAVA(0.0);
        
        for (double t = 0.0; t <= Transient; t += StepSize){
            w.Step(StepSize);
        }    
        
        DBp = w.n.NeuronOutput(10);
        VBp = w.n.NeuronOutput(13);
    
        w.Step(StepSize); // determine sign of derivative
    
        dDB = w.n.NeuronOutput(10) - DBp;
        dVB = w.n.NeuronOutput(13) - VBp;
        signtagDB = (dDB  > 0) ? 1 : -1;
        signtagVB = (dVB  > 0) ? 1 : -1;
        DBp = w.n.NeuronOutput(10);
        VBp = w.n.NeuronOutput(13);
        
        double xt = w.CoMx(), xtp;
        double yt = w.CoMy(), ytp;
    
        // Time loop
        for (double t = 0.0; t <= Duration; t += StepSize) {
            // Step simulation
            w.Step(StepSize);
            
            ///// Oscilation
            // check changes in sign of derivative
            dDB = w.n.NeuronOutput(10) - DBp;
            dVB = w.n.NeuronOutput(13) - VBp;
            signDB = (dDB  > 0) ? 1 : ((dDB  < 0) ? -1 : 0);
            signVB = (dVB  > 0) ? 1 : ((dVB  < 0) ? -1 : 0);
    
            oscDB += abs(DBp - w.n.NeuronOutput(10));
            oscVB += abs(VBp - w.n.NeuronOutput(13));
    
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
            DBp = w.n.NeuronOutput(10);
            VBp = w.n.NeuronOutput(13);
            
            //// Locomotion
            // Current and past centroid position
            xtp = xt; ytp = yt;
            xt = w.CoMx(); yt = w.CoMy();
            
            // Integration error check
            if (isnan(xt) || isnan(yt) || sqrt(pow(xt-xtp,2)+pow(yt-ytp,2)) > 100*AvgSpeed*StepSize){
                return 0.0;
            }
            
            // Fitness
            bodyorientation = w.Orientation();                  // Orientation of the body position
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




void Worm2Dosc::writeJson(TVector<double> &){}
evoPars Worm2Dosc::getEvoPars(){ return {".", 42, RANK_BASED, GENETIC_ALGORITHM, 
        100, 2000, 0.1, 0.5, UNIFORM, 
        1.1, 0.04, 1, 0, 0, 10, 40.0, 10.0, 0.005, 23, 44};}
int Worm2Dosc::getVectSize(){return 44;}