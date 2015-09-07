#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string.h>

#include "GLee.h"  // OpenGL extension header
#include <GL/gl.h>
#include <GL/glu.h>  // OpenGL headers
#include <GL/glut.h>

#include "Puck.cpp"

Puck* GamePuck;

float zNearDist = 1.0f;
float zFarDist = 1000.0f;								// Window and z-plane dimensions
float WindowWidth = 800.0f;
float WindowHeight = 600.0f;

GLfloat CameraPosition[] = {0.0f, -300.0f, 350.0f};		// Camera position and rotation
GLfloat CameraLookAt[] = {0.0f, 300.0f, 0.0f};			//((0,0,0) position and angle for 2D top-down view)

GLfloat CameraOffset[] = {0.0f, 0.0f, 0.0f};   // For free camera movement
GLfloat CameraRotation[] = {0.0f, 0.0f, 0.0f}; // For free camera rotation

GLfloat AmbientLight[] = {0.6f, 0.6f, 0.6f};
GLfloat Light0Position[] = {0.0f, 0.0f, -50.0f, 1.0f};
GLfloat Light0Intensity[] = {0.9f, 0.9f, 0.9f};


GLuint Player1Score = 1;				// Scores
GLuint Player2Score = 0;

GLfloat GoalHalfWidth = 25.0f;
GLfloat CentreOfTable[] = {0.0f, 25.0f, 1.0f};


Vector3D* Player1Position;
Vector3D* Player2Position;					// Player (i.e. bats) positions
Vector3D* DefaultPlayer1Position;
Vector3D* DefaultPlayer2Position;

GLuint Textures[9];
GLfloat HighestAnisotropy;  // Get highest supported anisotropic filtering

GLfloat UpdateTime =0.0;
GLfloat DeltaTick =0.0;
GLfloat FirstTick =0.0;
GLfloat FinalTick =0.0;
int i;
int PrevPlayer1State, PrevPlayer2State;
//int intersectTrue = 0;

void ResizeWindow(GLsizei w, GLsizei h);
void SetupWorldSpace(GLsizei w, GLsizei h);
void Setup2DOrthoSpace(GLsizei w, GLsizei h);
void RenderString(float x, float y, const char* string, void* FontToUse);
GLuint LoadTexture(char FileName[], int Width, int Height, int Depth, GLenum ColourComponents, GLenum PixelFormat);
void StartNewGame();
void ResetObjects();
void CheckPuckCollisions(Vector3D* BatToTest);
int GetTickCount();
void Update(float TimeElapsed);

void InputHandler();
void Player1Handler();
void Player2Handler();
void CheckBatMovement(int state, Vector3D* BatToMove);


