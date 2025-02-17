#include "c302NervousSystem.h"
#include <fstream>

using namespace std;


int main (int argc, const char* argv[])
{

c302NervousSystem n("main_sim");

ofstream fout("testc302NervousSystem-output.dat");
for (int i=0;i<10000;i++){
if (i==3000) n.SetChemicalSynapseWeight(22, 2, 1);

if (i==6000) n.SetNeuronExternalInput(53, 1);
   


n.EulerStep(1);
fout << i;
n.writeOutputValues(fout);
fout << endl;
//cout << n.NeuronOutput(1) << endl;
//cout << n.NeuronOutput(2) << endl;
//cout << n.NeuronOutput(3) << endl;
}
fout.close();

return 0;
}