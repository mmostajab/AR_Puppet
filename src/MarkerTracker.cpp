
#include "MarkerTracker.h"

#define _USE_MATH_DEFINES
#include <math.h>


MarkerTracker::MarkerTracker(std::string intrinsicFileName, double iMarkerSize)
{
    markerSize = iMarkerSize;
    camParam.readFromXMLFile(intrinsicFileName);
}

MarkerTracker::~MarkerTracker()
{
}

void MarkerTracker::findMarker( cv::Mat &frame, std::vector<Marker> &markers)
{
    cv::namedWindow("in", 1);
    camParam.resize(frame.size());
    std::vector < aruco::Marker > myMarkers;
    MDetector.detect(frame, myMarkers, camParam, markerSize);

    for (std::size_t i(0); i < myMarkers.size(); i++)
    {
        Marker tempMarker;
        myMarkers[i].draw(frame, cv::Scalar(0, 0, 255), 2);
        cv::Mat Rotation;
        cv::Rodrigues(myMarkers[i].Rvec, Rotation);
        //hard code !!!
        int code = myMarkers[i].id;
        if (code == 428 || code == 838 || code == 341 || code == 977 || code == 299)
            tempMarker.code = 0;
        else if (code == 64 || code == 477 || code == 985 || code == 175 || code == 882)
            tempMarker.code = 1;
        else if (code == 233 || code == 461 || code == 76 || code == 982 || code == 717)
            tempMarker.code = 2;
        else if (code == 1003 || code == 908 || code == 985 || code == 181 || code == 760)
            tempMarker.code = 3;

        tempMarker.code = myMarkers[i].id;
        
        tempMarker.resultMatrix[0] = Rotation.at<float>(0,0);
        tempMarker.resultMatrix[1] = Rotation.at<float>(0,1);
        tempMarker.resultMatrix[2] = Rotation.at<float>(0,2);
        tempMarker.resultMatrix[4] = Rotation.at<float>(1,0);
        tempMarker.resultMatrix[5] = Rotation.at<float>(1,1);
        tempMarker.resultMatrix[6] = Rotation.at<float>(1,2);
        tempMarker.resultMatrix[8] = Rotation.at<float>(2,0);
        tempMarker.resultMatrix[9] = Rotation.at<float>(2,1);
        tempMarker.resultMatrix[10] = Rotation.at<float>(2,2);


        cout << myMarkers[i] << endl;

        tempMarker.resultMatrix[3] = myMarkers[i].Tvec.at<float>(0, 0);
        tempMarker.resultMatrix[7] = myMarkers[i].Tvec.at<float>(1, 0);
        tempMarker.resultMatrix[11] = myMarkers[i].Tvec.at<float>(2, 0);
        tempMarker.resultMatrix[12] = 0;
        tempMarker.resultMatrix[13] = 0;
        tempMarker.resultMatrix[14] = 0;
        tempMarker.resultMatrix[15] = 1;

        // std::cout << "Rotation: " << Rotation << std::endl;
        // std::cout << "Translation: " << myMarkers[i].Tvec.at<float>(0, 0) << " " <<
                  // myMarkers[i].Tvec.at<float>(1, 0) << " " << myMarkers[i].Tvec.at<float>(2, 0) << std::endl;

        std::cout << "Result Matrix: " << std::endl;
        for(int k=0; k<4; k++)
        {
            for(int y=0; y<4; y++)
                std::cout << tempMarker.resultMatrix[k * 4 + y] <<" ";
            std::cout << std::endl;
        }

        markers.push_back(tempMarker);
    }
    cv::imshow("in", frame);
}