void DrawHockeyTable()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Textures[7]);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
    	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-125.0f, 200.0f, 0.0f); glTexCoord2f(0.0f, 1.0f); // Top left
	glVertex3f(125.0f, 200.0f, 0.0f);  glTexCoord2f(1.0f, 1.0f); // Top right
	glVertex3f(125.0f, -150.0f, 0.0f); glTexCoord2f(1.0f, 0.0f);  // Bottom right			// Table outline
	glVertex3f(-125.0f, -150.0f, 0.0f);glTexCoord2f(0.0f, 0.0f);  // Bottom left
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Textures[0]);	// Render pitch texture
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-100.0f, 185.0f, 1.0f); glTexCoord2f(0.0f, 1.0f); // Top left
	glVertex3f(100.0f, 185.0f, 1.0f);  glTexCoord2f(1.0f, 1.0f); // Top right			// Playing area
	glVertex3f(100.0f, -135.0f, 1.0f); glTexCoord2f(1.0f, 0.0f);  // Bottom right
	glVertex3f(-100.0f, -135.0f, 1.0f);glTexCoord2f(0.0f, 0.0f);  // Bottom left
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-GoalHalfWidth, -135.0f, 1.0f); // Top left
	glVertex3f(GoalHalfWidth, -135.0f, 1.0f);  // Top right
	glVertex3f(GoalHalfWidth, -150.0f, 1.0f);  // Bottom right
	glVertex3f(-GoalHalfWidth, -150.0f, 1.0f); // Bottom left				// Goal areas
    	glVertex3f(-GoalHalfWidth, 200.0f, 1.0f);  // Top left
	glVertex3f(GoalHalfWidth, 200.0f, 1.0f);   // Top right
	glVertex3f(GoalHalfWidth, 185.0f, 1.0f);   // Bottom right
	glVertex3f(-GoalHalfWidth, 185.0f, 1.0f);  // Bottom left
	glEnd();

   	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Textures[6]);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-125.0f, -150.0f, 25.0f); glTexCoord2f(0.0f, 3.0f); // Top left
	glVertex3f(125.0f, -150.0f, 25.0f);  glTexCoord2f(1.0f, 3.0f); // Top right			// Table bottom side
    	glVertex3f(125.0f, -150.0f, 0.0f);   glTexCoord2f(1.0f, 0.0f);  // Bottom right
	glVertex3f(-125.0f, -150.0f, 0.0f);  glTexCoord2f(0.0f, 0.0f);  // Bottom left

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-125.0f, 200.0f, 25.0f); glTexCoord2f(0.0f, 3.0f);  // Top left
	glVertex3f(125.0f, 200.0f, 25.0f);  glTexCoord2f(1.0f, 3.0f);  // Top right			// Table top side
    	glVertex3f(125.0f, 200.0f, 0.0f);   glTexCoord2f(1.0f, 0.0f); // Bottom right
	glVertex3f(-125.0f, 200.0f, 0.0f);  glTexCoord2f(0.0f, 0.0f);  // Bottom left

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-125.0f, 200.0f, 25.0f); glTexCoord2f(0.0f, 3.0f); // Top left
    	glVertex3f(-125.0f, -150.0f, 25.0f);glTexCoord2f(1.0f, 3.0f);  // Top right			// Table left side
	glVertex3f(-125.0f, -150.0f, 0.0f); glTexCoord2f(1.0f, 0.0f);  // Bottom right
	glVertex3f(-125.0f, 200.0f, 0.0f);  glTexCoord2f(0.0f, 0.0f); // Bottom left

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(125.0f, 200.0f, 25.0f); glTexCoord2f(0.0f, 3.0f); // Top left
    	glVertex3f(125.0f, -150.0f, 25.0f);glTexCoord2f(1.0f, 3.0f);  // Top right			// Table right side
	glVertex3f(125.0f, -150.0f, 0.0f); glTexCoord2f(1.0f, 0.0f);  // Bottom right
	glVertex3f(125.0f, 200.0f, 0.0f);  glTexCoord2f(0.0f, 0.0f); // Bottom left
   	glEnd();
	glDisable(GL_TEXTURE_2D);
}
void DrawScoreboard()
{
	char Score[10];
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Textures[1]);				// Render scoreboard texture
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(-10.0f, 10.0f, 0.0f); glTexCoord2f(0.0f, 1.0f);  // Top left
	glVertex3f(-4.0f, 10.0f, 0.0f);  glTexCoord2f(1.0f, 1.0f);   // Top right
	glVertex3f(-4.0f, 6.0f, 0.0f);   glTexCoord2f(1.0f, 0.0f);   // Bottom right
	glVertex3f(-10.0f, 6.0f, 0.0f);  glTexCoord2f(0.0f, 0.0f); // Bottom left
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0f, 1.0f, 0.0f);
	sprintf(Score, "%d", Player1Score);
	RenderString(-8.75f, 7.25f, Score, GLUT_BITMAP_TIMES_ROMAN_24);
	sprintf(Score, "%d", Player2Score);
	RenderString(-5.6f, 7.25f, Score, GLUT_BITMAP_TIMES_ROMAN_24);
} 
void RenderString(float x, float y, const char* string, void* FontToUse)
{
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
        glRasterPos3f(0, 0, 0.0f);
	     int Length = strlen(string);
	     for(int i = 0; i < Length; i++)
	     {
		   glutBitmapCharacter(FontToUse, string[i]);
	     }
   	glPopMatrix();
} 
void DrawPlayerBat(Vector3D* Position)
{
	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glTranslatef(Position->X, Position->Y, Position->Z);		// Cylinder for main area of bat
	gluCylinder(gluNewQuadric(), 7, 7, 7, 50, 50);

	glTranslatef(0.0f, 0.0f, 7.0f);
	glBegin(GL_POLYGON);
            
            for (double THETA = 0; THETA <= 2 * 3.14; THETA += 3.14 / 180)	// Circular top of bat
            {
				double x = (cos(THETA)) * 7;
				double y = (sin(THETA)) * 7;

                glVertex2d(x, y);
            }
        glEnd();
	
	glColor3f(0.2f, 0.9f, 0.8f);
	gluSphere(gluNewQuadric(), 5, 50, 50);
   	glPopMatrix();
}
void Draw()  // Display callback
{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Setup2DOrthoSpace(WindowWidth, WindowHeight);

	  	DrawScoreboard();
	 	ResizeWindow(WindowWidth, WindowHeight);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Textures[8]);		// Draw background texture
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex3f(-300.0f, 700.0f, -200.0f); glTexCoord2f(0.0f, 1.0f);  // Top left
		glVertex3f(300.0f, 700.0f, -200.0f); glTexCoord2f(1.0f, 1.0f);  // Top right
		glVertex3f(300.0f, -200.0f, -200.0f); glTexCoord2f(1.0f, 0.0f); // Bottom right
		glVertex3f(-300.0f, -200.0f, -200.0f); glTexCoord2f(0.0f, 0.0f);  // Bottom left
		glEnd();
		glDisable(GL_TEXTURE_2D);
	
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		DrawHockeyTable();
		GamePuck->Draw(0);
	 	DrawPlayerBat(Player1Position);
		DrawPlayerBat(Player2Position);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
}	
	   
