SSD-based-storage-system
========================
This project gives an innovative replacement algorithm for SSD-based hybrid storage system. The project contains four parts: input generator, core algorithm, disk simulator and event driver. Input generator produces customized various kinds of workloads. Core algorithm contains traditional replacement algorithms like FIFO and LRU in addition to our innovative algorithm. Disk simulator mimics behaviors of HDDs and SSDs. Last, event driver triggers other three parts to coordinate with each other and make the system work well.

## How to build and run

Currently we provide only Mac version of code to quick start using Xcode. But still we provide all source code that you can use to build and run in Linux or Windows. The only cons is there is no configuration file and Makefile :(. Soon later we will provide such compile options.

### Steps to build

1. Download all source code and unzip them to a directory. You can find the lastest version in https://github.com/bzhong/SSD-based-storage-system.
2. Download Tinyxml2, a open source xml parser, through link https://github.com/leethomason/tinyxml2. Then copy tinyxml2.h and tinyxml2.cpp to input_generator directory.
3. Compile and build using Xcode.

## Sample Input Files

We will describe a sample input file to show how to run the program. The whole input file (also named configuration file) should contain two major sections as follows:

```
<InputGenerator>
	<SSD>64G</SSD>
	<test_length>3Y</test_length>

	<FileSet id="0">
		<filenum>200000</filenum>
		<minsize>1K</minsize>
		<maxsize>20G</maxsize>
		<distribute>Average</distribute>
		<writerate>20</writerate>
		<singlerequest>10</singlerequest>
		<frequency>1H</frequency>
		<start>0</start>
		<length>3Y</length>
		<type id="0">PY</type>
	</FileSet>
	<FileSet id="2">
		<filenum>200000</filenum>
		<minsize>1K</minsize>
		<maxsize>20G</maxsize>
		<distribute>Normal</distribute>
		<writerate>20</writerate>
		<singlerequest>10</singlerequest>
		<frequency>1M</frequency>
		<start>0</start>
		<length>3Y</length>
		<type id="0">PY</type>
		<type id="1">DOC</type>
	</FileSet>
</InputGenerator>
```

The first one is global configuration. This section only has two labels: SSD size and test length. Notice that the atomical time interval in our project is minute. All the time parameters will be converted into the unit of minute. All the size parameters should be in these format: a number and a suffix, for example 256G, 32M, 1T or 4K. All the time parameters should be in these format: a number and a suffix(optional). Suffix can be ‘H’ for hour. ‘D’ for day, ‘W’ for week, ‘M’ for month, ’S’ for season, ’Y’ for year and none for minute. Please note that all the suffixes are CASE SENSITIVE!

The second section is file set configurations. Each xml file can have several file set configurations. Each configuration must have a file set id. A configuration without a file set id may be ignored by the parse module. The meaning of the labels are as follow:

- "Filenum": the total numbers of files in this set. 
- "distribute: the distributing method this file set will use to generate the file size. There are two available methods. The first one is average and the second one is Normal Distribution.
- "singlerequest":	that once a single request is triggered, how many file requests will be generated to the algorithm module.
- "frequency":	the time interval between which this file set will be triggered.
- "start":	the start time stamp when this file set can be triggered.
- "length":	the existing length that this file set will be during the whole test.
- "type":	the file type that this file set can have. This label must have an “id” attribute too.

## Code format standards

The code follows Google C++ style. Please refer to http://google-styleguide.googlecode.com/svn/trunk/cppguide.html.

