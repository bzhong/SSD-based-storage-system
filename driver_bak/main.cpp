//
//  main.cpp
//  SSD-based-storage-system
//
//  Created by Brady on 11/7/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "input_generator.h"
#include "disk.h"
#include "replacement_algo.h"
using namespace std;

void Initialize(MQAAlgo** replacement_algo, SSD** ssd, HDD** hdd) {
    srand(static_cast<unsigned int>(time(NULL)));
    cout << "Please choose repalcement algo: 0 for FIFO, 1 for LRU, 2 for RR and 3 for MQA" << endl;
    int replacement_algo_type;
    cin >> replacement_algo_type;
    switch(replacement_algo_type) {
        case kFIFO:
            //*replacement_algo = new FIFOAlgo();
            break;
        case kLRU:
            break;
        case kRR:
            break;
        case kMQA:
            *replacement_algo = new MQAAlgo(4);
            break;
    }
   *ssd = new SSD(TranslateSize("550MB"), TranslateSize("500MB"), TranslateSize("256GB")); // read_speed: MB/s, write_speed: MB/s, capacity: GB
    *hdd = new HDD(TranslateSize("126MB"), TranslateSize("126MB"), TranslateSize("4TB"), 10); // seek time: ms
}

void AddCommands(InputGenerator* input_generator) {
    int command_num;
    BigUInt input_file_num;
    string file_min_size, file_max_size;
    cout << "Please input number of commands: ";
    cin >> command_num;
    for (int count = 0; count < command_num; ++count) {
        Command tmp_command;
        cout << "command " << count + 1 << ": Please input number of files, file_min_size and file_max_size: " << endl;
        cin >> input_file_num >> file_min_size >> file_max_size;
        tmp_command.file_number = input_file_num;
        tmp_command.file_size_min = TranslateSize(file_min_size);
        tmp_command.file_size_max = TranslateSize(file_max_size);
        tmp_command.file_operation = kWriteOp;
        tmp_command.file_type = "txt";
        input_generator->AddCommand(tmp_command);
    }
}

int main() {
    InputGenerator* input_generator = new InputGenerator(2000); // vritual init time
    MQAAlgo* replacement_algo;
    //ReplaceAlgo* replacement_algo;
    SSD *ssd;
    HDD *hdd;
    //Disk* ssd, *hdd;
    Initialize(&replacement_algo, &ssd, &hdd);
    AddCommands(input_generator);
    input_generator->GenBatchOp();
    vector<FileOp> tmp = input_generator->get_request_sequence();
    
    for (int i = 0; i < tmp.size(); ++i) {
        // for debug
        //cout << tmp[i].file_name << "\t" << tmp[i].file_size << "\t" << tmp[i].file_type << "\t" << tmp[i].access_time << endl;
        replacement_algo->ExecFileOp(tmp[i], ssd, hdd);
    }
    cout << "total exec time for algo: " << replacement_algo->get_total_exec_time() << "us" << endl;
    cout << "total exec time for ssd: " << ssd->get_total_exec_time() << "us" << endl;
    cout << "total exec time for hdd: " << hdd->get_total_exec_time() << "us" << endl;
    return 0;
}