void Setup2DOrthoSpace(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    	glOrtho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, zFarDist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void SetupWorldSpace(GLsizei w, GLsizei h)
{
   float AspectRatio;
	
   if(h == 0)
   {
	AspectRatio = 1.0f;
   }
   else
   {
    	AspectRatio = (float) w / (float) h;
   }
	
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(45.0f, AspectRatio, zNearDist, zFarDist);
   gluLookAt(CameraPosition[0], CameraPosition[1], CameraPosition[2], 
   CameraPosition[0] + CameraLookAt[0], CameraPosition[1] + CameraLookAt[1], CameraLookAt[2], 0.0f, 1.0f, 0.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}
void ResizeWindow(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	WindowWidth = w;
	WindowHeight = h;
	SetupWorldSpace(w, h);
}

// This function takes in any standard image file and sets up an OpenGL texture
GLuint LoadTexture(char FileName[], int Width, int Height, int Depth, GLenum ColourComponents, GLenum PixelFormat)
{
	GLubyte* RawBitmap;
	FILE* fStream;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	fStream = fopen(FileName, "rb");

	RawBitmap = (GLubyte*) malloc(Width * Height * Depth * (sizeof(GLubyte)));  // Set appropriate memory and read in RAW bitmap
	fread(RawBitmap, Width * Height * Depth, 1, fStream);
	fclose(fStream);

	// Nasty bit over with, now to set up texturing:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Linear texture filtering
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, HighestAnisotropy);  // Anisotropic filtering
    	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	gluBuild2DMipmaps(GL_TEXTURE_2D, ColourComponents, Width, Height, PixelFormat, GL_UNSIGNED_BYTE, RawBitmap);  
	// Mipmaps used for further precision

	 free(RawBitmap);  // No longer required...
	 return 0;
}

void Setup()
{
	glGetFloatv(GL_TEXTURE_MAX_ANISOTROPY_EXT, &HighestAnisotropy);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_LINE_SMOOTH);	// Anti-aliasing enabled
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH_HINT);
	ResizeWindow(WindowWidth, WindowHeight);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); // Colors respond to lighting
	glEnable(GL_COLOR_MATERIAL);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_POSITION, Light0Position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Intensity);	// Setup light

	glEnable(GL_TEXTURE_2D);
	glGenTextures(8, Textures);
	
	glBindTexture(GL_TEXTURE_2D, Textures[0]);
	LoadTexture("./Textures/Pitch.tga", 256, 256, 3, GL_RGB8, GL_BGR_EXT);			// Load in texture data

	glBindTexture(GL_TEXTURE_2D, Textures[1]);
	LoadTexture("./Textures/Scoreboard.tga", 128, 256, 3, GL_RGB8, GL_BGR_EXT);
	
	glBindTexture(GL_TEXTURE_2D, Textures[6]);
	LoadTexture("./Textures/TableSides.tga", 64, 128, 3, GL_RGB8, GL_BGR_EXT);

	glBindTexture(GL_TEXTURE_2D, Textures[7]);
	LoadTexture("./Textures/TableTexture.tga", 256, 256, 3, GL_RGB8, GL_BGR_EXT);

	glBindTexture(GL_TEXTURE_2D, Textures[8]);
	LoadTexture("./Textures/GameBackground.tga", 256, 256, 3, GL_RGB8, GL_BGR_EXT);

	glDisable(GL_TEXTURE_2D);

	GamePuck = new Puck();

	DefaultPlayer1Position = new Vector3D(0.0f, -120.0f, 0.0f);
	DefaultPlayer2Position = new Vector3D(0.0f, 170.0f, 1.0f);
	glLineWidth(5.0f);
	StartNewGame();

}

