//
//  input_generator.cpp
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//
#include <sstream>
#include "input_generator.h"
#include "tinyxml2.h"

using namespace std;

FileOp gfileop;

BigUInt rrand(const BigUInt min, const BigUInt max)
{
    if (min == max) {
        return min;
    }
    return rand() % (max - min)  + min;
}

bool FileSet::GetNext()
{
    if (nextid >= num_in_single_request) {
        return false;
    }
    nextid++;
    BigUInt id = rrand(0, filelist.size());
    
    gfileop.file_name = filelist[id].name;
    gfileop.file_size = filelist[id].size;
    gfileop.file_type = filelist[id].type;
    
    if (!filelist[id].create) {
        gfileop.op_type = kCreateOp;
        filelist[id].create = true;
    }
    else
    {
        if (rrand(0, 100) < writerate) {
            gfileop.op_type = kWriteOp;
        }
        else
        {
            gfileop.op_type = kReadOp;
        }
        
    }
    return true;
}

void FileSet::Configuration(const CfgFileSet& cfg)
{
    num_in_single_request = cfg.num_in_single_request;
    frequency = cfg.frequency;
    start = cfg.start;
    len = cfg.len;
    nextTrigger = start;
    writerate = cfg.writerate;
    GenerateFile(cfg);
}

void FileSet::GetNextTrigger(void)
{
    double factor = distribution(generator);
    if (factor > DISTRIBUTEMAX) {
        factor = DISTRIBUTEMAX;
    }
    else if (factor < DISTRIBUTEMIN)
    {
        factor = DISTRIBUTEMIN;
    }
    
    nextTrigger += (BigUInt)(factor * (double)frequency);
}

const string FileSet::GenerateFileName(const CfgFileSet& cfg, int fileid)
{
    stringstream name;
    name << cfg.cfgid << fileid;

    return name.str();
}

void FileSet::GenerateFile(const CfgFileSet& cfg)
{
    Initial();
    
    BigUInt mean = (cfg.maxsize + cfg.minsize) / 2;
    
    
    default_random_engine generator;
    normal_distribution<double> distribution((double)mean,(double)((mean - cfg.minsize) / 2));
    
    for (int i = 0; i < cfg.filenum; i++)
    {
        FileAttr fa;
        fa.create = false;
        fa.name = GenerateFileName(cfg, i);
        
        if (cfg.file_type_list.size()) {
            fa.type = cfg.file_type_list[rrand(0, cfg.file_type_list.size())];
        }
        else
        {
            fa.type = kText;
        }
        
        if (cfg.maxsize == cfg.minsize) {
            fa.size = cfg.minsize;
        }
        else {
            switch (cfg.distribute) {
                case kRandom:
                    fa.size = rrand(cfg.minsize, cfg.maxsize);
                    break;
                case kNormal:
                    fa.size = (BigUInt)distribution(generator);
                    break;
                default:
                    exit(-1);
                    break;
            }
        }
        filelist.push_back(fa);
    }
}

void InputGenerator::SendRequest()
{
    mqa.ExecFileOp(gfileop);
    
    cout<<"op_type:"<<gfileop.op_type<<"\t";
    cout<<"name:"<<gfileop.file_name<<"\t";
    cout<<"size:"<<gfileop.file_size<<"\t";
    cout<<"type:"<<gfileop.file_type<<"\t";
    cout<<"access_time:"<<gfileop.access_time<<endl;
}

void InputGenerator::IdleTrigger(void)
{
    if (idle_chance > 0 && rrand(0, 100) < idle_chance) {
        idle_chance -= 100;
        gfileop.op_type = kIdleOp;
        SendRequest();
        return;
    }
    idle_chance += (100 - workload);
}

void InputGenerator::Run()
{
    idle_chance = 0;

    for (current = 0; current < test_lenth; current++)
    {
        for (int i = 0; i < setlist.size(); i++)
        {
            if (!setlist[i].CheckTrigger(current))
            {
                continue;
            }
            setlist[i].reset();
            while (setlist[i].GetNext())
            {
                gfileop.access_time = current;
                SendRequest();
                current++;
            }
            setlist[i].GetNextTrigger();
            IdleTrigger();
        }
        
    }
}

BigUInt Driver::ParseLength(const char* length)
{
    string slen = length;
    char unit = slen[slen.size() - 1];
    BigUInt factor;
    
    switch (unit) {
        case 'H':
            factor = MINS_IN_HOUR;
            break;
        case 'D':
            factor = MINS_IN_DAY;
            break;
        case 'W':
            factor = MINS_IN_WEEK;
            break;
        case 'M':
            factor = MINS_IN_MONTH;
            break;
        case 'S':
            factor = MINS_IN_SEASON;
            break;
        case 'Y':
            factor = MINS_IN_YEAR;
            break;
        default:
            factor = 1;
            break;
    }

    stringstream ss(slen);
    BigUInt num;
    ss >> num;
    return num * factor;
}

