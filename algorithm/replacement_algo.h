//
//  FIFO.h
//  coen283_project
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#ifndef __coen283_project__FIFO__
#define __coen283_project__FIFO__

#include <queue>
#include <list>
#include <map>
#include "op_structure.h"
#include "disk.h"
using namespace std;

class ReplaceAlgo {
public:
    virtual bool Replace(Disk* ssd) = 0;
    virtual void ExecFileOp(const FileOp& file_operation, Disk* ssd) = 0;
    long double get_total_exec_time();
protected:
    long double total_exec_time_;
};

class FIFOAlgo: public ReplaceAlgo {
public:
    FIFOAlgo();
    bool Replace(Disk* ssd);
    void ExecFileOp(const FileOp& file_operation, Disk* ssd);
private:
    queue<FileOp> file_pool_;
};

class MQAAlgo: public ReplaceAlgo {
public:
    MQAAlgo(const int& number_of_tier);
    bool Replace(Disk* ssd);
    void ExecFileOp(const FileOp& file_opeartion, Disk* ssd);
private:
    vector<list<FileOp> > file_pool_;
    map<string, pair<int, list<FileOp>::iterator> > file_search_table_;
};

#endif /* defined(__coen283_project__FIFO__) */