void StartNewGame()
{
	Player1Score = 0;
	Player2Score = 0;
	ResetObjects();
	intersectTrue =0;
	for(int i = 0; i < 3; i++) CameraRotation[i] = 0.0f;
	ResizeWindow(WindowWidth, WindowHeight);
}
void ResetObjects()
{
	GamePuck->Velocity = new Vector3D();
	GamePuck->CentrePosition = new Vector3D(CentreOfTable[0], CentreOfTable[1], CentreOfTable[2]);
	
	Player1Position = new Vector3D(DefaultPlayer1Position->X, DefaultPlayer1Position->Y, 
	                                 DefaultPlayer1Position->Z);
	Player2Position = new Vector3D(DefaultPlayer2Position->X, DefaultPlayer2Position->Y,
	                                 DefaultPlayer2Position->Z);
}

void IdleFunc()
{
		//Update Time
	FirstTick = FinalTick;
	FinalTick = glutGet(GLUT_ELAPSED_TIME);
	DeltaTick = (FinalTick - FirstTick) / 30;
	UpdateTime += DeltaTick;

	if(UpdateTime >= 0.5f) // Update game every 300th of a second
	{
		Update(0.125f);
		UpdateTime = 0.0f;

		glutPostRedisplay();
	      	glutSwapBuffers();
	}
}

