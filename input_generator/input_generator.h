//
//  input_generator.h
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#ifndef SSD_based_storage_system_input_generator_h
#define SSD_based_storage_system_input_generator_h

#include <vector>
#include <cstdlib>
#include "op_structure.h"
using namespace std;


class InputGenerator {
public:
    InputGenerator(const BigUInt& time);
    //FileOp* GenReadOp(const string& file_name, const BigUInt& file_size, const int& file_type, const int& op_type, const BigUInt& access_time);
    //FileOp GenWriteOp(const string& file_name, const BigUInt& file_size, const int& file_type, const int& op_type, const BigUInt& access_time);
    //FileOp GenCreateOp(const string& file_name, const BigUInt& file_size, const int& file_type, const int& op_type, const BigUInt& access_time);
    //FileOp GenDeleteOp(const string& file_name, const BigUInt& file_size, const int& file_type, const int& op_type, const BigUInt& access_time);
    string RandomFileName();
    BigUInt RandomFileSize(BigUInt min_size, BigUInt max_size);
    void set_system_init_time(const BigUInt& value);
    BigUInt GetCurrentTime();
    void GenBatchOp();
    vector<FileOp> get_request_sequence();
    void AddCommand(const Command& command);
    void ClearCommand();
private:
    vector<Command> commands_;
    BigUInt system_init_time_;
    vector<FileOp> request_sequence_;
};

typedef FileOp* (InputGenerator::*op_ptr)(const string& file_name, const BigUInt& file_size, const int& file_type, const int& op_type, const BigUInt& access_time);

#endif
