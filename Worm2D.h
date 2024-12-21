#include "Worm.h"

using namespace std;

ifstream & setParamsFromDump(ifstream &ifs, Worm &);
ostream& writeNSysToFile(ostream& os, NervousSystem& c);
istream& readNSysFromFile(istream& is, NervousSystem& c);
ostream& writeWSysToFile(ostream& os, Worm& w);
ostream& writeMuscSysToFile(ostream& os, Muscles& m);
ostream& writeGlobalParsToFile(ostream& os);
ostream& writeStretchSysToFile(ostream& os, StretchReceptor& s);
ostream& writeBodySysToFile(ostream& os, WormBody& b);


//string output_dir_name = "";
//string rename_file(const string & file_name);



/* class Worm2D : public Worm {
public:

    Worm2D(TVector<double> &v, double output);
    ifstream & setParamsFromDump(ifstream &ifs);

}; */