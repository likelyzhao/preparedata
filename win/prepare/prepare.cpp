#include "prepare.h"
#define  USINGGLOG
#ifdef USINGGLOG
#include "glog//logging.h"
#endif
#include "io.h"
#include "direct.h"
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "inlineTools.h"
#include "fstream"
//#include "pair"
#include "algorithm"
#include "iostream"

#define  RANDOMNUM 20

IplImage* rotateImage(IplImage* img, CvPoint centerRot,int degree,double scale ){
	double angle = degree  * CV_PI / 180.; // 弧度    
	double a = sin(angle), b = cos(angle);
	int width = scale * img->width;
	int height = scale  *img->height;
	int width_rotate = int(height * fabs(a) + width * fabs(b));
	int height_rotate = int(width * fabs(a) + height * fabs(b));
	//旋转数组map  
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]  
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]  
	float map[6];
	CvMat map_matrix = cvMat(2, 3, CV_32F, map);

// 	float diffx = width / 2 - scale * centerRot.x;
// 	float diffy = height/ 2 - scale * centerRot.y;

 	float diffx = width  - img->width;
	float diffy =/* (height - img->height)/2*/0;

// 	float offsetx = int(scale * centerRot.y * fabs(a) + scale * centerRot.x * fabs(b));
// 	float offsety = int(scale * centerRot.x * fabs(a) + scale * centerRot.y * fabs(b));

// 	offsetx -= scale *centerRot.x;
// 	offsety -= scale *centerRot.y;

//	diffx = diffy *fabs(a) + diffx * fabs(b) + (width_rotate - width) / 2;
//	diffy = diffx *fabs(a) + diffy * fabs(b);


	//centerRot.x = scale  * centerRot.y * fabs(a) + scale *centerRot.x * fabs(b);
	//centerRot.y = scale * centerRot.x * fabs(a) + scale *centerRot.y * fabs(b);


	// 旋转中心  
	CvPoint2D32f center = cvPoint2D32f(scale * centerRot.x , scale *centerRot.y );
	//CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);
	cv2DRotationMatrix(center, degree, scale, &map_matrix);
// 	map[2] += width;
// 	map[5] += height;
//   	map[2] += diffx;
//  	map[5] += diffy;


	map[2] += (width_rotate - scale * centerRot.x *2) / 2;
	map[5] += (height_rotate - scale * centerRot.y*2) / 2;

	IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), 8, 3);
	//对图像做仿射变换  
	//CV_WARP_FILL_OUTLIERS - 填充所有输出图像的象素。  
	//如果部分象素落在输入图像的边界外，那么它们的值设定为 fillval.  
	//CV_WARP_INVERSE_MAP - 指定 map_matrix 是输出图像到输入图像的反变换，  
	cvWarpAffine(img, img_rotate, &map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
	return img_rotate;
}


inline string getfilename(string s)
{
	size_t idx = s.find_last_of('\\');
	if (idx ==s.npos)
	{
		return s;
	}
	else
	{
	//	string ss = s.substr( idx + 1);
		return s.substr(idx);
	}

}
inline CvRect loadRect(string path)
{
	CvRect res;
	fstream fs;
	fs.open(path, ios::in);
	fs >> res.x >> res.y >> res.width >> res.height;
	fs.close();
	return res;
}

double inline maxoverlap(const CvRect &bb, const VecRect &bbgts) 
{ 
	double maxV = 0; 
	for (size_t i = 0; i < bbgts.size(); i++){
		maxV = MAX(maxV, overLapLabelBox(bb, bbgts[i]));
	}
		return maxV;
}


Prepare::Prepare()
{
#ifdef USINGGLOG
	google::InitGoogleLogging("Prepare");
	google::SetLogDestination(google::INFO,"E:\\glog\\");
	FLAGS_alsologtostderr = true;
	LOG(INFO) << "start";
#endif
	srand(time(NULL));
}

Prepare::~Prepare()
{

}

