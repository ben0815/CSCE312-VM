#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <stdlib.h>
#include <unordered_set>
#include <unistd.h>
#include <dirent.h>

//Global count variable
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
TranslateLabel(std::string _label, bool isIf) {
  std::string translated = "";

  if(isIf)
    translated = "@SP\nAM=M-1\nD=M\n@" + _label  + "\nD;JGT\n";
  else
    translated = "@" + _label + "\n0;JMP\n";

  return translated;
}

std::string
Parse(std::string _line) {
  size_t sz = _line.size();
  std::string translated = "";

  if(_line.find("push") < sz) {
    translated = ParsePush(_line);
    return translated;
  }
  else if(_line.find("pop") < sz) {
    translated = ParsePop(_line);
    return translated;
  }
  else if(_line.find("label") < sz) {
    std::string label = _line.substr(5, sz - 5);
    translated = "(" + label + ")\n";

    return translated;
  }
  else if(_line.find("if-goto") < sz) {
    std::string label = _line.substr(7, sz - 7);

    return TranslateLabel(label, true);
  }
  else if(_line.find("goto") < sz) {
    std::string label = _line.substr(4, sz - 4);

    return TranslateLabel(label, false);
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

int main(int argc, char** argv) {
  std::vector<std::string> files;
  const char *directory = "";
  bool isDir = false;

  //Usage: ./vm -d <directory>
  //     : ./vm -f <filepath>

  char c;
  while((c = getopt(argc, argv, "d:f:")) != -1) {
    switch (c) {
      case 'd':
        directory = optarg;
        isDir = true;
        break;
      case 'f':
        files.push_back(optarg);
        break;
      case '?':
        if(optopt == 'd' || optopt == 'f')
          std::cerr << "Options requires an argument." << std::endl;
        else if(isprint(optopt))
          std::cerr << "Unknown option `-" << c << "`." << std::endl;
        else
          std::cerr << "Unknown option character `" << optopt << "`" << std::endl;
        exit(1);
      default:
        std::cerr << "Something went wrong." << std::endl;
        exit(1);
    }
  }

  if(files.empty()) {
    std::cerr << "Bad usage: ./vm -d <directory>" << std::endl;
    std::cerr << "           ./vm -f <filepath>" <<std::endl;
    return 1;
  }

  //Populate files with all '.vm' files in given directory
  if(isDir) {
    DIR *dir;
    struct dirent *ent;

    dir = opendir(directory);
    if(dir != NULL) {
      while((ent = readdir(dir))) {
      std::string filename(ent->d_name);
        if(filename.find(".vm") < filename.size())
          files.push_back(filename);
      }
    }
  }

  //Get name of resulting asm file from directory or from file to be translated
  std::string asmFile = "";
  if(isDir) {
    std::string outFile(directory);
    if(outFile[outFile.size() - 1] == '/')
      outFile = outFile.substr(0, outFile.size() - 1);
    asmFile = outFile;
  }
  else
    asmFile = files[0];

  std::ofstream ofs(asmFile.substr(0, asmFile.size() - 2) + "asm");
  std::string line = "";

  //For each file read from it and output to one '.asm' file
  for(int i = 0; i < files.size(); ++i) {
    std::ifstream ifs(files[i]);
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
  }

  return 0;
}
