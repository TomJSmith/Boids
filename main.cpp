// CPSC 587 Created By: Andrew Owens
// This is a (very) basic program to
// 1) load shaders from external files, and make a shader program
// 2) make Vertex Array Object and Vertex Buffer Object for the triangle

// take a look at the following sites for further readings:
// opengl-tutorial.org -> The first triangle (New OpenGL, great start)
// antongerdelan.net -> shaders pipeline explained
// ogldev.atspace.co.uk -> good resource


// NOTE: this dependencies (include/library files) will need to be tweaked if
// you wish to run this on a non lab computer

#include<iostream>
#include<cmath>
#include<vector>
#include <fstream>
#include <iostream>

#include<GL/glew.h> // include BEFORE glut
#include<GL/glut.h>

#include "ShaderTools.h"
#include "Vec3f.h"
#include "Mat4f.h"
#include "OpenGLMatrixTools.h"
#include "Boid.h"
//#include "Camera.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using std::cout;
using std::endl;
using std::cerr;

GLuint vaoID;
GLuint basicProgramID;

// Could store these two in an array GLuint[]
GLuint vertBufferID;
GLuint colorBufferID;

Mat4f MVP;
Mat4f M;
Mat4f V;
Mat4f P;

int numBoids = 20;
int numSpheres = 100; 

// radiuses
float RA = 0.5;
float RC = 2.0;
float RM = 4.0;
float deltaT = 1.0;
float spaceRadius = 8.0;
float sphereAR = 2;

// our boids and obstacles
std::vector<Boid> boids;
std::vector<Vec3f> spheres;

// camera control
float angle = 0.0;
float xaxis = 0.0;
bool rotate = true;
float spin = 0.0;

// force scalling info and bait ball info
float RFS, RDS, CFS, CDS, MFS, MDS, ORFS, ORDS, ABFS, ORS, BBFS;
bool baitBall = false;
float baitX, baitY, baitZ;
Vec3f baitPos;


int WIN_WIDTH = 800, WIN_HEIGHT = 600;

// function declarations... just to keep things kinda organized.
void readSettings();
void initBoids();
void initSpheres();
void updateBoids();
float weight(float len);
void updateModel(Boid aBoid);
void displayFunc();
void resizeFunc();
void idleFunc();
void init();
void generateIDs();
void deleteIDs();
void setupVAO();
void loadBuffer();
void loadProjectionMatrix();
void loadModelViewMatrix();
void setupModelViewProjectionTransform();
void reloadMVPUniform();
int main( int, char** );
// function declarations

// read in the initial information from info.txt
void readSettings()
{
	char string[100];
	float data;
	std::ifstream infile("F:\\My Documents\\Projects\\Boids\\Tut01\\info.txt");
	if (!infile.is_open()) 
	{
		std::cout << "info.txt not opened" << std::endl;
	}
	else 
	{
		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> numBoids;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> numSpheres;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> ORS;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> RFS;
		
		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> RDS;
		
		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> CFS;
		
		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> CDS;
		
		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> MFS;
		
		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> MDS;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> ORFS;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> ABFS;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> baitBall;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> baitX;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> baitY;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> baitZ;

		for (int i = 0; i < 4; i++)
		{
			infile >> string;
		}
		infile >> BBFS;

		baitPos = Vec3f(baitX, baitY, baitZ);
	}
}

void initBoids()
{
	srand(time(NULL));
	float x, y, z;
	for (int i = 0; i < numBoids; i++)
	{
		x = ((rand() % 200) * 0.001) - 0.1;
		y = ((rand() % 200) * 0.001) - 0.1;
		z = ((rand() % 200) * 0.001) - 0.1;
		boids.push_back(Boid());
		boids[i].pos(Vec3f(x, y, z));
	}
}

