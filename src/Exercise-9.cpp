
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
//#include <GL/glew.h>

#include "DrawPrimitives.h"
#include <iostream>
#include <iomanip>
#include <map>

#ifdef __APPLE__
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
#endif

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "PoseEstimation.h"
#include "MarkerTracker.h"

#define GL_BGR_EXT 0x80E0

using namespace std;

cv::VideoCapture cap;

// Added in Exercise 9 - Start *****************************************************************

struct Position
{
    double x, y, z;
};

bool debugmode = false;
bool balldebug = false;

float resultMatrix_005A[16];
float resultMatrix_0272[16];
float resultTransposedMatrix[16];
float snowmanLookVector[4];
int towards = 0x005A;
int towardsList[2] = {0x005a, 0x0272};
int towardscounter = 0;
Position ballpos;
int ballspeed = 100;
// Added in Exercise 9 - End *****************************************************************

//camera settings
const int camera_width  = 640;
const int camera_height = 480;
const int virtual_camera_angle = 30;
unsigned char bkgnd[camera_width *camera_height * 3];


void initVideoStream( cv::VideoCapture &cap )
{
    if ( cap.isOpened() )
        cap.release();

    cap.open(0); // open the default camera
    if ( cap.isOpened() == false )
    {
        std::cout << "No webcam found, using a video file" << std::endl;
        cap.open("MarkerMovie.mpg");
        if ( cap.isOpened() == false )
        {
            std::cout << "No video file found. Exiting."      << std::endl;
            exit(0);
        }
    }

}


// Added in Exercise 9 - Start *****************************************************************
void multMatrix(float mat[16], float vec[4])
{
    for (int i = 0; i < 4; i++)
    {
        snowmanLookVector[i] = 0;
        for (int j = 0; j < 4; j++)
            snowmanLookVector[i] += mat[4 * i + j] * vec[j];
    }
}

void moveBall(float mat[16])
{
    float vector[3];
    vector[0] = mat[3] - ballpos.x;
    vector[1] = mat[7] - ballpos.y;
    vector[2] = mat[11] - ballpos.z;

    float length = sqrt( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2] );
    if (balldebug) std::cout << length << std::endl;
    if ( length < 0.01)
    {
        towards = towardsList[(towardscounter++) % 2];
        if (balldebug) std::cout << "target changed to marker " << towards << std::endl;
        ballspeed = 60 + 80 * rand() / RAND_MAX;
        return;
    }
    ballpos.x += vector[0] / (ballspeed * length);
    ballpos.y += vector[1] / (ballspeed * length);
    ballpos.z += vector[2] / (ballspeed * length);

}

void rotateToMarker(float thisMarker[16], float lookAtMarker[16], int markernumber)
{
    float vector[3];
    vector[0] = lookAtMarker[3] - thisMarker[3];
    vector[1] = lookAtMarker[7] - thisMarker[7];
    vector[2] = lookAtMarker[11] - thisMarker[11];

    if (towards == markernumber) moveBall(lookAtMarker);

    //normalize vector
    float help = sqrt( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2] );
    vector[0] /= help;
    vector[1] /= help;
    vector[2] /= help;

    if (debugmode) std::cout << "Vector: " << vector[0] << ", " << vector[1] << ", " << vector[2] << std::endl;

    float defaultLook[4] = {1, 0, 0, 0};
    multMatrix(thisMarker, defaultLook);

    //normalize snowmanLookVector
    help = sqrt( snowmanLookVector[0] * snowmanLookVector[0] + snowmanLookVector[1] * snowmanLookVector[1] + snowmanLookVector[2] * snowmanLookVector[2] );
    snowmanLookVector[0] /= help;
    snowmanLookVector[1] /= help;
    snowmanLookVector[2] /= help;

    if (debugmode) std::cout << "SnowmanLookVector: " << snowmanLookVector[0] << ", " << snowmanLookVector[1] << ", " << snowmanLookVector[2] << std::endl;

    float angle = (180 / M_PI) * acos( vector[0] * snowmanLookVector[0] + vector[1] * snowmanLookVector[1] + vector[2] * snowmanLookVector[2] );
    if ((vector[0] * snowmanLookVector[1] - vector[1] * snowmanLookVector[0]) < 0 ) angle *= -1;

    if (debugmode) std::cout << "Angle: " << angle << std::endl;

    glRotatef(angle, 0, 0, 1);
}
// Added in Exercise 9 - End *****************************************************************


/* program & OpenGL initialization */
void initGL(int argc, char *argv[])
{
    // initialize the GL library

    // Added in Exercise 8 - End *****************************************************************
    // pixel storage/packing stuff
    glPixelStorei( GL_PACK_ALIGNMENT,   1 ); // for glReadPixels​
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // for glTexImage2D​
    glPixelZoom( 1.0, -1.0 );
    // Added in Exercise 8 - End *****************************************************************

    // enable and set colors
    glEnable( GL_COLOR_MATERIAL );
    glClearColor( 0, 0, 0, 1.0 );

    // enable and set depth parameters
    glEnable( GL_DEPTH_TEST );
    glClearDepth( 1.0 );

    // light parameters
    GLfloat light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_amb[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_dif[] = { 0.7, 0.7, 0.7, 1.0 };

    // enable lighting
    glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
    glLightfv( GL_LIGHT0, GL_AMBIENT,  light_amb );
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_dif );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );


}


