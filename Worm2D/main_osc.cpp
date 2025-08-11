#include "Worm2Dmods.h"
#include "Evolution.h"


int main (int argc, const char* argv[])
{

    Worm2Dosc w1;
    EvolutionFull evo(argc,argv,&w1);

    return 0;
}