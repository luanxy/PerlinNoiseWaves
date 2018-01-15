#include "stdafx.h"

#include "perlin.h"
#include "vec3.h"

#include <GL/glut.h>
#include <iostream>
#include <math.h>


float t = 0;

// frame index
int g_frameIndex = 0;

void initRendering(){

    glEnable(GL_DEPTH_TEST);
}

void reshape(int w,int h){

    glViewport(0,0,w,h);
    
    glMatrixMode(GL_PROJECTION);
    
    glLoadIdentity();

	glOrtho(0, 1.5, -1.5, 1.5, -10, 10);
    
    gluPerspective(90, (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);

	
    
}

void keyPressed(int key,int x,int y){
}

vec3 grad(vec3 p) {
	return p.normalize();
}

// s-shaped curve for 0 <= t <= 1
float fade(float t) {
	return t*t*t*(t*(t*6.0 - 15.0) + 10.0);
}

// Perlin noise
float noise(vec3 p) {
	// calculate lattice points
	vec3 p0 = p.floorvec3();
	vec3 p1 = p0 + vec3(1.0, 0.0, 0.0);
	vec3 p2 = p0 + vec3(0.0, 1.0, 0.0);
	vec3 p3 = p0 + vec3(1.0, 1.0, 0.0);
	vec3 p4 = p0 + vec3(0.0, 0.0, 1.0);
	vec3 p5 = p4 + vec3(1.0, 0.0, 0.0);
	vec3 p6 = p4 + vec3(0.0, 1.0, 0.0);
	vec3 p7 = p4 + vec3(1.0, 1.0, 0.0);

	// look up gradients at lattice points
	vec3 g0 = grad(p0);
	vec3 g1 = grad(p1);
	vec3 g2 = grad(p2);
	vec3 g3 = grad(p3);
	vec3 g4 = grad(p4);
	vec3 g5 = grad(p5);
	vec3 g6 = grad(p6);
	vec3 g7 = grad(p7);

	float t0 = p.x - p0.x;
	// used for interpolation in horizontal direction
	float fade_t0 = fade(t0);

	float t1 = p.y - p0.y;
	// used for interpolation in vertical direction
	float fade_t1 = fade(t1); 

	float t2 = p.z - p0.z;
	float fade_t2 = fade(t2);

	// calculate dot products and interpolate
	float p0p1 = (1.0 - fade_t0) * g0.dot(p - p0) + fade_t0 * g1.dot(p - p1); 
	float p2p3 = (1.0 - fade_t0) * g2.dot(p - p2) + fade_t0 * g3.dot(p - p3); 

	float p4p5 = (1.0 - fade_t0) * g4.dot(p - p4) + g5.dot(p - p5); 
	float p6p7 = (1.0 - fade_t0) * g6.dot(p - p6) + g7.dot(p - p7); 

	float y1 = (1.0 - fade_t1) * p0p1 + fade_t1 * p2p3;
	float y2 = (1.0 - fade_t1) * p4p5 + fade_t1 * p6p7;

	return (1.0 - fade_t2) * y1 + fade_t2 * y2;
}

void drawParticles(float x, float y, float z){
    glPushMatrix();
    glTranslatef(x,y,z);
    glutSolidSphere(0.005,5,6);
    glPopMatrix();
}

void update(){
	t = t + 0.05;

	
}

void display(){

	// clear color and depth buffer
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// enable depth test
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// light source attributes
	GLfloat LightAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightDiffuse[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat LightSpecular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, 5.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	// surface material attributes
	GLfloat material_Ka[] = { 0.11f, 0.06f, 0.11f, 1.0f };
	GLfloat material_Kd[] = { 0.8f, 0.0f, 0.8f, 1.0f };
	GLfloat material_Ks[] = { 0.73f, 0.33f, 0.52f, 1.0f };
	GLfloat material_Ke[] = { 0.1f , 0.0f , 0.1f , 1.0f };
	GLfloat material_Se = 10;

	glMaterialfv(GL_FRONT, GL_AMBIENT, material_Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_Ks);
	glMaterialfv(GL_FRONT, GL_EMISSION, material_Ke);
	glMaterialf(GL_FRONT, GL_SHININESS, material_Se);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
	for (float i = 0; i < 2.0; i=i+0.01) {
		for (float j = -2.0; j < 0.0; j=j+0.01) {
			vec3 v1(i,t,j);
			float level1 = noise(v1) / 2.0;
			vec3 v2(i, t*0.5, j);
			float level2 = noise(v2) / 8.0;
			vec3 v3(i, t*0.25, j);
			float level3 = noise(v3) / 16.0;
			float n = level1 + level2 + level3;
			drawParticles(i, n, j);
		}
	}

	update();
    
    glutSwapBuffers();
}

void timer(int value) {
	// increase frame index
	g_frameIndex++;

	// render
	glutPostRedisplay();

	// reset timer
	glutTimerFunc(360, timer, 0);
}

int main(int argc,char **argv){

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(600,600);
	glutInitWindowPosition(100, 100);
    glutCreateWindow("Lab5");
    
    initRendering();
    
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(keyPressed);
	glutTimerFunc(360, timer, 0);

    glutMainLoop();
    
    return(0);
}