void Update(float TimeElapsed)
{
	for(int i = 0; i < 3; i++)
	{
		if(CameraRotation[i] >= 360.0f)
		{
			CameraRotation[i] = 0.0f;
		}
	}
	
	GamePuck->Update(TimeElapsed * DeltaTick);
	CheckPuckCollisions(Player1Position);
	CheckPuckCollisions(Player2Position);
	
	InputHandler();
}
void CheckPuckCollisions(Vector3D* BatToTest)
{

	if/* BOTTOM GOAL */(((GamePuck->CentrePosition->Y < -140.0f) && (GamePuck->CentrePosition->X > -25.0f) && 
		 (GamePuck->CentrePosition->X < 25.0f)) || /* TOP GOAL */((GamePuck->CentrePosition->Y > 190.0f) &&
		 (GamePuck->CentrePosition->X > -25.0f) && (GamePuck->CentrePosition->X < 25.0f)))
	{
		
		if(GamePuck->CentrePosition->Y < -140.0f)	// If goal scored, add goal on then reset puck to middle
		{
			Player2Score++;
		}
		else
		{
			Player1Score++;
		}
		ResetObjects();

		if((Player1Score >= 6) || (Player2Score >= 6))  // First to score 6 goals wins!
		{
			
			Player1Score = Player2Score = 0;
			ResetObjects();
		}
		
	}

	if(GamePuck->CentrePosition->Y < -140.0f)	// Bounce off bottom of table
	{
		//PlayOnce(L"./Sounds/Bounce.mp3");
		GamePuck->CentrePosition->Y = -130.0f;
		GamePuck->Velocity->Y *= -1;
	}
	else if(GamePuck->CentrePosition->Y > 190.0f)	// Bounce off top of table
	{
		//PlayOnce(L"./Sounds/Bounce.mp3");
		GamePuck->CentrePosition->Y = 189.0f;
		GamePuck->Velocity->Y *= -1;
	}
	else if(GamePuck->CentrePosition->X < -110.0f)	// Bounce off left of table
	{
		//PlayOnce(L"./Sounds/Bounce.mp3");
		GamePuck->CentrePosition->X = -109.0f;
		GamePuck->Velocity->X *= -1;
	}
	else if(GamePuck->CentrePosition->X > 110.0f)		// Bounce off right of table
	{
		//PlayOnce(L"./Sounds/Bounce.mp3");
		GamePuck->CentrePosition->X = 109.0f;
		GamePuck->Velocity->X *= -1;
	}

	if(GamePuck->Intersects(BatToTest) )
	{
		if((GamePuck->Velocity->Y == 0.0f) && (GamePuck->Velocity->X == 0.0f))  // If ball isn't moving, start it off...
		{
    			if(GamePuck->CentrePosition->Y > BatToTest->Y) GamePuck->Velocity->Y = 50.0f;  // Push ball upwards
    			else if(GamePuck->CentrePosition->Y < BatToTest->Y) GamePuck->Velocity->Y = -50.0f; // Push ball downwards
		}
	else
	{
    		if(GamePuck->Velocity->Y > 0.0f) GamePuck->Velocity->Y += 5.0f; else GamePuck->Velocity->Y -= 5.0f;
		GamePuck->Velocity->Y *= -1.0f;   // Reverse Y direction
	}

		//X-axis calculations go here...
		Vector3D DistanceBetweenCentres = BatToTest->Subtract(*GamePuck->CentrePosition);  // Find distance between centres
		//DistanceBetweenCentres->Normalise();  // Convert to unit vector
		GamePuck->Velocity->X = -DistanceBetweenCentres.X * 5.0f;

		if(GamePuck->Velocity->Y < 0.0f)
		{
			GamePuck->CentrePosition->Y -= 10.0f;
		}
		else
		{
			GamePuck->CentrePosition->Y += 10.0f;
		}
	}	
}