void display( GLFWwindow *window, const cv::Mat &img_bgr, std::vector<Marker> &markers )
{
    memcpy( bkgnd, img_bgr.data, sizeof(bkgnd) );

    int width0, height0;
    glfwGetFramebufferSize(window, &width0, &height0);
    //  reshape(window, width, height);

    // clear buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw background image
    glDisable( GL_DEPTH_TEST );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0.0, camera_width, 0.0, camera_height );

    glRasterPos2i( 0, camera_height - 1 );
    glDrawPixels( camera_width, camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd );

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);


    // move to marker-position
    glMatrixMode( GL_MODELVIEW );

    for ( int i = 0; i < markers.size(); i++)
    {

    }


    // Added in Exercise 9 - Start *****************************************************************
    float resultMatrix_005A[16];
    float resultMatrix_0272[16];
    for (int i = 0; i < markers.size(); i++)
    {
        const int code = markers[i].code;
        if (code == 0x005a)
        {
            for (int j = 0; j < 16; j++);
                //resultMatrix_005A[j] = markers[i].resultMatrix[j];
        }
        else if (code == 0x0272)
        {
            for (int j = 0; j < 16; j++);
                //resultMatrix_0272[j] = markers[i].resultMatrix[j];
        }
    }


    for (int x = 0; x < 4; ++x)
        for (int y = 0; y < 4; ++y)
            resultTransposedMatrix[x * 4 + y] = resultMatrix_005A[y * 4 + x];
    // Added in Exercise 9 - End *****************************************************************

    //glLoadTransposeMatrixf( resultMatrix );
    glLoadMatrixf( resultTransposedMatrix );
    drawSnowman(false);


    // Added in Exercise 9 - Start *****************************************************************
    rotateToMarker(resultMatrix_005A, resultMatrix_0272, 0x005a);

    drawSnowman( 0 );

    for (int x = 0; x < 4; ++x)
        for (int y = 0; y < 4; ++y)
            resultTransposedMatrix[x * 4 + y] = resultMatrix_0272[y * 4 + x];

    glLoadMatrixf( resultTransposedMatrix );

    rotateToMarker(resultMatrix_0272, resultMatrix_005A, 0x0272);

    drawSnowman( 1 );

    //drawBall
    glLoadIdentity();
    glTranslatef(ballpos.x, ballpos.y + 0.024, ballpos.z);
    glColor4f(1, 0, 0, 1);
    drawSphere(0.005, 10, 10);
    // Added in Exercise 9 - End *****************************************************************


    int key = cvWaitKey (10);
    if (key == 27) exit(0);
    // Added in Exercise 9 - Start *****************************************************************
    else if (key == 100) debugmode = !debugmode;
    else if (key == 98) balldebug = !balldebug;
    // Added in Exercise 9 - End *****************************************************************

}


void reshape( GLFWwindow *window, int width, int height )
{

    // set a whole-window viewport
    glViewport( 0, 0, (GLsizei)width, (GLsizei)height );

    // create a perspective projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Note: Just setting the Perspective is an easy hack. In fact, the camera should be calibrated.
    // With such a calibration we would get the projection matrix. This matrix could then be loaded
    // to GL_PROJECTION.
    // If you are using another camera (which you'll do in most cases), you'll have to adjust the FOV
    // value. How? Fiddle around: Move Marker to edge of display and check if you have to increase or
    // decrease.
    gluPerspective( virtual_camera_angle, ((GLfloat)width / (GLfloat)height), 0.01, 100 );

    // invalidate display
    //glutPostRedisplay();
}


