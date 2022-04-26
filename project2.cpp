#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

// Bret Hogan, Michael Bertucci, Brandon Whitney  
// Program used to look for deadlocks within a given matrix file

void readFile(string file, vector<int>& rTotalCount, map<int, vector<int>>& pReq, vector<vector<int>>& rAlloc)
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

    if(rTotalCount.empty())
    {
        position = 0;
      
        while(position < line.length())
        {
        commaPos = line.find(",", position);
        if(commaPos == string::npos) commaPos = line.length();
        numLen = commaPos - position;

        stringNum = line.substr(position, numLen);
        rTotalCount.push_back(stoi(stringNum));

        position = commaPos + 1;
       }

      continue;
    }
    
    if(pReq.size() < processCount) //then this line represents a process
    { 
      vector<int> pNew;
      int resourceIndex = resourceCount;

      numEndPos = line.length()-1;
      
      while(resourceIndex != 0)
      {
        commaPos = line.rfind(",", numEndPos);
        numLen = numEndPos - commaPos;

        stringNum = line.substr(commaPos + 1, numLen);

        pNew.insert(pNew.begin(), stoi(stringNum));//push_front

        numEndPos = commaPos - 1;
        --resourceIndex;
      }

      pReq.emplace(pReq.size(), pNew); //add row

      continue;
    } //if pReq.size() < numProcs

    vector<int> rNew;
    int pIndex = 0;

    position = 0;
    
    while(pIndex != processCount){
      commaPos = line.find(",", position);
      numLen = commaPos - position;

      stringNum = line.substr(position, numLen);
      
      rNew.push_back(stoi(stringNum));

      position = commaPos + 1;
      ++pIndex;
    }

    rAlloc.push_back(rNew); //add row

  }
}

vector<int> available(vector<int> const& rTotal, vector<vector<int>> const& rAlloc) // function checks available resources
{
    
    vector<int> result; // holds results
    for (int rowNum = 0; rowNum < rAlloc.size(); ++rowNum)
    {
        int total = 0;
        for (int colNum = 0; colNum < rAlloc[0].size(); ++colNum)
        {   
            total += rAlloc[rowNum][colNum]; // calculate total
        }
        
        result.push_back(rTotal[rowNum] - total); // add back
    }
    
    return result;
}

void reduce(vector<int>& rFree, map<int, vector<int>>& pReq, vector<vector<int>>& rAlloc) // function that reduces processes if they are able to reduce
{
     
    while (!pReq.empty()) // not reduced
    {
        // flag to determine if contains deadlock
        bool flg = false;
        for (auto iProc = pReq.begin(); iProc != pReq.end(); ++iProc)
        {
            vector<int> process = (*iProc).second;

            // create processes number and resource number
            int pNum = (*iProc).first;
            int rNum;

            // iterate through rescouces
            for (rNum = 0; rNum < process.size(); ++rNum)
            {
                if (process[rNum] > rFree[rNum])
                {
                    break;
                }
            }

            // if process can not go on, then continue
            if (rNum != process.size())
            {
                // move to next process
                continue;
            }

            // loop and update avaiable resources 
            for (int rNum = 0; rNum < rAlloc.size(); ++rNum)
            {
                // create resource vector to hold info
                vector<int> rVector = rAlloc[rNum];
                rFree[rNum] += rVector[pNum];
                rVector[pNum] = 0;
            }
            // remove process
            pReq.erase(iProc);
            // set flg to true
            flg = true;

            // go to next step in process 
            break;
        }

        if (!flg)
        {
            //deadlock has occured
            return;
        }
    }
}
bool determineDeadlockState(vector<int> &rFree,map<int, vector<int>>& pReq, vector<vector<int>>& rAlloc)
{
  reduce(rFree, pReq, rAlloc);
  
  return !pReq.empty();
}

int main(int argc, char* argv[]) {

    if (argc != 2)
    {
        cout << "How to run: ./a.out <filename> " << endl;
        cout << "<filename> is file where matrix is stored" << endl;
        return 1;
    }

    string file = argv[1];

    vector<int> rTotal, rFree;
    vector<vector<int>> rAlloc;
    map<int, vector<int>> pReq;

    readFile(file, rTotal, pReq, rAlloc);

    rFree = available(rTotal, rAlloc);

    if(determineDeadlockState(rFree, pReq, rAlloc))
    {
        cout << endl << "There is a Deadlock in this system." << endl;
    }
    else
    {
        cout << endl << "No Deadlock found." << endl; 
    }
    return 0;

}
