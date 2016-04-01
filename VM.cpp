#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

string
Add() {
  string command = "";

  return command;
}

string
PushConstant(string _number) {
  string command = "@SP\nA=M\nM=" + _number + "\n@SP\nM=M+1\n";

  return command;
}

string
ParseVM(string _line) {
  size_t sz = _line.size();
  string translated = "";

  //Check if command is a push constant x command
  if(_line.find("push constant") < sz) {
    //Get number x then use it to translate into assembly
    string number = _line.substr(14, sz - 14);
    translated = PushConstant(number);
  }
  else if(_line == "add")
    translated = Add();

  return translated;
}

string
Clean(string _line) {
  string cleaned = _line;

  //Search for '//' in each line
  size_t n = _line.find("//");

  //If '//' is found, erase everything after it
  if(n < _line.size())
    cleaned = _line.erase(n, _line.size());

  return cleaned;
}

int main() {
  string filename;

  cout << "Enter the name of the file you want to translate:" << endl;
  cin >> filename;

  //Construct input stream (.vm file)
  ifstream ifs(filename);

  //vector<string> vm;
  //vector to store assembly code after it is translated
  vector<string> assembly;

  string line;
  while(getline(ifs, line)) {
    //Get each line of VM commands and remove any comments
    string cleaned = Clean(line);

    //Remove any empty lines
    if(!cleaned.empty()) {
      //vm.push_back(cleaned);
      string command = ParseVM(cleaned);
      assembly.push_back(command);
    }
  }

  for(auto it : assembly){
    cout << it << endl;
  }

  return 0;
}