int Prepare::loadData(CStr path)
{
	m_dataset.init(path);
	m_dataset.loadAnnotations();
	return 0;
}

int Prepare::deleteNotxt()
{

	for (vecS::iterator it = m_dataset.trainSet.begin(); it != m_dataset.trainSet.end(); it++)
	{
		string txtname = *it;
		txtname.replace(txtname.end() - 4, txtname.end(), ".txt");
		if (_access(txtname.c_str(), 0) == -1)
		{
			m_dataset.trainSet.erase(it);
#ifdef USINGGLOG
			LOG(INFO) << it->c_str();
#endif
		}
	}
	return 0;
}

VecRect Prepare::getDivideCutRectangles(CvSize imgsize, int cutnum)
{
	VecRect res;
	int * widthcad = new int[cutnum+1]; 
	int * heightcad = new int[cutnum+1];

	for (int i = 0; i <= cutnum;i++)
	{
		widthcad[i] = imgsize.width / cutnum*i;
		heightcad[i] = imgsize.height / cutnum*i;
	}

	for (int i = 0; i < cutnum-1;i++)
	{
		for (int j = 0; j < cutnum;j++)
		{
			res.push_back(cvRect(widthcad[i], heightcad[j], 
				widthcad[i + 2] - widthcad[i], heightcad[j + 1] - heightcad[j]));
		}
	}

	for (int i = 0; i < cutnum; i++)
	{
		for (int j = 0; j < cutnum-1; j++)
		{
			res.push_back(cvRect(widthcad[i], heightcad[j], 
				widthcad[i + 1] - widthcad[i], heightcad[j + 2] - heightcad[j]));
		}
	}

	delete[]widthcad;
	delete[]heightcad;
	return res;
}

void Prepare::genetatesamplePosNeg()
{
	if (m_dataset.trainSet.size() == 0)
	{
		LOG(INFO) << "TrainingSet is empty!";
		return;
	}

	if (m_dataset.trainSet.size() != m_dataset.gtTrainBoxes.size())
	{
		LOG(INFO) << "TrainingSet and label size not match!";
		return;
	}

	string posPath = m_dataset.wkDir + "\\pos";
	mkdir(posPath.c_str());
	string negPath = m_dataset.wkDir + "\\neg";
	mkdir(negPath.c_str());

	for (int i = 0; i < m_dataset.trainNum;i++)
	{
		IplImage * im = cvLoadImage(m_dataset.trainSet[i].c_str());
		VecRect rects = getDivideCutRectangles(cvGetSize(im), 3);
		LOG_IF(INFO, i % 100 == 0) << "Processing " << i << " images";
		for (int j = 0; j < rects.size();j++)
		{
			cvSetImageROI(im, rects[j]);
			char tempfileName[255];
			sprintf(tempfileName, "\\Image_%d_Rect_%d.jpg", i, j);
			
			if (maxoverlap(rects[j], m_dataset.gtTrainBoxes[i]) > 0.95)		
			{
				string savepath = posPath + string(tempfileName);
				cvSaveImage(savepath.c_str(),im);
				savepath.replace(savepath.end() - 4, savepath.end(), ".txt");
				fstream fs;			
				fs.open(savepath, ios::out);
	
				for (size_t k = 0; k < m_dataset.gtTrainBoxes[i].size(); k++){
					if (overLapLabelBox(rects[j], m_dataset.gtTrainBoxes[i][k])>0.95)
					{
						fs << (m_dataset.gtTrainBoxes[i][k].x - rects[j].x)<<endl;
						fs << (m_dataset.gtTrainBoxes[i][k].y - rects[j].y)<<endl;
						fs << (m_dataset.gtTrainBoxes[i][k].width)<<endl;
						fs << (m_dataset.gtTrainBoxes[i][k].height)<<endl;
					}
				}
				fs.close();
				//// check crop
// 				savepath.replace(savepath.end() - 4, savepath.end(), ".jpg");
// 				IplImage * test = cvLoadImage(savepath.c_str());
// 				savepath.replace(savepath.end() - 4, savepath.end(), ".txt");
// 				fs.open(savepath, ios::in);
// 				CvPoint p1, p2;
// 				fs >> p1.x;
// 				fs >> p1.y;
// 				fs >> p2.x;
// 				fs >> p2.y;
// 				p2.x += p1.x;
// 				p2.y += p1.y;
// 				cvDrawRect(test, p1, p2, cvScalar(255.0, 0, 0));
// 				cvNamedWindow("test");
// 				cvShowImage("test", test);
// 				cvWaitKey();
// 				cvDestroyWindow("test");

			}
			else
			{
				string savepath = negPath + string(tempfileName);
				cvSaveImage(savepath.c_str(), im);
			}		

			cvResetImageROI(im);
		}
		cvReleaseImage(&im);
	}

};

