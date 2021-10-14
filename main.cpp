#include <stdio.h>
#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

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
            memset((void*)globals.points,0,globals.numPoints*sizeof(Point));
            memset((void*)globals.lines,0,globals.numLines*sizeof(Line));
            memset((void*)globals.triangles,0,globals.numTriangles*sizeof(Triangle));
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

    for(int i=0; i<globals.numLines; i++){
        Line* curr = &globals.lines[i];
        glLineWidth(curr->size);
        glBegin(GL_LINES);
        glColor3f(curr->p1.color.red, curr->p1.color.green, curr->p1.color.blue);
        glVertex2d(curr->p1.xPos, curr->p1.yPos);
        glColor3f(curr->p2.color.red, curr->p2.color.green, curr->p2.color.blue);
        glVertex2d(curr->p2.xPos, curr->p2.yPos);
        glEnd();
    }


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
    glEnd();

    if(globals.currMode->mode == LINE_MODE && globals.lines[globals.numLines].gotPoint){
        Line* curr = &globals.lines[globals.numLines];
        glLineWidth(curr->size);
        glBegin(GL_LINE_STRIP);
        glColor3f(curr->p1.color.red, curr->p1.color.green, curr->p1.color.blue);
        glVertex2d(curr->p1.xPos,curr->p1.yPos);
        glColor3f(globals.hoverPoint.color.red, globals.hoverPoint.color.green, globals.hoverPoint.color.blue);
        glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
        glEnd();
    }else if(globals.currMode->mode == TRIANGLE_MODE){
        Triangle* curr = &globals.triangles[globals.numTriangles];
        switch(curr->numPoints){
            case 1: {
                glBegin(GL_LINE_STRIP);
                glLineWidth(globals.PointSize);
                glVertex2d(curr->p1.xPos,curr->p1.yPos);
                glColor3f(globals.hoverPoint.color.red, globals.hoverPoint.color.green, globals.hoverPoint.color.blue);
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
                glColor3f(globals.hoverPoint.color.red, globals.hoverPoint.color.green, globals.hoverPoint.color.blue);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
                glVertex2d(curr->p2.xPos, curr->p2.yPos);
                glColor3f(globals.hoverPoint.color.red, globals.hoverPoint.color.green, globals.hoverPoint.color.blue);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
                glEnd();
            }
        }
    }



    for(int i=0; i<globals.numPoints; i++) {
        Point* curr = &globals.points[i];
        int size = globals.PointSize;
        glPointSize(curr->size);
        glBegin(GL_POINTS);
        glColor3f(curr->color.red,curr->color.blue,curr->color.green);
        glVertex2f(curr->xPos, curr->yPos);
        glEnd();
    }

    switch (globals.currMode->mode) {
        case LINE_MODE: {
            if(globals.lines[globals.numLines].gotPoint) {
                Point *curr = &globals.hoverPoint;
                int size = globals.lines[globals.numLines].size;
                glColor3f(curr->color.red, curr->color.green, curr->color.blue);
                float scale = 0.005;
                glBegin(GL_QUADS);
                glVertex2f((curr->xPos - size)*scale, (curr->yPos + size)*scale);
                glVertex2f((curr->xPos + size)*scale, (curr->yPos+size)*scale);
                glVertex2f((curr->xPos-size)*scale, (curr->yPos - size)*scale);
                glVertex2f((curr->xPos+size)*scale, (curr->yPos-size)*scale);
                glEnd();
            }
        }

        case TRIANGLE_MODE: {
            if(globals.triangles[globals.numTriangles].numPoints > 0) {
                Point *curr = &globals.hoverPoint;
                float size = (float)globals.PointSize/ (WINDOW_WIDTH / 2);
                glColor3f(curr->color.red, curr->color.green, curr->color.blue);
                float scale = 0.005;
                glBegin(GL_TRIANGLES);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos+size);
                glVertex2f(globals.hoverPoint.xPos-size, globals.hoverPoint.yPos-size);
                glVertex2f(globals.hoverPoint.xPos+size, globals.hoverPoint.yPos-size);
                glEnd();
            }
            break;
        }
        case POINT_MODE:
        default:{
            if(globals.numPoints){
                glPointSize(globals.PointSize);
                glBegin(GL_POINTS);
                glColor3f(globals.hoverPoint.color.red,globals.hoverPoint.color.blue,globals.hoverPoint.color.green);
                glVertex2f(globals.hoverPoint.xPos, globals.hoverPoint.yPos);
                glEnd();
            }
        }
    }
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
                Point* point = curr->gotPoint? &curr->p2: &curr->p1;
                point->xPos = xPos;
                point->yPos = yPos;
                point->color = {globals.hoverPoint.color.red,globals.hoverPoint.color.green,globals.hoverPoint.color.blue};
                curr->gotPoint = !curr->gotPoint;
                if(!curr->gotPoint) {
                    globals.numLines++;
                    globals.currColor = globals.currColor->next;
                }else {
                    curr->size = globals.PointSize;
                }

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
                point->color = {globals.hoverPoint.color.red,globals.hoverPoint.color.green,globals.hoverPoint.color.blue};
                curr->numPoints++;
                if(curr->numPoints == 3){
                    globals.numTriangles++;
                    globals.currColor = globals.currColor->next;
                }
                break;
            }

            case POINT_MODE:
            default: {
                if(globals.numPoints == globals.maxPoints){
                    globals.points = (Point*) realloc((void*)globals.points,sizeof(Point)*2*globals.maxPoints);
                    globals.maxPoints *= 2;
                }
                Point* curr = &globals.points[globals.numPoints];
                curr->size = globals.PointSize;
                curr->color = {globals.currColor->red,globals.currColor->blue, globals.currColor->green};
                curr->xPos = xPos;
                curr->yPos = yPos;
                globals.numPoints++;
            }
        }
    }



    glutPostRedisplay();
}

void processMouseHover(int x, int y){

    globals.hoverPoint.xPos = ((float) x - (WINDOW_WIDTH / 2)) / (WINDOW_WIDTH / 2);
    globals.hoverPoint.yPos = -((float) y - (WINDOW_HEIGHT / 2)) / (WINDOW_HEIGHT / 2);
    switch (globals.currMode->mode) {
        case LINE_MODE: {
            Line* curr = &globals.lines[globals.numLines];
            if(curr->gotPoint){
                Color* color = globals.currColor->next;
                globals.hoverPoint.color = {color->red, color->green, color->blue};
            }
            break;
        }

        case TRIANGLE_MODE: {
            Triangle* curr = &globals.triangles[globals.numTriangles];
            if(curr->numPoints > 0){
                Color* color = globals.currColor->next;
                globals.hoverPoint.color = {color->red, color->green, color->blue};
            }
            break;
        }
        case POINT_MODE:
        default:{
            globals.hoverPoint.color = {globals.currColor->red, globals.currColor->green, globals.currColor->blue};
        }
    }
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