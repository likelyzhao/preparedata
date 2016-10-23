#include "DataBarcode.h"


class Prepare
{
public:
	Prepare();
	~Prepare();
	int loadData(CStr path);
	int deleteNotxt();
	void genetatesamplePosNeg();
	VecRect getDivideCutRectangles(CvSize imgsize, int cutnum);
	vecS creatDirList(string path);
	void augmentPos();
	void augmentNeg();
	void creatTrainTestPosNeg();
protected:
private:
	DataBarcode m_dataset;
	vecS m_poslist;
	vecS m_neglist;
};