#include "c302NervousSystem.h"

using namespace std;

int main (int argc, const char* argv[])
{

c302NervousSystem n("main_sim");
//n.EulerStep(1);
//return 0;

//n.setSimulator("main_sim", "Worm2DNRNSimulation", 0.005);
for (int i=0;i<10;i++){
n.EulerStep(1);
cout << n.NeuronOutput(1) << endl;
cout << n.NeuronOutput(2) << endl;
cout << n.NeuronOutput(3) << endl;
}

return 0;
}