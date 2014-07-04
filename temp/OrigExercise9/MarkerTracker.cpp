
#include <opencv/cv.h>
#include <opencv/highgui.h>


#define _USE_MATH_DEFINES
#include <math.h>

#include "MarkerTracker.h"
#include "PoseEstimation.h"

// void trackbarHandler( int pos, void* slider_value ) {
//  *( (int*)slider_value ) = pos;
// }

// void bw_trackbarHandler(int pos, void* slider_value ) {
//  *( (int*)slider_value ) = pos;
// }

int subpixSampleSafe ( const cv::Mat &pSrc, const cv::Point2f &p )
{
    int x = int( floorf ( p.x ) );
    int y = int( floorf ( p.y ) );

    if ( x < 0 || x >= pSrc.cols  - 1 ||
            y < 0 || y >= pSrc.rows - 1 )
        return 127;

    int dx = int ( 256 * ( p.x - floorf ( p.x ) ) );
    int dy = int ( 256 * ( p.y - floorf ( p.y ) ) );

    unsigned char *i = ( unsigned char *) ( ( pSrc.data + y * pSrc.step ) + x );
    int a = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
    i += pSrc.step;
    int b = i[ 0 ] + ( ( dx * ( i[ 1 ] - i[ 0 ] ) ) >> 8 );
    return a + ( ( dy * ( b - a) ) >> 8 );
}

void MarkerTracker::init()
{
    std::cout << "Startup\n";
    cv::namedWindow(kWinName1, CV_WINDOW_AUTOSIZE);
    // cv::namedWindow(kWinName2, CV_WINDOW_AUTOSIZE);
    // cv::namedWindow(kWinName3, CV_WINDOW_AUTOSIZE);
    // cv::namedWindow(kWinName4, 0 );
    cvResizeWindow("Marker", 120, 120 );

    // int max = 255;
    // int slider_value = 100;
    // cv::createTrackbar( "Threshold", kWinName2, &slider_value, 255, trackbarHandler, &slider_value);

    // int bw_sileder_value = bw_thresh;
    // cv::createTrackbar( "BW Threshold", kWinName2, &slider_value, 255, bw_trackbarHandler, &bw_sileder_value);

    // memStorage = cvCreateMemStorage();
}

void MarkerTracker::cleanup()
{
    // cvReleaseMemStorage (&memStorage);

    cv::destroyWindow (kWinName1);
    // cv::destroyWindow (kWinName2);
    // cv::destroyWindow (kWinName3);
    // cv::destroyWindow (kWinName4);
    std::cout << "Finished\n";
}

