#include "utils.h"
#include <string.h>

//// helper functions

string nervousSystemName; // = "NervousSystem";
string nervousSystemNameForSim; // = "nmlNervousSystem";
string nervousSystemNameForEvol;
string output_dir_name; 


string rename_file(const string & file_name){
  if (output_dir_name != "") return output_dir_name + "/" + file_name;
  return file_name;
}

bool checkNervousSystemForJson(){
return (strcmp(nervousSystemName.c_str(), "NervousSystem") == 0);
}

