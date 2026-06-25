#include "../c302NervousSystem.h"
#include <fstream>

using namespace std;

const char* getParameter(int argc, const char* argv[], string parName, const char* defaultval)
{    
   const char* retval = defaultval;
   if (((argc-1) % 2) != 0)
   {cout << "The arguments are not configured correctly." << endl;exit(1);}
   for (int arg = 1; arg<argc; arg+=2) 
   if (strcmp(argv[arg],parName.c_str())==0) {retval = argv[arg+1];break;}
   return retval;
}


int main (int argc, const char* argv[])
{

string pop_string =  getParameter(argc,argv,"--popString","");
if (pop_string == "") {cout << "Need a population string, exiting"; return 0;}
int pop_size =  atoi(getParameter(argc,argv,"--popSize","0"));
if (pop_size <= 0) {cout << "Need a non-zero positive population size, exiting"; return 0;}
string dat_string =  getParameter(argc,argv,"--datString","");

c302NervousSystem n("main_sim", "parent");

n.SetPopStructure(pop_string, pop_size);
n.skipCalc = 1;

ofstream fout("testc302_output" + dat_string + ".dat");
for (int i=0;i<10000;i++){
if (i==3000) n.SetChemicalSynapseWeight(3, 1, 1);

if (i==6000) n.SetNeuronExternalInput(13, 1);

if (i==9000) n.SetNeuronBias(9, 1);


n.EulerStep(1);
fout << i;
n.writeOutputValues(fout);
fout << endl;
//cout << n.NeuronBias(55) << endl;
//cout << n.NeuronOutput(1) << endl;
//cout << n.NeuronOutput(2) << endl;
//cout << n.NeuronOutput(3) << endl;
}
fout.close();

return 0;
}