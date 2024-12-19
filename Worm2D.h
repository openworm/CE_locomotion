#include "Worm.h"

using namespace std;

ifstream & setParamsFromDump(ifstream &ifs, Worm &);
ostream& writeNSysToFile(ostream& os, NervousSystem& c);
istream& readNSysFromFile(istream& is, NervousSystem& c);
ostream& writeWSysToFile(ostream& os, Worm& w);

/* class Worm2D : public Worm {
public:

    Worm2D(TVector<double> &v, double output);
    ifstream & setParamsFromDump(ifstream &ifs);

}; */