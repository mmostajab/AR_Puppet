#ifndef __MYBIPED_
#define __MYBIPED_

#include "linkedstructure.h"
#include "link.h"
#include "Eigen/Dense"
#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define RANDOM_DIVISOR 1000.0

using namespace Eigen;

LinkedStructure leftHand;
LinkedStructure rightHand;
LinkedStructure leftFoot;
LinkedStructure rightFoot;

Vector2f leftHand_targetPoint = Vector2f::Zero();
Vector2f rightHand_targetPoint = Vector2f::Zero();
Vector2f leftFoot_targetPoint = Vector2f::Zero();
Vector2f rightFoot_targetPoint = Vector2f::Zero();

GLuint bodyTexture, headTexture;

GLuint loadTexture(const char* path)
{
    return 0;
}

void initBiped()
{
    leftHand.setBasePosition(Vector2f(0.007, -0.06));
    for (int i = 1; i <= 2; i++)
    {
      //Color c = {1.0f, 1.0f, 0.0f, 1.0f};
        Color c = {115/255.0, 15/255.0, 0.0f, 1.0f};
        Link *leftHand = new Link(c);
        if(i == 1)
            leftHand->mAngle = -3.14f/4;
        else
            leftHand->mAngle = 3.14f/4;

        leftHand->mLength = 0.010;

        ::leftHand.addLink(leftHand);
    }
    //leftHand_targetPoint = leftHand.getPosition();
   //leftHand_targetPoint(1) = -leftHand_targetPoint(1);
    leftHand.moveToPoint(leftHand_targetPoint);

    rightHand.setBasePosition(Vector2f(-0.007, -.06));
    for (int i = 1; i <= 2; i++)
    {
      //Color c = {1.0f, 0.0f, 1.0f, 1.0f};
        Color c = {115/255.0, 15/255.0, 0.0f, 1.0f};
        Link *rightHand = new Link(c);
        if(i == 1)
            rightHand->mAngle = 5 * 3.14f/4;
        else
            rightHand->mAngle = -3.14f/4;
        rightHand->mLength = 0.010;

        ::rightHand.addLink(rightHand);
    }
    //rightHand_targetPoint = rightHand.getPosition();
    //rightHand_targetPoint(1) = -rightHand_targetPoint(1);
    rightHand.moveToPoint(leftHand_targetPoint);

    leftFoot.setBasePosition(Vector2f(0.0, -0.03));
    for (int i = 1; i <= 2; i++)
    {
      //Color c = {0.0f, 0.0f, 1.0f, 1.0f};
        Color c = {115/255.0, 15/255.0, 0.0f, 1.0f};
        Link *leftFoot = new Link(c);
        leftFoot->mAngle = -3.14f/4;
        leftFoot->mLength = 0.015;

        ::leftFoot.addLink(leftFoot);
    }
    leftFoot_targetPoint = leftFoot.getPosition();
    leftFoot_targetPoint(1) = -leftFoot_targetPoint(1);
    leftFoot.moveToPoint(leftFoot_targetPoint);

    rightFoot.setBasePosition(Vector2f(-0.0, -0.03));
    for (int i = 1; i <= 2; i++)
    {
      //Color c = {0.0f, 1.0f, 0.0f, 1.0f};
        Color c = {115/255.0, 15/255.0, 0.0f, 1.0f};
        Link *rightFoot = new Link(c);
        if(i == 1)
            rightFoot->mAngle = 5 * 3.14f/4;
        else
            rightFoot->mAngle = +3.14f/4;
        rightFoot->mLength = 0.015;

        ::rightFoot.addLink(rightFoot);
    }
    rightFoot_targetPoint = rightFoot.getPosition();
    rightFoot_targetPoint(1) = -rightFoot_targetPoint(1);
    rightFoot.moveToPoint(rightFoot_targetPoint);

    //targetPoint = leftHand.getPointWithinRange();
    //leftHand.moveToPoint(targetPoint);
}
void drawBiped()
{

    glColor3f(1, 1, 1);
    //glTranslatef(0, 0.15, 0);
    //glutSolidSphere(15, 32, 32);
    //glTranslatef(0, -0.15, 0);

    GLUquadricObj *obj = gluNewQuadric();

   //gluQuadricTexture(obj, GL_TRUE);

   // glEnable(GL_TEXTURE_2D);
    //glBind(GL_TEXTURE_2D, 0);

    glColor3f(115/255.0, 15/255.0, 0.0f);
    //glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, -0.06);
    gluCylinder(obj, 0.005, 0.002, 0.03, 20, 20);
    glTranslatef(0, 0, 0.06);

    glTranslatef(0, 0, -0.07);
    gluSphere(obj, 0.01, 20, 20);
    glTranslatef(0, 0, 0.07);

   // gluDeleteQuadric(obj);
    //glDisable(GL_TEXTURE_2D);

    //glLoadIdentity();
    //glRotatef(-90, 1, 0, 0);

    leftHand.draw();
    rightHand.draw();
    leftFoot.draw();
    rightFoot.draw();
}

