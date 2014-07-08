#ifndef __MYBIPED_
#define __MYBIPED_

#include "linkedstructure.h"
#include "link.h"
#include "Eigen/Dense"

using namespace Eigen;

LinkedStructure leftHand;
LinkedStructure rightHand;
LinkedStructure leftFoot;
LinkedStructure rightFoot;

Vector2f leftHand_targetPoint = Vector2f::Zero();
Vector2f rightHand_targetPoint = Vector2f::Zero();
Vector2f leftFoot_targetPoint = Vector2f::Zero();
Vector2f rightFoot_targetPoint = Vector2f::Zero();

void initBiped()
{
    leftHand.setBasePosition(Vector2f(-0.02, -0.06));
    for (int i = 1; i <= 2; i++)
    {
      Color c = {1.0f, 1.0f, 0.0f, 1.0f};
        Link *leftHand = new Link(c);
        if(i == 1)
            leftHand->mAngle = -3.14f/4;
        else
            leftHand->mAngle = 3.14f/4;

        leftHand->mLength = 0.010;

        ::leftHand.addLink(leftHand);
    }
    leftHand_targetPoint = leftHand.getPosition();
    leftHand_targetPoint(1) = -leftHand_targetPoint(1);
    leftHand.moveToPoint(leftHand_targetPoint);

    rightHand.setBasePosition(Vector2f(0.02, -.06));
    for (int i = 1; i <= 2; i++)
    {
      Color c = {1.0f, 0.0f, 1.0f, 1.0f};
        Link *rightHand = new Link(c);
        if(i == 1)
            rightHand->mAngle = 5 * 3.14f/4;
        else
            rightHand->mAngle = -3.14f/4;
        rightHand->mLength = 0.010;

        ::rightHand.addLink(rightHand);
    }
    rightHand_targetPoint = rightHand.getPosition();
    rightHand_targetPoint(1) = -rightHand_targetPoint(1);
    rightHand.moveToPoint(rightHand_targetPoint);

    leftFoot.setBasePosition(Vector2f(0.0, -0.03));
    for (int i = 1; i <= 2; i++)
    {
      Color c = {0.0f, 0.0f, 1.0f, 1.0f};
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
      Color c = {0.0f, 1.0f, 0.0f, 1.0f};
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
    glTranslatef(0, 0.15, 0);
    //glutSolidSphere(15, 32, 32);
    glTranslatef(0, -0.15, 0);

    GLUquadricObj *obj = gluNewQuadric();
    glColor3f(1.0f, 0.0f, 0.0f);
    //glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, -0.06);
    gluCylinder(obj, 0.005, 0.002, 0.03, 20, 20);
    glTranslatef(0, 0, 0.06);
    //glLoadIdentity();
    //glRotatef(-90, 1, 0, 0);

    leftHand.draw();
    rightHand.draw();
    leftFoot.draw();
    rightFoot.draw();
}


#endif
