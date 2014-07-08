
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
//#include <GL/glew.h>

#include "DrawPrimitives.h"
#include <iostream>
#include <iomanip>


#include <opencv/cv.h>
#include <opencv/highgui.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "MarkerTracker.h"
#include "MarkerTracker_KK.h"
#include "myGL.h"

#include "combineWindowHandler.h"

#define GL_BGR_EXT 0x80E0

using namespace std;

cv::VideoCapture cap;

// Added in Exercise 9 - Start *****************************************************************

struct Position { double x,y,z; };

bool debugmode = false;
bool balldebug = false;



float snowmanLookVector[4];
int towards = 0x005A;
int towardsList[4] = {0x0b44, 0x1C44, 0x1228, 0x0272};
int towardscounter = 0;
Position ballpos;
int ballspeed = 100;
// Added in Exercise 9 - End *****************************************************************




void initVideoStream( cv::VideoCapture &cap ) {
	if( cap.isOpened() )
		cap.release();

	cap.open(0); // open the default camera
	if ( cap.isOpened()==false ) {
		std::cout << "No webcam found, using a video file" << std::endl;
		cap.open("MarkerMovie.mpg");
		if ( cap.isOpened()==false ) {
			std::cout << "No video file found. Exiting."      << std::endl;
			exit(0);
		}
	}

}


// Added in Exercise 9 - Start *****************************************************************
void multMatrix(float mat[16], float vec[4])
{
	for(int i=0; i<4; i++)
	{
		snowmanLookVector[i] = 0;
		for(int j=0; j<4; j++)
			  snowmanLookVector[i] += mat[4*i + j] * vec[j];
	}
}

void moveBall(float mat[16])
{
	float vector[3];
	vector[0] = mat[3] - ballpos.x;
	vector[1] = mat[7] - ballpos.y;
	vector[2] = mat[11] - ballpos.z;

	float length = sqrt( vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2] );
    //if(balldebug) std::cout << length << std::endl;
	if( length < 0.01) 
	{ 
		towards = towardsList[(towardscounter++)%2]; 
        //if(balldebug) std::cout << "target changed to marker " << towards << std::endl;
		ballspeed = 60 + 80 * rand()/RAND_MAX;
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

	if(towards == markernumber) moveBall(lookAtMarker);
	
	//normalize vector
	float help = sqrt( vector[0]*vector[0] + vector[1]*vector[1] + vector[2]*vector[2] );
	vector[0] /= help;
	vector[1] /= help;
	vector[2] /= help;

    //if(debugmode) std::cout << "Vector: " << vector[0] << ", " << vector[1] << ", " << vector[2] << std::endl;

	float defaultLook[4] = {1,0,0,0};
	multMatrix(thisMarker, defaultLook);

	//normalize snowmanLookVector
	help = sqrt( snowmanLookVector[0]*snowmanLookVector[0] + snowmanLookVector[1]*snowmanLookVector[1] + snowmanLookVector[2]*snowmanLookVector[2] );
	snowmanLookVector[0] /= help;
	snowmanLookVector[1] /= help;
	snowmanLookVector[2] /= help;

    //if(debugmode) std::cout << "SnowmanLookVector: " << snowmanLookVector[0] << ", " << snowmanLookVector[1] << ", " << snowmanLookVector[2] << std::endl;

	float angle = (180 / M_PI) * acos( vector[0] * snowmanLookVector[0] + vector[1] * snowmanLookVector[1] + vector[2] * snowmanLookVector[2] );
	if((vector[0] * snowmanLookVector[1] - vector[1] * snowmanLookVector[0]) < 0 ) angle *= -1;
	
    //if(debugmode) std::cout << "Angle: " << angle << std::endl;
	
	glRotatef(angle, 0, 0, 1);
}
// Added in Exercise 9 - End *****************************************************************


void display( GLFWwindow* window, const cv::Mat &img_bgr, std::vector<Marker> &markers )
{
	memcpy( bkgnd, img_bgr.data, sizeof(bkgnd) );

	int width0, height0;
	glfwGetFramebufferSize(window, &width0, &height0);
//	reshape(window, width, height);

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

    glRasterPos2i( 0, camera_height-1 );
    glDrawPixels( camera_width, camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd );

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);


    // move to marker-position
    glMatrixMode( GL_MODELVIEW );
    
	for( int i=0; i<markers.size(); i++){

	}
		

// Added in Exercise 9 - Start *****************************************************************

	for(int i=0; i<markers.size(); i++){
		const int code =markers[i].code;
        if(code == 0x0b44) {
			for(int j=0; j<16; j++)
                resultMatrix_0B44[j] = markers[i].resultMatrix[j];
        }else if(code == 0x1c44){
			for(int j=0; j<16; j++)
                resultMatrix_1C44[j] = markers[i].resultMatrix[j];
        } else if(code == 0x1228){
            for(int j=0; j<16; j++)
                resultMatrix_1228[j] = markers[i].resultMatrix[j];
        } else if(code == 0x0272){
            for(int j=0; j<16; j++)
                resultMatrix_0272[j] = markers[i].resultMatrix[j];
        }
	}


    //for (int x=0; x<4; ++x)
    //	for (int y=0; y<4; ++y)
    //        resultTransposedMatrix[x*4+y] = resultMatrix_005A[y*4+x];
// Added in Exercise 9 - End *****************************************************************

	//glLoadTransposeMatrixf( resultMatrix );
	glLoadMatrixf( resultTransposedMatrix );
    //drawSnowman(false);


