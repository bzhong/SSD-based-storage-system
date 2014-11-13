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

long double ReplaceAlgo::get_total_exec_time() {
    return total_exec_time_ * 1000000; // us
}

bool FIFOAlgo::Replace(Disk* ssd, Disk* hdd, const FileOp& file_operation) {
    if (file_pool_.empty()) {
        return false;
    }
    ssd->ReleaseSpaceByDeleteFile(file_pool_.front().file_size);
    hdd->AddFileAndUpdateTotalTime(file_pool_.front().op_type, file_pool_.front().file_size);
    file_pool_.pop();
    return true;
}

void FIFOAlgo::ExecFileOp(const FileOp& file_operation, Disk* ssd, Disk* hdd) {
    clock_t begin_time, end_time;
    begin_time = clock();
    while (file_operation.file_size > ssd->get_current_free_space()) {
        if (!Replace(ssd, hdd, file_operation)) {
            cerr << "the request size is larger than maximum. Ignored." << endl;
            return;
        }
    }
    file_pool_.push(file_operation);
    end_time = clock();
    total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
    ssd->AddFileAndUpdateTotalTime(file_operation.op_type, file_operation.file_size);
}

MQAAlgo::MQAAlgo(const int& number_of_tier) {
    file_pool_.resize(number_of_tier);
    total_exec_time_ = 0;
}

bool MQAAlgo::Replace(Disk *ssd, Disk* hdd, const FileOp& file_operation) {
    int last_tier = (int)file_pool_.size() - 1;
    while (file_pool_[last_tier].empty()) {
        --last_tier;
        if (last_tier < 0) {
            return false;
        }
    }
    ssd->ReleaseSpaceByDeleteFile(file_pool_[last_tier].front().file_size);
    hdd->AddFileAndUpdateTotalTime(file_pool_[last_tier].front().op_type, file_pool_[last_tier].front().file_size);
    file_pool_[last_tier].pop_front();
    return true;
}

void MQAAlgo::ExecFileOp(const FileOp& file_operation, Disk *ssd, Disk* hdd) {
    clock_t begin_time, end_time;
    begin_time = clock();
    if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
        pair<int, list<FileOp>::iterator> comb = file_search_table_[file_operation.file_name];
        comb.second->access_time = system_init_time + time(NULL);
        file_pool_[comb.first].push_back(*comb.second);
        file_pool_[comb.first].erase(comb.second);
    }
    else { // assume tier0: < 32KB, tier1: (32KB, 1MB), tier2: (1MB, 64MB), tier3: > 64MB
        while (file_operation.file_size > ssd->get_current_free_space()) {
            if (!Replace(ssd, hdd, file_operation)) {
                cerr << "the request size is larger than maximum. Ignored." << endl;
                return;
            }
        }
        int tier;
        if (file_operation.file_size < TranslateSize("32KB")) {
            tier = 0;
        }
        else if (file_operation.file_size < TranslateSize("1MB")) {
            tier = 1;
        }
        else if (file_operation.file_size < TranslateSize("64MB")) {
            tier = 2;
        }
        else {
            tier = 3;
        }
        file_pool_[tier].push_back(file_operation);
        list<FileOp>::iterator last_element = --file_pool_[tier].end();
        file_search_table_[file_operation.file_name] = pair<int, list<FileOp>::iterator>(tier, last_element);
    }
    end_time = clock();
    total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
    ssd->AddFileAndUpdateTotalTime(file_operation.op_type, file_operation.file_size);
}

