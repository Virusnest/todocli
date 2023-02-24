#include<iostream>
using namespace std;
void spacerPrint(char spc);

void endPrint(string text, int width,int stringlen);

void generateConfig();

int parseArgs(int argc, char *argv[]);

void draw();

void loadConfig();

void saveConfig();

void addTask(string name, int timestamp);

void removeTask(int index);

void toggleTask(int index);

void clearTasks(bool fullc);

void editTask(int index, string name);

void extendTask(int index, int timestamp);

bool checkTasks(int index);

string formatTimeLeft(int timestamp);