void initSpheres()
{
	srand(time(NULL));
	float x, y, z;
	for (int i = 0; i < numSpheres; i++)
	{
		x = ((rand() % 200) * 0.10) - 10;
		y = ((rand() % 200) * 0.10) - 10;
		z = ((rand() % 200) * 0.10) - 10;
		spheres.push_back(Vec3f(x, y, z));
	}
}

// called every frame
void updateBoids()
{
	// take into account other boids
	for (int i = 0; i < boids.size(); i++)
	{
		for (int j = 0; j < boids.size(); j++)
		{
			if (i == j)
			{
				continue;
			}
			int len = boids[i].pos().distance(boids[j].pos());
			if (len <= (RA * RDS))
			{
				Vec3f force = boids[i].pos() - boids[j].pos();
				force.normalize();
				force = force * weight(len);
				force = force * RFS;
				boids[i].addForce(force);
			}
			else if (len <= (RC * CDS))
			{
				Vec3f force = boids[j].vel() * weight(len);
				force = force * CFS;
				boids[i].addForce(force);
			}
			else if (len <= (RM * MDS))
			{
				Vec3f force = boids[i].pos() - boids[j].pos();
				force.normalize();
				force = force * weight(len);
				force = force * (-1.0);
				force = force * MFS;
				boids[i].addForce(force);
			}
		}
	}
	// take into account boundary of area
	for (int i = 0; i < boids.size(); i++)
	{
		float len = boids[i].pos().distance(Vec3f(0.0, 0.0, 0.0));
		if (len >= spaceRadius)
		{
			Vec3f force = Vec3f(0.0, 0.0, 0.0) - boids[i].pos();
			force.normalize();
			force = force * ((len - spaceRadius) * (len - spaceRadius));
			force = force * ABFS;
			boids[i].addForce(force);
		}
	}

	// take into account spheres
	for (int i = 0; i < boids.size(); i++)
	{
		for (int j = 0; j < numSpheres; j++)
		{
			float len = boids[i].pos().distance(spheres[j]);
			if (len <= (sphereAR * ORS))
			{
				//std::cout << "push" << std::endl;
				Vec3f force = boids[i].pos() - spheres[j];
				force.normalize();
				force = force * (1/((len - ((sphereAR * ORS)/10)) * (len - ((sphereAR * ORS) / 10)))) * 4.0;
				force = force * ORFS;
				boids[i].addForce(force);
			}
		}
	}

	// take into account bait ball if it exists
	if (baitBall)
	{
		for (int i = 0; i < numBoids; i++)
		{	
			float len = boids[i].pos().distance(baitPos);
			Vec3f force = baitPos - boids[i].pos();
			force.normalize();
			force = force * len * len * 50.0;
			force = force * BBFS;
			boids[i].addForce(force);
		}
	}	
}

// TODO make this a spline function thingy
float weight(float len)
{
	if (len == 0)
	{
		return 1.0;
	}
	else if (len <= RA)
	{
		return (1/(len * len)) * 0.5;
	}
	else if (len <= RC)
	{
		return ((len)* (len)) + ((len)* (len)* (len)* (len)) * 0.001;
	}
	else if (len <= RM)
	{
		return 0.005;
	}
	else
	{
		return 0.0;
	}
}

void displayFunc()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Use our shader
	glUseProgram( basicProgramID );

	// Use VAO that holds buffer bindings
	// and attribute config of buffers
	glBindVertexArray( vaoID );
	for (int i = 0; i < spheres.size(); i++)
	{
		loadModelViewMatrix();
		M = M * Mat4f({ 1.0, 0.0, 0.0, spheres[i].x(),
						0.0, 1.0, 0.0, spheres[i].y(),
						0.0, 0.0, 1.0, spheres[i].z(),
						0.0, 0.0, 0.0, 1.0 });
		setupModelViewProjectionTransform();
		reloadMVPUniform();
		//glutWireSphere(sphereAR/2, 20, 20);
		glutSolidSphere((sphereAR * ORS) / 10, 20, 20);
	}
	
	for (int i = 0; i < boids.size(); i++)
	{
		boids[i].move(deltaT);
		updateModel(boids[i]);
		setupModelViewProjectionTransform();
		reloadMVPUniform();

		glDrawArrays(GL_TRIANGLES, 0, 12);
	}
	glutSwapBuffers();
}


