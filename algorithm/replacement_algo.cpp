//
//  FIFO.cpp
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#include "replacement_algo.h"

long double ReplaceAlgo::get_total_exec_time() {
    return total_exec_time_ * 1000; // ms
}

long double ReplaceAlgo::get_ssd_exec_time() {
    return ssd_->get_total_exec_time();
}

long double ReplaceAlgo::get_hdd_exec_time() {
    return hdd_->get_total_exec_time();
}

BigUInt ReplaceAlgo::GetReqCount() {
    return req_count_;
}

BigUInt ReplaceAlgo::GetHitCount() {
    return hit_count_;
}

BigUInt ReplaceAlgo::GetTransferTimeDelay() {
    return hdd_->GetTransferTimeDelay();
}

FIFOAlgo::FIFOAlgo(const string& ssd_capacity,
         const string& ssd_read_speed,
         const string& ssd_write_speed,
         const string& hdd_read_speed,
         const string& hdd_write_speed,
         const string& hdd_capacity,
         const long double& hdd_seek_time) {
    ssd_ = new SSD(TranslateSize(ssd_read_speed), TranslateSize(ssd_write_speed), TranslateSize(ssd_capacity));
    hdd_ = new HDD(TranslateSize(hdd_read_speed), TranslateSize(hdd_write_speed), TranslateSize(hdd_capacity), hdd_seek_time);
    total_exec_time_ = 0;
    hit_count_ = req_count_ = 0;
}

FIFOAlgo::~FIFOAlgo() {
    delete ssd_;
    delete hdd_;
    ssd_ = hdd_ = NULL;
}

void FIFOAlgo::ExecReplace() {
    if (file_pool_.empty()) {
        cerr << "single file size is larger than capacity of ssd. Ignored." << endl;
        return;
    }
    ssd_->Delete(file_pool_.front());
    hdd_->Write(file_pool_.front());
    file_search_table_.erase(file_pool_.front().file_name);
    file_pool_.pop();
}

void FIFOAlgo::ExecFileOp(const FileOp& file_operation) {
    clock_t begin_time, end_time;
    int status;
    begin_time = clock();
    ++req_count_;
    if (file_operation.op_type == kReadOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            ++hit_count_;
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        else if (hdd_->Find(file_operation)) {
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Read(file_operation);
                if (status) {
                    cerr << "hdd read error for file: " << file_operation.file_name << ". The file doesn't exist." << endl;
                }
                return;
            }
            while (file_operation.file_size > ssd_->get_current_free_space()) {
                ExecReplace();
            }
            file_pool_.push(file_operation);
            file_search_table_.insert(file_operation.file_name);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd_->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap read from hdd." << endl;
                exit(1);
            }
            status = hdd_->Delete(file_operation);
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
        status = ssd_->Read(file_operation);
        if (status) {
            cerr << "ssd read error for file: " << file_operation.file_name << endl;
        }
    }
    else if (file_operation.op_type == kWriteOp || file_operation.op_type == kCreateOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            ++hit_count_;
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            ssd_->Delete(file_operation);
            ssd_->Write(file_operation);
            return;
        }
        else if (hdd_->Find(file_operation)) {
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Write(file_operation);
                if (status) {
                    cerr << "hdd write error for file: " << file_operation.file_name << endl;
                }
                return;
            }
            while (file_operation.file_size > ssd_->get_current_free_space()) {
                ExecReplace();
            }
            file_pool_.push(file_operation);
            file_search_table_.insert(file_operation.file_name);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd_->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap write from hdd." << endl;
                exit(1);
            }
            status = hdd_->Delete(file_operation);
            if (status) {
                cerr << "hdd delete error in swap write from hdd." << endl;
                exit(1);
            }
        }
        else {
            if (file_operation.op_type == kWriteOp) {
                cerr << "ssd write error. file " << file_operation.file_name << " doesn't exist. Ignored." << endl;
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                return;
            }
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Write(file_operation);
                if (status) {
                    cerr << "hdd write error for file: " << file_operation.file_name << endl;
                }
                return;
            }
            while (file_operation.file_size > ssd_->get_current_free_space()) {
                ExecReplace();
            }
            file_pool_.push(file_operation);
            file_search_table_.insert(file_operation.file_name);
            ++hit_count_;
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        status = ssd_->Write(file_operation);
        if (status) {
            cerr << "ssd write error for file: " << file_operation.file_name << endl;
        }
    }
}

