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
	//	printf("%s\n",tempfileName);
		imgNameList.push_back(string(tempfileName));	
	}
	fclose(fp);
printf("total training sample num= %d\n",(int)imgNameList.size());
	for(int i =0 ;i<imgNameList.size();i++)
	{
		size_t pos = imgNameList[i].find(".jpg");
		std::string txtname = imgNameList[i];
		txtname.replace(pos,4,".txt");
		//printf("%s\n",txtname.c_str());
		fp = fopen(txtname.c_str(),"r");
		if(fp == NULL)
		{
//			printf("no txt file in %s\n",txtname.c_str());
			continue;
		}
		int numRect;
		fscanf(fp,"%d\n",&numRect);
		std::vector<CvRect> templistRect;
		for(int j =0;j<numRect;j++)
		{
			CvRect rectTemp;
			int tempx,tempy,tempx2,tempy2,tempx3,tempy3;
			fscanf(fp,"%d,%d",&tempx,&tempy);
			fscanf(fp,"%d,%d",&tempx2,&tempy2);
			fscanf(fp,"%d,%d",&tempx3,&tempy3);
			rectTemp.x = tempx2;
			rectTemp.y = tempy2;
			rectTemp.width = tempx3 - tempx2;
			rectTemp.height = tempy - tempy2;
			templistRect.push_back(rectTemp);
			//			rectTemp.
			fscanf(fp,"%d,%d",&tempx,&tempy);
		}
		imgLocList.push_back(templistRect);

	}

	printf("testing\n");	
	/////testing 
	int idx = 200;
//	printf("%s\n",imgNameList[idx].c_str());
	IplImage * imgtest = cvLoadImage(imgNameList[idx].c_str(),CV_LOAD_IMAGE_GRAYSCALE);

//	for(int j =0;j<imgLocList[idx].size();j++)
//	{
//		cvRectangleR(imgtest,imgLocList[idx][j],cvScalar(255,0,0,0));
//	}

	cvNamedWindow("test2",CV_WINDOW_FREERATIO);
	cvShowImage("test2",imgtest);
	waitKey(-1);
	cvReleaseImage(&imgtest);

	
};

int Preparedata::CreatTrainingPair(std::string outdir)
{

}

	
