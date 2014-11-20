//
//  ssd.h
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#ifndef __coen283_project__disk__
#define __coen283_project__disk__

#include "op_structure.h"

class Disk {
public:
    long double get_total_exec_time();
    BigUInt get_current_free_space();
    BigUInt get_capacity_size();
    virtual int Write(const FileOp& file_operation);
    virtual int Read(const FileOp& file_operation);
    virtual bool Find(const FileOp& file_operation);
protected:
    long double total_exec_time_;
    BigUInt read_speed_; // unit: B
    BigUInt write_speed_; // unit: B
    BigUInt capacity_size_; // unit: B
    BigUInt current_free_space_; // unit: B
};

class HDD: public Disk {
public:
    HDD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size, const long double& s_time); // seek time: ms
    int Write(const FileOp& file_operation);
    int Read(const FileOp& file_operation);
    int Delete(const FileOp& file_operation);
    bool Find(const FileOp& file_operation);
private:
    unordered_map<string, FileOp> contents_;
    long double seek_time_;
};

class SSD: public Disk {
public:
    SSD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size);
    int Write(const FileOp& file_operation);
    int Read(const FileOp& file_operation);
    int Delete(const FileOp& file_operation);
    BigUInt get_buffer_size();
private:
    unordered_map<string, FileOp> contents_;
    BigUInt buffer_size_;
};

#endif /* defined(__coen283_project__ssd__) */
