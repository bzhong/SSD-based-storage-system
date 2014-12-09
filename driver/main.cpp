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

int main() {
    Driver dr;
    string filename;
    cout << "Please indicate the absolute path of configuration file you want to choose (e.g. /user/cfg.xml):" << endl;
    cin >> filename;
    dr.RegisterFile(filename);
    dr.Run();
}
