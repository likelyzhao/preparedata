#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#include "stdio.h"

using namespace cv;

#define FIXWIDTH 60
#define FIXHEIGHT 40


int main()
{
	IplImage * img;
	FILE * fp = fopen("Filelists/list.txt","r");	
	

	while (1)
	{

		fscanf(fp,	
	
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
