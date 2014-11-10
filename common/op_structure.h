//
//  op_structure.h
//  SSD-based-storage-system
//
//  Created by Brady on 11/9/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#ifndef SSD_based_storage_system_op_structure_h
#define SSD_based_storage_system_op_structure_h

#include <string>
#include <iostream>
#include <ctime>
using namespace std;

typedef unsigned long long BigUInt;
const int kFileNameLength = 20;


enum FileOpType {
    kReadOp = 0,
    kWriteOp = 1,
    kCreateOp = 2,
    kDeleteOp = 3,
};

enum FileType {
    kText = 0,
    kPicture = 1,
    kAudio = 2,
    kVideo = 3,
};

enum ReplacementAlgoType {
    kFIFO = 0,
    kLRU = 1,
    kRR = 2,
    kOptimizedAlgo = 3,
};


struct FileOp {
    string file_name;
    int op_type;
    BigUInt file_size;
    int file_type;
    BigUInt access_time;
    FileOp(): file_name(""), op_type(-1), file_size(0), file_type(-1), access_time(0) {}
};

struct Command {
    BigUInt file_size_min;
    BigUInt file_size_max;
    BigUInt file_number;
    int file_operation;
    int file_type;
    Command(): file_size_min(0), file_size_max(1024), file_number(1), file_operation(kReadOp), file_type(kText) {}
};

#endif
