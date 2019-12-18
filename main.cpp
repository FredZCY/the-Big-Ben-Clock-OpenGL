/* Copyright 2019, KAE&FERED, ALL Rights reserved.
*This is a Group Project for CST305 09/2019 XMUM.
*Coded by CST1709315 RAO KEYI and CST1709428 ZHANG CHENYU.
*This program is using OpenGL to draw a clock on BigBen.
*The surroundings are similar to British street style.
*The program satisfies all the basic requirements,
*providing comments with details and explanations.
*Note that the Sky environment changes over time
*Interaction operations are available.
*Enjoy and Have Fun!*/

// Include standard headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
// Include specific headers
#include <math.h>
#include <time.h>
#include <string>
// Include GLUT
#include <GL/glut.h>
// Include free-glut
//#include <gl\freeglut.h>

// Define Macros
#define XFORM_NONE    0
#define XFORM_ROTATE  1
#define XFORM_SCALE	2
#define PI 3.1415926535
#define BMP_Header_Length 54

// In glTexImage2D, the GL_RGB indicates that we are talking about a 3-component color,
// GL_BGR says how exactly it is represented in RAM.
// As a matter of fact, BMP does not store Red->Green->Blue
// but Blue->Green->Red, so we have to tell it to OpenGL:
#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

// Define variables
// For texture use
GLuint texName[9];
float x = 5;
float y = 5;
float z = 5;

// Variables For draw clock
SYSTEMTIME timeNow;
float h=90; // The middle height of the tower
float n = 360; // For drawing circle use
float R1 = 0.7;    // The first Clock radius
float R2 = 0.62;   // The second Clock radius
float r1 = 0.5;    // The radius of first dial scale
float r2 = 0.28;    // The radius of second dial scale
float ls = R2;    // Second hand
float lm = 0.47;    // Minute hand
float lh = r2;    // Hour hand

// Variables For the bus
float m=0; // The movement length of the bus
bool mf=0; // The flag of the bus movement

// Variables For interaction
float x_angle = 0.0, y_angle = 0.0, scale_size = 1;
int xform_mode = 0, ani = 0, timer = 75, release_x, release_y, press_x, press_y;


/***** Function for Loading Texture *****/
void load_texture(const char* file_name, GLuint &texture) {
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;


	/****Load bitmap image into memory****/
	// Opens the file then test if the file exists
	FILE* pFile = fopen(file_name, "rb");
	if (!pFile){printf("Image could not be opened\n");}

	// Reads the size information of the 24-bit bmp image
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);

	// Calculates the pixel data length
    GLint line_bytes = width * 3;
    while (line_bytes % 4 != 0)// bmp image guarantees that the length of the data area is a multiple of 4.
        ++line_bytes;
    total_bytes = line_bytes * height;

    // Reads pixel data
	pixels = (GLubyte*)malloc(total_bytes); // Defines a pointer to the image data in memory
	if (!pixels){fclose(pFile);exit(0);}
    fseek(pFile, BMP_Header_Length, SEEK_SET);
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile); // Closes the file
	}


	/****Load bitmap and convert to texture****/
	 // Generate the texture
	glGenTextures(1, &texture);             // The number of texture is 1, and store the pointer of first element
	glBindTexture(GL_TEXTURE_2D, texture);  // Binding 2D texture object

	// Set Texture-Filter parameters
	glTexParameteri(GL_TEXTURE_2D,          // Specifies the target
                    GL_TEXTURE_MIN_FILTER,  // The texture minifying function
                    GL_LINEAR);             // Returns the weighted average of the texture 4 elements
	glTexParameteri(GL_TEXTURE_2D,          // Specifies the target
                    GL_TEXTURE_MAG_FILTER,  // The texture magnification function
                    GL_LINEAR);             // Returns the weighted average of the texture 4 elements
	glTexParameteri(GL_TEXTURE_2D,          // Specifies the target
                    GL_TEXTURE_WRAP_S,      // Sets the wrap parameter for texture coordinate s
                    GL_REPEAT);             // The integer part of the coordinate will be ignored and a repeating pattern is formed
	glTexParameteri(GL_TEXTURE_2D,          // Specifies the target
                    GL_TEXTURE_WRAP_T,      // Sets the wrap parameter for texture coordinate t
                    GL_REPEAT);             // The integer part of the coordinate will be ignored and a repeating pattern is formed
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//specify a two-dimensional texture image, and associate image data with texture objects
	glTexImage2D(GL_TEXTURE_2D,                    // Specifies the target texture
                 0,                                // level 0 which is the base image level
                 GL_RGB,                           // use only R, G, and B components
                 width, height,                    // texture has width x height texels
                 0,                                // no border, this value must be 0
                 GL_BGR,                           // texels are in RGB format
                 GL_UNSIGNED_BYTE,                 // color components are unsigned bytes
                 pixels);                          //Specifies a pointer to the image data in memory.

}


/**** Function for Initialization ****/
void init() {
    // Basic settings
	glClearColor(0.4, 0.61, 0.97, 1);       // Set background color to black and opaque
    glShadeModel(GL_SMOOTH);                // Enable smooth shading
    glEnable(GL_DEPTH_TEST);                // Enable depth testing for z-culling

    // Set the path to BMP file and Load texture
	std::string textPath = __FILE__;
	const size_t last_slash_idx = textPath.rfind("\\");
	if (std::string::npos != last_slash_idx){textPath = textPath.substr(0, last_slash_idx);}
	load_texture((textPath + "\\BigBenFace.bmp").data(),texName[0]); //Load the BMP image to object texName[0]
	load_texture((textPath + "\\Ceiling.bmp").data(),texName[1]); //Load the BMP image to object texName[1]
    load_texture((textPath + "\\Right.bmp").data(),texName[2]); //Load the BMP image to object texName[2]
    load_texture((textPath + "\\Floor.bmp").data(),texName[3]); //Load the BMP image to object texName[3]
    load_texture((textPath + "\\Front.bmp").data(),texName[4]); //Load the BMP image to object texName[4]
    load_texture((textPath + "\\Back.bmp").data(),texName[5]); //Load the BMP image to object texName[5]
    load_texture((textPath + "\\Left.bmp").data(),texName[6]); //Load the BMP image to object texName[6]
    load_texture((textPath + "\\Night.bmp").data(),texName[7]); //Load the BMP image to object texName[7]
        load_texture((textPath + "\\Grassland.bmp").data(),texName[8]); //Load the BMP image to object texName[8]
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);   // Set pixel storage modes

    // Initialization of lighting
	GLfloat lightColor0[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat lightPos0[] = { -0.5f, 0.8f, 1.0f, 0.0f };
	GLfloat ambientColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };

    // Enable server-side GL capabilities
	glEnable(GL_COLOR_MATERIAL);    // Used since we have one or more material parameters track the current color
	glEnable(GL_LIGHTING);          // Enable us to use the current lighting parameters to compute the vertex color or index
	glEnable(GL_LIGHT0);            // Includes light 0 in the evaluation of the lighting equation
	glEnable(GL_NORMALIZE);         // Normal vectors are normalized to unit length after transformation and before lighting
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);        // Sets lighting model parameters
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);               // Returns light source parameter values
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);                // Returns light source parameter values
}


/**** Functions for rendering the whole Tower BigBen ****/
// Get the time of seconds
float Mysecond(struct tm *ptr){
    return ((PI/2)-(((float)ptr->tm_sec)/60)*2*PI);
}

// Get the time of minutes
float Mymin(struct tm *ptr){
    return ((PI/2)-((ptr->tm_min+ptr->tm_sec/60.0)/60)*2*PI);
}

// Get the time of hours
float Myhour(struct tm *ptr){
    if(0<ptr->tm_hour&&ptr->tm_hour<12)
    {
        return ((PI/2)-((float)ptr->tm_hour+ptr->tm_min/60.0)/12*2*PI);
    }else{
        return ((PI/2)-((ptr->tm_hour-12.0+ptr->tm_min/60.0)/12)*2*PI);
    }
}

