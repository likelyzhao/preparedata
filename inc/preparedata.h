#include "opencv/cv.h"
#include "string"
#include "vector"


class Preparedata
{
	public:
	Preparedata();
	~Preparedata();
	int ReadList(std::string listName);

	int CreatTrainingPair(std::string outDir);
	
	std::string GetImgNamebyIdx(int idx);
	std::vector<CvRect> GetImgLocbyIdx(int idx);
	
	int ImgRotateALL();
	int ImgAddBlurALL();
	int ImgPrespectiveALL();
	int ImgAddNoiseALL();
	int ImgScaleALL();
	int ImgBinary();
	int visualCheck();
	private:
	std::vector <std::string> imgNameList;
	std::vector <std::vector<CvRect> > imgLocList;		
};
