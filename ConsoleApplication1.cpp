// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"

#include "mesh.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

const int windowWidth = 800;
const int windowHeight = 600;

Mesh mesh;

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_NORMALIZE);
	glColor3f(1, 1, 1);

	mesh.drawMesh();
	//mesh.refine();

	glutSwapBuffers();
	glutPostRedisplay();
}

void myInit()
{
	glClearColor(0, 0, 0, 0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, windowWidth / windowHeight, 1.0, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3, 2.5, 2, 0, 0, 0, 0, 1, 0);

	mesh.readFile("2.obj");
	mesh.generateIndex();
	//mesh.refine();
}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("DEMO");

	myInit();
	glutDisplayFunc(myDisplay);

	glutMainLoop();
	return 0;
}
