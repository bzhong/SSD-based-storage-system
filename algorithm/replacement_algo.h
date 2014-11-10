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
#include "op_structure.h"
#include "disk.h"
using namespace std;

class ReplaceAlgo {
public:
    virtual bool Replace(Disk* ssd) = 0;
    virtual void ExecFileOp(const FileOp& file_operation, Disk* ssd) = 0;
    virtual long double get_total_exec_time() = 0;
};

class FIFOAlgo: public ReplaceAlgo {
public:
    FIFOAlgo();
    bool Replace(Disk* ssd);
    void ExecFileOp(const FileOp& file_operation, Disk* ssd);
    long double get_total_exec_time();
private:
    queue<FileOp> file_pool_;
    long double total_exec_time_;
};

#endif /* defined(__coen283_project__FIFO__) */