BigUInt Driver::ParseSize(const char* size)
{
    string ssize = size;
    char unit = ssize[ssize.size() - 1];
    BigUInt factor;
    switch (unit) {
        case 'K':
            factor = 1024;
            break;
        case 'M':
            factor = 1024 * 1024;
            break;
        case 'G':
            factor = 1024 * 1024 * 1024;
            break;
        case 'T':
            factor = (BigUInt)1024 * 1024 * 1024 * 1024;
            break;
        default:
            factor = 1;
            break;
    }
    stringstream ss(ssize);
    BigUInt num;
    ss >> num;
    return num * factor;
}

FileType Driver::ParseType(const char* type)
{
    if (!strcmp(type, "TXT")) {
        return kText;
    }
    else if (!strcmp(type, "DOC")) {
        return kText;
    }
    else if (!strcmp(type, "LIB")) {
        return kText;
    }
    else if (!strcmp(type, "MP3")) {
        return kAudio;
    }
    else if (!strcmp(type, "WAV")) {
        return kAudio;
    }
    else if (!strcmp(type, "JPG")) {
        return kPicture;
    }
    else if (!strcmp(type, "PNG")) {
        return kPicture;
    }
    else if (!strcmp(type, "MKV")) {
        return kVideo;
    }
    else if (!strcmp(type, "MP4")) {
        return kVideo;
    }
    else if (!strcmp(type, "C")) {
        return kText;
    }
    else if (!strcmp(type, "H")) {
        return kText;
    }
    else if (!strcmp(type, "XLS")) {
        return kText;
    }
    else
    {
        return kText;
    }
}

void Driver::ParseFileSet(tinyxml2::XMLElement* fileset, CfgFileSet& cfs)
{
    fileset->QueryIntAttribute( "id", &cfs.cfgid );
    
    fileset->FirstChildElement("filenum")->QueryIntText( &cfs.filenum );
    
    const char* size = fileset->FirstChildElement("minsize")->GetText();
    cfs.minsize = ParseSize(size);
    
    size = fileset->FirstChildElement("maxsize")->GetText();
    cfs.maxsize = ParseSize(size);
    
    const char* distribute = fileset->FirstChildElement("distribute")->GetText();
    if (!strcmp(distribute, "Average")) {
        cfs.distribute = kRandom;
    }
    else
    {
        cfs.distribute = kNormal;
    }
    
    fileset->FirstChildElement("writerate")->QueryIntText( &cfs.writerate );
    
    fileset->FirstChildElement("singlerequest")->QueryIntText( &cfs.num_in_single_request );
    
    const char* length = fileset->FirstChildElement("frequency")->GetText();
    cfs.frequency = ParseLength(length);

    length = fileset->FirstChildElement("start")->GetText();
    cfs.start = ParseLength(length);
    
    length = fileset->FirstChildElement("length")->GetText();
    cfs.len = ParseLength(length);
    
    tinyxml2::XMLElement * xtype = fileset->FirstChildElement("type");
    while (xtype != NULL) {
        cfs.AddType(ParseType(xtype->GetText()));
        xtype = xtype->NextSiblingElement("type");
    }
}

void Driver::Run()
{
    CfgGlobal cg;
    
    tinyxml2::XMLDocument cfg;
    
    cfg.LoadFile(cfgname.c_str());
    
    cfg.FirstChildElement( "InputGenerator" )->FirstChildElement( "workload" )->QueryIntText( &cg.workload );
    const char* length = cfg.FirstChildElement( "InputGenerator" )->FirstChildElement( "test_length" )->GetText();
    
    cg.test_lenth = ParseLength(length);
    
    ig.Configure(cg);
    
    tinyxml2::XMLElement* fileset = cfg.FirstChildElement( "InputGenerator" )->FirstChildElement( "FileSet" );
    
    if (fileset == NULL) {
        cout<<"No FileSet Config found!"<<endl;
        exit(-1);
    }
    while (fileset != NULL) {
        FileSet fs;
        CfgFileSet cfs;
        ParseFileSet(fileset, cfs);
        fs.Configuration(cfs);
        ig.AddFileSet(fs);
        fileset = fileset->NextSiblingElement("FileSet" );
    }
    
    ig.Run();
}