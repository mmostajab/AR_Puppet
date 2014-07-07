
#include "MarkerTracker.h"

#define _USE_MATH_DEFINES
#include <math.h>


MarkerTracker::MarkerTracker(std::string intrinsicFileName, double iMarkerSize)
{
    markerSize = iMarkerSize;
    camParam.readFromXMLFile(intrinsicFileName);

    priority[0][0] = 428;
    priority[0][1] = 341;
    priority[0][2] = 977;
    priority[0][3] = 299;
    priority[0][4] = 838;

    priority[1][0] = 64;
    priority[1][1] = 882;
    priority[1][2] = 177;
    priority[1][3] = 177;
    priority[1][4] = 477;

    priority[2][0] = 233;
    priority[2][1] = 982;
    priority[2][2] = 717;
    priority[2][3] = 461;
    priority[2][4] = 76;

    priority[3][0] = 760;
    priority[3][1] = 985;
    priority[3][2] = 181;
    priority[3][3] = 1005;
    priority[3][4] = 9908;

}

MarkerTracker::~MarkerTracker()
{
}

int MarkerTracker::uniqueMarker(int id, std::vector < std::pair <int, int> > & bestCandidates, int inx)
{
    for (int j = 0; j < 4; j++)
        for (int k = 0; k < 5; k++)
            if (id == priority[j][k])
            {
                // std::cout << "id: " << id << std::endl;
                // std::cout << "old: " << bestCandidates[j].first << " " << bestCandidates[j].second << std::endl;
                if(bestCandidates[j].first > k)
                {
                    bestCandidates[j].first = k;
                    bestCandidates[j].second = inx;
                }
                // std::cout << "current: " << bestCandidates[j].first << " " << bestCandidates[j].second << std::endl;
                return inx;
            }

    return -1;
}

void MarkerTracker::findMarker( cv::Mat &frame, std::vector<Marker> &markers)
{
    cv::namedWindow("in", 1);
    camParam.resize(frame.size());
    std::vector < aruco::Marker > myMarkers;
    MDetector.detect(frame, myMarkers, camParam, markerSize);
    std::vector < std::pair <int, int> > bestCandidates;
    for(int i=0; i<4; i++)
        bestCandidates.push_back(std::make_pair(5,-1));

    for (std::size_t i(0); i < myMarkers.size(); i++)
    {   
        int res = uniqueMarker(myMarkers[i].id, bestCandidates, i);
        // std::cout << "marker inx: " << res << std::endl;
    }

    for(std::size_t i (0); i < bestCandidates.size(); i++)
    {
        if(bestCandidates[i].second == -1)
            continue;

        aruco::Marker & bestMarker = myMarkers[bestCandidates[i].second];
        Marker tempMarker;
        bestMarker.draw(frame, cv::Scalar(0, 0, 255), 2);
        cv::Mat Rotation;
        cv::Rodrigues(bestMarker.Rvec, Rotation);

        tempMarker.resultMatrix[0] = Rotation.at<float>(0, 0);
        tempMarker.resultMatrix[1] = Rotation.at<float>(0, 1);
        tempMarker.resultMatrix[2] = Rotation.at<float>(0, 2);
        tempMarker.resultMatrix[4] = Rotation.at<float>(1, 0);
        tempMarker.resultMatrix[5] = Rotation.at<float>(1, 1);
        tempMarker.resultMatrix[6] = Rotation.at<float>(1, 2);
        tempMarker.resultMatrix[8] = Rotation.at<float>(2, 0);
        tempMarker.resultMatrix[9] = Rotation.at<float>(2, 1);
        tempMarker.resultMatrix[10] = Rotation.at<float>(2, 2);


        cout << bestMarker << endl;

        tempMarker.resultMatrix[3] = bestMarker.Tvec.at<float>(0, 0);
        tempMarker.resultMatrix[7] = bestMarker.Tvec.at<float>(1, 0);
        tempMarker.resultMatrix[11] = bestMarker.Tvec.at<float>(2, 0);
        tempMarker.resultMatrix[12] = 0;
        tempMarker.resultMatrix[13] = 0;
        tempMarker.resultMatrix[14] = 0;
        tempMarker.resultMatrix[15] = 1;

        // std::cout << "Rotation: " << Rotation << std::endl;
        // std::cout << "Translation: " << myMarkers[i].Tvec.at<float>(0, 0) << " " <<
        // myMarkers[i].Tvec.at<float>(1, 0) << " " << myMarkers[i].Tvec.at<float>(2, 0) << std::endl;

        // std::cout << "Result Matrix: " << std::endl;
        // for(int k=0; k<4; k++)
        // {
        // for(int y=0; y<4; y++)
        // std::cout << tempMarker.resultMatrix[k * 4 + y] <<" ";
        // std::cout << std::endl;
        // }

        markers.push_back(tempMarker);
    }
    cv::imshow("in", frame);
}