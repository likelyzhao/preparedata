#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#include "stdio.h"
#include "../inc/preparedata.h"
#include "string"

using namespace cv;

#define FIXWIDTH 60
#define FIXHEIGHT 40

Preparedata::Preparedata()
{

};

Preparedata::~Preparedata()
{

};


int Preparedata::ReadList(std::string listName)
{
	FILE * fp = fopen(listName.c_str(),"r");	
	if (fp == NULL)
	{
		printf("open file error\n");
		return -1;
	}

	while (feof(fp) != TRUE)
	{
		char tempfileName[255];
		fscanf(fp,"%s\n",tempfileName);
		printf("%s\n",tempfileName);
		imgNameList.push_back(string(tempfileName));	

	}

	fclose(fp);

};

