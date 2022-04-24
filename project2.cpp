#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

void readFile(string file, vector<int>& resourceTotal, map<int, vector<int>>& procRqsts, vector<vector<int>>& rcrsAlloc)
{
  ifstream fin(file);

  int processCount;
  int resourceCount; 

  string line;
  string stringNum;

  int position;
  int commaPos;
  int numLen; 
  int numEndPos;

  while(getline(fin, line))
  {
    if(line.empty()) continue; //skip blank lines
    if(line[0] == '\r') continue; //skip blank lines
    if(line[0] == '%') continue; //skip comment lines

    if(line.find("num_processes=") != string::npos)
    {
      stringNum = line.substr(line.find("=")+1);
      processCount = stoi(stringNum);
      continue;
    }

    if(line.find("num_resources=") != string::npos)
    {
      stringNum = line.substr(line.find("=")+1);
      resourceCount = stoi(stringNum);
      continue;
    }

    if(resourceTotal.empty())
    {
        position = 0;
      
        while(position < line.length()){
        commaPos = line.find(",", position);
        if(commaPos == string::npos) commaPos = line.length();
        numLen = commaPos - position;

        stringNum = line.substr(position, numLen);
        resourceTotal.push_back(stoi(stringNum));

        position = commaPos + 1;
      }

      continue;
    } //end if resourceTotal.empty()
    
    //if here, must be at matrix
    if(procRqsts.size() < processCount) //then this line represents a process
    { 
      vector<int> newProc;
      int resourceIndex = resourceCount;

      numEndPos = line.length()-1;
      
      while(resourceIndex != 0)
      {
        commaPos = line.rfind(",", numEndPos);
        numLen = numEndPos - commaPos;

        stringNum = line.substr(commaPos + 1, numLen);

        newProc.insert(newProc.begin(), stoi(stringNum));//push_front

        numEndPos = commaPos - 1;
        --resourceIndex;
      }

      procRqsts.emplace(procRqsts.size(), newProc); //add row

      continue;
    } //if procRqsts.size() < numProcs

    //if here, then the current line MUST represent a resource
    vector<int> newRcrs;
    int procIndex = 0;

    position = 0;
    
    while(procIndex != processCount){
      commaPos = line.find(",", position);
      numLen = commaPos - position;

      stringNum = line.substr(position, numLen);
      
      newRcrs.push_back(stoi(stringNum));

      position = commaPos + 1;
      ++procIndex;
    }

    rcrsAlloc.push_back(newRcrs);//add row

  }
}

int main(int numArgs, char* argLst[]){
  
    if (numArgs != 2)
    {
        cout << "How to run: ./a.out <filename> " << endl;
        cout << "<filename> is file where matrix is stored" << endl;
        return 1;
    }

  string file = argLst[1];

  vector<int> totalRcrs, availRcrs;
  vector<vector<int>> rcrsAlloc;
  map<int, vector<int>> procRqsts;

  readFile(file, totalRcrs, procRqsts, rcrsAlloc);

  return 0;

}