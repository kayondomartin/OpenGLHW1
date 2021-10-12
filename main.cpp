#include <stdio.h>
#include <GL/glut.h>
#include <iostream>

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600
#define WINDOW_XPOS     100
#define WINDOW_YPOS     100
#define MAX_POINTS      1024
#define POINT_MODE      0
#define LINE_MODE       1
#define TRIANGLE_MODE   2

struct Point{
    float xPos;
    float yPos;
    float size;
    struct Color {
        int red, blue, green;
    } color;
};

struct Color{
    int red;
    int blue;
    int green;
    Color* next;
    Color(int r, int b,int g,Color* n){
        red = r;
        blue = b;
        green = g;
        next = n;
    }
};

struct Line{
    Point p1;
    Point p2;
    float size;
};

struct Triangle{
    Point p1;
    Point p2;
    Point p3;
};
struct DrawingMode{
    int mode;
    DrawingMode* next;
    DrawingMode(){
        mode = 0;
        next = nullptr;
    }
    DrawingMode(int m, DrawingMode* n){
        mode = m;
        next = n;
    }
};
struct Global{
    DrawingMode *currMode; ///0 => Point mode, 1 => Line mode, 2 => Triangle mode
    Color* currColor;
    Point* pointModePoints;
    Point* lineModePoints;
    Point* triangleModePoints;
    Point hoverPoint;
    int PointSize = 5;
    size_t numPPoints = 0;
    size_t maxPPoints = MAX_POINTS;
    size_t numLPoints = 0;
    size_t maxLPoints = MAX_POINTS;
    size_t numTPoints = 0;
    size_t maxTPoints = MAX_POINTS;
    bool lineComplete = false;
    bool lineBegin = false;
    int PointIdxBegin = 0;
    int LineIdxBegin = 0;
};

Global globals;
void init(){
    globals.currMode = new DrawingMode(0,new DrawingMode(1, new DrawingMode(2, nullptr)));
    globals.currColor = new Color(1,1,1,new Color(1,0,0, new Color(0,1,0,new Color(0,0,1, nullptr))));
    globals.currColor->next->next->next->next = globals.currColor;
    globals.currMode->next->next->next = globals.currMode;
    globals.pointModePoints = (Point*) malloc(sizeof(Point)*1024);
    globals.lineModePoints = (Point*) malloc(sizeof(Point)*1024);
    globals.triangleModePoints = (Point*) malloc(sizeof(Point)*1024);
}

void normalKeyPresCallback(unsigned char key, int x, int y){
    switch(key){
        case '1': {
            globals.currMode = globals.currMode->next;
            break;
        }

        case '2': {
            globals.currColor = globals.currColor->next;
            break;
        }

        case 'c': {
            ///clear scene
            break;
        }

        case '+': {
            globals.PointSize += 1;
            if(globals.PointSize > 10)
                globals.PointSize = 10;
            break;
        }

        case '-': {
            globals.PointSize -= 1;
            if(globals.PointSize < 1)
                globals.PointSize = 1;
            break;
        }
    }
}

void renderScene(){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    switch (globals.currMode->mode) {
        case LINE_MODE: {
            if(globals.lineBegin) {
                glLineWidth(globals.PointSize);
                glColor3f(1, 1, 1);
                glBegin(GL_LINE_STRIP);

                for (int i = 0; i < globals.numLPoints; ++i) {
                    glVertex2f(globals.lineModePoints[i].xPos, globals.lineModePoints[i].yPos);
                }
                if (globals.numLPoints) {
                    glColor3f(globals.lineModePoints[globals.numLPoints - 1].color.red,
                              globals.lineModePoints[globals.numLPoints - 1].color.blue,
                              globals.lineModePoints[globals.numLPoints - 1].color.green);
                    glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
                }
                glEnd();
            }
            break;
        }

        case TRIANGLE_MODE: {

            break;
        }

        case POINT_MODE:
        default: {
            glPointSize(globals.PointSize);
            glBegin(GL_POINTS);
            for(int i=0; i<globals.numPPoints; i++) {
                glColor3f(globals.pointModePoints[i].color.red,globals.pointModePoints[i].color.blue,globals.pointModePoints[i].color.green);
                glVertex2f(globals.pointModePoints[i].xPos, globals.pointModePoints[i].yPos);
            }
            if(globals.numPPoints) {
                glColor3f(globals.pointModePoints[globals.numPPoints - 1].color.red, globals.pointModePoints[globals.numPPoints - 1].color.blue,
                          globals.pointModePoints[globals.numPPoints - 1].color.green);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
            }
            glEnd();
        }
    }

    glutSwapBuffers();
}

void processMouse(int button, int state, int x, int y){

    globals.hoverPoint.xPos = ((float) x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2);
    globals.hoverPoint.yPos = -((float) y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2);

    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if(globals.numPoints == globals.maxPoints){
            globals.points = (Point*) realloc((void*)globals.points,sizeof(Point)*2*globals.maxPoints);
            globals.maxPoints *= 2;
        }
        globals.points[globals.numPoints].size = globals.PointSize;
        globals.points[globals.numPoints].color = {globals.currColor->red,globals.currColor->blue, globals.currColor->green};
        globals.points[globals.numPoints].xPos = ((float) x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2);
        globals.points[globals.numPoints++].yPos = -((float) y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2);

        if(globals.lineBegin){
            globals.lineComplete = true;
            globals.lineBegin = false;
        }else{
            globals.lineBegin = true;
            globals.lineComplete = false;
        }
    }



    glutPostRedisplay();
}

void processMouseHover(int x, int y){
    globals.hoverPoint.xPos = ((float) x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2);
    globals.hoverPoint.yPos = -((float) y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2);
    glutPostRedisplay();
}


int main(int argc, char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowPosition(WINDOW_XPOS,WINDOW_YPOS);
    glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    glutCreateWindow("OpenGL HW1");
    init();
    glutDisplayFunc(renderScene);
    glutMouseFunc(processMouse);
    glutKeyboardFunc(normalKeyPresCallback);
    glutPassiveMotionFunc(processMouseHover);
    glutMainLoop();
}