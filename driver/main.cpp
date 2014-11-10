//
//  main.cpp
//  coen283_project
//
//  Created by Brady on 11/7/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include <iostream>
#include <ctime>
#include "input_generator.h"
using namespace std;

int main() {
    srand(time(NULL));
    InputGenerator* input_generator = new InputGenerator(2000);
    Command tmp_command;
    tmp_command.file_number = 100;
    
    input_generator->AddCommand(tmp_command);
    input_generator->GenBatchOp();
    vector<FileOp> tmp = input_generator->get_request_sequence();
    
    for (int i = 0; i < tmp.size(); ++i) {
        cout << tmp[i].file_name << "\t" << tmp[i].file_size
        << "\t" << tmp[i].file_type << "\t" << tmp[i].access_time << endl;
    }
    return 0;
}