#include "cv.h"
#include "highgui.h"
#include "string"
#include "vector"
#include "fstream"
#include "windows.h"
#include "getopt.h"

using namespace  std;

vector<string> creatDirList(string path)
{
	fstream fs;
	string cmd = "dir " + path + "*.jpg /s/b >1.txt";
	path += "dir.bat";
	fs.open(path.c_str(), ios::out);
	//	fs << "dir "<<path<<"\\*.jpg /s/b >1.txt";
	fs << cmd;
	fs.close();
	system(path.c_str());
	vector<string> temp;
	fs.open("1.txt", ios::in);
	while (fs.peek() != EOF)
	{
		string tempS;
		fs >> tempS;
		if (tempS.size() == 0)
		{
			continue;
		}
		temp.push_back(tempS);
	}
	fs.close();

	DeleteFileA("1.txt");
	DeleteFileA(path.c_str());
	return temp;
}

struct option longOptions[] =
{
	{ "filePath", optional_argument, NULL, 'f' },
	{ "resizeWidth", optional_argument, NULL, 'w' },
	{ "resizeHeigh", optional_argument, NULL, 'h' },
	{ 0, 0, 0, 0 }
};
#define  RESIZEWIDTH 40
#define  RESIZEHEIGHT 40

void main(int argc,char * argv[])
{
	int c;
	int optIdx = -1;
	int resizeWidth = RESIZEWIDTH;
	int resizeHeight = RESIZEHEIGHT;
	string dirPath = ".\\";

	while ((c = getopt_long(argc, argv, "h:w:f:", longOptions, &optIdx)) != -1)
	{
		//printf("opt = %c\n",c);
		switch (c)
		{
		case 'f':
		{
					dirPath = string(optarg);
					printf("file Path  = %s\n", dirPath.c_str());
					break;
		}
		case 'w':
		{
					resizeWidth = atoi(optarg);
					printf("resizewidth  = %d\n", resizeWidth);
					break;
		}
		case 'h':
		{
					resizeHeight = atoi(optarg);
					printf("resizeheight  = %d\n", resizeHeight);
					break;
		}

		}
	}

	vector<string> file = creatDirList(dirPath);
	IplImage * resizeImg = cvCreateImage(cvSize(resizeWidth, resizeHeight), 8, 1);
	for (int i = 0; i < file.size();i++)
	{
		IplImage * im = cvLoadImage(file[i].c_str(),0);
		cvResize(im, resizeImg, CV_INTER_AREA);
		cvSaveImage(file[i].c_str(), resizeImg);
		cvReleaseImage(&im);
	}
	cvReleaseImage(&resizeImg);

	return;
}