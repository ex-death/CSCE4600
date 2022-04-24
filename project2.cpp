#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

// Bret Hogan, Michael Bertucci, Brandon Whitney  
// Program used to look for deadlocks within a given matrix file

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
      
        while(position < line.length())
        {
        commaPos = line.find(",", position);
        if(commaPos == string::npos) commaPos = line.length();
        numLen = commaPos - position;

        stringNum = line.substr(position, numLen);
        resourceTotal.push_back(stoi(stringNum));

        position = commaPos + 1;
       }

      continue;
    }
    
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

    rcrsAlloc.push_back(newRcrs); //add row

  }
}

vector<int> available(vector<int> const& totalRcrs, vector<vector<int>> const& rcrsAlloc) // function checks available resources
{
    
    vector<int> result; // holds results
    for (int rowNum = 0; rowNum < rcrsAlloc.size(); ++rowNum)
    {
        int total = 0;
        for (int colNum = 0; colNum < rcrsAlloc[0].size(); ++colNum)
        {   
            total += rcrsAlloc[rowNum][colNum]; // calculate total
        }
        
        result.push_back(totalRcrs[rowNum] - total); // add back
    }
    
    return result;
}

void reduce(vector<int>& availRcrs, map<int, vector<int>>& procRqsts, vector<vector<int>>& rcrsAlloc, bool *deadlock) // function that reduces processes if they are able to reduce
{
     
    while (!procRqsts.empty()) // not reduced
    {
        // flag to determine if contains deadlock
        bool flg = false;
        for (auto processIterator = procRqsts.begin(); processIterator != procRqsts.end(); ++processIterator)
        {
            vector<int> process = (*processIterator).second;

            // create processes number and rescource number
            int processNumber = (*processIterator).first;
            int rescourceNumber;

            // iterate through rescouces
            for (rescourceNumber = 0; rescourceNumber < process.size(); ++rescourceNumber)
            {
                if (process[rescourceNumber] > availRcrs[rescourceNumber])
                {
                    break;
                }
            }

            // if process can not go on, then continue
            if (rescourceNumber != process.size())
            {
                // move to next process
                continue;
            }

            // loop and update avaiable rescources 
            for (int rescourceNumber = 0; rescourceNumber < rcrsAlloc.size(); ++rescourceNumber)
            {
                // create rescource vector to hold info
                vector<int> rescourcesVect = rcrsAlloc[rescourceNumber];
                availRcrs[rescourceNumber] += rescourcesVect[processNumber];
                rescourcesVect[processNumber] = 0;
            }
            // remove process
            procRqsts.erase(processIterator);
            // set flg to true
            flg = true;

            // go to next step in process 
            break;
        }

        if (!flg)
        {
            //deadlock has occured
            *deadlock = true;
            return;
        }
    }
}
bool determineDeadlockState(vector<int> &availRcrs,map<int, vector<int>>& procRqsts, vector<vector<int>>& rcrsAlloc)
{
  //reduce(availRcrs, procRqsts, rcrsAlloc);
  
  return !procRqsts.empty();
}

int main(int numArgs, char* argLst[]) {

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
    bool *deadlock;

    *deadlock = false;

    readFile(file, totalRcrs, procRqsts, rcrsAlloc);

    availRcrs = available(totalRcrs, rcrsAlloc);

    reduce(availRcrs, procRqsts, rcrsAlloc, deadlock);

    if(deadlock)
    {
        cout << endl << "There is a Deadlock in this system." << endl;
    }
    else
    {
        cout << endl << "No Deadlock found." << endl; 
    }
    return 0;

}
