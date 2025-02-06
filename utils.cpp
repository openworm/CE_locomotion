#include "utils.h"

//// helper functions

string output_dir_name = "";
string rename_file(const string & file_name){
  if (output_dir_name != "") return output_dir_name + "/" + file_name;
  return file_name;
}