void keyPressed (int key, int x, int y)
{
	switch (key)
	{
	      case GLUT_KEY_UP:	//Increase Y axis value
	
		Player1Position->Y += 5;
		Player1Handler();
		glutPostRedisplay();
		break;
	      case GLUT_KEY_DOWN:
		Player1Position->Y -= 5;
		Player1Handler();
		glutPostRedisplay();
		break;
	      case GLUT_KEY_LEFT:
		Player1Position->X -= 5;		
		Player1Handler();
		glutPostRedisplay();
	      	break;
	      case GLUT_KEY_RIGHT:
		Player1Position->X += 5;	
		Player1Handler();
		glutPostRedisplay();
	      	break;
	}
		
}
/*
void keyPressed (int key, int x, int y)
{
	int mod = glutGetModifiers();	
	if (intersectTrue != 1){
	switch (key)
	{
	      case GLUT_KEY_UP:	//Increase Y axis value
		if (mod == GLUT_ACTIVE_SHIFT)
		{
			Player2Position->Y += 5;
			Player2Handler();
		}
		else
		{
			Player1Position->Y += 5;
			Player1Handler();
		}
		glutPostRedisplay();
		break;
	      case GLUT_KEY_DOWN:
		if (mod == GLUT_ACTIVE_SHIFT)
		{
			Player2Position->Y -= 5;
			Player2Handler();
		}
		else
		{
			Player1Position->Y -= 5;
			Player1Handler();
		}
		glutPostRedisplay();
		break;
	      case GLUT_KEY_LEFT:
		if (mod == GLUT_ACTIVE_SHIFT)
		{
			Player2Position->X -= 5;
			Player2Handler();
		}
		else
		{
			Player1Position->X -= 5;		
			Player1Handler();
		}
		glutPostRedisplay();
	      	break;
	      case GLUT_KEY_RIGHT:
		if (mod == GLUT_ACTIVE_SHIFT)
		{
			Player2Position->X += 5;
			Player2Handler();
		}
		else
		{
			Player1Position->X += 5;	
			Player1Handler();
		}
		glutPostRedisplay();
	      	break;
	}
	}
	intersectTrue =0;	
}

*/

void InputHandler()
{	
	Player1Handler();
	Player2Handler();				
}

void Player1Handler()
{
	if((Player1Position->Y <= 25.0f) && (Player1Position->Y >= -130.0f))		// Move bat, clamp to table
	{
		CheckBatMovement(1, Player1Position);
		glutPostRedisplay();
	}
	else
	{
		 if(Player1Position->Y >= 25.0f)
		  {
			  Player1Position->Y = 25.0f;
		  }
		  else if(Player1Position->Y <= -130.0f)
		  {
			  Player1Position->Y = -130.0f;
		  }
	}
	
}
void Player2Handler()
{
	if((GamePuck->CentrePosition->Y >100.0f) && (GamePuck->CentrePosition->Y < 180.0f))	// AI for player 2
	  	{      	
			Player2Position->X = GamePuck->CentrePosition->X;
			Player2Position->Y = GamePuck->CentrePosition->Y;			
			CheckBatMovement(1, Player2Position);
			glutPostRedisplay();
		}
	else
	{
		 if(Player2Position->Y <= 25.0f)
		  {
			  Player2Position->Y = 25.0f;
		  }
		  else if(Player2Position->Y >= 180.0f)
		  {
			  Player2Position->Y = 180.0f;
		  }
	}
	
}
void CheckBatMovement(int state, Vector3D* BatToMove)
{
	
	if((state != 0) &&(BatToMove->X >= -110.0f) && (BatToMove->X <= 110.0f))
	{
		BatToMove->X += (DeltaTick * 5)*(5/1000.0f);
	}
	else
	{
		if(BatToMove->X <= -110.0f)			// Clamp bat movement to within sides of table
		{
			BatToMove->X = -110.0f;
		}
		else if(BatToMove->X >= 110.0f)
		{
			BatToMove->X = 110.0f;
		}
	}
	if(state != 0)
		BatToMove->Y += (DeltaTick * 5)*(5/1000.0f);
	
}

int main(int argc, char **argv)
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(100,100);
	glutCreateWindow("3D Air Hockey");
	glutIdleFunc(IdleFunc);
	glutDisplayFunc(Draw);
	glutSpecialFunc(keyPressed);
	glutReshapeFunc(ResizeWindow);	
	Setup();
	glutMainLoop();
	
	return 0;
}