// Renders the handrail of the upper part
void Handrail(float l, float height){
    for(int n=0; n<=3; n++)
    {
        glPushMatrix();
            glTranslatef(-3.2+2*n, h-19.5-l, 7.2);
            glScalef(1, height, 1);
            glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
            glutSolidCube(1.0);
        glPopMatrix();
    }

}

// Renders the upper part of Tower BigBen
void TowerUpper(){
    /* PATR-1 */
    // TOP NEEDLE
    glPushMatrix();
        glTranslated(0.0, 5+h, 0.0);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(1.5, 31.86,4,2);
    glPopMatrix();

	// TOP
	glPushMatrix();
        glRotatef(45, 0, 1, 0);
        glTranslatef(0, 22.7+h, 0);
        glScalef(4.5, 10, 4.5);
        glColor3f(0.50196078, 0.54117647, 0.52941176);//cold gray color
        glutSolidOctahedron();
	glPopMatrix();

    // The third gray box
	glPushMatrix();
        glRotatef(45, 0, 1, 0);
        glTranslatef(0, 10+h, 0);
        glScalef(8.5, 15, 8.5);
        glColor3f(0.50196078, 0.54117647, 0.52941176);//cold gray color
        glutSolidOctahedron();
	glPopMatrix();

	// Banana box near top
	glPushMatrix();
        glTranslatef(0, 11+11.3+h, 0);
        glScalef(6.3, 1, 6.3);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCube(1.0);
	glPopMatrix();

	// Gray box near top
    glPushMatrix();
        glTranslatef(0, 11+10.3+h, 0);
        glScalef(7.0, 1.2, 7.0);
        glColor3f(0.50196078, 0.54117647, 0.52941176);//cold gray color
        glutSolidCube(1.0);
	glPopMatrix();

    // TOP NEEDLE front Left
    glPushMatrix();
        glTranslated(-3.2, 22+h, 3.2);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 5, 4, 2);
    glPopMatrix();

    // TOP NEEDLE front right
    glPushMatrix();
        glTranslated(3.2, 22+h, 3.2);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 5, 4, 2);
    glPopMatrix();

    // TOP NEEDLE back right
    glPushMatrix();
        glTranslated(3.2, 22+h, -3.2);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 5, 4, 2);
    glPopMatrix();

    // TOP NEEDLE back left
    glPushMatrix();
        glTranslated(-3.2, 22+h, -3.2);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 5, 4, 2);
    glPopMatrix();

    //The second Banana box
	glPushMatrix();
        glTranslatef(0, 11+9+h, 0);
        glScalef(6, 5, 6);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCube(1.0);
	glPopMatrix();

	/* PATR-2 */
	// The second top
	glPushMatrix();
        glTranslatef(0, 11+6.2+h, 0);
        glScalef(6.5, 0.8, 6.5);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCube(1.0);
	glPopMatrix();

    // Banana box near top
	glPushMatrix();
        glTranslatef(0, 9.2+h, 0);
        glScalef(12, 1.2, 12);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCube(1.0);
	glPopMatrix();

    // Gray box near top
    glPushMatrix();
        glTranslatef(0, 8+h, 0);
        glScalef(12.8, 1.3, 12.8);
        glColor3f(0.50196078, 0.54117647, 0.52941176);//cold gray color
        glutSolidCube(1.0);
	glPopMatrix();

    // TOP NEEDLE front Left
    glPushMatrix();
        glTranslated(-6.1, 8.5+h, 6.1);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 8, 4, 2);
    glPopMatrix();

    // TOP NEEDLE front right
    glPushMatrix();
        glTranslated(6.1,8.5+h, 6.1);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 8, 4, 2);
    glPopMatrix();

    // TOP NEEDLE back right
    glPushMatrix();
        glTranslated(6.1, 8.5+h, -6.1);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 8, 4, 2);
    glPopMatrix();

    // TOP NEEDLE back left
    glPushMatrix();
        glTranslated(-6.1, 8.5+h, -6.1);
        glRotatef(90.0, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(0.3, 8, 4, 2);
    glPopMatrix();

}

