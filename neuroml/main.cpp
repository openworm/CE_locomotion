#include "NervousSystem2D.h"

int main (int argc, const char* argv[])
{

NervousSystem2D n;
n.setSimulator("main_sim", "Worm2DNRNSimulation", 0.005);
for (int i=0;i<10;i++){
n.EulerStep(1);
cout << n.NeuronOutput(1) << endl;
cout << n.NeuronOutput(2) << endl;
cout << n.NeuronOutput(3) << endl;
}

return 0;
}