int main(int argc, char *argv[])
{

    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    // initialize the window system
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(camera_width, camera_height, "Exercise 8 - Combine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Set callback functions for GLFW
    glfwSetFramebufferSizeCallback(window, reshape);

    glfwMakeContextCurrent(window);
    glfwSwapInterval( 1 );

    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    reshape(window, window_width, window_height);

    glViewport(0, 0, window_width, window_height);

    // initialize the GL library
    initGL(argc, argv);

    // setup OpenCV
    cv::Mat img_bgr;
    initVideoStream(cap);
    const double kMarkerSize = 0.032; // [m]
    MarkerTracker markerTracker(kMarkerSize);

    std::vector<Marker> markers;
    //  float resultMatrix[16];
    /* Loop until the user closes the window */

//    map <int, float* > markerMovement;
    while (!glfwWindowShouldClose(window))
    {
        markers.resize(0);
        /* Capture here */
        cap >> img_bgr;

        if (img_bgr.empty())
        {
            std::cout << "Could not query frame. Trying to reinitialize." << std::endl;
            initVideoStream(cap);
            cv::waitKey(1000); /// Wait for one sec.
            continue;
        }

        /* Track a marker */
        markerTracker.findMarker( img_bgr, markers);///resultMatrix);

        //for(int i=0; i<markers.size(); i++)
        //{
        //	int code = markers[i].code;
        //	float translation [3] = {markers[i].resultMatrix[3], markers[i].resultMatrix[7], markers[i].resultMatrix[11]};
			// float * t = & translation;
        //	markerMovement[code] = translation;
        //}

        /* Render here */
        display(window,           img_bgr, markers);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();


    return 0;
}


#if 0

#include <iostream>
#include <iomanip>

#include <GL/glut.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "PoseEstimation.h"


using namespace std;

int thresh = 100;
cv::VideoCapture cap;
cv::Mat img_bgr;
cv::Mat img_gray;
cv::Mat img_mono;


// Added in Exercise 9 - Start *****************************************************************

struct Position
{
    double x, y, z;
};

bool debugmode = false;
bool balldebug = false;
// Added in Exercise 9 - End *****************************************************************



int bw_thresh = 40;

const std::string kWinName1 = "Exercise 9 - Original Image";
const std::string kWinName2 = "Exercise 9 - Converted Image";
const std::string kWinName3 = "Exercise 9 - Stripe Image";
const std::string kWinName4 = "Exercise 9 - Marker";

CvMemStorage *memStorage;

// Added in Exercise 9 - Start *****************************************************************
float resultMatrix_005A[16];
float resultMatrix_0272[16];
float resultTransposedMatrix[16];
float snowmanLookVector[4];
int towards = 0x005A;
int towardsList[2] = {0x005a, 0x0272};
int towardscounter = 0;
Position ballpos;
int ballspeed = 100;
// Added in Exercise 9 - End *****************************************************************

//camera settings
const int width = 640;
const int height = 480;
const int camangle = 35;

unsigned char bkgnd[width *height * 3];


void trackbarHandler( int pos, void *slider_value )
{
    *( (int *)slider_value ) = pos;
}

void bw_trackbarHandler(int pos, void *slider_value )
{
    *( (int *)slider_value ) = pos;
}

void initVideoStream( cv::VideoCapture &cap )
{
    if ( cap.isOpened() )
        cap.release();

    cap.open(0); // open the default camera
    if ( cap.isOpened() == false )
    {
        std::cout << "No webcam found, using a video file" << std::endl;
        cap.open("MarkerMovie.mpg");
        if ( cap.isOpened() == false )
        {
            std::cout << "No video file found. Exiting."      << std::endl;
            exit(0);
        }
    }
}

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

// Added in Exercise 9 - Start *****************************************************************
void multMatrix(float mat[16], float vec[4])
{
    for (int i = 0; i < 4; i++)
    {
        snowmanLookVector[i] = 0;
        for (int j = 0; j < 4; j++)
            snowmanLookVector[i] += mat[4 * i + j] * vec[j];
    }
}

void moveBall(float mat[16])
{
    float vector[3];
    vector[0] = mat[3] - ballpos.x;
    vector[1] = mat[7] - ballpos.y;
    vector[2] = mat[11] - ballpos.z;

    float length = sqrt( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2] );
    if (balldebug) std::cout << length << std::endl;
    if ( length < 0.01)
    {
        towards = towardsList[(towardscounter++) % 2];
        if (balldebug) std::cout << "target changed to marker " << towards << std::endl;
        ballspeed = 60 + 80 * rand() / RAND_MAX;
        return;
    }
    ballpos.x += vector[0] / (ballspeed * length);
    ballpos.y += vector[1] / (ballspeed * length);
    ballpos.z += vector[2] / (ballspeed * length);

}

void rotateToMarker(float thisMarker[16], float lookAtMarker[16], int markernumber)
{
    float vector[3];
    vector[0] = lookAtMarker[3] - thisMarker[3];
    vector[1] = lookAtMarker[7] - thisMarker[7];
    vector[2] = lookAtMarker[11] - thisMarker[11];

    if (towards == markernumber) moveBall(lookAtMarker);

    //normalize vector
    float help = sqrt( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2] );
    vector[0] /= help;
    vector[1] /= help;
    vector[2] /= help;

    if (debugmode) std::cout << "Vector: " << vector[0] << ", " << vector[1] << ", " << vector[2] << std::endl;

    float defaultLook[4] = {1, 0, 0, 0};
    multMatrix(thisMarker, defaultLook);

    //normalize snowmanLookVector
    help = sqrt( snowmanLookVector[0] * snowmanLookVector[0] + snowmanLookVector[1] * snowmanLookVector[1] + snowmanLookVector[2] * snowmanLookVector[2] );
    snowmanLookVector[0] /= help;
    snowmanLookVector[1] /= help;
    snowmanLookVector[2] /= help;

    if (debugmode) std::cout << "SnowmanLookVector: " << snowmanLookVector[0] << ", " << snowmanLookVector[1] << ", " << snowmanLookVector[2] << std::endl;

    float angle = (180 / M_PI) * acos( vector[0] * snowmanLookVector[0] + vector[1] * snowmanLookVector[1] + vector[2] * snowmanLookVector[2] );
    if ((vector[0] * snowmanLookVector[1] - vector[1] * snowmanLookVector[0]) < 0 ) angle *= -1;

    if (debugmode) std::cout << "Angle: " << angle << std::endl;

    glRotatef(angle, 0, 0, 1);
}
// Added in Exercise 9 - End *****************************************************************