// Renders the clock on the Tower BigBen
void TowerClock(){
    // Gets the system clock
    struct tm *ptr;
    time_t it;
    it=time(NULL);
    ptr=localtime(&it);

    glPushMatrix();
        // 1.1 Draw the clock dial-Background-color
        glTranslated(0,h-3,7.5);
        glScalef(6.7,6.7,6.7);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//        glColor3f(1.0, 1.0, 1.0);//while color
//        glLineWidth(2.0);
//        glBegin(GL_POLYGON);
//            for(int i=0;i<n;i++)
//                glVertex2f(R1*cos(2*PI/n*i), R1*sin(2*PI/n*i));
//        glEnd();
        // 1.2 Draw the clock dial-outline1-color
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
//        glLineWidth(2.5);
//        glBegin(GL_POLYGON);
//            for(int i=0;i<n;i++)
//                glVertex2f(R1*cos(2*PI/n*i), R1*sin(2*PI/n*i));
//        glEnd();
        // 1.3 Draw the clock dial-outline2-color
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        glColor3f(0.0,0.0,0.0);
//        glLineWidth(2.5);
//        glBegin(GL_POLYGON);
//            for(int i=0;i<n;i++)
//                glVertex2f(R2*cos(2*PI/n*i), R2*sin(2*PI/n*i));
//        glEnd();

        // 2.1 Draw the dial hour scale
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//
//         glColor3f(0.0,0.0,0.0);
//         glLineWidth(1.5);
//
//         glBegin(GL_LINES);
//         for(int j=0;j<24;j++)
//         {
//             glVertex2f(r1*cos(2*PI/24*j),r1*sin(2*PI/24*j));
//             glVertex2f(r2*cos(2*PI/24*j),r2*sin(2*PI/24*j));
//         }
//         glEnd();
//
//        glBegin(GL_POLYGON);
//         for(int j=0;j<24;j++)
//             glVertex2f(r1*cos(2*PI/24*j),r1*sin(2*PI/24*j));
//         glEnd();
//
//         glLineWidth(2.5);
//         glBegin(GL_POLYGON);
//         for(int j=0;j<24;j++)
//             glVertex2f(r2*cos(2*PI/24*j),r2*sin(2*PI/24*j));
//         glEnd();

        // 3.1 Draw the second hand
        glLineWidth(2.0);
        glColor3f(0.0,0.0,0.0);
        glBegin(GL_LINES);
            glVertex2f(0.0,0.0);
            glVertex2f(cos(Mysecond(ptr))*ls,sin(Mysecond(ptr))*ls);
        glEnd();
        // 3.2 Draw the minute hand
        glLineWidth(3.0f);
        glColor3f(0.0,0.0,0.0);
        glBegin(GL_LINES);
            glVertex2f(0.0,0.0);
            glVertex2f(cos(Mymin(ptr))*lm,sin(Mymin(ptr))*lm);
        glEnd();
        // 3.3 Draw the hour hand
        glLineWidth(5.0f);
        glColor3f(0.0,0.0,0.0);
        glBegin(GL_LINES);
            glVertex2f(0.0,0.0);
            glVertex2f(cos(Myhour(ptr))*lh,sin(Myhour(ptr))*lh);
        glEnd();

    glPopMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Renders the face of clock
void drawFace(){
	// Using texture
    glColor3f(1,1,1); // Avoids previous color interference texture
    glBindTexture(GL_TEXTURE_2D, texName[0]); // Binding the target with texture

        // Front
        glPushMatrix();
            glTranslated(0,87,2); //7.5
            glBegin(GL_QUADS);
                glTexCoord2f( 1.0, 1.0);glVertex3f( x, y, z);   // upper right
                glTexCoord2f( 0.0, 1.0);glVertex3f( -x, y, z);  // upper left
                glTexCoord2f( 0.0, 0.0);glVertex3f(-x, -y, z);  // bottom left
                glTexCoord2f( 1.0, 0.0);glVertex3f(x, -y, z);   // bottom right
             glEnd();
         glPopMatrix();

        // Right
        glPushMatrix();
            glTranslated(2.4,87,0); //7.5
            glBegin(GL_QUADS);
                glTexCoord2f( 1.0, 1.0);glVertex3f( x, y, -z);   // upper right
                glTexCoord2f( 0.0, 1.0);glVertex3f( x, y, z);  // upper left
                glTexCoord2f( 0.0, 0.0);glVertex3f( x, -y, z);  // bottom left
                glTexCoord2f( 1.0, 0.0);glVertex3f( x, -y, -z);   // bottom right
             glEnd();
         glPopMatrix();

        // Left
        glPushMatrix();
            glTranslated(-2.4,87,0); //7.5
            glBegin(GL_QUADS);
                glTexCoord2f( 1.0, 1.0);glVertex3f( -x, y, z);   // upper right
                glTexCoord2f( 0.0, 1.0);glVertex3f( -x, y, -z);  // upper left
                glTexCoord2f( 0.0, 0.0);glVertex3f( -x, -y, -z);  // bottom left
                glTexCoord2f( 1.0, 0.0);glVertex3f( -x, -y, z);   // bottom right
             glEnd();
         glPopMatrix();

        // Back
        glPushMatrix();
            glTranslated(0,87,-2); //7.5
            glBegin(GL_QUADS);
                glTexCoord2f( 1.0, 1.0);glVertex3f( -x, y, -z);   // upper right
                glTexCoord2f( 0.0, 1.0);glVertex3f( x, y, -z);  // upper left
                glTexCoord2f( 0.0, 0.0);glVertex3f( x, -y, -z);  // bottom left
                glTexCoord2f( 1.0, 0.0);glVertex3f(-x, -y, -z);   // bottom right
             glEnd();
         glPopMatrix();
	glDisable(GL_TEXTURE_2D); // Disable server-side GL capabilities
}

// Renders the clock box of Tower BigBen
void TowerClockBox(){
    /* PART1-BOX */

    // The  first box
	glPushMatrix();
        glTranslatef(0, 6+h, 0);
        glScalef(12, 3, 12);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Top of outside box
	glPushMatrix();
        glTranslatef(0, h+3.5, 0);
        glScalef(15, 3, 15);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front left of outside box
	glPushMatrix();
        glTranslatef(-6.5, h-2.7, 6.5);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front right of outside box
	glPushMatrix();
        glTranslatef(6.5, h-2.7, 6.5);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back left of outside box
	glPushMatrix();
        glTranslatef(-6.5, h-2.7, -6.5);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back right of outside box
	glPushMatrix();
        glTranslatef(6.5, h-2.7, -6.5);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Bottom of outside box
	glPushMatrix();
        glTranslatef(0, h-9.5, 0);
        glScalef(15, 3, 15);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // The inside box
	glPushMatrix();
        glTranslatef(0, h-3, 0);
        glScalef(12, 12, 12);
        glColor3f(0.0, 0.0, 0.0);//black color
        glutSolidCube(1.0);
	glPopMatrix();

	/* PATR2-CLOCKS */
	// The Front clock
		TowerClock();

    // The Right clock
    glPushMatrix();
        glRotated(90, 0, 1, 0);
        TowerClock();
    glPopMatrix();

    // The Left clock
    glPushMatrix();
        glRotated(-90, 0, 1, 0);
        TowerClock();
    glPopMatrix();

    // The Left clock
    glPushMatrix();
        glRotated(180, 0, 1, 0);
        TowerClock();
    glPopMatrix();

}

// Render the middle-floor of the Tower Lower Part
void TowerMiddle(){
    /* PART1-FIRST-BOX */

    // The  first box
	glPushMatrix();
        glTranslatef(0, h-11.5, 0);
        glScalef(12, 2, 12);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCube(1.0);
	glPopMatrix();

    // Top of outside box
	glPushMatrix();
        glTranslatef(0, h-13.5, 0);
        glScalef(14, 3, 14);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5, 6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5, 6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5, -6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5, -6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Bottom of outside box
	glPushMatrix();
        glTranslatef(0, h-25.5, 0);
        glScalef(14, 3, 14);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // The inside box
	glPushMatrix();
        glTranslatef(0, h-19.5, 0);
        glScalef(12, 12, 12);
        glColor3f(0.0, 0.0, 0.0);//black color
        glutSolidCube(1.0);
	glPopMatrix();

	// The front handrail
	Handrail(0,15);

	// The back handrail
	glPushMatrix();
        glRotated(180,0,1,0);
        Handrail(0,15);
	glPopMatrix();

    // The left handrail
	glPushMatrix();
        glRotated(90,0,1,0);
        Handrail(0,15);
	glPopMatrix();

    // The right handrail
	glPushMatrix();
        glRotated(-90,0,1,0);
        Handrail(0,15);
	glPopMatrix();

	/* PART2-SECOND-BOX */
	float l=16.5;
    // The first box
	glPushMatrix();
        glTranslatef(0, h-11.5-l, 0);
        glScalef(12, 2, 12);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCube(1.0);
	glPopMatrix();

    // Top of outside box
	glPushMatrix();
        glTranslatef(0, h-13.5-l, 0);
        glScalef(14, 3, 14);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5-l, 6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5-l, 6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5-l, -6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5-l, -6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Bottom of outside box
	glPushMatrix();
        glTranslatef(0, h-25.5-l, 0);
        glScalef(14, 3, 14);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // The inside box
	glPushMatrix();
        glTranslatef(0, h-19.5-l, 0);
        glScalef(12, 12, 12);
        glColor3f(0.0, 0.0, 0.0);//black color
        glutSolidCube(1.0);
	glPopMatrix();

	// The front handrail
	Handrail(l,15);

	// The back handrail
	glPushMatrix();
        glRotated(180,0,1,0);
        Handrail(l,15);
	glPopMatrix();

    // The left handrail
	glPushMatrix();
        glRotated(90,0,1,0);
        Handrail(l,15);
	glPopMatrix();

    // The right handrail
	glPushMatrix();
        glRotated(-90,0,1,0);
        Handrail(l,15);
	glPopMatrix();

    /* PART3-Third-BOX */
	l=2*16.5; // Controls the position
    // The first box
	glPushMatrix();
        glTranslatef(0, h-11.5-l, 0);
        glScalef(12, 2, 12);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);// Banana color
        glutSolidCube(1.0);
	glPopMatrix();

    // Top of outside box
	glPushMatrix();
        glTranslatef(0, h-13.5-l, 0);
        glScalef(14, 3, 14);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5-l, 6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5-l, 6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5-l, -6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5-l, -6.2);
        glScalef(3, 15, 3);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Bottom of outside box
	glPushMatrix();
        glTranslatef(0, h-25.5-l, 0);
        glScalef(14, 3, 14);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // The inside box
	glPushMatrix();
        glTranslatef(0, h-19.5-l, 0);
        glScalef(12, 12, 12);
        glColor3f(0.0, 0.0, 0.0);// Black color
        glutSolidCube(1.0);
	glPopMatrix();

	// The front handrail
	Handrail(l,15);

	// The back handrail
	glPushMatrix();
        glRotated(180,0,1,0);
        Handrail(l,15);
	glPopMatrix();

    // The left handrail
	glPushMatrix();
        glRotated(90,0,1,0);
        Handrail(l,15);
	glPopMatrix();

    // The right handrail
	glPushMatrix();
        glRotated(-90,0,1,0);
        Handrail(l,15);
	glPopMatrix();

}

