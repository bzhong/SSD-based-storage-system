//
//  FIFO.h
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#ifndef __coen283_project__FIFO__
#define __coen283_project__FIFO__

#include "op_structure.h"
#include "disk.h"
using namespace std;

class ReplaceAlgo {
public:
    virtual bool Replace(SSD* ssd, HDD* hdd)=0;
    virtual void ExecFileOp(const FileOp& file_operation, SSD* ssd, HDD* hdd)=0;
};

class FIFOAlgo: public ReplaceAlgo {
public:
    FIFOAlgo();
    bool Replace(SSD* ssd, HDD* hd);
    void ExecFileOp(const FileOp& file_operation, SSD* ssd, HDD* hdd);
    long double get_total_exec_time();
private:
    queue<FileOp> file_pool_;
    long double total_exec_time_;
};

class MQAAlgo: public ReplaceAlgo {
public:
    MQAAlgo(const int& number_of_tier);
    bool Replace(SSD* ssd, HDD* hdd);
    void ExecFileOp(const FileOp& file_opeartion, SSD* ssd, HDD* hdd);
    void ExecReplace(SSD* ssd, HDD* hdd);
    int SetTier(const FileOp& file_operation);
    int AdjustByType(const FileOp& file_operation, const int& tier);
    void UpdateFileSearchTable(const FileOp& file_operation);
    void AddFile(const int& tier, const FileOp& file_operation);
    long double get_total_exec_time();
private:
    vector<list<FileOp> > file_pool_;
    map<string, pair<int, list<FileOp>::iterator> > file_search_table_; // file_name-><bucket_no, pointer*>
    BigUInt basic_threshold; // unit: minute
    BigUInt cur_threshold;
    vector<pair<double, int> > fspace_to_threshold_map;
    unordered_map<int, int> adjust_via_type;
    unordered_map<string, FileType> file_type_map;
    long double total_exec_time_;
    
};

#endif /* defined(__coen283_project__FIFO__) */
