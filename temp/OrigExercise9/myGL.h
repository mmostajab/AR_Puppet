#include "GLFW/glfw3.h"
#include <iomanip>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <cstdlib>

#include "MarkerTracker.h"

using std::min;

void printMat(const float* mat);

float resultMatrix_0B44[16];
float resultMatrix_0272[16];
float resultMatrix_1228[16];
float resultMatrix_1C44[16];

const float virtual_camera_angle = 30.0f;

void myReshape( GLFWwindow* window, int width, int height ) ;

struct stPos;

struct stVec
{
    float x, y, z;

    stVec(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f): x(_x), y(_y), z(_z)
    {
    }

    bool nonZero() const
    {   if(abs(x) >= 0.0001f || abs(y) >= 0.0001f || abs(z) >= 0.0001f)
            return true;
        return false;
    }

    const stVec& normalize()
    {
        float len = x * x + y * y + z * z;

        x /= len;
        y /= len;
        z /= len;

        return (*this);
    }

     stVec operator*(const float& f) const
     {
         stVec out;

         out.x = x * f;
         out.y = y * f;
         out.z = z * f;

         return out;
     }

     operator stPos();
};

struct stPos
{
    float x, y, z;
    float r, g, b;

    const stPos& operator=(const stPos& rop)
    {
        x = rop.x;
        y = rop.y;
        z = rop.z;

        r = rop.r;
        g = rop.g;
        b = rop.b;

        return *this;
    }

    operator stVec()
    {
        stVec vec;

        vec.x = x;
        vec.y = y;
        vec.z = z;

        return vec;
    }

    float len2()
    {
        return x * x + y * y + z * z;
    }
};

stVec::operator stPos()
{
    stPos pos;

    pos.x = x;
    pos.y = y;
    pos.z = z;

    return pos;
}

stPos markerApos, markerBpos, markerCpos, markerDpos;
std::vector<stPos> markerATrajectory, markerBTrajectory;

void drawHandle_Spheres(const stPos& a, const stPos& b)
{
    //glDisable(GL_DEPTH);

    glLoadIdentity();
    glBegin(GL_LINES);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
    glEnd();

    GLUquadric* quad;
    quad = gluNewQuadric();

    glLoadIdentity();
    glColor3f(a.r, a.g, a.b);
    glTranslated(a.x, a.y, a.z);
    gluSphere(quad, 0.1, 100, 100);

    glLoadIdentity();
    glColor3f(b.r, b.g, b.b);
    glTranslated(b.x, b.y, b.z);
    gluSphere(quad, 0.1, 100, 100);
}

void drawTrajectories()
{
    glLoadIdentity();
    glColor3f(markerApos.r, markerApos.g, markerApos.b);
    glBegin(GL_LINES);
    for(int i = 0; i < markerATrajectory.size(); i++)
        glVertex3d(markerATrajectory[i].x, markerATrajectory[i].y, markerATrajectory[i].z);
    glEnd();

    glLoadIdentity();
    glColor3f(markerBpos.r, markerBpos.g, markerBpos.b);
    glBegin(GL_LINES);
    for(int i = 0; i < markerBTrajectory.size(); i++)
        glVertex3d(markerBTrajectory[i].x, markerBTrajectory[i].y, markerBTrajectory[i].z);
    glEnd();
}



/* program & OpenGL initialization */
void myInitGL(int argc, char *argv[])
{
    // initialize the GL library
   // std::cout << "MyInitGL\n";
// Added in Exercise 8 - End *****************************************************************
    // pixel storage/packing stuff
    glPixelStorei( GL_PACK_ALIGNMENT,   1 ); // for glReadPixels​
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // for glTexImage2D​
    glPixelZoom( 1.0, -1.0 );
// Added in Exercise 8 - End *****************************************************************

    // enable and set colors
   // glEnable( GL_COLOR_MATERIAL );
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

    glDisable(GL_LIGHTING);

    markerApos.x = -0.5f; markerApos.y = 0.0f; markerApos.z = 0.0f; markerApos.r = 0; markerApos.g = 1; markerApos.b = 1;
    markerBpos.x =  0.5f; markerBpos.y = 0.0f; markerBpos.z = 0.0f; markerBpos.r = 1; markerBpos.g = 0; markerBpos.b = 1;

    markerCpos.x = 0.0f; markerCpos.y = 0.0f; markerCpos.z = -0.5f; markerCpos.r = 0; markerCpos.g = 1; markerCpos.b = 0;
    markerDpos.x = 0.0f; markerDpos.y = 0.0f; markerDpos.z = 0.5f; markerDpos.r = 1; markerDpos.g = 0; markerDpos.b = 0;
}

