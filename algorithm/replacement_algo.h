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
    virtual ~ReplaceAlgo() {}
    virtual void ExecReplace()=0;
    virtual void ExecFileOp(const FileOp& file_operation)=0;
    long double get_total_exec_time();
    long double get_ssd_exec_time();
    long double get_hdd_exec_time();
    BigUInt GetHitCount();
    BigUInt GetReqCount();
protected:
    long double total_exec_time_;
    Disk *ssd_, *hdd_;
    BigUInt hit_count_;
    BigUInt req_count_;
};

class MQAAlgo: public ReplaceAlgo {
public:
    MQAAlgo(const int& number_of_tier = 4,
            const string& ssd_read_speed = "550GB",
            const string& ssd_write_speed = "500GB",
            const string& ssd_capacity = "256GB",
            const string& hdd_read_speed = "126GB",
            const string& hdd_write_speed = "126GB",
            const string& hdd_capacity = "4TB",
            const long double& hdd_seek_time = 10);
    ~MQAAlgo();
    bool Replace(const BigUInt& needed_size);
    void ExecFileOp(const FileOp& file_operation);
    void ExecReplace();
    int SetTier(const FileOp& file_operation);
    int AdjustByType(const FileOp& file_operation, const int& tier);
    void UpdateFileSearchTable(const FileOp& file_operation);
    void AddFile(const int& tier, const FileOp& file_operation);
private:
    vector<list<FileOp> > file_pool_;
    unordered_map<string, pair<int, list<FileOp>::iterator> > file_search_table_; // file_name-><bucket_no, pointer*>
    BigUInt basic_threshold_; // unit: minute
    BigUInt cur_threshold_;
    vector<pair<double, int> > fspace_to_threshold_map_;
    unordered_map<int, int> adjust_via_type_;
    unordered_map<string, FileType> file_type_map_;
};

class FIFOAlgo: public ReplaceAlgo {
public:
    FIFOAlgo(const string& ssd_read_speed = "550GB",
             const string& ssd_write_speed = "500GB",
             const string& ssd_capacity = "256GB",
             const string& hdd_read_speed = "126GB",
             const string& hdd_write_speed = "126GB",
             const string& hdd_capacity = "4TB",
             const long double& hdd_seek_time = 10);
    ~FIFOAlgo();
    void ExecReplace();
    void ExecFileOp(const FileOp& file_operation);
private:
    queue<FileOp> file_pool_;
    unordered_set<string> file_search_table_;
};

class LRUAlgo: public ReplaceAlgo {
public:
    LRUAlgo(const string& ssd_read_speed = "550GB",
            const string& ssd_write_speed = "500GB",
            const string& ssd_capacity = "256GB",
            const string& hdd_read_speed = "126GB",
            const string& hdd_write_speed = "126GB",
            const string& hdd_capacity = "4TB",
            const long double& hdd_seek_time = 10);
    ~LRUAlgo();
    void ExecReplace();
    void UpdateFileSearchTable(const FileOp& file_operation);
    void ExecFileOp(const FileOp& file_operation);
private:
    list<FileOp> file_pool_;
    unordered_map<string, list<FileOp>::iterator> file_search_table_;
};

/*class ClockAlgo: public ReplaceAlgo {
public:
    ClockAlgo(const string& ssd_read_speed = "550GB",
            const string& ssd_write_speed = "500GB",
            const string& ssd_capacity = "256GB",
            const string& hdd_read_speed = "126GB",
            const string& hdd_write_speed = "126GB",
            const string& hdd_capacity = "4TB",
            const long double& hdd_seek_time = 10);
    ~ClockAlgo();
    void ExecReplace();
    void UpdateFileSearchTable(const FileOp& file_operation);
    void ExecFileOp(const FileOp& file_operation);
private:
    
};*/

#endif /* defined(__coen283_project__FIFO__) */
