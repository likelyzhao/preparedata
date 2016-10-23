#include "inlineTools.h"
//#include "algorithm"

inline double inlineTools::interUnio(const CvRect &bb, const CvRect &bbgt) 
{
	int bi[4];
	bi[0] = MAX(bb.x, bbgt.x);
	bi[1] = MAX(bb.y, bbgt.y);
	bi[2] = MIN(bb.x + bb.width, bbgt.x + bbgt.width);
	bi[3] = MIN(bb.y + bb.height, bbgt.y + bbgt.height);

	double iw = bi[2] - bi[0] + 1;
	double ih = bi[3] - bi[1] + 1;
	double ov = 0;
	if (iw > 0 && ih > 0){

		double ua = MIN(iw*ih / (bb.width*bb.height),
			iw*ih / (bbgt.width *bbgt.height));
		ov = ua;
		//	ov = iw*ih / ua;

	}

	return ov;
}

double inlineTools::overLapLabelBox(const CvRect &bb, const CvRect &bbgt)
{
	int bi[4];
	bi[0] = MAX(bb.x, bbgt.x);
	bi[1] = MAX(bb.y, bbgt.y);
	bi[2] = MIN(bb.x + bb.width, bbgt.x + bbgt.width);
	bi[3] = MIN(bb.y + bb.height, bbgt.y + bbgt.height);

	double iw = bi[2] - bi[0] + 1;
	double ih = bi[3] - bi[1] + 1;
	double ov = 0;
	if (iw > 0 && ih > 0){
		double ua = iw*ih / (bbgt.width * bbgt.height);
		ov = ua;

	}

	return ov;
}