// Renders the Door on the G-floor
void Door(){
	glPushMatrix();
        // Background
        glPushMatrix();
            glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
            glTranslatef(0, h-25.5-55.5-1.0, 7.2);
            glScalef(15, 10, 1);
            glutSolidCube(1);
        glPopMatrix();

        // Top door
        glPushMatrix();
            glColor3f(0.0, 0.0, 0.0);//black color
            glTranslatef(0, h-25.5-55.5-1.5, 7.8);
            glScalef(2, 3.5 , 0.01);
            glutSolidSphere(1, 222, 22);
        glPopMatrix();

        // Bottom door
        glPushMatrix();
            glColor3f(0.0, 0.0, 0.0);//black color
            glTranslatef(0, h-25.5-55.5-4, 7.3);
            glScalef(3.95, 4, 1);
            glutSolidCube(1);
        glPopMatrix();

	glPopMatrix();
}

// Renders the G-floor of the Tower
void TowerBottom(){

    float l=50; // Controls the position of door
    /* PART1-BOX */
    // The first box
	glPushMatrix();
        glTranslatef(0, h-11.5-3*16.5, 0);
        glScalef(12, 2, 12);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);// Banana color
        glutSolidCube(1.0);
	glPopMatrix();

    // Top of outside box
	glPushMatrix();
        glTranslatef(0, h-13.5-3*16.5, 0);
        glScalef(14, 3, 14);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5-55.5, 6.2);
        glScalef(4, 29, 4);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Front right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5-55.5, 6.2);
        glScalef(-4, 29, 4);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back left of outside box
	glPushMatrix();
        glTranslatef(-6.2, h-19.5-55.5, -6.2);
        glScalef(4, 29, -4);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

    // Back right of outside box
	glPushMatrix();
        glTranslatef(6.2, h-19.5-55.5, -6.2);
        glScalef(-4, 29, -4);
        glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
        glutSolidCube(1.0);
	glPopMatrix();

   // Bottom of outside box
	glPushMatrix();
        glTranslatef(0, h-25.5-55.5-7, 0);
        glScalef(14, 3, 14);
        glColor3f(0.50196078, 0.54117647, 0.52941176);// Cold gray color
        glutSolidCube(1.0);
	glPopMatrix();

    // The inside box
	glPushMatrix();
        glTranslatef(0, h-19.5-l-7, 0);
        glScalef(12,24,12);
        glColor3f(0.0, 0.0, 0.0);// Black color
        glutSolidCube(1.0);
	glPopMatrix();

	// The front handrail
	Handrail(l,17);

	// The back handrail
	glPushMatrix();
        glRotated(180,0,1,0);
        Handrail(l,17);
	glPopMatrix();

    // The left handrail
	glPushMatrix();
        glRotated(90,0,1,0);
        Handrail(l,17);
	glPopMatrix();

    // The right handrail
	glPushMatrix();
        glRotated(-90,0,1,0);
        Handrail(l,17);
	glPopMatrix();

	/* PART2- Renders Door */
    // The front door
	Door();
	// The back door
	glPushMatrix();
        glRotated(180,0,1,0);
        Door();
	glPopMatrix();

    // The left door
	glPushMatrix();
        glRotated(90,0,1,0);
        Door();
	glPopMatrix();

    // The right door
	glPushMatrix();
        glRotated(-90,0,1,0);
        Door();
	glPopMatrix();
}

// Renders the whole BigBen
void drawTower(){
    TowerUpper();
	TowerClockBox();
	TowerMiddle();
	TowerBottom();
}


/**** Functions for rendering other buildings ****/
// Renders the needles above columns for the left building
void needleL(float sl, float sb){
    glPushMatrix();
        glTranslated(-78+sl, 41, -21+sb);
        glRotatef(90, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(1.55,10,4,2);
    glPopMatrix();
}

// Renders the columns for the left building
void columnL(float sl,float sb){
    // One column
    glPushMatrix();
        glTranslatef(-78+sl, 42, -21+sb);
        glScalef(0.15,4.5,0.15);
        glRotated(90,1,0,0);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        GLUquadric* objCylinder = gluNewQuadric();
        gluCylinder(objCylinder, 10.0, 10.0, 10.0, 32, 5);
        gluDeleteQuadric(objCylinder);
    glPopMatrix();
}

// Renders the columns and needles above columns for the middle building
void columnAneedle(){
    // The needles above columns
    glPushMatrix();
        glTranslated(-78, 60, -21);
        glRotatef(90, -1.0, 0.0, 0.0);
        glColor3f(0.8901960784,0.8117647059,0.3411764706);//banana color
        glutSolidCone(2.5,15,4,2);
    glPopMatrix();
    // The columns
    glPushMatrix();
        glTranslatef(-78, 60, -21);
        glScalef(0.25,6,0.15);
        glRotated(90,1,0,0);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        GLUquadric* objCylinder = gluNewQuadric();
        gluCylinder(objCylinder, 10.0, 10.0, 10.0, 32, 5);
        gluDeleteQuadric(objCylinder);
    glPopMatrix();
}

// Renders the both of buildings
void drawBuild(){
    /* Renders the middle building */
    glPushMatrix();
        glColor3f(0,0,0);// Black color
        glTranslatef(-40, 20, -35);
        glScalef(80, 40, 25);
        glutSolidCube(1);
    glPopMatrix();

    // Gray roof of the middle building
    for(int n=0; n<=56; n++)
    {
        glPushMatrix();
            glTranslatef(-68+n, 40, -35);
            glScalef(17.5, 8, 17.5);
            glRotatef(45, 0, 1, 0);
            glColor3f(0.50196078, 0.54117647, 0.52941176);//Cold gray color
            glutSolidOctahedron();
        glPopMatrix();
    }

    // Blanched almond wall raw of the middle building
    for(int n=0; n<3; n++)
    {
        glPushMatrix();
            glTranslatef(-40, 38-n*15, -35);
            glScalef(81, 4, 26);
            glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
            glutSolidCube(1.0);
        glPopMatrix();
    }

    // Blanched almond wall column of the middle building (back and front)
    for(int n=0; n<8; n++)
    {
        glPushMatrix();
            glTranslatef(-73.5+n*9.5, 20, -35);
            glScalef(3, 40 , 27);
            glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
            glutSolidCube(1.0);
        glPopMatrix();
    }

    // Blanched almond wall column of the middle building (left and right)
    for(int n=0; n<3; n++)
    {
        glPushMatrix();
            glTranslatef(-40, 20, -28-7*n);
            glScalef(81, 40 , 2);
            glColor3f(1, 0.9019607843, 0.6490196078);// Blanched almond color
            glutSolidCube(1.0);
        glPopMatrix();
    }

    // Front columns and needles of the middle building
    for(int n=0; n<9; n++)
    {
        float s=n*9.5;
        columnL(s,0);
        needleL(s,0);
    }

    // Back columns and needles of the middle building
    glPushMatrix();
        glTranslated(-80,0,-70);
        glRotated(180,0,1,0);
        for(int n=0; n<9; n++)
        {
            float s=n*9.5;
            columnL(s,0);
            needleL(s,0);
        }
    glPopMatrix();

    // Right columns and needles of the back building
    glPushMatrix();
    glTranslated(79,0,-3.5);
    for (int n=0; n<4;n++)
    {
        columnL(0,-n*7);
        needleL(0,-n*7);
    }
    glPopMatrix();

    // Left columns and needles of the middle building
    glPushMatrix();
    glTranslated(-3,0,-3.5);
    for (int n=0; n<4;n++)
    {
        columnL(0,-n*7);
        needleL(0,-n*7);
    }
    glPopMatrix();
    // The bottom
    glPushMatrix();
        glTranslatef(-40, 5, -35);
        glScalef(81, 10 , 26);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanched almond color
        glutSolidCube(1.0);
    glPopMatrix();

    // Top door
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);//black color
        glTranslatef(-93+29, 6,-21);
        glScalef(2, 3 , 0.01);
        glutSolidSphere(1, 222, 22);
    glPopMatrix();

    // Bottom door
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);//black color
        glTranslatef(-93+29, 2.9, -21);
        glScalef(3.95, 4, 1);
        glutSolidCube(1);
    glPopMatrix();

    /* Renders the Left building */
    // The main body of the building
    glPushMatrix();
        glColor3f(0,0,0);//cold black color
        glTranslatef(-100, 30, -35);
        glScalef(25, 60, 25);
        glutSolidCube(1);
    glPopMatrix();

    // Roof of the left building
    glPushMatrix();
        glTranslatef(-100, 60, -35);
        glScalef(17.5, 20, 17.5);
        glRotatef(45, 0, 1, 0);
        glColor3f(0.50196078, 0.54117647, 0.52941176);//cold gray color
        glutSolidOctahedron();
    glPopMatrix();

    // Columns and needles front left
    glPushMatrix();
        glTranslated(-34,0,0);
        columnAneedle();
    glPopMatrix();

    // Columns and needles front right
    glPushMatrix();
        glTranslated(-34+25,0,0);
        columnAneedle();
    glPopMatrix();

    // Columns and needles back right
    glPushMatrix();
        glTranslated(-34+25,0,-26);
        columnAneedle();
    glPopMatrix();

    // Columns and needles back left
    glPushMatrix();
        glTranslated(-34,0,-26);
        columnAneedle();
    glPopMatrix();

    // Blanchedalmond wall raw bigger of the left building
    for(int n=0; n<4; n++)
    {
        glPushMatrix();
            glTranslatef(-100, 59-16.5*n, -35);
            glScalef(26, 3, 26);
            glColor3f(1, 0.9019607843, 0.6490196078);//blanchedalmond color
            glutSolidCube(1.0);
        glPopMatrix();
    }

    // Blanchedalmond wall raw smaller of the left building
    for(int n=0; n<4; n++)
    {
        glPushMatrix();
            glTranslatef(-100, 50-16.5*n, -35);
            glScalef(26, 1, 26);
            glColor3f(1, 0.9019607843, 0.6490196078);//blanchedalmond color
            glutSolidCube(1.0);
        glPopMatrix();
    }

    // Blanchedalmond wall column of the left building (back and front)
    glPushMatrix();
        glTranslatef(-100, 30, -35);
        glScalef(6, 60 , 26);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanchedalmond color
        glutSolidCube(1.0);
    glPopMatrix();

    // Blanchedalmond wall column of the left building (right and left)
    glPushMatrix();
        glTranslatef(-100, 30, -35);
        glScalef(26, 60 , 6);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanchedalmond color
        glutSolidCube(1.0);
    glPopMatrix();
    // The bottom
    glPushMatrix();
        glTranslatef(-100, 5, -35);
        glScalef(26, 10 , 26);
        glColor3f(1, 0.9019607843, 0.6490196078);//blanchedalmond color
        glutSolidCube(1.0);
    glPopMatrix();

    // Top door
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);//black color
        glTranslatef(-93, 6,-20);
        glScalef(2, 3 , 0.01);
        glutSolidSphere(1, 222, 22);
    glPopMatrix();

    // Bottom door
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);//black color
        glTranslatef(-93, 2.9, -20);
        glScalef(3.95, 4, 1);
        glutSolidCube(1);
    glPopMatrix();
}