void init()
{
    cout << "Startup\n";
    cv::namedWindow("Exercise 8 - Original Image", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Exercise 8 - Converted Image", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Exercise 8 - Stripe", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Marker", 0 );
    cvResizeWindow("Marker", 120, 120 );
    initVideoStream(cap);

    int max = 255;
    int slider_value = 100;
    cv::createTrackbar( "Threshold", "Exercise 5 - Converted Image", &slider_value, 255, trackbarHandler, &slider_value);

    int bw_sileder_value = bw_thresh;
    cv::createTrackbar( "BW Threshold", "Exercise 5 - Converted Image", &slider_value, 255, bw_trackbarHandler, &bw_sileder_value);

    memStorage = cvCreateMemStorage();
}

void idle()
{
    bool isFirstStripe = true;

    bool isFirstMarker = true;


    {
        cap >> img_bgr;

        if (img_bgr.empty())
        {
            std::cout << "Could not query frame. Trying to reinitialize." << std::endl;
            initVideoStream(cap);
            cv::waitKey(1000); /// Wait for one sec.
            return;
        }

        // Added in Exercise 8 - Start *****************************************************************
        memcpy( bkgnd, img_bgr.data, sizeof(bkgnd) );
        // Added in Exercise 8 - End *****************************************************************

        cv::cvtColor( img_bgr, img_gray, CV_BGR2GRAY );
        cv::threshold( img_gray, img_mono, thresh, 255, CV_THRESH_BINARY);

        // Find Contours with old OpenCV APIs
        CvSeq *contours;
        CvMat img_mono_(img_mono);

        cvFindContours(
            &img_mono_, memStorage, &contours, sizeof(CvContour),
            CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE
        );

        for (; contours; contours = contours->h_next)
        {
            CvSeq *result = cvApproxPoly(
                                contours, sizeof(CvContour), memStorage, CV_POLY_APPROX_DP,
                                cvContourPerimeter(contours) * 0.02, 0
                            );

            if (result->total != 4)
            {
                continue;
            }

            cv::Mat result_ = cv::cvarrToMat(result); /// API 1.X to 2.x
            cv::Rect r = cv::boundingRect(result_);
            if (r.height < 20 || r.width < 20 || r.width > img_mono.cols - 10 || r.height > img_mono.rows - 10)
            {
                continue;
            }

            const cv::Point *rect = (const cv::Point *) result_.data;
            int npts = result_.rows;
            // draw the polygon
            cv::polylines( img_bgr, &rect, &npts, 1,
                           true,           // draw closed contour (i.e. joint end to start)
                           CV_RGB(255, 0, 0), // colour RGB ordering (here = green)
                           2,              // line thickness
                           CV_AA, 0);


            float lineParams[16];
            cv::Mat lineParamsMat( cv::Size(4, 4), CV_32F, lineParams); // lineParams is shared

            for (int i = 0; i < 4; ++i)
            {
                cv::circle (img_bgr, rect[i], 3, CV_RGB(0, 255, 0), -1);

                double dx = (double)(rect[(i + 1) % 4].x - rect[i].x) / 7.0;
                double dy = (double)(rect[(i + 1) % 4].y - rect[i].y) / 7.0;

                int stripeLength = (int)(0.8 * sqrt (dx * dx + dy * dy));
                if (stripeLength < 5)
                    stripeLength = 5;

                //make stripeLength odd (because of the shift in nStop)
                stripeLength |= 1;

                //e.g. stripeLength = 5 --> from -2 to 2
                int nStop  = stripeLength >> 1;
                int nStart = -nStop;

                cv::Size stripeSize;
                stripeSize.width = 3;
                stripeSize.height = stripeLength;

                cv::Point2f stripeVecX;
                cv::Point2f stripeVecY;

                //normalize vectors
                double diffLength = sqrt ( dx * dx + dy * dy );
                stripeVecX.x = dx / diffLength;
                stripeVecX.y = dy / diffLength;

                stripeVecY.x =  stripeVecX.y;
                stripeVecY.y = -stripeVecX.x;

                cv::Mat iplStripe( stripeSize, CV_8UC1 );
                ///             IplImage* iplStripe = cvCreateImage( stripeSize, IPL_DEPTH_8U, 1 );

                // Array for edge point centers
                cv::Point2f points[6];

                for (int j = 1; j < 7; ++j)
                {
                    double px = (double)rect[i].x + (double)j * dx;
                    double py = (double)rect[i].y + (double)j * dy;

                    cv::Point p;
                    p.x = (int)px;
                    p.y = (int)py;
                    cv::circle ( img_bgr, p, 2, CV_RGB(0, 0, 255), -1);

                    for ( int m = -1; m <= 1; ++m )
                    {
                        for ( int n = nStart; n <= nStop; ++n )
                        {
                            cv::Point2f subPixel;

                            subPixel.x = (double)p.x + ((double)m * stripeVecX.x) + ((double)n * stripeVecY.x);
                            subPixel.y = (double)p.y + ((double)m * stripeVecX.y) + ((double)n * stripeVecY.y);

                            cv::Point p2;
                            p2.x = (int)subPixel.x;
                            p2.y = (int)subPixel.y;

                            if (isFirstStripe)
                                cv::circle ( img_bgr, p2, 1, CV_RGB(255, 0, 255), -1);
                            else
                                cv::circle ( img_bgr, p2, 1, CV_RGB(0, 255, 255), -1);

                            int pixel = subpixSampleSafe (img_gray, subPixel);

                            int w = m + 1; //add 1 to shift to 0..2
                            int h = n + ( stripeLength >> 1 ); //add stripelenght>>1 to shift to 0..stripeLength


                            iplStripe.at<uchar>(h, w) = (uchar)pixel;
                            ///                         *(iplStripe->imageData + h * iplStripe->widthStep  + w) =  pixel; //set pointer to correct position and safe subpixel intensity
                        }
                    }

                    //use sobel operator on stripe
                    // ( -1 , -2, -1 )
                    // (  0 ,  0,  0 )
                    // (  1 ,  2,  1 )
                    std::vector<double> sobelValues(stripeLength - 2);
                    ///                 double* sobelValues = new double[stripeLength-2];
                    for (int n = 1; n < (stripeLength - 1); n++)
                    {
                        unsigned char *stripePtr = &( iplStripe.at<uchar>(n - 1, 0) );
                        ///                     unsigned char* stripePtr = ( unsigned char* )( iplStripe->imageData + (n-1) * iplStripe->widthStep );
                        double r1 = -stripePtr[ 0 ] - 2 * stripePtr[ 1 ] - stripePtr[ 2 ];

                        stripePtr += 2 * iplStripe.step;
                        ///                     stripePtr += 2*iplStripe->widthStep;
                        double r3 =  stripePtr[ 0 ] + 2 * stripePtr[ 1 ] + stripePtr[ 2 ];
                        sobelValues[n - 1] = r1 + r3;
                    }

                    double maxVal = -1;
                    int maxIndex = 0;
                    for (int n = 0; n < stripeLength - 2; ++n)
                    {
                        if ( sobelValues[n] > maxVal )
                        {
                            maxVal = sobelValues[n];
                            maxIndex = n;
                        }
                    }

                    double y0, y1, y2; // y0 .. y1 .. y2
                    y0 = (maxIndex <= 0) ? 0 : sobelValues[maxIndex - 1];
                    y1 = sobelValues[maxIndex];
                    y2 = (maxIndex >= stripeLength - 3) ? 0 : sobelValues[maxIndex + 1];

                    //formula for calculating the x-coordinate of the vertex of a parabola, given 3 points with equal distances
                    //(xv means the x value of the vertex, d the distance between the points):
                    //xv = x1 + (d / 2) * (y2 - y0)/(2*y1 - y0 - y2)

                    double pos = (y2 - y0) / (4 * y1 - 2 * y0 - 2 * y2 ); //d = 1 because of the normalization and x1 will be added later

                    // This would be a valid check, too
                    //if (std::isinf(pos)) {
                    //  // value is infinity
                    //  continue;
                    //}

                    if (pos != pos)
                    {
                        // value is not a number
                        continue;
                    }

                    cv::Point2f edgeCenter; //exact point with subpixel accuracy
                    int maxIndexShift = maxIndex - (stripeLength >> 1);

                    //shift the original edgepoint accordingly
                    edgeCenter.x = (double)p.x + (((double)maxIndexShift + pos) * stripeVecY.x);
                    edgeCenter.y = (double)p.y + (((double)maxIndexShift + pos) * stripeVecY.y);

                    cv::Point p_tmp;
                    p_tmp.x = (int)edgeCenter.x;
                    p_tmp.y = (int)edgeCenter.y;
                    cv::circle ( img_bgr, p_tmp, 1, CV_RGB(0, 0, 255), -1);

                    points[j - 1].x = edgeCenter.x;
                    points[j - 1].y = edgeCenter.y;

                    if (isFirstStripe)
                    {
                        cv::Mat iplTmp;
                        cv::resize( iplStripe, iplTmp, cv::Size(100, 300) );
                        cv::imshow ( kWinName3, iplTmp );//iplStripe );
                        isFirstStripe = false;
                    }

                } // end of loop over edge points of one edge

                // we now have the array of exact edge centers stored in "points"
                cv::Mat mat( cv::Size(1, 6), CV_32FC2, points);
                cv::fitLine ( mat, lineParamsMat.row(i), CV_DIST_L2, 0, 0.01, 0.01 );
                // cvFitLine stores the calculated line in lineParams in the following way:
                // vec.x, vec.y, point.x, point.y

                cv::Point p;
                p.x = (int)lineParams[4 * i + 2] - (int)(50.0 * lineParams[4 * i + 0]);
                p.y = (int)lineParams[4 * i + 3] - (int)(50.0 * lineParams[4 * i + 1]);

                cv::Point p2;
                p2.x = (int)lineParams[4 * i + 2] + (int)(50.0 * lineParams[4 * i + 0]);
                p2.y = (int)lineParams[4 * i + 3] + (int)(50.0 * lineParams[4 * i + 1]);

                cv::line ( img_bgr, p, p2, CV_RGB(0, 255, 255), 1, 8, 0);

            } // end of loop over the 4 edges

            // so far we stored the exact line parameters and show the lines in the image
            // now we have to calculate the exact corners
            cv::Point2f corners[4];

            for (int i = 0; i < 4; ++i)
            {
                int j = (i + 1) % 4;
                double x0, x1, y0, y1, u0, u1, v0, v1;
                x0 = lineParams[4 * i + 2]; y0 = lineParams[4 * i + 3];
                x1 = lineParams[4 * j + 2]; y1 = lineParams[4 * j + 3];

                u0 = lineParams[4 * i + 0]; v0 = lineParams[4 * i + 1];
                u1 = lineParams[4 * j + 0]; v1 = lineParams[4 * j + 1];

                // (x|y) = p + s * vec
                // s = Ds / D (see cramer's rule)
                // (x|y) = p + (Ds / D) * vec
                // (x|y) = (p * D / D) + (Ds * vec / D)
                // (x|y) = (p * D + Ds * vec) / D
                // (x|y) = a / c;
                double a =  x1 * u0 * v1 - y1 * u0 * u1 - x0 * u1 * v0 + y0 * u0 * u1;
                double b = -x0 * v0 * v1 + y0 * u0 * v1 + x1 * v0 * v1 - y1 * v0 * u1;
                double c =  v1 * u0 - v0 * u1;

                if ( fabs(c) < 0.001 ) //lines parallel?
                {
                    std::cout << "lines parallel" << std::endl;
                    continue;
                }

                a /= c;
                b /= c;

                //exact corner
                corners[i].x = a;
                corners[i].y = b;
                cv::Point p;
                p.x = (int)corners[i].x;
                p.y = (int)corners[i].y;

                cv::circle ( img_bgr, p, 5, CV_RGB(255, 255, 0), -1);
            } //finished the calculation of the exact corners

            cv::Point2f targetCorners[4];
            targetCorners[0].x = -0.5; targetCorners[0].y = -0.5;
            targetCorners[1].x =  5.5; targetCorners[1].y = -0.5;
            targetCorners[2].x =  5.5; targetCorners[2].y =  5.5;
            targetCorners[3].x = -0.5; targetCorners[3].y =  5.5;

            //create and calculate the matrix of perspective transform
            cv::Mat projMat( cv::Size(3, 3), CV_32FC1 );
            projMat = cv::getPerspectiveTransform( corners, targetCorners );
            ///         cv::warpPerspectiveQMatrix ( corners, targetCorners, projMat);

            //create image for the marker
            //          markerSize.width  = 6;
            //          markerSize.height = 6;
            cv::Mat iplMarker( cv::Size(6, 6), CV_8UC1 );

            //change the perspective in the marker image using the previously calculated matrix
            cv::warpPerspective( img_gray, iplMarker, projMat, cv::Size(6, 6) );

            cv::threshold(iplMarker, iplMarker, bw_thresh, 255, CV_THRESH_BINARY);

            //now we have a B/W image of a supposed Marker

            // check if border is black
            int code = 0;
            for (int i = 0; i < 6; ++i)
            {
                //int pixel1 = ((unsigned char*)(iplMarker->imageData + 0*iplMarker->widthStep + i))[0]; //top
                //int pixel2 = ((unsigned char*)(iplMarker->imageData + 5*iplMarker->widthStep + i))[0]; //bottom
                //int pixel3 = ((unsigned char*)(iplMarker->imageData + i*iplMarker->widthStep))[0]; //left
                //int pixel4 = ((unsigned char*)(iplMarker->imageData + i*iplMarker->widthStep + 5))[0]; //right
                int pixel1 = iplMarker.at<uchar>(0, i);
                int pixel2 = iplMarker.at<uchar>(5, i);
                int pixel3 = iplMarker.at<uchar>(i, 0);
                int pixel4 = iplMarker.at<uchar>(i, 5);
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

            //copy the BW values into cP
            int cP[4][4];
            for ( int i = 0; i < 4; ++i)
            {
                for ( int j = 0; j < 4; ++j)
                {
                    cP[i][j] = iplMarker.at<uchar>(i + 1, j + 1);
                    cP[i][j] = (cP[i][j] == 0) ? 1 : 0; //if black then 1 else 0
                }
            }

            //save the ID of the marker
            int codes[4];
            codes[0] = codes[1] = codes[2] = codes[3] = 0;
            for (int i = 0; i < 16; i++)
            {
                int row = i >> 2;
                int col = i % 4;

                codes[0] <<= 1;
                codes[0] |= cP[row][col]; // 0ｰ

                codes[1] <<= 1;
                codes[1] |= cP[3 - col][row]; // 90ｰ

                codes[2] <<= 1;
                codes[2] |= cP[3 - row][3 - col]; // 180ｰ

                codes[3] <<= 1;
                codes[3] |= cP[col][3 - row]; // 270ｰ
            }

            if ( (codes[0] == 0) || (codes[0] == 0xffff) )
            {
                continue;
            }

            //account for symmetry
            code = codes[0];
            // Added in Exercise 5 - Start *****************************************************************
            int angle = 0;
            // Added in Exercise 5 - End *******************************************************************
            for ( int i = 1; i < 4; ++i )
            {
                if ( codes[i] < code )
                {
                    code = codes[i];
                    // Added in Exercise 5 - Start *****************************************************************
                    angle = i;
                    // Added in Exercise 5 - End *******************************************************************
                }
            }

            printf ("Found: %04x\n", code);

            if ( isFirstMarker )
            {
                cv::imshow ( kWinName4, iplMarker );
                isFirstMarker = false;
            }

            // Added in Exercise 5 - Start *****************************************************************
            //correct the order of the corners
            if (angle != 0)
            {
                cv::Point2f corrected_corners[4];
                for (int i = 0; i < 4; i++)  corrected_corners[(i + angle) % 4] = corners[i];
                for (int i = 0; i < 4; i++)  corners[i] = corrected_corners[i];
            }

            // transfer screen coords to camera coords
            for (int i = 0; i < 4; i++)
            {
                corners[i].x -= 160; //here you have to use your own camera resolution (x) * 0.5
                corners[i].y = -corners[i].y + 120; //here you have to use your own camera resolution (y) * 0.5
            }

            // Added in Exercise 9 - Start *****************************************************************
            if (code == 0x005a) estimateSquarePose( resultMatrix_005A, (cv::Point2f *)corners, 0.045 );
            else if (code == 0x0272) estimateSquarePose( resultMatrix_0272, (cv::Point2f *)corners, 0.045 );
            // Added in Exercise 9 - End *****************************************************************

            float resultMatrix[16];
            estimateSquarePose( resultMatrix, (cv::Point2f *)corners, 0.045 );

            //this part is only for printing
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    cout << setw(6);
                    cout << setprecision(4);
                    cout << resultMatrix[4 * i + j] << " ";
                }
                cout << "\n";
            }
            cout << "\n";
            float x, y, z;
            x = resultMatrix[3];
            y = resultMatrix[7];
            z = resultMatrix[11];
            cout << "length: " << sqrt(x * x + y * y + z * z) << "\n";
            cout << "\n";
            // Added in Exercise 5 - End *****************************************************************



        } // end of loop over contours

        cv::imshow(kWinName1, img_bgr);
        cv::imshow(kWinName2, img_mono);



        isFirstStripe = true;

        isFirstMarker = true;


        cvClearMemStorage ( memStorage );
    } // end of main loop

    cvClearMemStorage ( memStorage );

    int key = cvWaitKey (10);
    if (key == 27) exit(0);
    // Added in Exercise 9 - Start *****************************************************************
    else if (key == 100) debugmode = !debugmode;
    else if (key == 98) balldebug = !balldebug;
    // Added in Exercise 9 - End *****************************************************************

    glutPostRedisplay();
}

