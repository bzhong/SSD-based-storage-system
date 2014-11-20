//
//  FIFO.cpp
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "replacement_algo.h"

FIFOAlgo::FIFOAlgo() {
    total_exec_time_ = 0;
}

long double FIFOAlgo::get_total_exec_time() {
    return total_exec_time_ * 1000000; // us
}

bool FIFOAlgo::Replace(SSD* ssd, HDD* hdd) {
    if (file_pool_.empty()) {
        return false;
    }
    ssd->Delete(file_pool_.front());
    hdd->Write(file_pool_.front());
    file_pool_.pop();
    return true;
}

void FIFOAlgo::ExecFileOp(const FileOp& file_operation, SSD* ssd, HDD* hdd) {
    clock_t begin_time, end_time;
    begin_time = clock();
    while (file_operation.file_size > ssd->get_current_free_space()) {
        if (!Replace(ssd, hdd)) {
            cerr << "the request size is larger than maximum. Ignored." << endl;
            return;
        }
    }
    file_pool_.push(file_operation);
    end_time = clock();
    total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
    switch (file_operation.op_type) {
        case kReadOp:
            ssd->Read(file_operation);
            break;
        case kWriteOp:
            ssd->Write(file_operation);
            break;
    }
}

MQAAlgo::MQAAlgo(const int& number_of_tier) {
    file_pool_.resize(number_of_tier);
    total_exec_time_ = 0;
    cur_threshold = basic_threshold = 100;
    fspace_to_threshold_map.push_back(make_pair(0.3, 10));
    fspace_to_threshold_map.push_back(make_pair(0.6, 2));
    fspace_to_threshold_map.push_back(make_pair(1, 1));
    adjust_via_type = {
        {kText, 1},
        {kPicture, 1},
        {kAudio, 1},
        {kVideo, 1}
    };    
    // type: txt, pdf, doc, xls, jpg, png, mp3, wav, mp4, mkv, others; +-1
    file_type_map = {
        {"txt", kText},
        {"pdf", kText},
        {"doc", kText},
        {"xls", kText},
        {"jpg", kPicture},
        {"png", kPicture},
        {"mp3", kAudio},
        {"wav", kAudio},
        {"mp4", kVideo},
        {"mkv", kVideo},
        {"other", kOther}
    };
}

long double MQAAlgo::get_total_exec_time() {
    return total_exec_time_ * 1000000; // us
}

bool MQAAlgo::Replace(SSD *ssd, HDD* hdd) {
    int last_tier = (int)file_pool_.size() - 1;
    while (ssd->get_current_free_space() < ssd->get_buffer_size()) {
        while (file_pool_[last_tier].empty()) {
            --last_tier;
            if (last_tier < 0) {
                return false;
            }
        }
        hdd->Write(file_pool_[last_tier].front());
        ssd->Delete(file_pool_[last_tier].front());
        file_pool_[last_tier].pop_front();
    }
    return true;
}

void MQAAlgo::ExecReplace(SSD* ssd, HDD* hdd) {
    time_t timer;
    time(&timer);
    timer = (BigUInt)timer;
    for (int tier = 0; tier < 4; ++tier) {
        while (!file_pool_[tier].empty()) {
            if (cur_threshold - timer + file_pool_[tier].front().access_time <= 0) {
                hdd->Write(file_pool_[tier].front());
                ssd->Delete(file_pool_[tier].front());
                file_pool_[tier].pop_front();
            }
        }
    }
    if (ssd->get_current_free_space() < ssd->get_buffer_size()) {
        Replace(ssd, hdd);
    }
}

int MQAAlgo::AdjustByType(const FileOp &file_operation, const int& tier) {
    FileType file_type = file_type_map.at(file_operation.file_type);
    if (file_type < tier) {
        return tier - adjust_via_type.at(file_type);
    }
    else if (file_type > tier) {
        return tier + adjust_via_type.at(file_type);
    }
    return tier;
}

/*** tier0: < 1MB, tier1: (1MB, 64MB), tier2: (64MB, 1GB), tier3: > 1GB ***/
int MQAAlgo::SetTier(const FileOp &file_operation) {
    int tier;
    if (file_operation.file_size < TranslateSize("1MB")) {
        tier = 0;
    }
    else if (file_operation.file_size < TranslateSize("64MB")) {
        tier = 1;
    }
    else if (file_operation.file_size < TranslateSize("1GB")) {
        tier = 2;
    }
    else {
        tier = 3;
    }
    tier = AdjustByType(file_operation, tier);
    if (tier < 0) {
        tier = 0;
    }
    if (tier > 3) {
        tier = 3;
    }
    return tier;
}

void MQAAlgo::UpdateFileSearchTable(const FileOp &file_operation) {
    pair<int, list<FileOp>::iterator> comb = file_search_table_[file_operation.file_name];
    comb.second->access_time = file_operation.access_time;
    comb.second->op_type = file_operation.op_type;
    comb.second->file_size = file_operation.file_size; // may have problems with special read (>)
    file_pool_[comb.first].push_back(*comb.second);
    file_pool_[comb.first].erase(comb.second);
}

void MQAAlgo::AddFile(const int &tier, const FileOp &file_operation) {
    file_pool_[tier].push_back(file_operation);
    list<FileOp>::iterator last_element = --file_pool_[tier].end();
    file_search_table_[file_operation.file_name] = pair<int, list<FileOp>::iterator>(tier, last_element);
}

void MQAAlgo::ExecFileOp(const FileOp& file_operation, SSD *ssd, HDD* hdd) {
    clock_t begin_time, end_time;
    int status;
    begin_time = clock();
    if (file_operation.op_type == kReadOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            UpdateFileSearchTable(file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        else if (hdd->Find(file_operation)) {
            int tier = SetTier(file_operation); //need frequency to adjust
            AddFile(tier, file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap read from hdd." << endl;
                exit(1);
            }
            status = hdd->Delete(file_operation);
            if (status) {
                cerr << "hdd delete error in swap read from hdd." << endl;
                exit(1);
            }
        }
        else {
            cerr << "file: " << file_operation.file_name << " doesn't exist. Ignored." << endl;
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            return;
        }
        status = ssd->Read(file_operation);
        if (status) {
            cerr << "ssd read error for file: " << file_operation.file_name << endl;
        }
        return;
    }
    else if (file_operation.op_type == kWriteOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            UpdateFileSearchTable(file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        else if (hdd->Find(file_operation)) {
            int tier = SetTier(file_operation);
            AddFile(tier, file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap write from hdd." << endl;
                exit(1);
            }
            status = hdd->Delete(file_operation);
            if (status) {
                cerr << "hdd delete error in swap write from hdd." << endl;
                exit(1);
            }
        }
        else {
            int tier = SetTier(file_operation);
            AddFile(tier, file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        status = ssd->Write(file_operation);
        if (status) {
            cerr << "ssd write error for file: " << file_operation.file_name << endl;
        }
        return;
    }
    else if (file_operation.op_type == kIdleOp) {
        double percentage = (double)ssd->get_current_free_space() / ssd->get_capacity_size();
        if (percentage < fspace_to_threshold_map[0].first) {
            cur_threshold /= fspace_to_threshold_map[0].second;
        }
        else if (percentage < fspace_to_threshold_map[1].first) {
            cur_threshold /= fspace_to_threshold_map[1].second;
        }
        ExecReplace(ssd, hdd);
    }
}

