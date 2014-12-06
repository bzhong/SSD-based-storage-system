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
    Disk() {}
    virtual ~Disk() {}
    long double get_total_exec_time(); // unit: ms
    BigUInt get_current_free_space();
    BigUInt get_capacity_size();
    virtual BigUInt get_buffer_size();
    virtual int Write(const FileOp& file_operation)=0;
    virtual int Read(const FileOp& file_operation)=0;
    virtual int Delete(const FileOp& file_operation)=0;
    virtual bool Find(const FileOp& file_operation)=0;
    virtual void set_idle_signal(bool value);
    BigUInt GetTransferTimeDelay();
protected:
    long double total_exec_time_;
    BigUInt read_speed_; // unit: Byte
    BigUInt write_speed_; // unit: Byte
    BigUInt capacity_size_; // unit: Byte
    BigUInt current_free_space_; // unit: Byte
    bool idle_signal_;
    BigUInt transfer_time_delay_;

};

class HDD: public Disk {
public:
    HDD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size, const long double& s_time); // seek time: ms
    ~HDD() {}
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
    ~SSD() {}
    int Write(const FileOp& file_operation);
    int Read(const FileOp& file_operation);
    int Delete(const FileOp& file_operation);
    bool Find(const FileOp& file_operation);
    BigUInt get_buffer_size();
private:
    unordered_map<string, FileOp> contents_;
    BigUInt buffer_size_;
};

#endif /* defined(__SSD-based-storage-system__ssd__) */