void cleanup()
{
    cvReleaseMemStorage (&memStorage);

    cvDestroyWindow ("Exercise 9 - Original Image");
    cvDestroyWindow ("Exercise 9 - Converted Image");
    cvDestroyWindow ("Exercise 9 - Stripe");
    cvDestroyWindow ("Marker");
    cout << "Finished\n";
}

// Added in Exercise 9 - Start *****************************************************************
void drawSnowman( bool female )
{
    glRotatef( -90, 1, 0, 0 );
    glScalef(0.03, 0.03, 0.03);

    // draw 3 white spheres
    glColor4f( 1.0, 1.0, 1.0, 1.0 );
    glutSolidSphere( 0.8, 10, 10 );
    glTranslatef( 0.0, 0.8, 0.0 );
    glutSolidSphere( 0.6, 10, 10 );
    if (female)
    {
        glPushMatrix();
        glRotatef(90, 0, 1, 0);
        glTranslatef(-0.2, 0.05, 0.3);
        glutSolidSphere(0.32, 10, 10);
        glTranslatef(0.4, 0, 0);
        glutSolidSphere(0.32, 10, 10);
        glPopMatrix();
    }
    glTranslatef( 0.0, 0.6, 0.0 );
    glutSolidSphere( 0.4, 10, 10 );

    // draw the eyes
    glPushMatrix();
    glColor4f( 0.0, 0.0, 0.0, 1.0 );
    glTranslatef( 0.2, 0.2, 0.2 );
    glutSolidSphere( 0.066, 10, 10 );
    glTranslatef( 0, 0, -0.4 );
    glutSolidSphere( 0.066, 10, 10 );
    glPopMatrix();

    // draw a nose
    glColor4f( 1.0, 0.5, 0.0, 1.0 );
    glTranslatef( 0.3, 0.0, 0.0 );
    glRotatef( 90, 0, 1, 0 );
    glutSolidCone( 0.1, 0.3, 10, 1 );
}
// Added in Exercise 9 - End *****************************************************************

