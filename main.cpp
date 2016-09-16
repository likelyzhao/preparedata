#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#include "stdio.h"
#include "inc/preparedata.h"
using namespace cv;

#define FIXWIDTH 60
#define FIXHEIGHT 40


int main()
{
	Preparedata myprepare;
	myprepare.ReadList("./FileLists/list.txt");

	return 0;


	IplImage * img;
	FILE * fp = fopen("./FileLists/list.txt","r");	
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
	}

	fclose(fp);





	
	img = cvLoadImage("1.jpg",CV_LOAD_IMAGE_GRAYSCALE);		
	cvNamedWindow("test",CV_WINDOW_AUTOSIZE);
	IplImage * imgResized = cvCreateImage(cvSize(FIXWIDTH,FIXHEIGHT),img->depth,img->nChannels);

	cvResize(img,imgResized);

	cvShowImage("test",imgResized);
	waitKey(-1);
	cvReleaseImage(&img);
	printf("hello\n");

}
