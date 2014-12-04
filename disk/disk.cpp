//
//  ssd.cpp
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "disk.h"

long double Disk::get_total_exec_time() {
    return total_exec_time_ * 1000; 
}

BigUInt Disk::get_capacity_size() {
    return capacity_size_;
}

BigUInt Disk::get_current_free_space() {
    return current_free_space_;
}

BigUInt Disk::get_buffer_size() {
    return 0;
}

SSD::SSD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size) {
    total_exec_time_ = 0;
    read_speed_ = r_speed;
    write_speed_ = w_speed;
    capacity_size_ = c_size;
    current_free_space_ = capacity_size_;
    buffer_size_ = capacity_size_ / 4;
}

BigUInt SSD::get_buffer_size() {
    return buffer_size_;
}

int SSD::Write(const FileOp &file_operation) {
    if (contents_.find(file_operation.file_name) != contents_.end()) {
        // currently we don't support rewrite different size
        //current_free_space_ = current_free_space_ + contents_.at(file_operation.file_name).file_size - file_operation.file_size;
        contents_[file_operation.file_name] = file_operation;
        total_exec_time_ += (long double)file_operation.file_size / write_speed_;
        return 0;
    }
    else if (file_operation.file_size <= current_free_space_) {
        current_free_space_ -= (long double)file_operation.file_size;
        contents_[file_operation.file_name] = file_operation;
        total_exec_time_ += (long double)file_operation.file_size / write_speed_;
        return 0;
    }
    else {
        // list error code
        return 1;
    }
}

int SSD::Read(const FileOp &file_operation) {
    if (contents_.find(file_operation.file_name) != contents_.end()) {
        total_exec_time_ += (long double)file_operation.file_size / read_speed_;
        return 0;
    }
    // list error code
    return 1;
}

int SSD::Delete(const FileOp &file_operation) {
    if (contents_.find(file_operation.file_name) != contents_.end()) {
        contents_.erase(file_operation.file_name);
        current_free_space_ += file_operation.file_size;
        return 0;
    }
    else {
        // list error code
        return 1;
    }
}

bool SSD::Find(const FileOp &file_operation) {
    if (contents_.find(file_operation.file_name) != contents_.end()) {
        return true;
    }
    else {
        return false;
    }
}

HDD::HDD(const BigUInt& r_speed, const BigUInt& w_speed, const BigUInt& c_size, const long double& s_time) {
    total_exec_time_ = 0;
    read_speed_ = r_speed;
    write_speed_ = w_speed;
    capacity_size_ = c_size;
    current_free_space_ = capacity_size_;
    seek_time_ = s_time;
}

int HDD::Write(const FileOp &file_operation) {
    //if (file_operation.file_size <= current_free_space_) {
        contents_[file_operation.file_name] = file_operation;
        total_exec_time_ += (long double)file_operation.file_size / write_speed_ + seek_time_ / 1000;
        //current_free_space_ -= (long double)file_operation.file_size;
        return 0;
    //}
    /*else {
        cerr << "HDD write error." << endl;
        // list error code
        return 1;
    }*/
}

int HDD::Read(const FileOp& file_operation) {
    if (contents_.find(file_operation.file_name) != contents_.end()) {
        total_exec_time_ += (long double)file_operation.file_size / read_speed_ + seek_time_ / 1000;
        return 0;
    }
    // list error code
    return 1;
}

int HDD::Delete(const FileOp &file_operation) {
    if (contents_.find(file_operation.file_name) != contents_.end()) {
        contents_.erase(file_operation.file_name);
        //current_free_space_ += file_operation.file_size;
        return 0;
    }
    else {
        // list error code
        return 1;
    }
}

bool HDD::Find(const FileOp &file_operation) {
    if (contents_.find(file_operation.file_name) != contents_.end()) {
        return true;
    }
    else {
        return false;
    }
}
