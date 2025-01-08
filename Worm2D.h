#include "Worm.h"
#include <vector>

using namespace std;

ifstream & setParamsFromDump(ifstream &ifs, Worm &);
ostream& writeNSysToFile(ostream& os, NervousSystem& c);
istream& readNSysFromFile(istream& is, NervousSystem& c);
ostream& writeWSysToFile(ostream& os, Worm& w);
ostream& writeMuscSysToFile(ostream& os, Muscles& m);
ostream& writeGlobalParsToFile(ostream& os);
ostream& writeStretchSysToFile(ostream& os, StretchReceptor& s);
ostream& writeBodySysToFile(ostream& os, WormBody& b);

string rename_file(const string & file_name);
void writeWormParams(Worm & w);
template<class T> vector<T> getVector(TVector<T> & vec, int size);
