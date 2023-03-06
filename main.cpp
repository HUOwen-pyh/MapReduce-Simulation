#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "Procexec/procexec.h"
#include "Procexec/countexec.h"
using namespace std;

static const string cmdReq = "Input your operations or \"help\" for usage: ";
static const string helpReq = "\nUsage:\n\n\"add\" to add new items into specified file\n\"count\" for words' total numbers\n\"sort\" for the index of files where each word appears\n\"exit\" to exit the process\n\n";
static const string addReq = "Input index of file and your string: ";
static const string rangeReq = "Input the query range: ";
static const string errorInfo = "No such operation, input \"help\" for usage\n";
static int mapNum = 0, reduceNum = 0;

int main(int argc, char **argv)
{
    assert(argc == 3);
    mapNum = stoi(string(argv[1])), reduceNum = stoi(string(argv[2]));

    string curReq;
    int rangeL = 0, rangeR = 0;
    int fileIdx = 0;
    string addStr;
    fstream outFile;
    countExec countSys(mapNum, reduceNum);

    while (true)
    {
        cout << cmdReq;
        cin >> curReq;
        if (curReq == "exit")
        {
            countSys.Exit();
            break;
        }
        if (curReq == "help")
        {
            cout << helpReq;
            continue;
        }
        if (curReq == "add")
        {
            cout << addReq;
            cin >> fileIdx >> addStr;
            assert(fileIdx >= 0);
            string fileName = "./fileDir/text";
            fileName += to_string(fileIdx);
            fileName += ".txt";
            outFile.open(fileName, ios::app | ios::out);
            outFile << addStr << " ";
            outFile.close();
            continue;
        }
        if (curReq == "count")
        {
            cout << rangeReq;
            cin >> rangeL >> rangeR;
            countSys.exec(rangeL, rangeR);
            continue;
        }
        if (curReq == "sort")
        {
            cout << "TO DO!\n";
            continue;
        }
        cout << errorInfo;
    }

    return 0;
}