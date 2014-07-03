#include <opencv2/opencv.hpp>
#include <iomanip>

#include "glfunctions.h"

using namespace Eigen;
using namespace std;

#include "PoseEstimation.h"


bool grabVideo( cv::VideoCapture &capture, int blockSize = 35, double C = 9 )
{

    const int sizeOfPoly = 4;
    // set the resolution of video
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    //    int value = 50;

    // set the windows and its name
    cv::namedWindow("Capture Video With Adaptive Threshold", 1);
    for (;;)
    {
        cv::Mat frame;
        std::vector <std::vector <cv::Point> > contours;
        std::vector <cv::Vec4i> hierarchy;
        capture >> frame ; // get a new frame from camera
        // clone the input frame inside threshold and adaptive frame
        cv::Mat adaptiveThresholdFrame = frame.clone();

        cv::cvtColor(adaptiveThresholdFrame, adaptiveThresholdFrame, CV_BGR2GRAY);

        cv::imshow("Capture Video With Adaptive Threshold", frame);
        // apply binary adaptive threshold on adaptiveThresholdFrame with MEAN method
        cv::adaptiveThreshold(adaptiveThresholdFrame, adaptiveThresholdFrame, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, C);
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
                //                CvPoint p;
                //                                    p.x=(int)lineParams[4*i+2] - (int)(50.0*lineParams[4*i+0]);
                //                                    p.y=(int)lineParams[4*i+3] - (int)(50.0*lineParams[4*i+1]);

                //                                    CvPoint p2;
                //                                    p2.x = (int)lineParams[4*i+2] + (int)(50.0*lineParams[4*i+0]);
                //                                    p2.y = (int)lineParams[4*i+3] + (int)(50.0*lineParams[4*i+1]);

                cv::Point2f p1;
                p1.x = line[j][2] - (50.0f * line[j][0]);
                p1.y = line[j][3] - (50.0f * line[j][1]);

                cv::Point2f p2;
                p2.x = line[j][2] + (50.0f * line[j][0]);
                p2.y = line[j][3] + (50.0f * line[j][1]);

                cv::line(frame, p1, p2, cyan, 1, 8, 0);

            }

            std::vector <cv::Point2f> corners(4);
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
                cv::circle(frame, corners[j], 5, yellow, -1);
            }

            std::vector <cv::Point2f> targetCorners(4);
            targetCorners[0].x = -0.5; targetCorners[0].y = -0.5;
            targetCorners[1].x =  5.5; targetCorners[1].y = -0.5;
            targetCorners[2].x =  5.5; targetCorners[2].y =  5.5;
            targetCorners[3].x = -0.5; targetCorners[3].y =  5.5;

            cv::Mat projectMatrix = cv::getPerspectiveTransform(corners, targetCorners);
            cv::Size markerSize (6, 6);
            cv::Mat markerImage (markerSize, CV_8U);

            cv::warpPerspective(frame, markerImage, projectMatrix, markerSize);

            int value = 50;
            // set the trackbar by defult value =50
            cv::createTrackbar( "Threshold", "Capture Video With Threshold", &value, 255,  NULL);
            // apply binary threshold on thresholdFrame
            cv::threshold(markerImage, markerImage, value, 255, cv::THRESH_BINARY);

            int code;
            int angle;
            int markerPoints[4][4];
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 4; k++)
                {
                    if (markerImage.at<int> (j + 1, k + 1) == 0)
                        markerPoints[j][k] = 1;
                    else
                        markerPoints[j][k] = 0;
                }

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

            printf ("Found: %04x\n", code);

            for (int j = 0; j < 4; j++)
            {
                corners[j].x -= 320; //here you have to use your own camera resolution (x) * 0.5
                corners[j].y = -corners[i].y + 240; //here you have to use your own camera resolution (y) * 0.5
            }

            float resultMatrix[16];

            //

            CvPoint2D32f corners_old_C_API[4];
            for (int j = 0; j < 4; j++)

                corners_old_C_API[j] = corners[j];
            //


            estimateSquarePose( resultMatrix, corners_old_C_API, 0.025f );

            cv::Mat R = (cv::Mat_<float>(3, 3) << resultMatrix[0], resultMatrix[1], resultMatrix[2],
                         resultMatrix[4], resultMatrix[5], resultMatrix[6],
                         resultMatrix[8], resultMatrix[9], resultMatrix[10]);

            cv::Mat T = (cv::Mat_ <float> (3, 1) << resultMatrix[3], resultMatrix[7], resultMatrix[11]);

            std::cout << "Rotation: " << R << std::endl;
            std::cout << "Translation: " << T << std::endl;

            //this part is only for printing
            // for (int j = 0; j < 4; ++j)
            // {
            //     for (int k = 0; k < 4; ++k)
            //     {
            //         std::cout << std::setw(6);
            //         std::cout << std::setprecision(4);
            //         std::cout << resultMatrix[4 * j + k] << " ";
            //     }
            //     std::cout << "\n";
            // }
            // std::cout << "\n";

            float x, y, z;
            x = resultMatrix[3];
            y = resultMatrix[7];
            z = resultMatrix[11];
            std::cout << "length: " << sqrt(x * x + y * y + z * z) << "\n";
            std::cout << "\n";

        }

        cv::imshow("Capture Video With Adaptive Threshold", frame);
        // wiat until ESC key pressed
        if (cv::waitKey(30) == 27)
            break;
    }
    // destroy all windows and release cv::VideoCapture
    cv::destroyAllWindows();
    capture.release();
    return true;
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "\nInsufficient Argumnets...\n");
        fprintf(stderr, "\tUsage: %s [-Parameters] [Option Parameters for Adaptive Threshold]", argv[0]);
        fprintf(stderr, "\n\tAvailable Parameters:");
        fprintf(stderr, "\n\t-W: Capture Video from default webcam.");
        fprintf(stderr, "\n\t-F (filename): Capture from File.");
        fprintf(stderr, "\n\t(int)blockSize (doubel)C: Set the parameters for Adaptive Threshold");
        fprintf(stderr, "\n\nExample:");
        fprintf(stderr, "\n\t%s -W: Run for grabbing video from webcame with default values for adaptiveThreshold, blockSize=3, C=5", argv[0]);
        fprintf(stderr, "\n\t%s -W 35 9: Run for grabbing video from webcame where blockSize=35, C=9", argv[0]);
        fprintf(stderr, "\n\t%s -F MarkerMovie.mpg: Run for show video file with default values for adaptiveThreshold, blockSize=3, C=5", argv[0]);
        fprintf(stderr, "\n\t%s -F MarkerMovie.mpg 35 9: Run for show video file where blockSize=35, C=9\n\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "-W") == 0)
    {
        cv::VideoCapture capture(0); // open the default camera
        if (!capture.isOpened()) // check if we succeeded
        {
            printf("No camera was found.\n");
            return -1;
        }

        printf("Capturing Video from Defult camera started...\n");
        bool result = false;
        if (argc > 2)
            result = grabVideo(capture, atoi(argv[2]), atoi(argv[3]));
        else
            result = grabVideo(capture);
        if (result == true)
        {
            printf("Capturing Video ended successfully...\n");
        }
    }
    else if (strcmp(argv[1], "-F") == 0)
    {
        cv::VideoCapture capture(argv[2]); // open the AVI File
        if (!capture.isOpened()) // check if we succeeded
        {
            printf("No input video file found.\n");
            return -1;
        }

        printf("Capturing Video from file %s started ...\n", argv[2]);
        bool result = false;
        if (argc > 3)
            result = grabVideo(capture, atoi(argv[3]), atoi(argv[3]));
        else
            result = grabVideo(capture);
        if (result == true)
        {
            printf("Capturing Video ended successfully...\n");
        }
    }

    srand ( time(NULL) );

    // Constructing the linked structure by
    // adding links
    for (int i = 1; i <= 2; i++)
    {
      Color c = {1.0f, 1.0f, 0.0f, 1.0f};
        Link *leftHand = new Link(c);
        leftHand->mAngle = -3.14f/8;
        leftHand->mLength = 10;

        ::leftHand.addLink(leftHand);
    }

    for (int i = 1; i <= 2; i++)
    {
      Color c = {1.0f, 0.0f, 1.0f, 1.0f};
        Link *rightHand = new Link(c);
        if(i == 1)
            rightHand->mAngle = 3.14f-3.14f/4;
        else
            rightHand->mAngle = -3.14f/4;
        rightHand->mLength = 10;

        ::rightHand.addLink(rightHand);
    }

    leftFoot.setBasePosition(Vector2f(0, -20));
    for (int i = 1; i <= 2; i++)
    {
      Color c = {0.0f, 0.0f, 1.0f, 1.0f};
        Link *leftFoot = new Link(c);
        leftFoot->mAngle = 3.14f/4;
        leftFoot->mLength = 15;

        ::leftFoot.addLink(leftFoot);
    }

    rightFoot.setBasePosition(Vector2f(0, -20));
    for (int i = 1; i <= 2; i++)
    {
      Color c = {0.0f, 1.0f, 0.0f, 1.0f};
        Link *rightFoot = new Link(c);
        if(i == 1)
            rightFoot->mAngle = 3.14f-3.14f/4;
        else
            rightFoot->mAngle = -3.14f/4;
        rightFoot->mLength = 15;

        ::rightFoot.addLink(rightFoot);
    }

    //targetPoint = leftHand.getPointWithinRange();
    //leftHand.moveToPoint(targetPoint);

    // GLUT initialization.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Biped - AR Project");

    // Register call backs.
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeMainWindow);
    glutKeyboardFunc(graphicKeys);
    glutMotionFunc(mouseMovement);
    glutIdleFunc(idle);
    glutTimerFunc(30, timer, 0);

    // OpenGL initialization
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat global_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightModelfv(GL_AMBIENT_AND_DIFFUSE, global_ambient);

    // Enter GLUT loop.
    glutMainLoop();

    return 0;
}
