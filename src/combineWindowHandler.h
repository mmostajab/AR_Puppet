#ifndef COMBINE_WINDOW_HANDLER__
#define COMBINE_WINDOW_HANDLER__

#include "myBiped.h"

#define GL_BGR_EXT 0x80E0

float resultTransposedMatrix[16];

//camera settings
const int camera_width  = 640;
const int camera_height = 480;
unsigned char bkgnd[camera_width*camera_height*3];

void combineDisplay( GLFWwindow* window, const cv::Mat &img_bgr, std::vector<Marker_KK> &markers )
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
    	
	float resultMatrix_005A[16];

    int found = 0;
	for(int i=0; i<markers.size(); i++){
		const int code =markers[i].code;
        if(code == 0x005a) {
			for(int j=0; j<16; j++)
				resultMatrix_005A[j] = markers[i].resultMatrix[j];
            found = 1;
        }
	}

    if(found > 0)
    {
        for (int x=0; x<4; ++x)
            for (int y=0; y<4; ++y)
                resultTransposedMatrix[x*4+y] = resultMatrix_005A[y*4+x];

	//glLoadTransposeMatrixf( resultMatrix );
        glLoadMatrixf( resultTransposedMatrix );

        drawBiped();
    }
}


void combineReshape( GLFWwindow* window, int width, int height ) {

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

#endif
