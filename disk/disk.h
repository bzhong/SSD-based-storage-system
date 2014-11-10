//
//  ssd.h
//  coen283_project
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#ifndef __coen283_project__disk__
#define __coen283_project__disk__

#include "op_structure.h"

class Disk {
public:
    virtual void ExecTimeAddedToTotalTime(const int& file_op_type, const BigUInt& file_size);
    virtual void ReleaseSpaceByDeleteFile(const BigUInt& file_size);
    virtual long double get_total_exec_time();
    virtual BigUInt get_current_free_space();
protected:
    long double total_exec_time_;
    BigUInt read_speed_; // unit: B
    BigUInt write_speed_; // unit: B
    BigUInt capacity_size_; // unit: B
    BigUInt current_free_space_; // unit: B
};

class HDD: public Disk {
public:
    HDD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size);
};

class SSD: public Disk {
public:
    SSD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size);
};

#endif /* defined(__coen283_project__ssd__) */
