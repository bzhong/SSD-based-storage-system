//
//  input_generator.cpp
//  coen283_project
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "input_generator.h"
using namespace std;

InputGenerator::InputGenerator(const BigUInt& time) {
    system_init_time_ = time;
}

void InputGenerator::set_system_init_time(const BigUInt& value) {
    system_init_time_ = value;
}

/*FileOp* InputGenerator::GenReadOp(const string& file_name, const BigUInt& file_size, const int& file_type, const int& op_type, const BigUInt& access_time) {
    FileOp *cur_op = new FileOp();
    cur_op->file_name = file_name;
    cur_op->op_type = op_type;
    cur_op->file_size = file_size;
    cur_op->file_type = file_type;
    cur_op->access_time = access_time;
    return cur_op;
}*/

/*
FileOp InputGenerator::GenWriteOp() {

}

FileOp InputGenerator::GenCreateOp() {

}

FileOp InputGenerator::GenDeleteOp() {

}
*/

string InputGenerator::RandomFileName() {
    string file_name;
    for (int count = 0; count < kFileNameLength; ++count) {
        int classification = (count == 0? (rand() % 3 + 1) : (rand() % 4));
        switch (classification) {
            case 0:
                file_name.append(1, rand() % 10 + '0');
                break;
            case 1:
                file_name.append(1, rand() % 26 + 'A');
                break;
            case 2:
                file_name.append(1, rand() % 26 + 'a');
                break;
            case 3:
                file_name.append(1, '_');
                break;
        }
    }
    return file_name;
}

BigUInt InputGenerator::RandomFileSize(BigUInt min_size, BigUInt max_size) {
    return rand() % (max_size - min_size) + min_size;
}

BigUInt InputGenerator::GetCurrentTime() {
    time_t timer;
    time(&timer);
    return (BigUInt)timer + system_init_time_;
}

void InputGenerator::GenBatchOp() {
    vector<FileOp> batch_op;
    for (int command_num = 0; command_num < commands_.size(); ++command_num) {
        op_ptr foperation;
        //foperation = &InputGenerator::GenReadOp;
        for (int file_num = 0; file_num < commands_[command_num].file_number; ++file_num) {
            BigUInt file_min = commands_[command_num].file_size_min;
            BigUInt file_max = commands_[command_num].file_size_max;
            FileOp tmp;
            tmp.file_name = RandomFileName();
            tmp.file_size = RandomFileSize(file_min, file_max);
            tmp.file_type = commands_[command_num].file_type;
            tmp.op_type = commands_[command_num].file_operation;
            tmp.access_time = GetCurrentTime();
            //FileOp* tmp = (this->*foperation)(RandomFileName(), RandomFileSize(file_min, file_max), commands_[command_num].file_type, commands_[command_num].file_operation, GetCurrentTime());
            batch_op.push_back(tmp);
        }
    }
    request_sequence_ = batch_op;
}

vector<FileOp> InputGenerator::get_request_sequence() {
    return request_sequence_;
}

void InputGenerator::AddCommand(const Command& command)
{
    commands_.push_back(command);
}

void InputGenerator::ClearCommand() {
    commands_.clear();
}