#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

void readInputs()
{
  // needs to read the text example file found in Project-2-input-example.txt

  // Ignore % symbol as those are comments
  
  // take in num_processes, and num_resources
  
  // take in amount of units in each resource
  
  // take in M, the adjacency array

ifstream inFileStrm(inFileName);

  int numProcs = -1, numRcrs = -1; //-1 is impossible real val

  string line;
  //LOOP THRU lines OF inFile
  while(getline(inFileStrm, line)){
    if((line.empty())||(line[0] == '\r')) continue; //skip blank lines
    if(line[0] == '%') continue; //skip comment lines

    if(line.find("num_processes=") != string::npos){
      string parsedNum = line.substr(line.find("=")+1);
      //cerr << "num_processes: " << parsedNum << endl;
      numProcs = stoi(parsedNum);
      continue;
    }

    if(line.find("num_resources=") != string::npos){
      string parsedNum = line.substr(line.find("=")+1);
      //cerr << "num_resources: " << parsedNum << endl;
      numRcrs = stoi(parsedNum);
      continue;
    }

    if(totalRcrs.empty()){
      size_t numPos = 0;
      //LOOP THRU avail resources line
      while(numPos < line.length()){
        size_t commaPos = line.find(",", numPos);
        if(commaPos == string::npos) commaPos = line.length();
        size_t numLen = commaPos - numPos;

        string numStr = line.substr(numPos, numLen);
        //cerr << "totalRcrs_num: " << numStr << endl;
        totalRcrs.push_back(stoi(numStr));

        numPos = commaPos + 1;
      }//END LOOP THRU avail resources line

      continue;
    }//end if totalRcrs.empty()
    
    //if here, must be at matrix
    if(procRqsts.size() < numProcs){//then this line represents a process
      vector<int> newProc;
      int rcrsIndex = numRcrs;

      size_t numEndPos = line.length()-1;
      //LOOP THRU resources
      while(rcrsIndex != 0){
        size_t commaPos = line.rfind(",", numEndPos);
        size_t numLen = numEndPos - commaPos;

        string numStr = line.substr(commaPos + 1, numLen);

        //cerr << "rcrs_num: " << numStr << endl;
        newProc.insert(newProc.begin(), stoi(numStr));//push_front

        numEndPos = commaPos - 1;
        --rcrsIndex;
      }//END LOOP THRU resources

      procRqsts.emplace(procRqsts.size(), newProc); //add row

      continue;
    }//if procRqsts.size() < numProcs

    //if here, then the current line MUST represent a resource
    vector<int> newRcrs;
    int procIndex = 0;

    size_t numPos = 0;
    //LOOP THRU avail resources line
    while(procIndex != numProcs){
      size_t commaPos = line.find(",", numPos);
      size_t numLen = commaPos - numPos;

      string numStr = line.substr(numPos, numLen);
      //cerr << "proc_num: " << numStr << endl;
      newRcrs.push_back(stoi(numStr));

      numPos = commaPos + 1;
      ++procIndex;
    }//END LOOP THRU avail resources line

    rcrsAlloc.push_back(newRcrs);//add row


}



int main()
{
  // call readInputs
  
  // use reduction method to check for deadlock
  
  // output message
  return 0;

}
