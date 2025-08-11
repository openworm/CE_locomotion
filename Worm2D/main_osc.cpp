#include "Worm2Dmods.h"
#include "Evolution.h"


int main (int argc, const char* argv[])
{

    Worm2Dosc w1;
    EvolutionFull evo(argc,argv,&w1);
    bool do_evol = atoi(getParameter(argc,argv,"--doevol","0"));
    if (do_evol) 
    {
        evo.configure();
    }

    Worm2Dosc w2(evo.rename_file("best.gen.dat"));
    



    return 0;
}