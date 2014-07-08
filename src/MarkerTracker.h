#ifndef MARKER_TRACKER_H
#define MARKER_TRACKER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <aruco/aruco.h>

struct Marker
{
    int code;
    float resultMatrix[16];
};


class MarkerTracker
{
public:
    MarkerTracker(std::string intrinsicFileName, double iMarkerSize);
    virtual ~MarkerTracker();
    void findMarker(cv::Mat &img_bgr, std::vector<Marker> &markers);
    void uniqueMarker(int id, std::vector < std::pair <int, int> > & bestCandidates, int inx);

private:
    aruco::CameraParameters camParam;
    aruco::MarkerDetector MDetector;
    int priority[4][5];
    double markerSize;

};

#endif // MARKER_TRACKER_H