void update(float* transform, stPos& pos)
{
    stPos nextPos;

    nextPos.x = transform[0] * pos.x + transform[1] * pos.y + transform[2] * pos.z + transform[3];
    nextPos.y = transform[4] * pos.x + transform[5] * pos.y + transform[6] * pos.z + transform[7];
    nextPos.z = transform[8] * pos.x + transform[9] * pos.y + transform[10] * pos.z + transform[11];

    stVec mov;
    mov.x = nextPos.x - pos.x;
    mov.y = nextPos.y - pos.y;
    mov.z = nextPos.z - pos.z;

    float movLen = mov.x * mov.x + mov.y * mov.y + mov.z * mov.z;

    if(movLen > 1)
        return;

    pos.x += mov.x / 100;
    pos.y += mov.y / 100;
    pos.z += mov.z / 100;
}

void update(const stVec& markerATranslation, const stVec& markerBTranslation, const stVec& markerCTranslation, const stVec& markerDTranslation)
{
    bool updateA = false, updateB = false, updateC = false, updateD = false;

    markerApos.x += markerATranslation.x;    markerApos.y += markerATranslation.y;    markerApos.z += markerATranslation.z;
    markerBpos.x += markerBTranslation.x;    markerBpos.y += markerBTranslation.y;    markerBpos.z += markerBTranslation.z;
    markerCpos.x += markerCTranslation.x;    markerCpos.y += markerCTranslation.y;    markerCpos.z += markerCTranslation.z;
    markerDpos.x += markerDTranslation.x;    markerDpos.y += markerDTranslation.y;    markerDpos.z += markerDTranslation.z;

     /*if(markerATranslation.nonZero() && markerBTranslation.nonZero())
    {
        stVec translationAB;
        translationAB.x = markerATranslation.x - markerBTranslation.x;
        translationAB.y = markerATranslation.y - markerBTranslation.y;
        translationAB.z = markerATranslation.z - markerBTranslation.z;

        markerApos.x += min(markerATranslation.x, markerBTranslation.x) + abs(translationAB.x);

        markerApos.z += min(markerATranslation.z, markerBTranslation.z) + abs(translationAB.z);

        if(markerATranslation.x > 0)
        {
            markerApos.x += min(markerATranslation.x, markerBTranslation.x) + abs(translationAB.x);
            markerBpos.x -= min(markerATranslation.x, markerBTranslation.x) + abs(translationAB.x);
        }
        else
        {
            markerApos.x -= min(markerATranslation.x, markerBTranslation.x) + abs(translationAB.x);
            markerBpos.x += min(markerATranslation.x, markerBTranslation.x) + abs(translationAB.x);
        }

        if(markerATranslation.y > 0)
        {
            markerApos.y += min(markerATranslation.y, markerBTranslation.y) + abs(translationAB.y);
            markerBpos.y -= min(markerATranslation.y, markerBTranslation.y) + abs(translationAB.y);
        }
        else
        {
            markerApos.y -= min(markerATranslation.y, markerBTranslation.y) + abs(translationAB.y);
            markerBpos.y += min(markerATranslation.y, markerBTranslation.y) + abs(translationAB.y);
        }

        if(markerATranslation.z > 0)
        {
            markerApos.z += min(markerATranslation.z, markerBTranslation.z) + abs(translationAB.z);
            markerBpos.z -= min(markerATranslation.z, markerBTranslation.z) + abs(translationAB.z);
        }
        else
        {
            markerApos.z -= min(markerATranslation.z, markerBTranslation.z) + abs(translationAB.z);
            markerBpos.z += min(markerATranslation.z, markerBTranslation.z) + abs(translationAB.z);
        }

        updateA = true;
        updateB = true;
    }

    if(markerCTranslation.nonZero() && markerDTranslation.nonZero())
    {
        stVec translationAB;
        translationAB.x = markerCTranslation.x - markerDTranslation.x;
        translationAB.y = markerCTranslation.y - markerDTranslation.y;
        translationAB.z = markerCTranslation.z - markerDTranslation.z;

        markerCpos.x += min(markerCTranslation.x, markerDTranslation.x) + abs(translationAB.x);

        markerCpos.z += min(markerCTranslation.z, markerDTranslation.z) + abs(translationAB.z);

        if(markerCTranslation.x > 0)
        {
            markerCpos.x += min(markerCTranslation.x, markerDTranslation.x) + abs(translationAB.x);
            markerDpos.x -= min(markerCTranslation.x, markerDTranslation.x) + abs(translationAB.x);
        }
        else
        {
            markerCpos.x -= min(markerCTranslation.x, markerDTranslation.x) + abs(translationAB.x);
            markerDpos.x += min(markerCTranslation.x, markerDTranslation.x) + abs(translationAB.x);
        }

        if(markerCTranslation.y > 0)
        {
            markerCpos.y += min(markerCTranslation.y, markerDTranslation.y) + abs(translationAB.y);
            markerDpos.y -= min(markerCTranslation.y, markerDTranslation.y) + abs(translationAB.y);
        }
        else
        {
            markerCpos.y -= min(markerCTranslation.y, markerDTranslation.y) + abs(translationAB.y);
            markerDpos.y += min(markerCTranslation.y, markerDTranslation.y) + abs(translationAB.y);
        }

        if(markerCTranslation.z > 0)
        {
            markerCpos.z += min(markerCTranslation.z, markerDTranslation.z) + abs(translationAB.z);
            markerDpos.z -= min(markerCTranslation.z, markerDTranslation.z) + abs(translationAB.z);
        }
        else
        {
            markerCpos.z -= min(markerCTranslation.z, markerDTranslation.z) + abs(translationAB.z);
            markerDpos.z += min(markerCTranslation.z, markerDTranslation.z) + abs(translationAB.z);
        }

        updateA = true;
        updateB = true;
    }

    if(updateA && updateB && updateC && updateD)
        return;

    if(!updateA && !updateB)
    {
        if(markerATranslation.nonZero())
        {
            markerApos.x += markerATranslation.x;
            markerApos.y += markerATranslation.y;
            markerApos.z += markerATranslation.z;

            markerBpos.x -= markerATranslation.x;
            markerBpos.y -= markerATranslation.y;
            markerBpos.z -= markerATranslation.z;

        }
        else if(markerBTranslation.nonZero())
        {
            markerBpos.x += markerBTranslation.x;
            markerBpos.y += markerBTranslation.y;
            markerBpos.z += markerBTranslation.z;

            markerApos.x -= markerBTranslation.x;
            markerApos.y -= markerBTranslation.y;
            markerApos.z -= markerBTranslation.z;
        }

        updateA = true;
        updateB = true;
    }

    if(!updateC && !updateD)
    {
        if(markerCTranslation.nonZero())
        {
            markerCpos.x += markerCTranslation.x;
            markerCpos.y += markerCTranslation.y;
            markerCpos.z += markerCTranslation.z;

            markerDpos.x -= markerCTranslation.x;
            markerDpos.y -= markerCTranslation.y;
            markerDpos.z -= markerCTranslation.z;

        }
        else if(markerDTranslation.nonZero())
        {
            markerDpos.x += markerDTranslation.x;
            markerDpos.y += markerDTranslation.y;
            markerDpos.z += markerDTranslation.z;

            markerCpos.x -= markerDTranslation.x;
            markerCpos.y -= markerDTranslation.y;
            markerCpos.z -= markerDTranslation.z;
        }
    }*/
}

