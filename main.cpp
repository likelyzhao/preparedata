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
	//myprepare.CreatTrainingPair("/home/zzj");
	myprepare.ReadList("./FileLists/list.txt");
	printf("testing\n");	

	
	return 0;



IplImage * img;

	img = cvLoadImage("/home/zzj/caffe/Data/Barcode/Pictures10894_13_23_34.jpg",CV_LOAD_IMAGE_GRAYSCALE);		
	if (img ==NULL)
{
	printf("img = NULL\n");
}
		
	cvNamedWindow("test",CV_WINDOW_AUTOSIZE);
	IplImage * imgResized = cvCreateImage(cvSize(FIXWIDTH,FIXHEIGHT),img->depth,img->nChannels);

	cvResize(img,imgResized);

	cvShowImage("test",img);
	waitKey(-1);
	cvReleaseImage(&img);
	printf("hello\n");

}