void display()
{
    // clear buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw background image
    glDisable( GL_DEPTH_TEST );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0.0, width, 0.0, height );

    glRasterPos2i( 0, height - 1 );
    glDrawPixels( width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd );

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);

    // move to origin
    glMatrixMode( GL_MODELVIEW );

    for (int x = 0; x < 4; ++x)
        for (int y = 0; y < 4; ++y)
            resultTransposedMatrix[x * 4 + y] = resultMatrix_005A[y * 4 + x];

    glLoadMatrixf( resultTransposedMatrix );
    // Added in Exercise 9 - Start *****************************************************************
    rotateToMarker(resultMatrix_005A, resultMatrix_0272, 0x005a);

    drawSnowman( 0 );

    for (int x = 0; x < 4; ++x)
        for (int y = 0; y < 4; ++y)
            resultTransposedMatrix[x * 4 + y] = resultMatrix_0272[y * 4 + x];

    glLoadMatrixf( resultTransposedMatrix );

    rotateToMarker(resultMatrix_0272, resultMatrix_005A, 0x0272);

    drawSnowman( 1 );

    //drawBall
    glLoadIdentity();
    glTranslatef(ballpos.x, ballpos.y + 0.024, ballpos.z);
    glColor4f(1, 0, 0, 1);
    glutSolidSphere(0.005, 10, 10);
    // Added in Exercise 9 - End *****************************************************************

    // redraw
    glutSwapBuffers();
}

