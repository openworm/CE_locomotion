#include "Simulation.h"

//string Simulation::rename_file(string filename){return sp.directoryName + "/" + filename;}

void Simulation::runSimulation(Worm2Dbase & w)
{
    w.activateInputScheduleForSimulation();
    w.activateFuncableSchedulesForSimulation();

    //w.setBasename(sp.directoryName);
    
    //w.dataReset();
    //w.closeAll();
    //w.writeData();

    //cout << "running simulation sim" << endl;

    for (double t = 0.0; t < sp.Transient; t += sp.StepSize) w.Step();

    for (double t = 0.0; t <= sp.Duration; t += sp.StepSize){
        
        w.Step();
        w.writeDataCheck();
        //w.DumpBodyState(bodyfile, sp.skip_steps);
        //w.DumpCurvature(curvfile, sp.skip_steps);
        //w.DumpActState(actfile, sp.skip_steps);
        w.DumpVal("vel.dat",w.getVelocity()/sp.StepSize);
        w.DumpNSOrdered();
    }
    
    
}