vecS Prepare::creatDirList(string path)
{
	fstream fs;
	string cmd = "dir "  +  path  + "*.jpg /s/b >1.txt";
	path += "dir.bat";
	fs.open(path.c_str(),ios::out);
//	fs << "dir "<<path<<"\\*.jpg /s/b >1.txt";
	fs << cmd;
	fs.close();
	system(path.c_str());
	vecS temp;	
	fs.open("1.txt", ios::in);
	while (fs.peek() != EOF)
	{
		string tempS;
		fs >>tempS;
		if (tempS.size() ==0)
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

void Prepare::augmentPos()
{
	//posPath =
	string augPath = m_dataset.wkDir + "\\aug";
	mkdir(augPath.c_str());

	m_poslist = creatDirList(m_dataset.wkDir + "\\pos\\");
	LOG(INFO) << "loading pos sample " << m_poslist.size();
	for (int i = 0; i < m_poslist.size();i++)
	{
		string savepath = m_poslist[i];
		IplImage * imgSrc = cvLoadImage(savepath.c_str());
		savepath.replace(savepath.end() - 4, savepath.end(), ".txt");
		CvRect res = loadRect(savepath);
		CvPoint centerBar = cvPoint(res.x + res.width / 2, res.y + res.height / 2);
		CvPoint centerImg =cvPoint(imgSrc->width / 2, imgSrc->height / 2);

		////////// add 1 rotate 90 
		IplImage * rotateImg = rotateImage(imgSrc, centerImg, 90,1);
		string savefilename = getfilename(m_poslist[i]);

		savefilename.resize(savefilename.size() - 4,NULL);
		savefilename.append("_Rotate_90.jpg");

		cvSaveImage((augPath + savefilename).c_str(), rotateImg);
		cvReleaseImage(&rotateImg);
		//// add random angle  no need 

// 		if ((centerBar.x - imgSrc->width / 4) > 0 && (centerBar.x + imgSrc->width / 4) < imgSrc->width
// 			&& (centerBar.y - imgSrc->height / 4) > 0 && (centerBar.y + imgSrc->height / 4) < imgSrc->height)
// 		{
// 			cvSetImageROI(imgSrc, cvRect(centerBar.x - imgSrc->width / 4, centerBar.y - imgSrc->height / 4, imgSrc->width / 2, imgSrc->height / 2));
// 			IplImage * imcrop = cvCloneImage(imgSrc);
// 			rotateImg = rotateImage(imcrop, centerImg, 0, 2);
// 		}


// 		rotateImg = rotateImage(imgSrc, centerImg, 30, 1);
// 		cvDrawRect(rotateImg, cvPoint(rotateImg->width / 2 - imgSrc->width / 3,			rotateImg->height / 2 - imgSrc->height / 3),
// 			cvPoint(rotateImg->width / 2 + imgSrc->width / 3, rotateImg->height / 2 + imgSrc->height / 3),
// 			cvScalar(255, 0, 0, 0));
// 		cvNamedWindow("test",0);
// 		cvShowImage("test", rotateImg);
// 		cvWaitKey();
// 		cvDestroyWindow("test");

// 		rotateImg = rotateImage(imgSrc, 270);
// 		cvNamedWindow("test");
// 		cvShowImage("test", rotateImg);
// 		cvWaitKey();
// 		cvDestroyWindow("test");

		/// add blur 
		IplImage * smoothImg =  cvCloneImage(imgSrc);
		int kernelsize = ((float)rand() / RAND_MAX) * 15 + 6;
		if (kernelsize %2 ==0)
		{
			kernelsize++;
		}
		cvSmooth(imgSrc, smoothImg, CV_GAUSSIAN,kernelsize);
	//	cvSmooth(imgSrc, smoothImg, CV_BILATERAL, 1, 150,240,480);
		
// 		cvNamedWindow("test");
// 		cvShowImage("test", smoothImg);
// 		cvWaitKey();
// 		cvDestroyWindow("test");

		savefilename = getfilename(m_poslist[i]);

		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_blur.jpg");

		cvSaveImage((augPath + savefilename).c_str(), smoothImg);
		cvReleaseImage(&smoothImg);


		IplImage * lowcontract = cvCloneImage(imgSrc);
		cvConvertScale(imgSrc, lowcontract, 0.35, 0.5);
// 		cvNamedWindow("test");
// 		cvShowImage("test", lowcontract);
// 		cvWaitKey();
// 		cvDestroyWindow("test");
		savefilename = getfilename(m_poslist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_lowcontrast_0.3.jpg");
		cvSaveImage((augPath + savefilename).c_str(), lowcontract);

		cvConvertScale(imgSrc, lowcontract, 0.2, 0.5);
// 		cvNamedWindow("test");
// 		cvShowImage("test", lowcontract);
// 		cvWaitKey();
// 		cvDestroyWindow("test");
		savefilename = getfilename(m_poslist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_lowcontrast_0.1.jpg");
		cvSaveImage((augPath + savefilename).c_str(), lowcontract);

		cvReleaseImage(&lowcontract);

		IplImage * highcontract = cvCloneImage(imgSrc);
		cvConvertScale(imgSrc, highcontract, 2, 0.5);
		savefilename = getfilename(m_poslist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_highcontract_2.jpg");
		cvSaveImage((augPath + savefilename).c_str(), highcontract);

		cvConvertScale(imgSrc, highcontract, 3.5, 0.5);
		savefilename = getfilename(m_poslist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_highcontract_4.jpg");
		cvSaveImage((augPath + savefilename).c_str(), highcontract);
		cvReleaseImage(&highcontract);


 		cvReleaseImage(&imgSrc);
		//cvReleaseImage(&rotateImg);
	}


}

void Prepare::augmentNeg()
{
	//posPath =
	string augPath = m_dataset.wkDir + "\\augNeg";
	mkdir(augPath.c_str());
	m_neglist = creatDirList(m_dataset.wkDir + "\\neg\\");
	LOG(INFO) << "loading Neg sample " << m_neglist.size();
	for (int i = 0; i < m_neglist.size(); i++)
	{
		string savepath = m_neglist[i];
		LOG(INFO) << m_neglist[i];
		IplImage * imgSrc = cvLoadImage(savepath.c_str());
		CvPoint centerImg = cvPoint(imgSrc->width / 2, imgSrc->height / 2);
		////////// add 1 rotate 90 
		IplImage * rotateImg = rotateImage(imgSrc, centerImg, 90, 1);
		string savefilename = getfilename(m_neglist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_Rotate_90.jpg");

		cvSaveImage((augPath + savefilename).c_str(), rotateImg);
		cvReleaseImage(&rotateImg);

		/// random cut wowowo 
		for (int j = 0; j < RANDOMNUM; j++)
		{
			float p1  = ((float)rand() / RAND_MAX) ;
			float p2 = ((float)rand() / RAND_MAX);
			float p3 = ((float)rand() / RAND_MAX);
			float p4 = ((float)rand() / RAND_MAX);

			CvRect cropRect = cvRect(min(p1, p2) * imgSrc->width,
				min(p3, p4) * imgSrc->height,
				(max(p1, p2) - min(p1, p2)) * imgSrc->width,
				(max(p3, p4) - min(p3, p4)) * imgSrc->height);
			// filter retangcle with area 

			if (cropRect.width * cropRect.height > 0.2 * imgSrc->width * imgSrc->height)
			{
				cvSetImageROI(imgSrc, cropRect);			
				string savefilename = getfilename(m_neglist[i]);

				IplImage * imgsave = cvCreateImage(cvSize(imgSrc->width,imgSrc->height), 8, 3);
				cvResize(imgSrc,imgsave);

				savefilename.resize(savefilename.size() - 4, NULL);
				char formatfilestring[255];
				sprintf(formatfilestring,"_Rcut_%d.jpg", j);
				savefilename.append(formatfilestring);

				cvSaveImage((augPath + savefilename).c_str(), imgsave);
				cvResetImageROI(imgSrc);
				cvReleaseImage(&imgsave);
			}

		}

		/// add blur 
		IplImage * smoothImg = cvCloneImage(imgSrc);
		int kernelsize = ((float)rand() / RAND_MAX) * 15 + 6;
		if (kernelsize % 2 == 0)
		{
			kernelsize++;
		}
		cvSmooth(imgSrc, smoothImg, CV_GAUSSIAN, kernelsize);
		savefilename = getfilename(m_neglist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_blur.jpg");
		cvSaveImage((augPath + savefilename).c_str(), smoothImg);
		cvReleaseImage(&smoothImg);


		IplImage * lowcontract = cvCloneImage(imgSrc);
		cvConvertScale(imgSrc, lowcontract, 0.35, 0.5);
		// 		cvNamedWindow("test");
		// 		cvShowImage("test", lowcontract);
		// 		cvWaitKey();
		// 		cvDestroyWindow("test");
		savefilename = getfilename(m_neglist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_lowcontrast_0_3.jpg");
		cvSaveImage((augPath + savefilename).c_str(), lowcontract);

		cvReleaseImage(&lowcontract);

		IplImage * highcontract = cvCloneImage(imgSrc);
		cvConvertScale(imgSrc, highcontract, 2, 0.5);
		savefilename = getfilename(m_neglist[i]);
		savefilename.resize(savefilename.size() - 4, NULL);
		savefilename.append("_highcontract_2.jpg");
		cvSaveImage((augPath + savefilename).c_str(), highcontract);
		cvReleaseImage(&highcontract);


		cvReleaseImage(&imgSrc);
		//cvReleaseImage(&rotateImg);
	}


}

void Prepare::creatTrainTestPosNeg()
{
	vector<pair<string, int> > totalsamples;
	int testNum = 0;
	m_poslist = creatDirList(m_dataset.wkDir + "\\pos\\");
	LOG(INFO) << "loading pos sample " << m_poslist.size();
	for (int i = 0; i < m_poslist.size(); i++)
	{
		pair<string, int> a = make_pair(m_poslist[i],0);
		totalsamples.push_back(a);
	}
	m_neglist = creatDirList(m_dataset.wkDir + "\\neg\\");
	LOG(INFO) << "loading neg sample " << m_neglist.size();
	for (int i = 0; i < m_neglist.size(); i++)
	{
		pair<string, int> a = make_pair(m_neglist[i], 1);
		totalsamples.push_back(a);
	}	
	random_shuffle(totalsamples.begin(), totalsamples.end());
	printf("total num of trainfile is %d\n", totalsamples.size());
	printf("please set the num of validating sets\n");
	cin >> testNum;
	fstream fs;
	fs.open("test.txt", ios::out);
	for (int i = 0; i < testNum;i++)
	{
		fs << totalsamples[i].first << " " << totalsamples[i].second<<endl;
	}
	fs.close();
	
	fs.open("train.txt", ios::out);
	for (int i = testNum; i < totalsamples.size(); i++)
	{
		fs << totalsamples[i].first << " " << totalsamples[i].second<<endl;
	}
	fs.close();




}
