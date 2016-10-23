/************************************************************************/
/* This source code is free for both academic and industry use.         */
/* Some important information for better using the source code could be */
/* found in the project page: http://mmcheng.net/bing					*/
/************************************************************************/

//s#include "stdafx.h"
#include "cv.h"
#include "string"
#include "vector"

using namespace std;


typedef vector<CvRect> VecRect;
typedef vector<int> vecI;
typedef const string CStr;
//typedef const Mat CMat;
typedef vector<string> vecS;
//typedef vector<Mat> vecM;
typedef vector<float> vecF;
typedef vector<double> vecD;


#define _S(str) ((str).c_str())

#pragma once
using namespace  std;

class DataBarcode
{

public:
	DataBarcode();
	DataBarcode(const string &wkDir);
	~DataBarcode(void);

	// Organization structure data for the dataset
	string wkDir; // Root working directory, all other directories are relative to this one
	string resDir, localDir; // Directory for saving results and local data
	string TrImgscript, TestImgscript; // Image and annotation path

	// Information for training and testing
	int trainNum, testNum;
	vecS trainSet, testSet; // File names (NE) for training and testing images

	vector<VecRect> gtTrainBoxes, gtTestBoxes; // Ground truth bounding boxes for training and testing images
	vector<vecI> gtTrainClsIdx, gtTestClsIdx; // Object class indexes  


	// Load annotations
	void loadAnnotations();

	static bool cvt2OpenCVYml(CStr &annoDir); // Needs to call yml.m in this solution before running this function.

	static bool importSaliencyBench(CStr &salDir = "./THUS10000/", CStr &vocDir = "./THUS10000/");

	static void importPaulData(CStr &inData = "Z:/datasets/toMing/", CStr &outData = "D:/WkDir/DetectionProposals/Paul/");

	static bool importImageNetBenchMark(CStr &orgDir = "D:/WkDir/ImageNet/", CStr &newDir = "D:/WkDir/DetectionProposals/ImgNet/");

	static inline double interUnio(const VecRect &box1, const VecRect &box2);

	// Get training and testing for demonstrating the generative of the objectness over classes
	void getTrainTest();
	int init(CStr wkdir);

public: // Used for testing the ability of generic over classes
	void loadDataGenericOverCls();

private:
	//void loadBox(FileNode &fn, vector<Vec4i> &boxes, vecI &clsIdx);
	bool loadBBoxes(string &nameNE, VecRect &boxes, vecI &clsIdx);
	static void getXmlStrVOC(CStr &fName, string &buf);
	static inline string keepXmlChar(CStr &str);
	static bool cvt2OpenCVYml(CStr &yamlName, CStr &ymlName); // Needs to call yml.m in this solution before running this function.

};

