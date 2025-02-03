#include "NervousSystem2D.h"

int main (int argc, const char* argv[])
{

NervousSystem2D n;
n.setSimulator("main_sim", "Worm2DNRNSimulation", 0.005);
n.EulerStep(0.1);

return 0;
}