#ifndef MARKER_TRACKER_KK_H
#define MARKER_TRACKER_KK_H

#include <opencv/cv.h>

struct Marker_KK{
	int code;
	float resultMatrix[16];
};

class CvMemStorage;

class MarkerTracker_KK{
public:
	MarkerTracker_KK(double kMarkerSize_) 
		:	thresh(100),
		bw_thresh(100),
		kMarkerSize(kMarkerSize_)
	{
		init();
	}
	MarkerTracker_KK(double kMarkerSize_, int thresh_, int bw_thresh_) 
		:	thresh(thresh_),
		bw_thresh(bw_thresh_),
		kMarkerSize(kMarkerSize_)
	{
		init();
	}
	~MarkerTracker_KK(){
		cleanup();
	}
    void findMarker( cv::Mat &img_bgr, std::vector<Marker_KK> &markers );
//	void findMarker( cv::Mat &img_bgr, float resultMatrix[16] );
protected:
	void init( );
	void cleanup( );

	//camera settings
	const double kMarkerSize; // Marker size [m]

	cv::Mat img_gray;
	cv::Mat img_mono;

	int thresh; // Threshold (gray to mono)
	int bw_thresh; // threshold for (gray maker to ID image)

	CvMemStorage* memStorage;
};

#endif // MARKER_TRACKER_H
