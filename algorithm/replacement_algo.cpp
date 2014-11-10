//
//  FIFO.cpp
//  coen283_project
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "replacement_algo.h"

FIFOAlgo::FIFOAlgo() {
    total_exec_time_ = 0;
}

long double FIFOAlgo::get_total_exec_time() {
    return total_exec_time_ * 1000000; // us
}

bool FIFOAlgo::Replace(Disk* ssd) {
    if (file_pool_.empty()) {
        return false;
    }
    ssd->ReleaseSpaceByDeleteFile(file_pool_.front().file_size);
    file_pool_.pop();
    return true;
}

void FIFOAlgo::ExecFileOp(const FileOp& file_operation, Disk* ssd) {
    clock_t begin_time, end_time;
    begin_time = clock();
    while (file_operation.file_size > ssd->get_current_free_space()) {
        if (!Replace(ssd)) {
            cerr << "the request size is larger than maximum. Ignored." << endl;
            return;
        }
    }
    file_pool_.push(file_operation);
    end_time = clock();
    total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
}