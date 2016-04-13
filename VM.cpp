#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
//I have placed a "@TODO" where things need to be added.
//If you have any questions about the code just text me or put them at the top
//here and commit to git.
//
//Currently it reads in from an input .vm file then cleans each line (removes
//comments and extra spacing) and parses each line.
//The Parse function checks if the command is a push, add, sub, etc. and
//translates to assembly code
//
//All we need to do for project 7 is to figure out the assembly code for each
//operation (sub, neg, and, etc.) and add it to the Parse function
////////////////////////////////////////////////////////////////////////////////

string
ParsePush(string _line) {
  size_t sz = _line.size();
  string command = "";

  //Check type of push command
  if(_line.find("constant") < sz) {
    //Get number then use it to translate into assembly
    string number = (_line.substr(12, sz - 12));
    if (number == "1")
      command = "@SP\nA=M\nM=1\n@SP\nM=M+1\n";
    else
      command = "@" + number + "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  //@TODO
  //Different push commands for project 8 will go here (local, this, etc.)

  return command;
}

string
Parse(string _line) {
  size_t sz = _line.size();
  string translated = "";

  //Check if command is a push constant x command
  if(_line.find("push") < sz) {
    translated = ParsePush(_line);
  }
  //@TODO
  //Add assembly code for each operation
  else if(_line == "add")
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nM=D+M\n";
  else if(_line == "sub")
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nM=M-D\n";
  else if(_line == "neg")
    translated = "@SP\nAM=M-1\nD=M\nM=-D";
  else if(_line == "eq")
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@EQTRUE\nD;JEQ\n@SP\nM=0\n@EQEND\n0;JMP\n(EQTRUE)\n@SP\nA=M-1\nM=-1\n(EQEND)\n
  else if(_line == "gt")
    translated = "";
  else if(_line == "lt")
    translated = "";
  else if(_line == "and")
    translated = "";
  else if(_line == "or")
    translated = "";
  else if(_line == "not")
    translated = "";

  return translated;
}

string
Clean(string _line) {
  //Search for '//' in each line
  size_t n = _line.find("//");

  //If '//' is found, erase everything after it
  if(n < _line.size())
    _line.erase(n, _line.size());

  string cleaned = "";

  //remove spaces
  for(int i = 0; i < _line.size(); ++i) {
    if(!isspace(_line[i]))
        cleaned += _line[i];
  }

  return cleaned;
}

int main() {
  string filename;

  cout << "Enter the name of the file you want to translate:" << endl;
  cin >> filename;

  //Construct input stream (.vm file)
  ifstream ifs(filename);
  ofstream ofs(filename.substr(0, filename.size() - 2) + "asm");

  string line;
  while(getline(ifs, line)) {
    //Get each line of VM commands and remove any comments
    string cleaned = Clean(line);

    //Remove any empty lines
    //Translate lines into assembly code and output to .asm file
    if(!cleaned.empty()) {
      string command = Parse(cleaned);
      ofs << command << endl;
    }
  }

  return 0;
}
