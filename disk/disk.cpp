//
//  ssd.cpp
//  coen283_project
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "disk.h"

void Disk::ExecTimeAddedToTotalTime(const int &file_op_type, const BigUInt &file_size) {
    long double exec_time = 0.0;
    switch (file_op_type) {
        case kReadOp:
            exec_time = (long double)file_size / read_speed_;
            break;
        case kWriteOp:
            exec_time = (long double)file_size / write_speed_;
            break;
    }
    total_exec_time_ += exec_time;
    current_free_space_ -= (long double)file_size;
}

void Disk::ReleaseSpaceByDeleteFile(const BigUInt& file_size) {
    current_free_space_ += file_size;
}

long double Disk::get_total_exec_time() {
    return total_exec_time_ * 1000000; //us
}

BigUInt Disk::get_current_free_space() {
    return current_free_space_;
}

SSD::SSD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size) {
    total_exec_time_ = 0;
    read_speed_ = r_speed;
    write_speed_ = w_speed;
    capacity_size_ = c_size;
    current_free_space_ = capacity_size_;
}

HDD::HDD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size) {
    total_exec_time_ = 0;
    read_speed_ = r_speed;
    write_speed_ = w_speed;
    capacity_size_ = c_size;
    current_free_space_ = capacity_size_;
}