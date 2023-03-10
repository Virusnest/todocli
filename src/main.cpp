#include <stdio.h>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include <stdlib.h>
#include "main.h"
#include <sys/stat.h>
#include <json/json.h>

using namespace std;
using namespace Json;

struct winsize w;
string version = "todocli | 1.0.2";
int systime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
int day = 86400;
string configPath = string(getenv("HOME"))+"/.config/todocli/config.json";
Value root;
int main(int argc, char *argv[]) {
  loadConfig();
  if(parseArgs(argc, argv)==1){
    return 0;
  }
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  draw();
  return 0;
}

int parseArgs(int argc, char *argv[]) {
  if (argc > 1) {
    string word = argv[1];
    if (word == "-h"||word == "--help"){
      printf("Usage:\n\
  -h Display This Screen\n\
  -a [NAME]  [+DAYS] Add New Item\n\
  -r [INDEX] Remove Item\n\
  -t [INDEX] Toggle Item State\n\
  -e [INDEX] [NAME] Edit Item\n\
  -x [INDEX] [+DAYS] Extend Item\n\
  -c Clear All DONE Items\n\
  -ca Clear All Items\n\
  -v Show Version");
    } else if (word == "-a"||word == "--add"){
      if(argc > 2) {
        try{
          addTask(string(argv[2]), (stoi(argv[3])*day)+systime);
        }catch(...){printf("-a [NAME]  [+DAYS] Add New Item\n");}
        saveConfig();
      }
    } else if (word == "-r"||word == "--remove"){
      if(argc > 2) {
        try{
          removeTask(stoi(argv[2]));
        }catch(...){printf("-r [INDEX] Remove Item\n");}
        saveConfig();
      } 
    } else if (word == "-t"||word == "--toggle"){
      if(argc > 2) {
        try{
          toggleTask(stoi(argv[2]));
        }catch(...){printf("-t [INDEX] Toggle Item State\n");}
        saveConfig();
      }
    }else if (word == "-v"||word == "--version"){
      printf(version.c_str());
    }else if (word == "-c"||word == "--clear"){
      clearTasks(false);
      saveConfig();
    }else if (word == "-ca"||word == "--clear-all"){
      clearTasks(true);
      saveConfig();
    }else if (word == "-e"||word == "--edit"){
      if(argc > 2) {
        try{
          editTask(stoi(argv[2]),string(argv[3]));
        }catch(...){printf("-e [INDEX] [NAME] Edit Item\n");}
        saveConfig();
      }
    }else if(word == "-x"||word == "--extend"){
      if(argc > 2) {
        try{
          extendTask(stoi(argv[2]),stoi(argv[3]));
        }catch(...){printf("-x [INDEX] [+DAYS] Extend Item\n");}
        saveConfig();
      }
    }else{
      printf("Invalid Argument");
    }
    return 1;
  }
  return 0;
}

void spacerPrint(char spc) {
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  for(int i = 0; i < w.ws_col; i++) {
    printf("%c",spc);
  }
}

void endPrint(string text, int width,int stringlen){
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  for(int i = 0; i < w.ws_col-width-stringlen+1; i++) {
    printf(" ");
  }
  printf("%s",text.c_str());
}

void generateConfig() {
  root["tasks"][0]["timestamp"] = systime + day;
  root["tasks"][0]["name"] = "Task 1";
  root["tasks"][0]["done"] = false;
  mkdir((string(getenv("HOME"))+"/.config/todocli").c_str(),0777);
  ofstream stream;
  stream.open(configPath);
  StreamWriterBuilder builder;
  builder.settings_["indentation"] = "\t";
  builder.newStreamWriter() -> write(root, &stream);
  stream.close();
}

void loadConfig() {
  ifstream f;
  f.open(configPath);
  if(f.is_open() == false) {
    generateConfig();
  }else{
    f >> root;
  }
  f.close();
}

void saveConfig() {
  ofstream stream;
  stream.open(configPath);
  StreamWriterBuilder builder;
  builder.settings_["indentation"] = "\t";
  builder.newStreamWriter() -> write(root, &stream);
  stream.close();
}

void draw() {
  if(root["tasks"].empty()){
    return;
  } 
  //draw the screen
  spacerPrint('-');
  int loopindex = 1;
  for(Json::Value x : root["tasks"]){
    string listItem = to_string(loopindex) + ". \"" + x["name"].asString() + "\"";
    listItem = listItem + " " +  formatTimeLeft(x["timestamp"].asInt()-systime) +(x["timestamp"] < systime ? " past":" left");
    printf("%s", listItem.c_str());
    char buffer [26];
    time_t rawtime = x["timestamp"].asInt();
    strftime (buffer,80,"%c",localtime(&rawtime));
    string time = buffer;
    time.erase(time.length()-5,time.length());
    if(x["done"].asBool() == true) {
      endPrint(" \033[1;32m[DONE]\033[0m\n",listItem.length(),8);
    }else if (x["timestamp"].asInt() < systime) {
      endPrint(time+" \033[1;31m[OVERDUE]\033[0m\n",listItem.length(),30);
    } else {
      endPrint(time +" [TODO]\n",listItem.length(),27);
    }
    listItem = "";
    loopindex++;
  }
  spacerPrint('-');
}

void addTask(string name, int timestamp) {
  int size = root["tasks"].size();
  root["tasks"][size]["timestamp"] = timestamp;
  root["tasks"][size]["name"] = name;
  root["tasks"][size]["done"] = false;
}

void removeTask(int index) {
  Value removed;
  checkTasks(index-1) ? root["tasks"].removeIndex(index-1,&removed) : printf("Invalid Index");
}
void clearTasks(bool fullc) {
  for(int i = root["tasks"].size()-1; i >= 0; i--) {
    if(root["tasks"][i]["done"].asBool()) {
      removeTask(i+1);
    }else if(fullc) {
      removeTask(i+1);
    }
  }
}
void editTask(int index, string name){
  checkTasks(index-1) ? root["tasks"][index-1]["name"] = name : printf("Invalid Index");
}
void toggleTask(int index) {
  checkTasks(index-1) ? root["tasks"][index-1]["done"] = !root["tasks"][index-1]["done"].asBool() : printf("Invalid Index");
}

void extendTask(int index, int timestamp) {
  checkTasks(index-1) ? root["tasks"][index-1]["timestamp"] = root["tasks"][index-1]["timestamp"].asInt() + (day*timestamp) : printf("Invalid Index");
}

bool checkTasks(int index) {
  return root["tasks"].isValidIndex(index);
}

string formatTimeLeft(int timestamp) {
  int days = timestamp / day;
  int hours = (timestamp % day) / 3600;
  string time = to_string(abs(days)) + "d, " + to_string(abs(hours)) + "h";
  return time;
}

