//#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "GLee.h"  // OpenGL extension header
#include <GL/gl.h>
#include <GL/glu.h>  // OpenGL headers
#include <GL/glut.h>
#include <math.h>

#include "Vector3D.cpp"

class Puck
{
public:
	
	Vector3D* CentrePosition;
	Vector3D* Velocity;
	float Radius, Height;

	GLUquadricObj* Quadric;

	Puck()
	{
		CentrePosition = new Vector3D();
		Velocity = new Vector3D(0.0f, 0.0f, 0.0f);

		Quadric = gluNewQuadric();
		Radius = 5.0f;
		Height = 3.0f;
	}

	void Update(float TimeElapsed)

	{
		Vector3D Scalar_mult = Velocity->ScaleBy(TimeElapsed*2);		
		Vector3D NewPosition = CentrePosition->Add(Scalar_mult); // s = ut;
		CentrePosition = new Vector3D(NewPosition.X, NewPosition.Y, NewPosition.Z);
	}
	void Draw(int Texture)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		
		glPushMatrix();
		glTranslatef(CentrePosition->X, CentrePosition->Y, CentrePosition->Z);
		gluCylinder(Quadric, Radius, Radius, Height, 50, 50);

		glTranslatef(0.0f, 0.0f, Height);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_POLYGON);
            
            for (double THETA = 0; THETA <= 2 * 3.14; THETA += 3.14 / 180)
            {
				double x = (cos(THETA)) * Radius;
				double y = (sin(THETA)) * Radius;

              glVertex3d(x,y, 0.0f);
            }

            glEnd();
			glPopMatrix();
        
	}
	bool Intersects(Vector3D* PointToTest)
	{
        float Distance;
		Vector3D PointDifference = CentrePosition->Subtract(*PointToTest);

		Distance = abs(PointDifference.GetLength());
			
			if(Distance < 2 * Radius)
			{
				return true;
			}

		return false;
	}
};