// Added in Exercise 9 - Start *****************************************************************
//	rotateToMarker(resultMatrix_005A, resultMatrix_0272, 0x005a);

    //drawSnowman( 0 );

    //for (int x=0; x<4; ++x)
    //	for (int y=0; y<4; ++y)
    //		resultTransposedMatrix[x*4+y] = resultMatrix_0272[y*4+x];

	glLoadMatrixf( resultTransposedMatrix );
	
    //rotateToMarker(resultMatrix_0272, resultMatrix_005A, 0x0272);

    //drawSnowman( 1 );

	//drawBall
	glLoadIdentity();
	glTranslatef(ballpos.x, ballpos.y + 0.024, ballpos.z);
	glColor4f(1,0,0,1);
	drawSphere(0.005, 10, 10);
// Added in Exercise 9 - End *****************************************************************


	int key = cvWaitKey (10);
	if (key == 27) exit(0);
	// Added in Exercise 9 - Start *****************************************************************
	else if (key == 100) debugmode = !debugmode;
	else if (key == 98) balldebug = !balldebug;
	// Added in Exercise 9 - End *****************************************************************
	
}


void reshape( GLFWwindow* window, int width, int height ) {

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
	gluPerspective( virtual_camera_angle, ((GLfloat)width/(GLfloat)height), 0.01, 100 );

	// invalidate display
	//glutPostRedisplay();
}


int main(int argc, char* argv[]) 
{
	bool firstRun = true;
    GLFWwindow* combineWindow, *markerPositionWindow;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	// initialize the window system
    /* Create a windowed mode window and its OpenGL context */
    combineWindow = glfwCreateWindow(camera_width, camera_height, "Combine Window", NULL, NULL);
    if (!combineWindow)
    {
        glfwTerminate();
        return -1;
    }

    markerPositionWindow = glfwCreateWindow(camera_width, camera_height, "Marker Positions", NULL, NULL);
    if (!markerPositionWindow)
    {
        glfwTerminate();
        return -1;
    }

    initBiped();
	
	// Set callback functions for GLFW
    //glfwSetFramebufferSizeCallback(window, reshape);
    //glfwSetFramebufferSizeCallback(markerPositionWindow, myReshape);
    glfwSetFramebufferSizeCallback(combineWindow, combineReshape);

    //glfwMakeContextCurrent(window); 
	glfwSwapInterval( 1 );
	
    glfwSetWindowPos(markerPositionWindow, 640, 0);

    int window_width, window_height, markerPositionWindow_width, markerPositionWindow_height;


    //glfwMakeContextCurrent(markerPositionWindow);
    //glfwSwapInterval( 1 );
    //glfwGetFramebufferSize(markerPositionWindow, &markerPositionWindow_width, &markerPositionWindow_height);
    //reshape(markerPositionWindow, markerPositionWindow_width, markerPositionWindow_height);
    //glViewport(0, 0, window_width, window_height);


    glfwMakeContextCurrent(combineWindow);
    glfwSwapInterval( 1 );
    glfwGetFramebufferSize(combineWindow, &window_width, &window_height);
    combineReshape(combineWindow, window_width, window_height);
    glViewport(0, 0, window_width, window_height);

	// initialize the GL library
    //initGL(argc, argv);
    myInitGL(argc, argv);

    // setup OpenCV
	cv::Mat img_bgr;
	initVideoStream(cap);
	const double markerSize = 0.032; // [m]
	MarkerTracker markerTracker("camera.yml",markerSize);
    MarkerTracker_KK markerTracker_KK(0.048);
	
	std::vector<Marker> markers;
    std::vector<Marker_KK> markers_KK;

    int updateIter = 0;

//	float resultMatrix[16];
	/* Loop until the user closes the window */
    while (!glfwWindowShouldClose(combineWindow))
	{
		markers.resize(0);
		/* Capture here */

        for(int i = 0; i < 5; i++)
            cap >> img_bgr;

        cv::Mat img_bgr_mov = img_bgr.clone();
		
		if(img_bgr.empty()){
			std::cout << "Could not query frame. Trying to reinitialize." << std::endl;
			initVideoStream(cap);
			cv::waitKey(1000); /// Wait for one sec.
			continue;
		}

		/* Track a marker */
        markerTracker_KK.findMarker( img_bgr, markers_KK);
        markerTracker.findMarker( img_bgr_mov, markers);///resultMatrix);

		
		/* Render here */
        //display(window,           img_bgr, markers);


		/* Swap front and back buffers */


        glfwMakeContextCurrent(combineWindow);
        combineDisplay(combineWindow, img_bgr, markers_KK);
        glfwSwapBuffers(combineWindow);

        glfwMakeContextCurrent(markerPositionWindow);
        myDisplay(markerPositionWindow, markers);
        glfwSwapBuffers(markerPositionWindow);

//        if(updateIter < 3)
//        {
//            updateIter++;
//        }
//        else
//        {
            updateA(movA.x, movA.y, movA.z, markerA_update);

            //if(isUpdatedB)
                updateB(movB.x, movB.y, movB.z, markerB_update);

           //if(isUpdatedC)
                updateC(movC.x, movC.y, movC.z, markerC_update);

            //if(isUpdatedD)
                updateD(movD.x, movD.y, movD.z, markerD_update);

         //   updateIter = 0;
        //}

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

  
    return 0;
}
