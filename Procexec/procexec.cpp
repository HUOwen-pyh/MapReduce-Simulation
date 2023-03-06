#include "procexec.h"

const bool procExec::UNREACHABLE = false;

procExec::procExec(int m, int r) : mapNum(m), reduceNum(r), rangeL(0), rangeR(0), isExit(false)
{
    assert(mapNum >= 0 && reduceNum >= 0);
}

void procExec::map_worker()
{
    assert(UNREACHABLE);
}

void procExec::reduce_worker()
{
    assert(UNREACHABLE);
}

void procExec::out_res()
{
    assert(UNREACHABLE);
}

bool procExec::get_task(string &nextIdx)
{
    lock_guard<mutex> qg(queueLock);
    if (taskQueue.empty())
    {
        return false;
    }
    nextIdx = taskQueue.front();
    taskQueue.pop();
    return true;
}

void procExec::add_middle(int key, int val)
{
    lock_guard<mutex> ml(containerLock);
    middleMap[key].insert(val);
    return;
}

void procExec::add_reduce(int val)
{
    lock_guard<mutex> ml(containerLock);
    reduceSet.insert(val);
    return;
}

void procExec::exec(int l, int r)
{
    rangeL = l;
    rangeR = r;
    assert(l >= 0 && r >= l);
    for (int i = rangeL; i <= rangeR; i++)
    {
        taskQueue.push(to_string(i));
    }

    flag += mapNum;
    middleMap.clear();
    {
        unique_lock<mutex> tmplock(execLatch);
        mapCv.notify_all();
        masterCv.wait(tmplock, [&]()
                      { return (flag == 0); });
    }

    for (int i = 0; i < reduceNum; i++)
    {
        taskQueue.push(to_string(i));
    }

    flag += reduceNum;
    reduceSet.clear();
    {
        unique_lock<mutex> tmplock(execLatch);
        reduceCv.notify_all();
        masterCv.wait(tmplock, [&]()
                      { return (flag == 0); });
    }

    out_res();
    return;
}

void procExec::Exit()
{
    isExit = true;
    mapCv.notify_all();
    reduceCv.notify_all();
    return;
}