MQAAlgo::MQAAlgo(const string& ssd_capacity,
                 const int& number_of_tier,
                 const string& ssd_read_speed,
                 const string& ssd_write_speed,
                 const string& hdd_read_speed,
                 const string& hdd_write_speed,
                 const string& hdd_capacity,
                 const long double& hdd_seek_time) {
    ssd_ = new SSD(TranslateSize(ssd_read_speed), TranslateSize(ssd_write_speed), TranslateSize(ssd_capacity));
    hdd_ = new HDD(TranslateSize(hdd_read_speed), TranslateSize(hdd_write_speed), TranslateSize(hdd_capacity), hdd_seek_time);
    file_pool_.resize(number_of_tier);
    total_exec_time_ = 0;
    hit_count_ = req_count_ = 0;
    cur_threshold_ = basic_threshold_ = 100;
    fspace_to_threshold_map_.push_back(make_pair(0.3, 10));
    fspace_to_threshold_map_.push_back(make_pair(0.6, 2));
    fspace_to_threshold_map_.push_back(make_pair(1, 1));
    adjust_via_type_ = {
        {kText, 1},
        {kPicture, 1},
        {kAudio, 1},
        {kVideo, 1}
    };    
    // type: txt, pdf, doc, xls, jpg, png, mp3, wav, mp4, mkv, others; +-1
    file_type_map_ = {
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

MQAAlgo::~MQAAlgo() {
    delete ssd_;
    delete hdd_;
    ssd_ = hdd_ = NULL;
}

bool MQAAlgo::Replace(const BigUInt& needed_size) {
    int last_tier = (int)file_pool_.size() - 1, status;
    while (ssd_->get_current_free_space() < needed_size) {
        while (file_pool_[last_tier].empty()) {
            --last_tier;
            if (last_tier < 0) {
                return false;
            }
        }
        status = hdd_->Write(file_pool_[last_tier].front());
        if (status) {
            cerr << "hdd write error in Replace process." << endl;
            exit(1);
        }
        status = ssd_->Delete(file_pool_[last_tier].front());
        if (status) {
            cerr << "ssd delete error in Replace process." << endl;
            exit(1);
        }
        file_search_table_.erase(file_pool_[last_tier].front().file_name);
        file_pool_[last_tier].pop_front();
    }
    return true;
}

void MQAAlgo::ExecReplace() {
    time_t timer;
    time(&timer);
    timer = (BigUInt)timer;
    for (int tier = 0; tier < 4; ++tier) {
        while (!file_pool_[tier].empty()) {
            if (cur_threshold_ - timer + file_pool_[tier].front().access_time <= 0) {
                hdd_->Write(file_pool_[tier].front());
                ssd_->Delete(file_pool_[tier].front());
                file_search_table_.erase(file_pool_[tier].front().file_name);
                file_pool_[tier].pop_front();
            }
            else {
                break;
            }
        }
    }
}

int MQAAlgo::AdjustByType(const FileOp &file_operation, const int& tier) {
    if (file_operation.op_type < tier) {
        return tier - adjust_via_type_.at(file_operation.op_type);
    }
    else if (file_operation.op_type > tier) {
        return tier + adjust_via_type_.at(file_operation.op_type);
    }
    /*FileType file_type = file_type_map.at(file_operation.file_type);
    if (file_type < tier) {
        return tier - adjust_via_type.at(file_type);
    }
    else if (file_type > tier) {
        return tier + adjust_via_type.at(file_type);
    }*/
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
    comb.second->file_size = file_operation.file_size; // may have problems with special write (>)
    file_pool_[comb.first].push_back(*comb.second);
    file_pool_[comb.first].erase(comb.second);
    list<FileOp>::iterator last_element = --file_pool_[comb.first].end();
    file_search_table_[file_operation.file_name] = pair<int, list<FileOp>::iterator>(comb.first, last_element);
}

void MQAAlgo::AddFile(const int &tier, const FileOp &file_operation) {
    file_pool_[tier].push_back(file_operation);
    list<FileOp>::iterator last_element = --file_pool_[tier].end();
    file_search_table_[file_operation.file_name] = pair<int, list<FileOp>::iterator>(tier, last_element);
}

void MQAAlgo::ExecFileOp(const FileOp &file_operation) {
    clock_t begin_time, end_time;
    int status;
    begin_time = clock();
    ++req_count_;
    if (file_operation.op_type == kReadOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            ++hit_count_;
            UpdateFileSearchTable(file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        else if (hdd_->Find(file_operation)) {
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Read(file_operation);
                if (status) {
                    cerr << "hdd read error for file: " << file_operation.file_name << ". The file doesn't exist." << endl;
                }
                return;
            }
            if (file_operation.file_size > ssd_->get_current_free_space()) {
                Replace(file_operation.file_size);
            }
            int tier = SetTier(file_operation); //need frequency to adjust
            AddFile(tier, file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd_->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap read from hdd." << endl;
                exit(1);
            }
            status = hdd_->Delete(file_operation);
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
        status = ssd_->Read(file_operation);
        if (status) {
            cerr << "ssd read error for file: " << file_operation.file_name << endl;
        }
        return;
    }
    else if (file_operation.op_type == kWriteOp || file_operation.op_type == kCreateOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            ++hit_count_;
            UpdateFileSearchTable(file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            ssd_->Delete(file_operation);
            ssd_->Write(file_operation);
            return;
        }
        else if (hdd_->Find(file_operation)) {
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Write(file_operation);
                if (status) {
                    cerr << "hdd write error for file: " << file_operation.file_name << endl;
                }
                return;
            }
            if (file_operation.file_size > ssd_->get_current_free_space()) {
                Replace(file_operation.file_size);
            }
            int tier = SetTier(file_operation);
            AddFile(tier, file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd_->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap write from hdd." << endl;
                exit(1);
            }
            status = hdd_->Delete(file_operation);
            if (status) {
                cerr << "hdd delete error in swap write from hdd." << endl;
                exit(1);
            }
            
        }
        else {
            if (file_operation.op_type == kWriteOp) {
                cerr << "ssd write error. file " << file_operation.file_name << " doesn't exist. Ignored." << endl;
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                return;
            }
            while (file_operation.file_size > ssd_->get_current_free_space()) {
                if (file_operation.file_size > ssd_->get_capacity_size()) {
                    //cout << "file " << file_operation.file_name << " is too large to put in ssd. write to hdd." << endl;
                    end_time = clock();
                    total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                    status = hdd_->Write(file_operation);
                    if (status) {
                        cerr << "hdd write error for file: " << file_operation.file_name << endl;
                    }
                    return;
                }
                cur_threshold_ = basic_threshold_ / fspace_to_threshold_map_[0].second;
                ExecReplace();
                if (file_operation.file_size > ssd_->get_current_free_space()) {
                    Replace(file_operation.file_size);
                }
            }
            cur_threshold_ = basic_threshold_;
            int tier = SetTier(file_operation);
            AddFile(tier, file_operation);
            ++hit_count_;
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        status = ssd_->Write(file_operation);
        if (status) {
            cerr << "ssd write error for file: " << file_operation.file_name << endl;
        }
        return;
    }
    else if (file_operation.op_type == kIdleOp) {
        end_time = clock();
        total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        cur_threshold_ = basic_threshold_;
        double percentage = (double)ssd_->get_current_free_space() / ssd_->get_capacity_size();
        if (percentage < fspace_to_threshold_map_[0].first) {
            cur_threshold_ /= fspace_to_threshold_map_[0].second;
        }
        else if (percentage < fspace_to_threshold_map_[1].first) {
            cur_threshold_ /= fspace_to_threshold_map_[1].second;
        }
        else {
            cur_threshold_ = basic_threshold_;
        }
        ssd_->set_idle_signal(true);
        hdd_->set_idle_signal(true);
        ExecReplace();
        if (ssd_->get_current_free_space() < ssd_->get_buffer_size()) {
            Replace(ssd_->get_buffer_size());
        }
        ssd_->set_idle_signal(false);
        hdd_->set_idle_signal(false);
    }
}

LRUAlgo::LRUAlgo(const string& ssd_capacity,
                 const string& ssd_read_speed,
                 const string& ssd_write_speed,
                 const string& hdd_read_speed,
                 const string& hdd_write_speed,
                 const string& hdd_capacity,
                 const long double& hdd_seek_time) {
    ssd_ = new SSD(TranslateSize(ssd_read_speed), TranslateSize(ssd_write_speed), TranslateSize(ssd_capacity));
    hdd_ = new HDD(TranslateSize(hdd_read_speed), TranslateSize(hdd_write_speed), TranslateSize(hdd_capacity), hdd_seek_time);
    total_exec_time_ = 0;
    hit_count_ = req_count_ = 0;
}

LRUAlgo::~LRUAlgo() {
    delete ssd_;
    delete hdd_;
    ssd_ = hdd_ = NULL;
}

void LRUAlgo::ExecReplace() {
    if (file_pool_.empty()) {
        cerr << "single file size is larger than capacity of ssd. Ignored." << endl;
        return;
    }
    ssd_->Delete(file_pool_.front());
    hdd_->Write(file_pool_.front());
    file_search_table_.erase(file_pool_.front().file_name);
    file_pool_.pop_front();
}

void LRUAlgo::UpdateFileSearchTable(const FileOp &file_operation) {
    list<FileOp>::iterator comb = file_search_table_[file_operation.file_name];
    comb->access_time = file_operation.access_time;
    comb->op_type = file_operation.op_type;
    comb->file_size = file_operation.file_size;
    file_pool_.push_back(*comb);
    file_pool_.erase(comb);
    list<FileOp>::iterator last_element = --file_pool_.end();
    file_search_table_[file_operation.file_name] = last_element;
}

void LRUAlgo::ExecFileOp(const FileOp& file_operation) {
    clock_t begin_time, end_time;
    int status;
    begin_time = clock();
    ++req_count_;
    if (file_operation.op_type == kReadOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            UpdateFileSearchTable(file_operation);
            ++hit_count_;
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        else if (hdd_->Find(file_operation)) {
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Read(file_operation);
                if (status) {
                    cerr << "hdd read error for file: " << file_operation.file_name << ". The file doesn't exist." << endl;
                }
                return;
            }
            while (file_operation.file_size > ssd_->get_current_free_space()) {
                ExecReplace();
            }
            file_pool_.push_back(file_operation);
            file_search_table_[file_operation.file_name] = --file_pool_.end();
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd_->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap read from hdd." << endl;
                exit(1);
            }
            status = hdd_->Delete(file_operation);
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
        status = ssd_->Read(file_operation);
        if (status) {
            cerr << "ssd read error for file: " << file_operation.file_name << endl;
        }
        return;
    }
    else if (file_operation.op_type == kWriteOp || file_operation.op_type == kCreateOp) {
        if (file_search_table_.find(file_operation.file_name) != file_search_table_.end()) {
            ++hit_count_;
            UpdateFileSearchTable(file_operation);
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            ssd_->Delete(file_operation);
            ssd_->Write(file_operation);
            return;
        }
        else if (hdd_->Find(file_operation)) {
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Write(file_operation);
                if (status) {
                    cerr << "hdd Write error for file: " << file_operation.file_name << endl;
                }
                return;
            }
            while (file_operation.file_size > ssd_->get_current_free_space()) {
                ExecReplace();
            }
            file_pool_.push_back(file_operation);
            file_search_table_[file_operation.file_name] = --file_pool_.end();
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
            status = ssd_->Write(file_operation);
            if (status) {
                cerr << "ssd write error in swap write from hdd." << endl;
                exit(1);
            }
            status = hdd_->Delete(file_operation);
            if (status) {
                cerr << "hdd delete error in swap write from hdd." << endl;
                exit(1);
            }
        }
        else {
            if (file_operation.op_type == kWriteOp) {
                cerr << "ssd write error. file " << file_operation.file_name << " doesn't exist. Ignored." << endl;
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                return;
            }
            if (file_operation.file_size > ssd_->get_capacity_size()) {
                end_time = clock();
                total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
                status = hdd_->Write(file_operation);
                if (status) {
                    cerr << "hdd write error for file: " << file_operation.file_name << endl;
                }
                return;
            }
            while (file_operation.file_size > ssd_->get_current_free_space()) {
                ExecReplace();
            }
            file_pool_.push_back(file_operation);
            file_search_table_[file_operation.file_name] = --file_pool_.end();
            ++hit_count_;
            end_time = clock();
            total_exec_time_ += ((long double)(end_time - begin_time)) / CLOCKS_PER_SEC;
        }
        status = ssd_->Write(file_operation);
        if (status) {
            cerr << "ssd write error for file: " << file_operation.file_name << endl;
        }
        return;
    }
}

/*ClockAlgo::ClockAlgo(const string& ssd_read_speed,
          const string& ssd_write_speed,
          const string& ssd_capacity,
          const string& hdd_read_speed,
          const string& hdd_write_speed,
          const string& hdd_capacity,
          const long double& hdd_seek_time) {
    ssd_ = new SSD(TranslateSize(ssd_read_speed), TranslateSize(ssd_write_speed), TranslateSize(ssd_capacity));
    hdd_ = new HDD(TranslateSize(hdd_read_speed), TranslateSize(hdd_write_speed), TranslateSize(hdd_capacity), hdd_seek_time);
    total_exec_time_ = 0;
    hit_count_ = req_count_ = 0;
}

ClockAlgo::~ClockAlgo() {
    delete ssd_;
    delete hdd_;
    ssd_ = hdd_ = NULL;
}

void ClockAlgo::ExecFileOp(const FileOp& file_operation) {
    
}*/