void printMat(const float* mat)
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            std::cout << mat[i * 4 + j] << " ";
        }

        std::cout << '\n';
    }
}

void setIdentity(float* mat)
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            if(i == j)
                mat[i * 4 + j] = 1;
            else
                mat[i * 4 + j] = 0;
        }
    }
}

void myDisplay( GLFWwindow* window, std::vector<Marker> &markers )
{
    //std::cout << "What is going on.............................................\n";
    // memcpy( bkgnd, img_bgr.data, sizeof(bkgnd) );

    int width0, height0;
    glfwGetFramebufferSize(window, &width0, &height0);
    myReshape(window, width0, height0);

    // clear buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // move to marker-position
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    bool markerA_update = false, markerB_update = false, markerC_update = false, markerD_update = false;

    setIdentity(resultMatrix_0B44);
    setIdentity(resultMatrix_1228);
    setIdentity(resultMatrix_1C44);
    setIdentity(resultMatrix_0272);

    for(int i=0; i<markers.size(); i++){
        const int code =markers[i].code;
        if(code == 0x0b44) {
            for(int j=0; j<16; j++)
                resultMatrix_0B44[j] = markers[i].resultMatrix[j];
            markerA_update = true;
        }else if(code == 0x1228){
            for(int j=0; j<16; j++)
                resultMatrix_1228[j] = markers[i].resultMatrix[j];
            markerB_update = true;
        } else if(code == 0x1C44){
            for(int j=0; j<16; j++)
                resultMatrix_1C44[j] = markers[i].resultMatrix[j];
            markerC_update = true;
        } else if(code == 0x0272){
            for(int j=0; j<16; j++)
                resultMatrix_0272[j] = markers[i].resultMatrix[j];
            markerD_update = true;
        }

        //std:://std::cout << code << "\n";
    }

    //std::cout << "Marker A Translation:  " << markerA_Translate.x << ", " << markerA_Translate.y << ", " << markerA_Translate.z << std::endl;

    //if(markerA_Translate.x < 0 && markerA_Translate.y < 0)
        //std::cout << "the translation is negative!!!\n";

    if(markerA_update || markerB_update)
    {
        if(markerA_update && markerB_update)
        {
            stPos prevMarkerApos = markerApos, prevMarkerBpos = markerBpos;

            update(resultMatrix_0B44, markerApos);
            update(resultMatrix_1228, markerBpos);

            stPos newPosMarkerA;

            newPosMarkerA.x = markerApos.x - markerBpos.x;
            newPosMarkerA.y = markerApos.y - markerBpos.y;
            newPosMarkerA.z = markerApos.z - markerBpos.z;

            float newPosLength = newPosMarkerA.len2();

            if(newPosLength > 0.01)
            {

                stPos colors;
                colors.x = markerApos.r;
                colors.y = markerApos.g;
                colors.z = markerApos.b;
                colors.r = markerBpos.r;
                colors.g = markerBpos.g;
                colors.b = markerBpos.b;

                markerApos = stPos(stVec(newPosMarkerA).normalize() *  0.5f);
                markerBpos = stPos(stVec(newPosMarkerA).normalize() * -0.5f);

                markerApos.r = colors.x;    markerBpos.r = colors.r;
                markerApos.g = colors.y;    markerBpos.g = colors.g;
                markerApos.b = colors.z;    markerBpos.b = colors.b;
            }
            else
            {
                markerApos = prevMarkerApos;
                markerBpos = prevMarkerBpos;
            }

        }
        else if(markerA_update && !markerB_update)
        {
            stPos prevMarkerApos = markerApos;
            update(resultMatrix_0B44, markerApos);

            float posLen = markerApos.len2();
            if(posLen > 0.01)
            {
                stPos colors;
                colors.x = markerApos.r;
                colors.y = markerApos.g;
                colors.z = markerApos.b;
                colors.r = markerBpos.r;
                colors.g = markerBpos.g;
                colors.b = markerBpos.b;

                markerApos = stPos(stVec(markerApos).normalize() *  0.5f);
                markerBpos = stPos(stVec(markerApos).normalize() * -0.5f);

                markerApos.r = colors.x;    markerBpos.r = colors.r;
                markerApos.g = colors.y;    markerBpos.g = colors.g;
                markerApos.b = colors.z;    markerBpos.b = colors.b;
            }
            else
            {
                markerApos = prevMarkerApos;
            }
        }
        else if(!markerA_update && markerB_update)
        {
            stPos prevMarkerBpos = markerBpos;
            update(resultMatrix_0B44, markerBpos);

            float posLen = markerBpos.len2();
            if(posLen > 0.01)
            {
                stPos colors;
                colors.x = markerApos.r;
                colors.y = markerApos.g;
                colors.z = markerApos.b;
                colors.r = markerBpos.r;
                colors.g = markerBpos.g;
                colors.b = markerBpos.b;

                markerBpos = stPos(stVec(markerBpos).normalize() *  0.5f);
                markerApos = stPos(stVec(markerBpos).normalize() * -0.5f);

                markerApos.r = colors.x;    markerBpos.r = colors.r;
                markerApos.g = colors.y;    markerBpos.g = colors.g;
                markerApos.b = colors.z;    markerBpos.b = colors.b;
            }
            else
            {
                markerBpos = prevMarkerBpos;
            }
        }

        markerATrajectory.push_back(markerApos);
        markerBTrajectory.push_back(markerBpos);
    }

    std::cout << "Marker Positions = \n";
    std::cout << "Marker A: " << markerApos.x << " " << markerApos.y << " " << markerApos.z << std::endl;
    std::cout << "Marker B: " << markerBpos.x << " " << markerBpos.y << " " << markerBpos.z << std::endl;
    std::cout << "Marker C: " << markerCpos.x << " " << markerCpos.y << " " << markerCpos.z << std::endl;
    std::cout << "Marker D: " << markerDpos.x << " " << markerDpos.y << " " << markerDpos.z << std::endl;
    std::cout << std::endl << std::endl;

    std::cout << "mat = \n";
    printMat(resultMatrix_1228);



    std::cout << "Marker A Position:  " << markerApos.x << ", " << markerApos.y << ", " << markerApos.z << std::endl;

    //std::cout << "Updaaaaaaaaaaaaaaaaaaaaaaate----\n" << markerA_Translate.y << " " << markerB_Translate.y << " " << markerC_Translate.y << " " << markerD_Translate.y;

    //update(markerA_Translate, markerB_Translate, markerC_Translate, markerD_Translate);
    //update(stVec(0.8, 0.8, 1.8), stVec(0.1, 0.1, 0.01), stVec(0.01, 0.01, 0.01), stVec(0.01, 0.01, 0.01));

    GLUquadric* quad;
    quad = gluNewQuadric();
    glLoadIdentity();
    glColor3f(1, 1, 1);
    glTranslated(0, 0, 0);
    gluSphere(quad, 0.03, 100, 100);

    drawTrajectories();
    drawHandle_Spheres(markerApos, markerBpos);
    drawHandle_Spheres(markerCpos, markerDpos);

    //glLoadIdentity();
    //GLUquadric* quad;
    //quad = gluNewQuadric();
    //glColor3f(1, 1, 0);

    //glTranslatef(-0.5, 0, 0);

    //gluSphere(quad, 0.1, 100, 100);
    //drawSphere(0.005, 100, 100);

    //glLoadIdentity();
    //glRotated(90, 0, 1, 0);
    //glTranslated(0., 0, -0.5);
    //gluCylinder(quad, 0.05, 0.05, 1, 10, 10);

    //glLoadIdentity();
   // glColor3f(1, 0, 0);
    //glTranslated(-0.5f, 0, 0);
    //gluSphere(quad, 0.1, 10, 10);

    //glLoadIdentity();
    //glColor3f(0, 0, 1);
    //glTranslated(0.5f, 0, 0);
    //gluSphere(quad, 0.1, 10, 10);

    //glLoadIdentity();
    //glColor3f(1, 0, 0);
    //glTranslated(0, 0, 0.5f);
    //gluSphere(quad, 0.1, 10, 10);

    //glLoadIdentity();
    //glColor3f(0, 0, 1);
    //glTranslated(0.5f, 0, -0.5f);
    //gluSphere(quad, 0.1, 10, 10);


   // for (int x=0; x<4; ++x)
   //     for (int y=0; y<4; ++y)
    //        resultTransposedMatrix[x*4+y] = resultMatrix_005A[y*4+x];
// Added in Exercise 9 - End *****************************************************************

    //glLoadTransposeMatrixf( resultMatrix );
    //glLoadMatrixf( resultTransposedMatrix );
   // drawSnowman(false);


// Added in Exercise 9 - Start *****************************************************************
    //rotateToMarker(resultMatrix_005A, resultMatrix_0272, 0x005a);

    //drawSnowman( 0 );

    //for (int x=0; x<4; ++x)
     //   for (int y=0; y<4; ++y)
     //       resultTransposedMatrix[x*4+y] = resultMatrix_0272[y*4+x];

   // glLoadMatrixf( resultTransposedMatrix );

    //rotateToMarker(resultMatrix_0272, resultMatrix_005A, 0x0272);

    //drawSnowman( 1 );

    //drawBall
    glLoadIdentity();
    glColor4f(1,0,0,1);

}


void myReshape( GLFWwindow* window, int width, int height ) {

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
    //gluPerspective( 120, ((GLfloat)width/(GLfloat)height), 0.01, 100 );
    //gluOrtho2D(-1, 1, -1, 1);
    gluPerspective(30, ((GLfloat)width/(GLfloat)height), 0.01, 100);
    //glFrustum(-1, 1, -1, 1, 0, 1);
    gluLookAt(0, 2, -2, 0, 0, 0, 0, 1, 0);
    //gluPers

    // invalidate display
    //glutPostRedisplay();
}
