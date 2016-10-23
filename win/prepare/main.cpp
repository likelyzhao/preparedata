#include "prepare.h"

void main()
{
	Prepare mypre;
	mypre.loadData("E:\\DL");
	mypre.creatTrainTestPosNeg();
	//mypre.augmentNeg();
	//mypre.augmentPos();
	//mypre.genetatesamplePosNeg();
	//mypre.deleteNotxt();

}