// TODO
void updateModel(Boid aBoid)
{
	loadModelViewMatrix();
	M = M * aBoid.getModel();
}

void idleFunc()
{
	updateBoids();
	if (rotate)
	{
		spin += 0.1f;
	}

  setupModelViewProjectionTransform();

	// send changes to GPU
	reloadMVPUniform();

	glutPostRedisplay();
}

void resizeFunc( int width, int height )
{
    WIN_WIDTH = width;
    WIN_HEIGHT = height;

    glViewport( 0, 0, width, height );

    loadProjectionMatrix();
    reloadMVPUniform();

    glutPostRedisplay();
}

void generateIDs()
{
	//replace './' with 'F:\\My Documents\\Projects\\Boids\\Tut01\\'
	std::string vsSource = loadShaderStringfromFile( "F:\\My Documents\\Projects\\Boids\\Tut01\\basic_vs.glsl" );
	std::string fsSource = loadShaderStringfromFile( "F:\\My Documents\\Projects\\Boids\\Tut01\\basic_fs.glsl" );
	basicProgramID = CreateShaderProgram( vsSource, fsSource );

	// load IDs given from OpenGL
	glGenVertexArrays( 1, &vaoID );
	glGenBuffers( 1, &vertBufferID );
	glGenBuffers( 1, &colorBufferID );
}

void deleteIDs()
{
	glDeleteProgram( basicProgramID );
	glDeleteVertexArrays( 1, &vaoID );
	glDeleteBuffers( 1, &vertBufferID );
	glDeleteBuffers( 1, &colorBufferID );
}

void loadProjectionMatrix()
{
    // Perspective Only

	// field of view angle 60 degrees
	// window aspect ratio
	// near Z plane > 0
	// far Z plane

    P = PerspectiveProjection(  60, // FOV
                                static_cast<float>(WIN_WIDTH)/WIN_HEIGHT, // Aspect
                                0.01,   // near plane
                                5 ); // far plane depth
}

void loadModelViewMatrix()
{
    M = UniformScaleMatrix( 0.05 );	// scale Quad First
    M = TranslateMatrix( 0, 0, 0 ) * M;	// translate away from (0,0,0)

    // view doesn't change, but if it did you would use this
    V = IdentityMatrix()  * TranslateMatrix(0,0, -1 + xaxis) * RotateAboutYMatrix(angle + spin);
}

void setupModelViewProjectionTransform()
{
	MVP = P * V * M; // transforms vertices from right to left (odd huh?)
}

void reloadMVPUniform()
{
	GLint mvpID = glGetUniformLocation( basicProgramID, "MVP" );

	glUseProgram( basicProgramID );
	glUniformMatrix4fv( 	mvpID,		// ID
				1,		// only 1 matrix
				GL_TRUE,	// transpose matrix, Mat4f is row major
				MVP.data()	// pointer to data in Mat4f
			);
}

void setupVAO()
{
	glBindVertexArray( vaoID );

	glEnableVertexAttribArray( 0 ); // match layout # in shader
	glBindBuffer( GL_ARRAY_BUFFER, vertBufferID );
	glVertexAttribPointer(
		0,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
	);

	glEnableVertexAttribArray( 1 ); // match layout # in shader
	glBindBuffer( GL_ARRAY_BUFFER, colorBufferID );
	glVertexAttribPointer(
		1,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
	);

	glBindVertexArray( 0 ); // reset to default
}

