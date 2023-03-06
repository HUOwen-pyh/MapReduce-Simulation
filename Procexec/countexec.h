#ifndef _COUNTEXEC_
#define _COUNTEXEC_

#include <fstream>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <string>
#include "procexec.h"
using namespace std;

class countExec : public procExec
{
    static const string mapName;
    static const string middleName;
    static const string reduceName;
    static const string suffName;

protected:
    void map_worker();
    void reduce_worker();
    void out_res();

public:
    countExec() = delete;
    ~countExec() = default;
    explicit countExec(int m, int r);
};

#endif