/**** Functions for rendering British style street ****/
// Renders Telephone booth
void telephone(){
    // The main body of the telephone booth
    glPushMatrix();
        glTranslatef(128.5, 10.0, 78.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(14.0, 25.0, 15.1);
        glutSolidCube(1.0f);
    glPopMatrix();

    // The roof of the telephone booth
    glPushMatrix();
        glColor3f(1.0, 0.0, 0.0);
        glTranslatef(128.8, 20.0, 78.5);
        glScaled(16, 16, 16);
        glutSolidSphere(0.5, 8, 5);
	glPopMatrix();

    // The front window
    glPushMatrix();
        glTranslatef(128.5, 11.2, 88.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 1.0, 1.0);
        glScalef(12.0, 18.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Front window but the red line is one
    glPushMatrix();
        glTranslatef(128.5, 5.8, 89.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(12.0, 2.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Front window but the red line is two
    glPushMatrix();
        glTranslatef(128.5, 10.8, 89.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(12.0, 2.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();

    // Front window but red line three
    glPushMatrix();
        glTranslatef(128.5, 16.5, 89.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(12.0, 2.0, 1.0);
        glutSolidCube(1.0f); //kubus solid
    glPopMatrix();

    // The front window but the white one line
    glPushMatrix();
        glTranslatef(125.5, 11.5, 90.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(1.0, 16.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();

    // The front window but the two white lines
    glPushMatrix();
        glTranslatef(130.5, 11.5, 90.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(1.0, 16.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// Renders white lines in the street
void way(){
    for(int n=0; n<10;n++)
    {
        //One while lines
        glPushMatrix();
            glTranslatef(195.0, -2.5, -180.0+50*n);
            glColor3f(0.9f, 0.9f, 0.9f);
            glScalef(5.0, 5.0, 50.5);
            glutSolidCube(0.5f);
        glPopMatrix();
    }
}

// Renders the moving bus
void bus(){
    //bus down
    glPushMatrix();
        glTranslatef(178.5, 10.0, 68.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(12.0, 15.0, 55.1);
        glutSolidCube(1.0f);
    glPopMatrix();

    //bus above
    glPushMatrix();
        glTranslatef(178.5, 23.0, 68.5);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 0.0, 0.0);
        glScalef(12.0, 8.0, 55.1);
        glutSolidCube(1.0f);
    glPopMatrix();

   //block black line
    glPushMatrix();
        glTranslatef(178.5, 18, 68.5);
        glRotatef(3,0,1,0);
        glColor3f(0.0, 0.0, 0.0);
        glScalef(12.0, 1.0, 55.1);
        glutSolidCube(1.0f);
    glPopMatrix();

    //Left front wheel
    glPushMatrix();
        glColor3f(0.9f, 0.9f, 0.9f);
        glTranslatef(175.5-3, 4, 68.5+22);
        //glRotatef(0.0,0.0,1.0,1.0);
        glRotatef(90.0,0.0,1.0,0.0);
        glColor3f(0.0f, 0.0f, 0.f);
        glutSolidTorus(1,3,100,200);
    glPopMatrix();

    // tight front wheels
    glPushMatrix();
        glColor3f(0.9f, 0.9f, 0.9f);
        glTranslatef(175.5-4, 4, 68.5-20);
        //glRotatef(0.0,0.0,1.0,1.0);
        glRotatef(90.0,0.0,1.0,0.0);
        glColor3f(0.0f, 0.0f, 0.f);
        glutSolidTorus(1,3,100,200);
    glPopMatrix();

    //Left back wheel
    glPushMatrix();
        glColor3f(0.9f, 0.9f, 0.9f);
        glTranslatef(175.5+3, 4, 68.5+22);
        //glRotatef(0.0,0.0,1.0,1.0);
        glRotatef(90.0,0.0,1.0,0.0);
        glColor3f(0.0f, 0.0f, 0.f);
        glutSolidTorus(1,3,100,200);
    glPopMatrix();

    // right back wheels
    glPushMatrix();
        glColor3f(0.9f, 0.9f, 0.9f);
        glTranslatef(175.5+4, 4, 68.5-20);
        //glRotatef(0.0,0.0,1.0,1.0);
        glRotatef(90.0,0.0,1.0,0.0);
        glColor3f(0.0f, 0.0f, 0.f);
        glutSolidTorus(1,3,100,200);
    glPopMatrix();

    // front windshield
    glPushMatrix();
        glTranslatef(178.9, 12.8, 96);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 1.0, 1.0);
        glScalef(12.0, 8.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();

    // front windshield
    glPushMatrix();
        glTranslatef(178.9, 24.8, 96);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 1.0, 1.0);
        glScalef(12.0, 6.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();

    // back glass
    glPushMatrix();
        glTranslatef(177.9, 11.8, 40.0);
        glRotatef(3,0,1,0);
        glColor3f(1.0, 1.0, 1.0);
        glScalef(14.0, 8.0, 1.0);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// Renders the trees on both sides of the road
void tree(){

    // trees close to the left road
    for(int n=0; n<10; n++)
    {
      	//tree leaf
        glPushMatrix();
            glColor3f(0.4f, 0.2f, 0.0f);
            glTranslatef(140.0, -2.5, -180.0+n*50);
            glRotatef(270, 1, 0, 0);
            glScaled(4,4,35);
            glutSolidCone(0.5,1,5,3);
        glPopMatrix();

        //tree root
        glPushMatrix();
            glColor3f(0.5f, 0.7f, 0.1f);
            glTranslatef(140.0, 25.5, -180.0+n*50);
            glScaled(20,20,20);
            glutSolidSphere(0.5,8,5);
        glPopMatrix();
    }

    // trees close to the right road
    for(int n=0; n<10; n++)
    {
      	//tree leaf
        glPushMatrix();
            glColor3f(0.4f, 0.2f, 0.0f);
            glTranslatef(240.0, -2.5, -180.0+n*50);
            glRotatef(270, 1, 0, 0);
            glScaled(4,4,35);
            glutSolidCone(0.5,1,5,3);
        glPopMatrix();

        //tree root
        glPushMatrix();
            glColor3f(0.5f, 0.7f, 0.1f);
            glTranslatef(240.0, 25.5, -180.0+n*50);
            glScaled(20,20,20);
            glutSolidSphere(0.5,8,5);
        glPopMatrix();
    }

    // trees in front of the buildings
    for(int n=0; n<2; n++)
    {
      	//tree leaf
        glPushMatrix();
            glColor3f(0.4f, 0.2f, 0.0f);
            glTranslatef(-20.0-n*20, -2.5, 30);
            glRotatef(270, 1, 0, 0);
            glScaled(4,4,35);
            glutSolidCone(0.5,1,5,3);
        glPopMatrix();

        //tree root
        glPushMatrix();
            glColor3f(0.5f, 0.7f, 0.1f);
            glTranslatef(-20.0-n*20, 25.5, 30);
            glScaled(20,20,20);
            glutSolidSphere(0.5,8,5);
        glPopMatrix();
    }

}

// Renders the streetlights on both sides of the road
void streetlight (){
   //The streetlights close to the left street
    glPushMatrix();
        glTranslated(0,8,-15);
        for(int z=0;z<5; z++)
        {
            //The button of streetlight
            glPushMatrix();
                glColor3f(1.1f, 1.2f, 0.0f);
                glTranslatef(161.0-5, -12.5, -135.0+(z*100));
                glRotatef(270, 1, 0, 0);
                glScaled(4,4,35);
                glutSolidCone(0.5,1,5,3);
            glPopMatrix();

            //The top inside of streetlight
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(161.0-5, 19.5, -135.0+(z*100));
                glScaled(20,20,20);
                glutSolidSphere(0.12,80,50);
            glPopMatrix();

            //The top outside of streetlight
            glPushMatrix();
                glColor3f(0.9f, 0.9f, 0.9f);
                glTranslatef(161.0-5, 18.5, -135.0+(z*100));
                glRotatef(45.0,0.0,1.0,0.0);
                glRotatef(-90.0,1.0,0.0,0.0);
                glColor3f(0.9f, 0.9f, 0.9f);
                glutSolidTorus(2,2,15,100);
            glPopMatrix();

            //the button outside of the street light
            glPushMatrix();
                glTranslatef(161.0-5, -2.5, -135.0+(z*100));
                glRotatef(45.0,0.0,1.0,0.0);
                glRotatef(-90.0,1.0,0.0,0.0);
                glColor3f(0.9f, 0.9f, 0.9f);
                glutSolidTorus(2,2,5,100);
            glPopMatrix();
        }
    glPopMatrix();

    //The streetlights close to the right street
    glPushMatrix();
        glTranslated(80,8,-5);
        for(int z=0;z<5; z++)
        {
            //The button of streetlight
            glPushMatrix();
                glColor3f(1.1f, 1.2f, 0.0f);
                glTranslatef(161.0-5, -12.5, -148.0+(z*100));
                glRotatef(270, 1, 0, 0);
                glScaled(4,4,35);
                glutSolidCone(0.5,1,5,3);
            glPopMatrix();

            //The top inside of streetlight
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(161.0-5, 19.5, -148.0+(z*100));
                glScaled(20,20,20);
                glutSolidSphere(0.12,80,50);
            glPopMatrix();

            //The top outside of streetlight
            glPushMatrix();
                glColor3f(0.9f, 0.9f, 0.9f);
                glTranslatef(161.0-5, 18.5, -148.0+(z*100));
                glRotatef(45.0,0.0,1.0,0.0);
                glRotatef(-90.0,1.0,0.0,0.0);
                glColor3f(0.9f, 0.9f, 0.9f);
                glutSolidTorus(2,2,15,100);
            glPopMatrix();

            //the button outside of the street light
            glPushMatrix();
                glTranslatef(161.0-5, -2.5, -148.0+(z*100));
                glRotatef(45.0,0.0,1.0,0.0);
                glRotatef(-90.0,1.0,0.0,0.0);
                glColor3f(0.9f, 0.9f, 0.9f);
                glutSolidTorus(2,2,5,100);
            glPopMatrix();
        }
    glPopMatrix();

}

// Renders the stones next to the Telephone booth
void stone (){
    //big stone
    glPushMatrix();
        glColor3f(0.5f, 0.5f, 0.5f);
        glTranslatef(118.0, -4.5, 91.0);
        glScaled(10,10,10);
        glutSolidSphere(0.5,8,5);
	glPopMatrix();

	//small stone
	glPushMatrix();
        glColor3f(0.5f, 0.5f, 0.5f);
        glTranslatef(128.0, -2.5, 94.0);
        glScaled(5,5,5);
        glutSolidSphere(0.5,8,5);
	glPopMatrix();
}

// Renders the whole British style street
void Drawstreet(){
    //The gray street ground
    glPushMatrix();
        glColor3f(0.5f, 0.5f, 0.5f);
        glTranslatef(160, -3.0, 0);
        glScaled(70, 2, 500);
        glutSolidCube(1);
    glPopMatrix();

    //The telephone booth and stones
    glPushMatrix();
        glTranslatef(-55,0,-105);
        telephone();
        stone();
    glPopMatrix();

    //The components around the street
    glPushMatrix();
        glTranslated(-35,0,-35);
        way();
        tree();
        streetlight();
    glPopMatrix();

    //The car
    glPushMatrix();
        glTranslated(-35,0,-250+m);
        bus();
    glPopMatrix();
}



/**** Functions for rendering Ground and Platform ****/
// Renders the stairs
void Stairs(){
	glPushMatrix();
        //glTranslated(0.0, -5, 0.0);
		glColor3f(0.5, 0.5, 0.5);

		//The first base ground
		//For the tower
        glPushMatrix();
            glTranslatef(0, -0.4, 0);
            glScaled(25, 2, 25);
            glutSolidCube(1);
        glPopMatrix();
        //For the middle building
        glPushMatrix();
            glTranslatef(-40, -0.4, -35);
            glScalef(81, 2, 26);
            glutSolidCube(1);
        glPopMatrix();
        //For the Left building
        glPushMatrix();
            glTranslatef(-100, -0.4, -35);
            glScalef(26, 2, 26);
            glutSolidCube(1);
        glPopMatrix();

        //The second base ground
        //For the tower
        glPushMatrix();
            glTranslatef(0, -0.9, 0);
            glScaled(35, 1, 35);
            glutSolidCube(1);
        glPopMatrix();
        //For the middle building
        glPushMatrix();
            glTranslatef(-40, -0.9, -35);
            glScalef(88, 1, 33);
            glutSolidCube(1);
        glPopMatrix();
        //For the Left building
        glPushMatrix();
            glTranslatef(-100, -0.9, -35);
            glScalef(33, 1, 33);
            glutSolidCube(1);
        glPopMatrix();

        //The third base ground
        //road under the Tower
        glPushMatrix();
            glTranslatef(0, -2.0, 0);
            glScaled(40, 2, 40);
            glutSolidCube(1);
        glPopMatrix();
        //For the middle building
        glPushMatrix();
            glTranslatef(-40, -2.0, -35);
            glScalef(88, 2, 33);
            glutSolidCube(1);
        glPopMatrix();
        //For the Left building
        glPushMatrix();
            glTranslatef(-100, -2.0, -35);
            glScalef(33, 2, 33);
            glutSolidCube(1);
        glPopMatrix();


        //road between the middle building and the tower
        glPushMatrix();
            glTranslatef(-10, -2.0, -21.5);
            glScaled(20, 2, 5);
            glutSolidCube(1);
        glPopMatrix();

        //The longest road
        glPushMatrix();
            glTranslatef(-59, -2.0, 0);
            glScaled(80, 2, 8);
            glutSolidCube(1);
        glPopMatrix();

        //The roan between the middle building and the longest road
        glPushMatrix();
            glTranslatef(-65, -2.0, -10);
            glScaled(8, 2, 25);
            glutSolidCube(1);
        glPopMatrix();

        //The road between the left building and the longest road
        glPushMatrix();
            glTranslatef(-95, -2.0, -12);
            glScaled(8, 2, 28);
            glutSolidCube(1);
        glPopMatrix();

	glPopMatrix();

	glPushMatrix();
        //The longest road
        glTranslatef(80, -3.0, 0);
        glScaled(120, 2, 12);
        glutSolidCube(1);
	glPopMatrix();

}

// Renders the green land
void DayLand(){
    //Normal color way to draw Grassland
	glPushMatrix();
        glTranslatef(0, -4.5, 0);
        glScalef(500, 3, 500);
        glColor3f(0, 0.56, 0);//grassland color
        glutSolidCube(1.0);
	glPopMatrix();

    // Using texture
    glEnable(GL_TEXTURE_2D);
    //Ceiling
    glBindTexture(GL_TEXTURE_2D, texName[8]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 50.0, 50.0);glVertex3f( 250, -2.9, -250 );   // upper right
            glTexCoord2f( 0.0, 50.0);glVertex3f( -250, -2.9, -250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( -250, -2.9, 250 );   // bottom left
            glTexCoord2f( 50.0, 0.0);glVertex3f( 250, -2.9, 250 );    // bottom right
        glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

// Renders the dusk sky environment
void DuskSpace(){
    // Using texture
    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1); // Avoids previous color interference texture

	//Ceiling
    glBindTexture(GL_TEXTURE_2D, texName[1]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( 250, 200, -250 );   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( -250, 200, -250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( -250, 200, 250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( 250, 200, 250 );    // bottom right
        glEnd();
	glPopMatrix();

    //Right
    glBindTexture(GL_TEXTURE_2D, texName[2]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1, 1);glVertex3f( 250, 200, -250);   // upper right
            glTexCoord2f( 0.0, 1);glVertex3f( 250, 200, 250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( 250, -2.9, 250 );   // bottom left
            glTexCoord2f( 1, 0.0);glVertex3f( 250, -2.9, -250 );    // bottom right
        glEnd();
	glPopMatrix();

	//Front
    glBindTexture(GL_TEXTURE_2D, texName[5]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( 250, 200, 250 );   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( -250, 200, 250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( -250, -2.9, 250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( 250, -2.9, 250 );    // bottom right
        glEnd();
	glPopMatrix();

	//Back
    glBindTexture(GL_TEXTURE_2D, texName[4]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( -250, 200, -250 );   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( 250, 200, -250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( 250, -2.9, -250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( -250, -2.9, -250 );    // bottom right
        glEnd();
	glPopMatrix();

    //Left
    glBindTexture(GL_TEXTURE_2D, texName[6]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( -250, 200, 250);   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( -250, 200, -250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( -250, -2.9, -250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( -250, -2.9, 250 );    // bottom right
        glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

    //Floor
    //Normal color way to draw Grassland
	glPushMatrix();
        glTranslatef(0, -4.5, 0);
        glScalef(500, 4, 500);
        //glColor3f(0.1607843137, 0.1411764706, 0.1294117647); // Ivory black
        glColor3f(0.4392156863, 0.5019607843, 0.4117647059); // Slate gray
        glutSolidCube(1.0);
	glPopMatrix();
}

// Renders the night sky environment
void NightSpace(){
    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1); // Avoids previous color interference texture

	//Ceiling
    glBindTexture(GL_TEXTURE_2D, texName[7]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( 250, 200, -250 );   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( -250, 200, -250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( -250, 200, 250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( 250, 200, 250 );    // bottom right
        glEnd();
	glPopMatrix();

    //Right
    glBindTexture(GL_TEXTURE_2D, texName[7]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1, 1);glVertex3f( 250, 200, -250);   // upper right
            glTexCoord2f( 0.0, 1);glVertex3f( 250, 200, 250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( 250, -2.9, 250 );   // bottom left
            glTexCoord2f( 1, 0.0);glVertex3f( 250, -2.9, -250 );    // bottom right
        glEnd();
	glPopMatrix();

	//Front
    glBindTexture(GL_TEXTURE_2D, texName[7]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( 250, 200, 250 );   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( -250, 200, 250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( -250, -2.9, 250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( 250, -2.9, 250 );    // bottom right
        glEnd();
	glPopMatrix();

	//Back
    glBindTexture(GL_TEXTURE_2D, texName[7]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( -250, 200, -250 );   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( 250, 200, -250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( 250, -2.9, -250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( -250, -2.9, -250 );    // bottom right
        glEnd();
	glPopMatrix();

    //Left
    glBindTexture(GL_TEXTURE_2D, texName[7]); // Binding the target with texture
	glPushMatrix();
        glBegin(GL_QUADS);
            glTexCoord2f( 1.0, 1.0);glVertex3f( -250, 200, 250);   // upper right
            glTexCoord2f( 0.0, 1.0);glVertex3f( -250, 200, -250 );  // upper left
            glTexCoord2f( 0.0, 0.0);glVertex3f( -250, -2.9, -250 );   // bottom left
            glTexCoord2f( 1.0, 0.0);glVertex3f( -250, -2.9, 250 );    // bottom right
        glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

    //Floor
    //Normal color way to draw Grassland
	glPushMatrix();
        glTranslatef(0, -4.5, 0);
        glScalef(500, 4, 500);
        //glColor3f(0.1607843137, 0.1411764706, 0.1294117647); // Ivory black
        glColor3f(0.4392156863, 0.5019607843, 0.4117647059); // Slate gray
        glutSolidCube(1.0);
	glPopMatrix();

}



/**** Event Handler Functions ****/
/* Handler for mouse event. Called back when a user presses and releases mouse
   buttons in the window, each press and each release generates a mouse callback. */
void mymouse(int button, int state, int x, int y){
	if (state == GLUT_DOWN) {
		press_x = x; press_y = y;
		if (button == GLUT_LEFT_BUTTON)
			xform_mode = XFORM_ROTATE;
		else if (button == GLUT_RIGHT_BUTTON)
			xform_mode = XFORM_SCALE;
	}
	else if (state == GLUT_UP) {
		xform_mode = XFORM_NONE;
	}
}

/* Handler for motion event. Called back when the mouse moves
   within the window while one or more mouse buttons are pressed. */
void mymotion(int x, int y){
	if (xform_mode == XFORM_ROTATE) {
        x_angle += (x - press_x) / 10.0;
		if (x_angle > 180)
            x_angle -= 360;
		else if (x_angle < -180)
            x_angle += 360;
		press_x = x;

		y_angle += (y - press_y) / 10.0;
		if (y_angle > 180)
            y_angle -= 360;
		else if (y_angle < -180)
            y_angle += 360;
		press_y = y;
	}
	else if (xform_mode == XFORM_SCALE) {
		float old_size = scale_size;
		scale_size *= (1 + (y - press_y) / 600.0);
		if (scale_size < 0) scale_size = old_size;
		press_y = y;
	}
	glutPostRedisplay();
}

/* Handler for Keyboard event. When a user types into the window, each key
   press generating an ASCII character will generate a keyboard callback. */
void keyFunc(unsigned char key, int x, int y){

	if (key == 'a' || key=='A') {
		x_angle = x_angle + 2;          // Rotates around the clock horizontally to the left
	}
	if (key == 'd' || key=='D') {
		x_angle = x_angle - 2;          // Rotates around the clock horizontally to the right
	}
	if (key == 'w' || key=='W') {
         scale_size = scale_size + 0.05;  // Scales up into the clock
	}
	if (key == 's' || key=='S') {
        scale_size = scale_size - 0.05; // Scales down out of the clock
	}
	glutPostRedisplay();        // Marks the current window as needing to be redisplayed
}

/* Handler for Special Keyboard event. Called back when
   keyboard function or directional keys are pressed. */
void specialKeys(int key, int x, int y){
    switch(key)
    {
        case GLUT_KEY_UP:
            scale_size = scale_size + 0.05;     // Scales up into the clock
            break;
        case GLUT_KEY_DOWN:
            scale_size = scale_size - 0.05;     // Scales down out of the clock
            break;
        case GLUT_KEY_LEFT:
            x_angle = x_angle + 2;             // Rotates around the clock horizontally to the left
            break;
        case GLUT_KEY_RIGHT:
            x_angle = x_angle - 2;             // Rotates around the clock horizontally to the right
            break;
    }
    glutPostRedisplay();       // Marks the current window as needing to be redisplayed
}

/* Handler for idle event. Called back when window system events are not being received.
   If enabled, the idle callback is continuously called when events are not being received.
   The callback routine has no parameters.*/
void myIdle() {

    GetLocalTime(&timeNow);  // Gets the system time

    // The movement of the bus
    if( m==0 )
    {
        mf=0; // Moves in
        m=m+1;
    }
    else if( m==400 )
    {
        mf=1; // Moves back
        m=m-1;
    }
    else if( 0<m && m<400 && mf==1 )
    {
        m=m-1;
    }
    else if( 0<m && m<400 && mf==0 )
    {
        m=m+1;
    }

	glutPostRedisplay(); // Redraws, recalls display(), change the value of variable
}

/* Handler for window-repaint event. Called back when the window first appears and
   whenever the window needs to be re-painted. */
void display(){
    glClearColor(0.4, 0.61, 0.97, 0); // Specifies clear values for the color buffers to get the blue sky
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Clears buffers to preset values to enable color writing and depth testing
    glEnable(GL_TEXTURE_2D);    // Enables texture server, two-dimensional texturing is performed
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // Texture and material mixing method

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-45.0, 20.0, 35.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0);  // Looks at the Upper left of the clock
	glRotatef(x_angle, 0, 1, 0);        // Rotates according to the parameter
	glRotatef(y_angle, 1, 0, 0);        // Rotates according to the parameter

	glScalef(scale_size, scale_size, scale_size);  // Scales according to the parameter
    glTranslatef(0, -90, 0);            // To put the clock at the (0,0,0) which enables us to view the clock easily

    // Calls the drawing functions to build our Big Ben with the surroundings
    drawFace();
    drawTower();
    drawBuild();
    Drawstreet();
	Stairs();

	//Let the sky environment change over time
    struct tm *ptr;
    time_t it;
    it=time(NULL);
    ptr=localtime(&it);
    // Renders dusk sky from 6PM to 7PM
	if(18<=ptr->tm_hour && ptr->tm_hour<19)
        {
            GLfloat ambientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor); // Sets lighting model parameters
            DuskSpace();
        }
    // Renders night sky from 7PM to 6AM
    else if( (19<=ptr->tm_hour&&ptr->tm_hour<24) || (0<=ptr->tm_hour&&ptr->tm_hour<6))
        {
            GLfloat ambientColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor); // Sets lighting model parameters
            NightSpace();
        }
    // Renders day sky at other times
    else
        {
            GLfloat ambientColor[] = { 0.4f, 0.4f, 0.4f, 1.0f };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor); // Sets lighting model parameters
            DayLand();
        }
	glFlush();          // Forces execution of GL commands in finite time
	glutSwapBuffers();             // Posts EGL surface color buffer to a native window
	glutPostRedisplay();           // Marks the current window as needing to be redisplayed
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height){
    // GLsizei for non-negative integer
   // Computes aspect ratio of the new window
   if (height == 0) height = 1;// To prevent dividing by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
   // Sets the view-port to cover the new window

   glViewport(0, 0, width, height);
   // Sets the aspect ratio of the clipping volume to match the view-port

   glMatrixMode(GL_PROJECTION);  // To operate on the Projection Matrix

   glLoadIdentity();             // Reset

   // Enables perspective projection with fovy, aspect, zNear and zFar, specifies a viewing frustum into the world coordinate system
   gluPerspective(60.0f, aspect, 1.0f, 900.0f);

   glMatrixMode(GL_MODELVIEW);  //To operate on the Translation Matrix
}


/**** Main function ****/
/* GLUT runs as a console application starting at main() */
int main(int argc, char** argv){
    glutInit(&argc, argv);            // Initializes GLUT
    glutInitDisplayMode(GLUT_RGB |    //
                        GLUT_DOUBLE | // Enables double buffered mode
                        GLUT_DEPTH);  //
    glutInitWindowSize(1280, 960);    // Sets the window's initial width & height
    glutInitWindowPosition(50, 50);   // Locates the window's initial top-left corner
	glutCreateWindow("The Big Ben");  // Creates window with the given title

	glutDisplayFunc(display);         // Registers callback handler for window re-paint event
	glutReshapeFunc(reshape);         // Registers callback handler for window re-size event

	glutMouseFunc(mymouse);           // Registers callback handler for mouse event
	glutMotionFunc(mymotion);         // Registers callback handler for motion event
	glutKeyboardFunc(keyFunc);        // Registers callback handler for keyboard event
	glutSpecialFunc(specialKeys);     // Registers callback handler for special keyboard event
    glutIdleFunc(&myIdle);            // Registers callback handler for Idle event

    //Some description will be shown in the console output
    printf("|***************** This program uses OpenGL to draw a clock on BigBen ******************|\n\n"
           "The description of the Program:\n"
           "    * This is a Group Project for CST305 09/2019 XMUM.\n"
           "    * Coded by CST1709315 RAO KEYI and CST1709482 ZHANG CHENYU.\n"
           "    * The surroundings are similar to British street style.\n"
           "    * There are three needles on the clock: second hand, minute hand and hour hand.\n"
           "    * The needles move accordingly to the computer's clock\n"
           "    * There are numbers and patterns on the clock in Roman numerals using the Texture feature.\n\n"
           "    * Note that the Sky environment changes over time");

    printf("The description of the Techniques:\n"
           "     1. Shading\n"
           "     2. Hidden surface removal\n"
           "     3. Texture Mapping\n"
           "     \n");

    printf("The interaction operations:\n"
           "    * Hold the left mouse button to rotate.\n"
           "    * Hold the right mouse button to scale.\n"
           "    * Hold 'a' key or 'A' key to rotate around the clock horizontally to the left.\n"
           "    * Hold 'd' key or 'D' key to rotate around the clock horizontally to the right.\n"
           "    * Hold 'w' key, 'W' key or 'UP' to scale up into the clock.\n"
           "    * Hold 's' key, 'S' key or 'DOWN' to scale down out of the clock.\n");
    printf("\nCopyright 2019, KAE&FRED, ALL Rights reserved.\n");

    init();                         // Our own OpenGL initialization
	glutMainLoop();                 // Enter the infinite event-processing loop
	return 0;
}
