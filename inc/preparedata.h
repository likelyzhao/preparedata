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
	int ImgRotateALL();
	int ImgAddBlurALL();
	int ImgPrespectiveALL();
	int ImgAddNoiseALL();
	int ImgScaleALL();
	int ImgBinary();
	private:
	std::vector <std::string> imgNameList;
	std::vector <std::vector<CvRect> > imgLocList;		
};
