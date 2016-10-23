/*#include "StdAfx.h"*/
#include "DataBarcode.h"

#include "iostream"
#include "windows.h"
#include <fstream>


static BOOL MkDir(CStr&  _path)
{
	if (_path.size() == 0)
		return FALSE;

	static char buffer[1024];
	strcpy(buffer, _S(_path));
	for (int i = 0; buffer[i] != 0; i++) {
		if (buffer[i] == '\\' || buffer[i] == '/') {
			buffer[i] = '\0';
			CreateDirectoryA(buffer, 0);
			buffer[i] = '/';
		}
	}
	return CreateDirectoryA(_S(_path), 0);
}

static vecS loadStrList(CStr &fName)
{
	ifstream fIn(fName);
	string line;
	vecS strs;
	while (getline(fIn, line) && line.size())
		strs.push_back(line);
	return strs;
}

static bool writeStrList(CStr &fName, const vecS &strs)
{
	FILE *f = fopen(_S(fName), "w");
	if (f == NULL)
		return false;
	for (size_t i = 0; i < strs.size(); i++)
		fprintf(f, "%s\n", _S(strs[i]));
	fclose(f);
	return true;
}

// Load annotations
int  DataBarcode::init(CStr _wkDir){

	if (_wkDir == wkDir)
	{
		return 0;
	}
	wkDir = _wkDir;
	resDir = wkDir + "Results/";
	localDir = wkDir + "Local/";
	TrImgscript = wkDir + "\\Train.txt";
	TestImgscript = wkDir + "\\Test.txt";
	MkDir(resDir);
	MkDir(localDir);

	trainSet = loadStrList(TrImgscript);
	testSet = loadStrList(TestImgscript);
	//classNames = loadStrList(wkDir + "ImageSets/Main/class.txt");

	// testSet.insert(testSet.end(), trainSet.begin(), trainSet.end());	
	// testSet.resize(min(1000, (int)testSet.size()));

	trainNum = trainSet.size();
	testNum = testSet.size();

	return 1;
}

DataBarcode::DataBarcode()
:wkDir("")
{

}


DataBarcode::DataBarcode(const string &_wkDir)
{
	init(_wkDir);
}

DataBarcode::~DataBarcode()
{
}


void DataBarcode :: loadAnnotations()
{
	gtTrainBoxes.resize(trainNum);
	gtTrainClsIdx.resize(trainNum);

	for (int i = 0; i < trainSet.size();i++)
	{
		int intr = trainSet[i].find(".jpg");
		string txtName = trainSet[i];
		txtName.replace(intr, intr + 4, ".txt");

		if (!loadBBoxes(txtName, gtTrainBoxes[i], gtTrainClsIdx[i]))
			return;
	}

	gtTestBoxes.resize(testNum);
	gtTestClsIdx.resize(testNum);
	for (int i = 0; i < testSet.size(); i++)
	{
		int intr = testSet[i].find(".jpg");
		string txtName = testSet[i];
		txtName.replace(intr, intr + 4, ".txt");

		if (!loadBBoxes(txtName, gtTestBoxes[i], gtTestClsIdx[i]))
			return;
	}
	printf("Load annotations (generic over classes) finished\n");

}


bool DataBarcode::loadBBoxes(string &nameNE, VecRect &boxes, vecI &clsIdx)
{
	string fName = nameNE;

	fstream fs;
	int numbarcode ;
	fs.open(fName, ios::in);
	fs >> numbarcode;
	boxes.clear();
	clsIdx.clear();
	for (int i = 0; i < numbarcode;i++)
	{
		int p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y;
		char temp;
		fs >> p1x >> temp >> p1y;
		fs >> p2x >> temp >> p2y;
		fs >> p3x >> temp >> p3y;
		fs >> p4x >> temp >> p4y;

// 		boxes.push_back(Vec8i(p1x, p1y, p2x, p2y,
// 						      p3x, p3y, p4x, p4y));

		boxes.push_back(cvRect(p2x, p2y,p4x-p2x, p4y-p2y));

		clsIdx.push_back(1);
	}
	fs.close();
	return true;
}