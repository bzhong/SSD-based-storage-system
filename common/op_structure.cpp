//
//  op_structure.cpp
//  SSD-based-storage-system
//
//  Created by Brady on 11/19/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "op_structure.h"

BigUInt TranslateSize(const string& file_size) {
    unsigned long end_pos = 0;
    BigUInt real_size = 0;
    if (file_size.find("KB") != string::npos) {
        end_pos = file_size.find("KB");
        real_size = stoi(file_size.substr(0, end_pos)) * 1024;
    }
    else if (file_size.find("MB") != string::npos) {
        end_pos = file_size.find("MB");
        real_size = stoi(file_size.substr(0, end_pos)) * 1024 * 1024;
    }
    else if (file_size.find("GB") != string::npos) {
        end_pos = file_size.find("GB");
        BigUInt tmp = stoi(file_size.substr(0, end_pos));
        real_size = tmp * 1024 * 1024 * 1024;
    }
    else if (file_size.find("TB") != string::npos) {
        end_pos = file_size.find("TB");
        real_size = stoi(file_size.substr(0, end_pos)) * 1024 * 1024 * 1024 * 1024;
    }
    else if (file_size.find("B") != string::npos) {
        end_pos = file_size.find("B");
        real_size = stoi(file_size.substr(0, end_pos));
    }
    else {
        real_size = stoi(file_size.substr(0, end_pos));;
    }
    return real_size;
}