void resize( int w, int h)
{
    //    width = w;
    //  height = h;

    // set a whole-window viewport
    glViewport( 0, 0, width, height );

    // create a perspective projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Note: Just setting the Perspective is an easy hack. In fact, the camera should be calibrated.
    // With such a calibration we would get the projection matrix. This matrix could then be loaded
    // to GL_PROJECTION.
    // If you are using another camera (which you'll do in most cases), you'll have to adjust the FOV
    // value. How? Fiddle around: Move Marker to edge of display and check if you have to increase or
    // decrease.
    gluPerspective( camangle, ((double)width / (double)height), 0.01, 100 );

    // invalidate display
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    cout << "Startup\n";

    // initialize the window system
    glutInit( &argc, argv );
    glutInitWindowSize( width, height );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutCreateWindow("AR Exercise 9 - Combine");

    // initialize the GL library

    // pixel storage/packing stuff
    glPixelStorei( GL_PACK_ALIGNMENT,   1 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glPixelZoom( 1.0, -1.0 );

    // enable and set colors
    glEnable( GL_COLOR_MATERIAL );
    glClearColor( 0, 0, 0, 1.0 );

    // enable and set depth parameters
    glEnable( GL_DEPTH_TEST );
    glClearDepth( 1.0 );

    // light parameters
    GLfloat light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_amb[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_dif[] = { 0.7, 0.7, 0.7, 1.0 };

    // enable lighting
    glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
    glLightfv( GL_LIGHT0, GL_AMBIENT,  light_amb );
    glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_dif );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    // make functions known to GLUT
    //glutDisplayFunc( display );
    glutReshapeFunc( resize  );
    glutIdleFunc( idle );

    // setup OpenCV
    init();

    // for tracker debugging...
    //while (1) idle();

    // Added in Exercise 9 - Start *****************************************************************
    std::cout << glGetString(GL_VENDOR) << std::endl;
    std::cout << glGetString(GL_RENDERER) << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    //  std::cout << glGetString(GL_EXTENSIONS) << std::endl;
    std::cout << "Press 'd' for printing debug information. Press 'b' for ball debug information." << std::endl;
    // Added in Exercise 9 - End *****************************************************************

    // start the action
    glutMainLoop();

    return 0;
}
#endif