void loadBuffer()
{
	//pyramid shape
	std::vector< Vec3f > verts;
	// close face
	verts.push_back( Vec3f( 0.0, 0.2, 0.0 ) );
	verts.push_back( Vec3f( 0.3, 0.0, 0.0 ) );
	verts.push_back( Vec3f( 0.0, 0.0, 0.1 ) );
	// bottom face
	verts.push_back(Vec3f(0.0, 0.0, 0.1));
	verts.push_back(Vec3f(0.3, 0.0, 0.0));
	verts.push_back(Vec3f(0.0, 0.0, -0.1));
	// far face
	verts.push_back(Vec3f(0.0, 0.2, 0.0));
	verts.push_back(Vec3f(0.0, 0.0, -0.1));
	verts.push_back(Vec3f(0.3, 0.0, 0.0));
	// back face
	verts.push_back(Vec3f(0.3, 0.0, 0.0));
	verts.push_back(Vec3f(0.0, 0.0, 0.1));
	verts.push_back(Vec3f(0.0, 0.0, -0.1));




	glBindBuffer( GL_ARRAY_BUFFER, vertBufferID );
	glBufferData(	GL_ARRAY_BUFFER,
			sizeof(Vec3f)*12,	// byte size of Vec3f, 4 of them
			verts.data(),		// pointer (Vec3f*) to contents of verts
			GL_STATIC_DRAW );	// Usage pattern of GPU buffer

	// RGB values for the 4 vertices of the quad
	const float colors[] = {
			// close face
			0.5f,	0.5f,	0.5f,
			0.5f,	0.5f,	0.5f,
			0.5f,	0.5f,	1.5f,
			// bottom face
			0.7f,	0.7f,	0.7f,
			0.7f,	0.7f,	0.7f,
			0.7f,	0.7f,	0.7f,
			// far face
			0.9f,	0.9f,	0.9f,
			0.9f,	0.9f,	0.9f,
			0.9f,	0.9f,	0.9f,
			// back face
			1.0f,	0.0f,	0.0f,
			1.0f,	0.0f,	0.0f,
			1.0f,	0.0f,	0.0f	};

	glBindBuffer( GL_ARRAY_BUFFER, colorBufferID );
	glBufferData(	GL_ARRAY_BUFFER,
			sizeof(colors),
			colors,
			GL_STATIC_DRAW );
}

void init()
{
	glEnable( GL_DEPTH_TEST );

	// SETUP SHADERS, BUFFERS, VAOs

	generateIDs();
	setupVAO();
	loadBuffer();

    loadModelViewMatrix();
    loadProjectionMatrix();
	setupModelViewProjectionTransform();
	reloadMVPUniform();
}

void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.1f;

	switch (key) {
	case GLUT_KEY_LEFT:
		angle -= 1.0f;
		//lx = sin(angle);
		//lz = -cos(angle);
		break;
	case GLUT_KEY_RIGHT:
		angle += 1.0f;
		//lx = sin(angle);
		//lz = -cos(angle);
		break;
	case GLUT_KEY_UP:
		xaxis += 0.1;
		//x += lx * fraction;
		//z += lz * fraction;
		break;
	case GLUT_KEY_DOWN:
		xaxis -= 0.1;
		//x -= lx * fraction;
		//z -= lz * fraction;
		break;
	case GLUT_KEY_F8:
		rotate = !rotate;
		break;
	}
}

int main( int argc, char** argv )
{
	readSettings();

    glutInit( &argc, argv );
	// Setup FB configuration
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

	glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT );
	glutInitWindowPosition( 0, 0 );

	glutCreateWindow( "Boids" );

	glewExperimental=true; // Needed in Core Profile
	// Comment out if you want to us glBeign etc...
	if( glewInit() != GLEW_OK )
	{
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	cout << "GL Version: :" << glGetString(GL_VERSION) << endl;

	initBoids();
	initSpheres();

    glutDisplayFunc( displayFunc );
	glutReshapeFunc( resizeFunc );
    glutIdleFunc( idleFunc );
	glutSpecialFunc(processSpecialKeys);


	init(); // our own initialize stuff func

	glutMainLoop();

	// clean up after loop
	deleteIDs();

	return 0;
}
