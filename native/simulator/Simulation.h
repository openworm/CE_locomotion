#include "Worm2D.h"
//#include "Evolution.h"

//struct simPars;


struct simPars{
    string directoryName;
   // int skip_steps;
    double Duration;       //
    double Transient;       //
    double StepSize;
};


class Simulation
{
public:

    Simulation(const simPars & sp1):sp(sp1)
    {
        //curvfile.open(rename_file("sim_curv.dat"));
        //bodyfile.open(rename_file("sim_body.dat"));
        //actfile.open(rename_file("sim_act.dat"));
        //velfile.open(rename_file("sim_vel.dat"));
        //nsfile.open(rename_file("sim_ns.dat"));

    }
    void runSimulation(Worm2Dbase & w);


    ~Simulation(){
        //actfile.close(); curvfile.close(); 
        //bodyfile.close(); 
        //velfile.close(); nsfile.close();
        }

        simPars sp;
   
    
private:
//const simPars sp;
//const simPars sp;
//string rename_file(string filename);
//ofstream velfile, nsfile;

};
