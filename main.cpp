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
    bool gotPoint = false;
};

struct Triangle{
    Point p1;
    Point p2;
    Point p3;
    int numPoints = 0;
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
    DrawingMode *currMode;
    Color* currColor;
    Point* points;
    Line* lines;
    Triangle* triangles;
    Point hoverPoint;
    int PointSize = 5;
    size_t numPoints = 0;
    size_t maxPoints = MAX_POINTS;
    size_t numLines = 0;
    size_t maxLines = MAX_POINTS;
    size_t numTriangles = 0;
    size_t maxTriangles = MAX_POINTS;
};

Global globals;
void init(){
    globals.currMode = new DrawingMode(0,new DrawingMode(1, new DrawingMode(2, nullptr)));
    globals.currColor = new Color(1,1,1,new Color(1,0,0, new Color(0,1,0,new Color(0,0,1, nullptr))));
    globals.currColor->next->next->next->next = globals.currColor;
    globals.currMode->next->next->next = globals.currMode;
    globals.points = (Point*) malloc(sizeof(Point)*1024);
    globals.lines = (Line*) malloc(sizeof(Line)*1024);
    globals.triangles = (Triangle*) malloc(sizeof(Triangle)*1024);
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
            globals.numPoints = 0;
            globals.numTriangles = 0;
            globals.numLines = 0;
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
    glLineWidth(globals.PointSize);
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    for(int i=0; i<globals.numLines; i++){
        Line* curr = &globals.lines[i];
        glColor3f(curr->p1.color.red, curr->p1.color.green, curr->p1.color.blue);
        glVertex2d(curr->p1.xPos, curr->p1.yPos);
        glColor3f(curr->p2.color.red, curr->p2.color.green, curr->p2.color.blue);
        glVertex2d(curr->p2.xPos, curr->p2.yPos);
    }
    glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
    glEnd();

    glLineWidth(1);
    glBegin(GL_TRIANGLES);
    for(int i=0; i<globals.numTriangles; i++){
        Triangle *curr = &globals.triangles[i];
        glColor3f(curr->p1.color.red, curr->p1.color.green, curr->p1.color.blue);
        glVertex2d(curr->p1.xPos, curr->p1.yPos);
        glColor3f(curr->p2.color.red, curr->p2.color.green, curr->p2.color.blue);
        glVertex2d(curr->p2.xPos, curr->p2.yPos);
        glColor3f(curr->p3.color.red, curr->p3.color.green, curr->p3.color.blue);
        glVertex2d(curr->p3.xPos, curr->p3.yPos);
    }
    glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
    glEnd();

    if(globals.currMode->mode == LINE_MODE && globals.lines[globals.numLines].gotPoint){
        glBegin(GL_LINE_STRIP);
        Line* curr = &globals.lines[globals.numLines];
        glVertex2d(curr->p1.xPos,curr->p1.yPos);
        glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
        glEnd();
    }else if(globals.currMode->mode == TRIANGLE_MODE){
        Triangle* curr = &globals.triangles[globals.numTriangles];
        switch(curr->numPoints){
            case 1: {
                glBegin(GL_LINE_STRIP);
                glVertex2d(curr->p1.xPos,curr->p1.yPos);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
                glEnd();
                break;
            }

            case 2: {
                Triangle* curr = &globals.triangles[globals.numTriangles];
                glBegin(GL_LINES);
                glVertex2d(curr->p1.xPos, curr->p1.yPos);
                glVertex2d(curr->p2.xPos, curr->p2.yPos);
                glEnd();
                glBegin(GL_LINE_STRIP);
                glVertex2d(curr->p1.xPos,curr->p1.yPos);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
                glVertex2d(curr->p2.xPos, curr->p2.yPos);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
                glEnd();
            }
        }
    }

    glPointSize(globals.PointSize);
    glBegin(GL_POINTS);
    for(int i=0; i<globals.numPoints; i++) {
        glColor3f(globals.points[i].color.red,globals.points[i].color.blue,globals.points[i].color.green);
        glVertex2f(globals.points[i].xPos, globals.points[i].yPos);
    }
    if(globals.numPoints) {
//        glColor3f(globals.pointModePoints[globals.numPPoints - 1].color.red, globals.pointModePoints[globals.numPPoints - 1].color.blue,
//                  globals.pointModePoints[globals.numPPoints - 1].color.green);
        glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
    }
    glEnd();

    glutSwapBuffers();
}

void processMouse(int button, int state, int x, int y){

    float xPos = ((float) x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2);
    float yPos = -((float) y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2);
    globals.hoverPoint.xPos = xPos;
    globals.hoverPoint.yPos = yPos;

    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        switch (globals.currMode->mode) {

            case LINE_MODE: {
                if(globals.numLines == globals.maxLines){
                    globals.lines = (Line*) realloc((void*)globals.lines,sizeof(Line)*2*globals.maxLines);
                    globals.maxLines *= 2;
                }
                Line* curr = &globals.lines[globals.numLines];
                curr->size = globals.PointSize;
                Point* point = curr->gotPoint? &curr->p2: &curr->p1;
                point->xPos = xPos;
                point->yPos = yPos;
                curr->gotPoint = !curr->gotPoint;
                if(!curr->gotPoint)
                    globals.numLines++;
                //globals.lines[globals.numLines].color = {globals.currColor->red,globals.currColor->blue, globals.currColor->green};

                break;
            }

            case TRIANGLE_MODE: {
                if(globals.numTriangles == globals.maxTriangles){
                    globals.triangles = (Triangle*) realloc((void*)globals.triangles,sizeof(Triangle)*2*globals.maxTriangles);
                    globals.maxTriangles *= 2;
                }
                //globals.triangles[globals.numTriangles].size = globals.PointSize;
                //globals.lines[globals.numLines].color = {globals.currColor->red,globals.currColor->blue, globals.currColor->green};
                Triangle* curr = &globals.triangles[globals.numTriangles];
                Point* point = curr->numPoints==0? &curr->p1: curr->numPoints==1? &curr->p2: &curr->p3;
                point->xPos = xPos;
                point->yPos = yPos;
                curr->numPoints++;
                if(curr->numPoints == 3){
                    globals.numTriangles++;
                }
                break;
            }

            case POINT_MODE:
            default: {
                if(globals.numPoints == globals.maxPoints){
                    globals.points = (Point*) realloc((void*)globals.points,sizeof(Point)*2*globals.maxPoints);
                    globals.maxPoints *= 2;
                }
                globals.points[globals.numPoints].size = globals.PointSize;
                globals.points[globals.numPoints].color = {globals.currColor->red,globals.currColor->blue, globals.currColor->green};
                globals.points[globals.numPoints].xPos = xPos;
                globals.points[globals.numPoints++].yPos = yPos;
            }
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