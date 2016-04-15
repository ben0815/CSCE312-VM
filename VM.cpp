#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <stdlib.h>

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
  else if(_line.find("local") < sz) {
    std::string number = (_line.substr(9, sz - 9));
    command = "@LCL\nD=M\n@" + number + "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  else if(_line.find("argument") < sz) {
    std::string number = (_line.substr(12, sz - 12));
    command = "@ARG\nD=M\n@" + number + "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  else if(_line.find("this") < sz) {
    std::string number = (_line.substr(8, sz - 8));
    command = "@THIS\nD=M\n@" + number + "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  else if(_line.find("that") < sz) {
    std::string number = (_line.substr(8, sz - 8));
    command = "@THAT\nD=M\n@" + number + "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  else if(_line.find("temp") < sz) {
    std::string number = (_line.substr(8, sz - 8));
    int n = 5 + atoi(number.c_str());
    std::string n2 = std::to_string(n);
    command = "@R5\nD=M\n@" + n2 + "\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  else if(_line.find("pointer") < sz) {
    std::string number = (_line.substr(11, sz - 11));
    if (number == "0")
      command = "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
    else if(number == "1")
      command = "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }
  else if(_line.find("static") < sz) {
    std::string number = (_line.substr(10, sz - 10));
    int n = 16 + atoi(number.c_str());
    std::string n2 = std::to_string(n);
    command = "@" + n2 + "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
  }

  return command;
}

std::string
ParsePop(std::string _line) {
  size_t sz = _line.size();
  std::string command = "";

  //Check type of push command
  if(_line.find("local") < sz) {
    //Get number then use it to translate into assembly
    std::string number = (_line.substr(8, sz - 8));
    command = "@LCL\nD=M\n@" + number + "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
  }
  else if(_line.find("argument") < sz) {
    std::string number = (_line.substr(11, sz - 11));
    command = "@ARG\nD=M\n@" + number + "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
  }
  else if(_line.find("this") < sz) {
    std::string number = (_line.substr(7, sz - 7));
    command = "@THIS\nD=M\n@" + number + "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
  }
  else if(_line.find("that") < sz) {
    std::string number = (_line.substr(7, sz - 7));
    command = "@THAT\nD=M\n@" + number + "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
  }
  else if(_line.find("temp") < sz) {
    std::string number = (_line.substr(7, sz - 7));
    int n = 5 + atoi(number.c_str());
    std::string n2 = std::to_string(n);
    command = "@R5\nD=M\n@" + n2 + "\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
  }
  else if(_line.find("pointer") < sz) {
    std::string number = (_line.substr(10, sz - 10));
    if (number == "0")
      command = "@THIS\nD=A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
    else if(number == "1")
      command = "@THAT\nD=A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
  }
  else if(_line.find("static") < sz) {
    std::string number = (_line.substr(9, sz - 9));
    int n = 16 + atoi(number.c_str());
    std::string n2 = std::to_string(n);
    command = "@" + n2 + "\nD=A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
  }
  else
    return "";

  return command;
}
std::string
Parse(std::string _line) {
  size_t sz = _line.size();
  std::string translated = "";

  //Check if command is a push constant x command
  if(_line.find("push") < sz) {
    translated = ParsePush(_line);
    return translated;
  }
  else if(_line.find("pop") < sz) {
    translated = ParsePop(_line);
    return translated;
  }
  else if(_line == "add")
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nM=D+M\n";
  else if(_line == "sub")
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nM=M-D\n";
  else if(_line == "neg")
    translated = "@SP\nA=M-1\nD=M\nM=-D";
  else if(_line == "eq") {
    std::string c = std::to_string(++count);
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@EQTRUE" + c +
      "\nD;JEQ\n@SP\nA=M-1\nM=0\n@EQEND" + c + "\n0;JMP\n(EQTRUE" + c +
      ")\n@SP\nA=M-1\nM=-1\n(EQEND" + c + ")\n";
  }
  else if(_line == "gt") {
    std::string c = std::to_string(++count);
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@GTTRUE" + c +
      "\nD;JGT\n@SP\nA=M-1\nM=0\n@GTEND" + c + "\n0;JMP\n(GTTRUE" + c +
      ")\n@SP\nA=M-1\nM=-1\n(GTEND" + c +")\n";
  }
  else if(_line == "lt") {
    std::string c = std::to_string(++count);
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n@LTTRUE" + c +
      "\nD;JLT\n@SP\nA=M-1\nM=0\n@LTEND" + c + "\n0;JMP\n(LTTRUE" + c +
      ")\n@SP\nA=M-1\nM=-1\n(LTEND" + c +")\n";
  }
  else if(_line == "and")
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nM=M&D\n";
  else if(_line == "or")
    translated = "@SP\nAM=M-1\nD=M\nA=A-1\nM=M|D\n";
  else if(_line == "not")
    translated = "@SP\nA=M-1\nM=!M\n";
  else
    translated = "";

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