// This function gets called every 10ms
void updateBiped()
{

//  leftHand.update();
//  //leftHand.update();
//  if(leftHand.isTargetResolved())
//  {
//      leftHand_targetPoint = leftHand.getPointWithinRange((rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR);
//      //leftHand_targetPoint = leftHand.getPointWithinRange(-0.0001, -0.0001, -0.0001);
//      leftHand.moveToPoint(leftHand_targetPoint);
//  }

//  rightHand.update();
//  //rightHand.update();
//  if(rightHand.isTargetResolved())
//  {
//      rightHand_targetPoint = rightHand.getPointWithinRange((rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR);
//      //rightHand_targetPoint = rightFoot.getPointWithinRange(-0.0001, -0.0001, -0.0001);
//      rightHand.moveToPoint(rightHand_targetPoint);
//  }

//  leftFoot.update();
//  //leftFoot.update();
//  if(leftFoot.isTargetResolved())
//  {
//       leftFoot_targetPoint = leftFoot.getPointWithinRange((rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR);
//      //leftFoot_targetPoint = leftFoot.getPointWithinRange(-0.0001, -0.0001, -0.0001);
//      leftFoot.moveToPoint(leftFoot_targetPoint);
//  }

//  rightFoot.update();
//  //rightFoot.update();
//  if(rightFoot.isTargetResolved())
//  {
//      rightFoot_targetPoint = rightFoot.getPointWithinRange((rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR, (rand() % 200 - 100) / RANDOM_DIVISOR);
//      //rightFoot_targetPoint = rightFoot.getPointWithinRange(-0.0001, -0.0001, -0.0001);
//      rightFoot.moveToPoint(rightFoot_targetPoint);
//  }

//  std::cout << "update is called...\n";
}

void updateA(float x, float y, float z, bool u)
{


    if(u)
    {
        rightFoot.update();
        rightFoot_targetPoint = rightFoot.getPointWithinRange(x, y, z);
        rightFoot.moveToPoint(rightFoot_targetPoint);
    }
}

void updateB(float x, float y, float z, bool u)
{


    if(u)
    {
        leftHand.update();
        leftHand_targetPoint = leftHand.getPointWithinRange(x, y, z);
        leftHand.moveToPoint(leftHand_targetPoint);
    }
}

void updateC(float x, float y, float z, bool u)
{


    if(u)
    {
        leftFoot.update();
        leftFoot_targetPoint = leftFoot.getPointWithinRange(x, y, z);
        leftFoot.moveToPoint(leftFoot_targetPoint);
    }
}

void updateD(float x, float y, float z, bool u)
{


    if(u)
    {
        rightHand.update();
        rightHand_targetPoint = rightHand.getPointWithinRange(x, y, z);
        rightHand.moveToPoint(rightHand_targetPoint);
    }
}


#endif
