//
//  input_generator.h
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//

#ifndef SSD_based_storage_system_input_generator_h
#define SSD_based_storage_system_input_generator_h

#include <cstdlib>
#include <random>
#include <chrono>
#include <fstream>
#include "replacement_algo.h"
#include "tinyxml2.h"
#include "op_structure.h"
#include "replacement_algo.h"

using namespace std;
const int DEFAULT_FILE_IN_SET = 1;
const BigUInt DEFAULT_FILE_SIZE = 1024 * 1024;
const string DEFAULT_SSD_SIZE = "256GB";

const BigUInt MINS_IN_HOUR = 60;
const BigUInt MINS_IN_DAY = MINS_IN_HOUR * 24;
const BigUInt MINS_IN_WEEK = MINS_IN_DAY * 7;
const BigUInt MINS_IN_MONTH = MINS_IN_DAY * 30;
const BigUInt MINS_IN_SEASON = MINS_IN_MONTH * 3;
const BigUInt MINS_IN_YEAR = MINS_IN_SEASON * 4;

const double DISTRIBUTEMIN = 0.5;
const double DISTRIBUTEMAX = 1.5;

const int DEFAULT_WORKLOAD = 80;

enum DistributeType
{
    kRandom = 0,
    kNormal = 1,
    kInvalid
};

struct CfgGlobal
{
    BigUInt test_lenth;
    string ssd_size;
    CfgGlobal(): test_lenth(MINS_IN_YEAR), ssd_size(DEFAULT_SSD_SIZE) {};
};

struct CfgFileSet
{
    int cfgid;
    int filenum;
    BigUInt minsize;
    BigUInt maxsize;
    int distribute;
    int writerate;
    int num_in_single_request;
    BigUInt frequency;
    BigUInt start;
    BigUInt len;

    vector<FileType> file_type_list;
    CfgFileSet(): cfgid(0),
                    filenum(DEFAULT_FILE_IN_SET),
                    minsize(DEFAULT_FILE_SIZE),
                    maxsize(DEFAULT_FILE_SIZE),
                    distribute(kRandom),
                    writerate(0),
                    num_in_single_request(DEFAULT_FILE_IN_SET),
                    frequency(MINS_IN_WEEK),
                    start(MINS_IN_SEASON),
                    len(MINS_IN_SEASON) {};
    void AddType(FileType t)
    {
        file_type_list.push_back(t);
    }
};

struct Statics
{
    BigUInt hit;
    BigUInt req;
    BigUInt time_delay;
    BigUInt Algo;
    BigUInt ssd;
    BigUInt hdd;
    Statics():hit(0),req(0),time_delay(0),Algo(0),ssd(0),hdd(0){};
};


class FileSet
{
public:
    FileSet()
    {
        long long seed = std::chrono::system_clock::now().time_since_epoch().count();
        generator = default_random_engine((unsigned int)seed);
        distribution = normal_distribution<double>(1.0, (1.0 - DISTRIBUTEMIN) / 2);
    };
    ~FileSet(){};
    
    void Initial()
    {
        filelist.clear();
        reset();
    }
    
    void Configuration(const CfgFileSet& cfg);
    bool GetNext();
    bool CheckTrigger(BigUInt current)
    {
        return (current <= (start + len) && current >= nextTrigger);
    }
    void reset()
    {
        nextid = 0;
    }
    void GetNextTrigger(void);
private:
    struct FileAttr
    {
        BigUInt size;
        FileType type;
        string name;
        bool create;
    };
    
    int nextid;
    int writerate;
    int num_in_single_request;
    BigUInt frequency;
    BigUInt start;
    BigUInt len;
    
    
    BigUInt nextTrigger;
    
    vector<FileAttr> filelist;
    
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;

    const string GenerateFileName(const CfgFileSet& cfg, int fileid);
    
    void GenerateFile(const CfgFileSet& cfg);

};

class InputGenerator {
public:
    InputGenerator();
    ~InputGenerator();
    void Configure(const CfgGlobal& cfg)
    {
        test_lenth = cfg.test_lenth;
        ssd_size = cfg.ssd_size;
        
        replace_algo[0] = new MQAAlgo((const string)ssd_size);
        replace_algo[1] = new FIFOAlgo(ssd_size);
        replace_algo[2] = new LRUAlgo(ssd_size);
    }
    
    void AddFileSet(const FileSet& fileset)
    {
        setlist.push_back(fileset);
    }
    void Run(void);
    
private:
    BigUInt test_lenth;
    string ssd_size;
    
    
    BigUInt current;
    vector<FileSet> setlist;

    ReplaceAlgo *replace_algo[3];
    
    ofstream outf;
    
    void IdleTrigger(void);
    void SendRequest(void);
    void ProcStatics(void);
    
};

class Driver
{
private:
    string cfgname;
    InputGenerator ig;
    
    BigUInt ParseLength(const char* length);
    BigUInt ParseSize(const char* size);
    FileType ParseType(const char* type);
    
    void ParseFileSet(tinyxml2::XMLElement* fileset, CfgFileSet& cfs);
public:
    Driver()
    {
    }
    ~Driver()
    {
        
    }
    void Run();
    
    void RegisterFile(const string& conf)
    {
        cfgname = conf;
    }
};
#endif
