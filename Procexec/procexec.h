#ifndef _PROCEXEC_
#define _PROCEXEC_

#include <cassert>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
using namespace std;

class procExec
{
    static const bool UNREACHABLE;

protected:
    int mapNum;
    int reduceNum;
    int rangeL;
    int rangeR;
    bool isExit;
    queue<string> taskQueue;
    unordered_map<int, unordered_set<int>> middleMap;
    unordered_set<int> reduceSet;

protected:
    mutex queueLock;
    mutex containerLock;
    mutex execLatch;
    condition_variable masterCv;
    condition_variable mapCv;
    condition_variable reduceCv;
    atomic<int> flag = 0;

protected:
    virtual void map_worker();
    virtual void reduce_worker();
    virtual void out_res();

    bool get_task(string &nextIdx);
    void add_middle(int key, int val);
    void add_reduce(int val);

public:
    procExec() = delete;
    ~procExec() = default;
    explicit procExec(int m, int r);
    void exec(int l, int r);
    void Exit();
};

#endif