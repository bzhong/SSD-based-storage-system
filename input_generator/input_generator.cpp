//
//  input_generator.cpp
//  SSD-based-storage-system
//
//  Created by Brady on 11/8/14.
//  Copyright (c) 2014 Qianqian Zhong. All rights reserved.
//
#include <iomanip>
#include <sstream>
#include <fstream>
#include "input_generator.h"
#include "tinyxml2.h"

using namespace std;

FileOp gfileop;
Statics gStatic[3];

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
    name<<hex<<uppercase<<setw(2)<<setfill('0')<<cfg.cfgid
        <<setw(6)<<setfill('0')<<fileid;
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

InputGenerator::InputGenerator() {

    replace_algo[0] = NULL;
    replace_algo[1] = NULL;
    replace_algo[2] = NULL;
    
    outf.open("result.txt");
}

InputGenerator::~InputGenerator() {
    for (int count = 0; count < 3; ++count) {
        if (replace_algo[count]) {
            delete replace_algo[count];
        }
        replace_algo[count] = NULL;
    }
    outf.close();
}

void InputGenerator::SendRequest()
{
    for (int count = 0; count < 3; ++count) {
        replace_algo[count]->ExecFileOp(gfileop);
    }
}

void InputGenerator::IdleTrigger(void)
{
    gfileop.op_type = kIdleOp;
    gfileop.access_time = current;
    SendRequest();
    return;
}

void InputGenerator::ProcStatics(void)
{
    for (int count = 0; count < 3; ++count) {
        Statics temp;
        temp = gStatic[count];
        
        gStatic[count].hit = replace_algo[count]->GetHitCount();
        gStatic[count].req = replace_algo[count]->GetReqCount();
        gStatic[count].time_delay = replace_algo[count]->GetTransferTimeDelay();
        gStatic[count].Algo = replace_algo[count]->get_total_exec_time();
        gStatic[count].ssd = replace_algo[count]->get_ssd_exec_time();
        gStatic[count].hdd = replace_algo[count]->get_hdd_exec_time();

        temp.hit = gStatic[count].hit - temp.hit;
        temp.req = gStatic[count].req - temp.req;
        temp.time_delay = gStatic[count].time_delay - temp.time_delay;
        temp.Algo = gStatic[count].Algo - temp.Algo;
        temp.ssd = gStatic[count].ssd - temp.ssd;
        temp.hdd = gStatic[count].hdd - temp.hdd;
        
        
        cout.precision(3);
        cout << (long double)temp.hit / temp.req<<"\t";
        cout << temp.time_delay << "\t";
        cout<<temp.Algo<<"\t\t";
        cout<<temp.ssd<<"\t\t";
        cout<<temp.hdd<<"\t\t";
        
        outf.precision(3);
        outf << (long double)temp.hit / temp.req<<"\t";
        outf << temp.time_delay << "\t";
        outf<<temp.Algo<<"\t";
        outf<<temp.ssd<<"\t";
        outf<<temp.hdd<<"\t";
    }
}

void InputGenerator::Run()
{
    cout << "Number" << "\t";
    for (int i = 0; i < 3; ++i) {
        cout << "HitRate" << "\t";
        cout << "HDD_Req" << "\t";
        switch (i) {
            case 0:
                cout << "MQA_time" << "\t";
                break;
            case 1:
                cout << "FIFO_time" << "\t";
                break;
            case 2:
                cout << "LRU_time" << "\t";
                break;
        }
        cout << "SSD_time" << "\t";
        cout << "HDD_time" << "\t";
    }
    cout << endl;
    
    for (current = 0; current < test_lenth; current++)
    {
        if ((current % MINS_IN_DAY) == 0) {
            IdleTrigger();
        }
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
            }
            setlist[i].GetNextTrigger();
        }
        if (((current + 1) % MINS_IN_MONTH) == 0) {
            cout<<(current + 1)/MINS_IN_MONTH<<"\t\t";
            outf<<(current + 1)/MINS_IN_MONTH<<"\t";
            ProcStatics();
            cout<<endl;
            outf<<endl;
        }
    }
    
    // print results about exec time and hit rate.
    cout << "print results: 1 for MQA, 2 for FIFO, 3 for LRU" << endl;
    outf << "print results: 1 for MQA, 2 for FIFO, 3 for LRU" << endl;
    for (int count = 0; count < 3; ++count) {
        cout << "Algo " << count + 1 << endl;
        cout << "algo exec time: " << replace_algo[count]->get_total_exec_time() << "ms" << endl;
        cout << "ssd exec time: " << replace_algo[count]->get_ssd_exec_time() << "ms" << endl;
        cout.precision(20);
        cout << "hdd exec time: " << replace_algo[count]->get_hdd_exec_time() << "ms" << endl;
        cout.precision(5);
        cout << "hit count: " << (long double)replace_algo[count]->GetHitCount() / replace_algo[count]->GetReqCount() << endl;
        
        outf << "Algo " << count + 1 << endl;
        outf << "algo exec time: " << replace_algo[count]->get_total_exec_time() << "ms" << endl;
        outf << "ssd exec time: " << replace_algo[count]->get_ssd_exec_time() << "ms" << endl;
        outf.precision(20);
        outf << "hdd exec time: " << replace_algo[count]->get_hdd_exec_time() << "ms" << endl;
        outf.precision(5);
        outf << "hit count: " << (long double)replace_algo[count]->GetHitCount() / replace_algo[count]->GetReqCount() << endl;
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
        return kOther;
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
    
    const char* ssd_capability = cfg.FirstChildElement( "InputGenerator" )->FirstChildElement( "SSD" )->GetText();
    
    cg.ssd_size = ssd_capability;
    cg.ssd_size.push_back('B');
    
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
