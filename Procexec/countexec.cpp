#include "countexec.h"

const string countExec::mapName = "./fileDir/text";
const string countExec::middleName = "./fileDir/countMiddle";
const string countExec::reduceName = "./fileDir/countReduce";
const string countExec::suffName = ".txt";

countExec::countExec(int m, int r) : procExec(m, r)
{
    flag = (mapNum + reduceNum);

    for (int i = 0; i < mapNum; i++)
    {
        thread mapTh(&countExec::map_worker, this);
        mapTh.detach();
    }

    for (int i = 0; i < reduceNum; i++)
    {
        thread reduceTh(&countExec::reduce_worker, this);
        reduceTh.detach();
    }

    unique_lock<mutex> tmplock(execLatch);
    masterCv.wait(tmplock, [&]()
                  { return (flag == 0); });
}

void countExec::map_worker()
{
    string fileid = "";
    fstream fin, fout;
    string curfile;
    unordered_map<int, unordered_map<string, int>> countMp;
    while (true)
    {
        {
            unique_lock<mutex> tmplock(execLatch);

            flag--;
            if (!countMp.empty())
            {
                for (auto &[id, mp] : countMp)
                {
                    add_middle(flag, id);
                    curfile = middleName + to_string(flag) + "_" + to_string(id) + suffName;
                    fout.open(curfile, ios::out | ios::trunc);
                    assert(fout.is_open());
                    for (auto &[k, v] : mp)
                    {
                        fout << k << " " << v << " ";
                    }
                    fout.close();
                }

                countMp.clear();
            }
            if (flag == 0)
            {
                masterCv.notify_all();
            }
            mapCv.wait(tmplock);
        }

        if (isExit)
        {
            return;
        }
        while (get_task(fileid))
        {
            curfile = mapName + fileid + suffName;
            fin.open(curfile, ios::in);
            if (!fin.is_open())
            {
                continue;
            }
            string cur = "";
            while (fin >> cur)
            {
                int pagehash = hash<string>()(cur) % reduceNum;
                countMp[pagehash][cur]++;
            }
            fin.close();
        }
    }
    return;
}

void countExec::reduce_worker()
{
    string fileid = "";
    fstream fin, fout;
    string curfile;
    unordered_map<string, int> countMp;

    while (true)
    {
        {
            unique_lock<mutex> tmplock(execLatch);
            flag--;
            if (!countMp.empty())
            {
                add_reduce(flag);
                curfile = reduceName + to_string(flag) + suffName;
                fout.open(curfile, ios::out | ios::trunc);
                assert(fout.is_open());
                for (auto &[str, v] : countMp)
                {
                    fout << str << " " << v << " ";
                }
                countMp.clear();
                fout.close();
            }
            if (flag == 0)
            {
                masterCv.notify_all();
            }
            reduceCv.wait(tmplock);
        }

        if (isExit)
        {
            return;
        }
        while (get_task(fileid))
        {
            string str = "";
            int ifileid = stoi(fileid);
            int val = 0;
            for (int i = 0; i < mapNum; i++)
            {
                if (!middleMap[i].count(ifileid))
                {
                    continue;
                }
                curfile = middleName + to_string(i) + "_" + fileid + suffName;
                fin.open(curfile, ios::in);
                if (!fin.is_open())
                {
                    continue;
                }

                while (fin >> str >> val)
                {
                    countMp[str] += val;
                }
                fin.close();
            }
        }
    }
    return;
}

void countExec::out_res()
{
    fstream fin;
    string curfile;
    for (int i : reduceSet)
    {
        curfile = reduceName + to_string(i) + suffName;
        fin.open(curfile, ios::in);
        if (!fin.is_open())
        {
            continue;
        }
        string cur = "";
        int val = 0;
        while (fin >> cur >> val)
        {
            cout << cur << " " << val << endl;
        }
        fin.close();
    }
    return;
}