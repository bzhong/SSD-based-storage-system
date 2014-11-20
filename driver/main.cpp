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
    dr.RegisterFile("/Users/qianqianzhong/Documents/github/coen283_project/cfg.xml");
    dr.Run();
}