void MarkerTracker::findMarker( cv::Mat &frame, std::vector<Marker> &markers, int blockSize, double C)
{
    const int sizeOfPoly = 4;

    std::vector <cv::Vec4i> hierarchy;
    std::vector <std::vector <cv::Point> > contours;
    // clone the input frame inside threshold and adaptive frame
    cv::Mat adaptiveThresholdFrame;
    cv::Mat grayFrame ;

    cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

    // apply binary adaptive threshold on adaptiveThresholdFrame with MEAN method
    cv::adaptiveThreshold(grayFrame, adaptiveThresholdFrame, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, C);
    // find contours
    cv::findContours( adaptiveThresholdFrame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    std::vector <std::vector <cv::Point> > contoursPoly ( contours.size() );
    std::vector <std::vector <cv::Point> > selectedPoly;


    for ( size_t i = 0; i < contours.size(); i++ )
    {
        cv::approxPolyDP( cv::Mat(contours[i]), contoursPoly[i], cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true );
        if (contoursPoly[i].size() == sizeOfPoly && fabs(cv::contourArea(cv::Mat(contoursPoly[i]))) > 2000
                && cv::isContourConvex(cv::Mat(contoursPoly[i])) )
        {
            selectedPoly.push_back(contoursPoly[i]);

        }
    }
    cv::Scalar redPen = CV_RGB(255, 0, 0);
    cv::Scalar greenPen = CV_RGB(0, 255, 0);
    cv::Scalar bluePen = CV_RGB(0, 0, 255);
    cv::Scalar cyan = CV_RGB(0, 255, 255);
    cv::Scalar yellow = CV_RGB(255, 255, 0);

    for ( size_t i = 0; i < selectedPoly.size(); i++ )
    {
        cv::polylines(frame, selectedPoly[i], true, redPen, 2);
        cv::Vec4f line[4];
        for (size_t j = 0; j < sizeOfPoly; j++)
        {
            cv::circle(frame, selectedPoly[i][j], 3, greenPen, -1);
            double dx = (selectedPoly[i][(j + 1) % 4].x - selectedPoly[i][j].x) / 7.0;
            double dy = (selectedPoly[i][(j + 1) % 4].y - selectedPoly[i][j].y) / 7.0;

            int striplength = (int)(0.8 * std::sqrt (dx * dx + dy * dy));
            if (striplength < 5)
                striplength = 5;

            if (striplength % 2 == 0)
                striplength ++;

            cv::Size stripeSize (3, striplength);

            cv::Point2f stripVecX;
            cv::Point2f strinVecY;

            double diffLength = std::sqrt (dx * dx + dy * dy);
            stripVecX.x = dx / diffLength;
            stripVecX.y = dy / diffLength;

            strinVecY.x = stripVecX.y;
            strinVecY.y = - stripVecX.x;

            cv::Mat stripeMat(stripeSize, CV_8U);
            cv::Mat stripSobelMat(stripeSize, CV_8U);

            std::vector <cv::Point2f> edgePoints;
            for (int k = 1; k < 7; ++k)
            {
                double px = selectedPoly[i][j].x + k * dx;
                double py = selectedPoly[i][j].y + k * dy;

                cv::Point2f drawPoint (px, py);
                cv::circle(frame, drawPoint, 2, bluePen, -1);

                int widthRange = static_cast<int> (stripeSize.width / 2);
                int heigthRange = static_cast<int> (stripeSize.height / 2);
                for (int w = - widthRange ; w <= widthRange ; w++)
                {
                    for (int h = - heigthRange ; h <= heigthRange ; h++)
                    {
                        cv::Point2f subpixel;
                        subpixel.x = px + w * stripVecX.x + h * strinVecY.x ;
                        subpixel.y = py + w * stripVecX.y + h * strinVecY.y ;

                        stripeMat.at<int>(w + widthRange , h + heigthRange);
                    }
                }

                cv::Sobel(stripeMat, stripSobelMat, CV_8U, 0, 1, 3);
                edgePoints.push_back(drawPoint);
            }


            cv::fitLine(edgePoints, line[j], CV_DIST_L2, 0, 0.01, 0.01);
            cv::Point2f p1;
            p1.x = line[j][2] - (50.0f * line[j][0]);
            p1.y = line[j][3] - (50.0f * line[j][1]);

            cv::Point2f p2;
            p2.x = line[j][2] + (50.0f * line[j][0]);
            p2.y = line[j][3] + (50.0f * line[j][1]);

            cv::line(frame, p1, p2, cyan, 1, 8, 0);

        }

        cv::Point2f corners[4];
        for (int j = 0; j < sizeOfPoly; j++)
        {

            int currentIndex = j;
            int nextIndex = (j + 1) % 4;

            float u0 = line[currentIndex][0];
            float v0 = line[currentIndex][1];
            float x0 = line[currentIndex][2];
            float y0 = line[currentIndex][3];

            float u1 = line[nextIndex][0];
            float v1 = line[nextIndex][1];
            float x1 = line[nextIndex][2];
            float y1 = line[nextIndex][3];

            float a =  x1 * u0 * v1 - y1 * u0 * u1 - x0 * u1 * v0 + y0 * u0 * u1;
            float b = -x0 * v0 * v1 + y0 * u0 * v1 + x1 * v0 * v1 - y1 * v0 * u1;
            float c =  v1 * u0 - v0 * u1;

            if ( std::fabs(c) < 0.001 ) //lines parallel?
            {
                std::cout << "lines parallel" << std::endl;
                continue;
            }

            a /= c;
            b /= c;

            corners[j].x = a;
            corners[j].y = b;
            cv::circle(frame, corners[j], 3, greenPen, -1);
        }

        cv::Point2f targetCorners[4];
        targetCorners[0].x = -0.5; targetCorners[0].y = -0.5;
        targetCorners[1].x =  5.5; targetCorners[1].y = -0.5;
        targetCorners[2].x =  5.5; targetCorners[2].y =  5.5;
        targetCorners[3].x = -0.5; targetCorners[3].y =  5.5;

        cv::Mat projectMatrix( cv::Size(3, 3), CV_32FC1 );
        projectMatrix = cv::getPerspectiveTransform(corners, targetCorners);

        cv::Mat markerImage( cv::Size(6, 6), CV_8UC1 );
        // cv::Size markerSize (6, 6);
        // cv::Mat markerImage (markerSize, CV_8U);

        // cv::warpPerspective(adaptiveThresholdFrame, markerImage, projectMatrix, markerSize);
        cv::warpPerspective( grayFrame, markerImage, projectMatrix, cv::Size(6, 6) );
        int value = 100;
        // set the trackbar by defult value =50
        // cv::createTrackbar( "Threshold", "Capture Video With Threshold", &value, 255,  NULL);
        // apply binary threshold on thresholdFrame
        cv::threshold(markerImage, markerImage, value, 255, cv::THRESH_BINARY);
        // imshow(kWinName4, markerImage);

        int code = 0;
        int angle;

        for (int i = 0; i < 6; ++i)
        {
            //int pixel1 = ((unsigned char*)(iplMarker->imageData + 0*iplMarker->widthStep + i))[0]; //top
            //int pixel2 = ((unsigned char*)(iplMarker->imageData + 5*iplMarker->widthStep + i))[0]; //bottom
            //int pixel3 = ((unsigned char*)(iplMarker->imageData + i*iplMarker->widthStep))[0]; //left
            //int pixel4 = ((unsigned char*)(iplMarker->imageData + i*iplMarker->widthStep + 5))[0]; //right
            int pixel1 = markerImage.at<uchar>(0, i);
            int pixel2 = markerImage.at<uchar>(5, i);
            int pixel3 = markerImage.at<uchar>(i, 0);
            int pixel4 = markerImage.at<uchar>(i, 5);
            if ( ( pixel1 > 0 ) || ( pixel2 > 0 ) || ( pixel3 > 0 ) || ( pixel4 > 0 ) )
            {
                code = -1;
                break;
            }
        }

        if ( code < 0 )
        {
            continue;
        }


        int markerPoints[4][4];
        for ( int i = 0; i < 4; ++i)
        {
            for ( int j = 0; j < 4; ++j)
            {
                markerPoints[i][j] = markerImage.at<uchar>(i + 1, j + 1);
                markerPoints[i][j] = (markerPoints[i][j] == 0) ? 1 : 0; //if black then 1 else 0
            }
        }

        // for (int i = 0; i < 4; i++)
        // {
        //     for (int j = 0; j < 4; j++)
        //         std::cout << markerPoints[i][j] << " ";
        //     std::cout << std::endl;
        // }

        int codes[4];
        codes[0] = codes[1] = codes[2] = codes[3] = 0;
        for (int i = 0; i < 16; i++)
        {
            int row = i >> 2;
            int col = i % 4;

            codes[0] <<= 1;
            codes[0] |= markerPoints[row][col];

            codes[1] <<= 1;
            codes[1] |= markerPoints[3 - col][row];

            codes[2] <<= 1;
            codes[2] |= markerPoints[3 - row][3 - col];

            codes[3] <<= 1;
            codes[3] |= markerPoints[col][3 - row];
        }

        if ( (codes[0] == 0) || (codes[0] == 0xffff) )
            continue;

        //account for symmetry
        code = codes[0];
        for ( int i = 1; i < 4; ++i )
        {
            if ( codes[i] < code )
            {
                code = codes[i];
                angle = i;
            }
        }

        if (angle != 0)
        {
            std::vector <cv::Point2f> corrected_corners(4);
            for (int j = 0; j < 4; j++)
                corrected_corners[(j + angle) % 4] = corners[j];
            for (int j = 0; j < 4; j++)
                corners[j] = corrected_corners[j];
        }


        // for(int i=0 ; i< sizeOfPoly; i++)
        // {
            // int nextIndex = (i + 1) % 4;
            // cv::circle(frame, corners[i], 5, yellow, -1);
            // cv::line(frame, corners[i], corners[nextIndex], redPen, 3, 8, 0);
// 
        // }

        printf ("Found: %04x\n", code);

        for (int j = 0; j < 4; j++)
        {
            corners[j].x -= frame.cols * 0.5; //here you have to use your own camera resolution (x) * 0.5
            corners[j].y = -corners[i].y + frame.rows * 0.5; //here you have to use your own camera resolution (y) * 0.5
        }

        Marker marker;
        marker.code = code;
        //
        // estimateSquarePose( resultMatrix, corners_old_C_API, 0.025f );
        estimateSquarePose( marker.resultMatrix, (cv::Point2f *)corners, kMarkerSize );

        markers.push_back(marker);


        // Added in Exercise 9 - End *****************************************************************

        //this part is only for printing
        // for (int i = 0; i < 4; ++i)
        // {
        //     for (int j = 0; j < 4; ++j)
        //     {
        //         std::cout << std::setw(6);
        //         std::cout << std::setprecision(4);
        //         std::cout << marker.resultMatrix[4 * i + j] << " ";
        //     }
        //     std::cout << "\n";
        // }
        // std::cout << "\n";
        // float x, y, z;
        // x = marker.resultMatrix[3];
        // y = marker.resultMatrix[7];
        // z = marker.resultMatrix[11];
        // std::cout << "length: " << sqrt(x * x + y * y + z * z) << "\n";
        // std::cout << "\n";
    } // end of loop over contours

    cv::imshow(kWinName1, frame);
    // cv::imshow(kWinName2, img_mono);

    if (cv::waitKey(10) == 27)
        exit(0);
}