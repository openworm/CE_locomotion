#include "c302NervousSystem.h"
#include <fstream>

using namespace std;


int main (int argc, const char* argv[])
{

c302NervousSystem n("main_sim");

ofstream fout("testc302NervousSystem-output.dat");
for (int i=0;i<10000;i++){
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