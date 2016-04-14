#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

//Global count variable
//Necessary to make sure if a file contains multiple eq's that each label is
//different
size_t count = 0;

std::string
ParsePush(std::string _line) {
  size_t sz = _line.size();
  std::string command = "";

  //Check type of push command
  if(_line.find("constant") < sz) {
    //Get number then use it to translate into assembly
    std::string number = (_line.substr(12, sz - 12));
    if (number == "1")
      command = "@SP\nA=M\nM=1\n@SP\nM=M+1\n";
    else
      command = "@" + number + "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  //@TODO
  //Different push commands for project 8 will go here (local, this, etc.)

  return command;
}

std::string
Parse(std::string _line) {
  size_t sz = _line.size();
  std::string translated = "";

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
  else if(_line == "eq") {
    std::string c = std::to_string(++count);
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@EQTRUE" + c + "\nD;JEQ\n@SP\nM=0\n@EQEND" + c + "\n0;JMP\n(EQTRUE" + c + ")\n@SP\nA=M-1\nM=-1\n(EQEND" + c + ")\n";
  }
  else if(_line == "gt")
    translated = "";
  else if(_line == "lt")
    translated = "";
  else if(_line == "and")
    translated = "@SP\nAM=M-1\nD=M\n@SP\nM=M-1\nA=M\nM=M&D\n@SP\nM=M+1";
  else if(_line == "or")
    translated = "@SP\nAM=M-1\nD=M\n@SP\nM=M-1\nA=M\nM=M|D\n@SP\nM=M+1";
  else if(_line == "not")
    translated = "@SP\nAM=M-1\nM=!M\n@SP\nM=M+1";

  return translated;
}

std::string
Clean(std::string _line) {
  //Search for '//' in each line
  size_t n = _line.find("//");

  //If '//' is found, erase everything after it
  if(n < _line.size())
    _line.erase(n, _line.size());

  std::string cleaned = "";

  //remove spaces
  for(int i = 0; i < _line.size(); ++i) {
    if(!isspace(_line[i]))
        cleaned += _line[i];
  }

  return cleaned;
}

int main() {
  std::string filename;

  std::cout << "Enter the name of the file you want to translate:" << std::endl;
  std::cin >> filename;

  //Construct input stream (.vm file)
  std::ifstream ifs(filename);
  std::ofstream ofs(filename.substr(0, filename.size() - 2) + "asm");

  std::string line;
  while(getline(ifs, line)) {
    //Get each line of VM commands and remove any comments
    std::string cleaned = Clean(line);

    //Remove any empty lines
    //Translate lines into assembly code and output to .asm file
    if(!cleaned.empty()) {
      std::string command = Parse(cleaned);
      ofs << command << std::endl;
    }
  }

  return 0;
}
