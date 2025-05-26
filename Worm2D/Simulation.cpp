#include "Simulation.h"

string Simulation::rename_file(string filename){return sp.directoryName + "/" + filename;}

void Simulation::runSimulation(Worm2Dm & w)
{
    cout << "running simulation sim" << endl;

    for (double t = 0.0; t <= sp.Transient; t += sp.StepSize) w.Step(sp.StepSize);

    for (double t = 0.0; t <= sp.Duration; t += sp.StepSize){
        
        w.Step(sp.StepSize);
        w.DumpBodyState(bodyfile, sp.skip_steps);
        w.DumpCurvature(curvfile, sp.skip_steps);
        w.DumpActState(actfile, sp.skip_steps);
        w.DumpVal(velfile,sp.skip_steps,w.getVelocity()/sp.StepSize);
        w.DumpNSOrdered(nsfile, sp.skip_steps);
    }
    
    
}
