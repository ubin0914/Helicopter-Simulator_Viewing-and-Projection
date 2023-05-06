#include <iostream>
#include <math.h>
#include <vector>
#include <chrono>
#include <ctime>
#include <GL/glut.h>

#include "PublicValue.h"
#include "Helicopter.h"
#include "Environment.h"

using namespace std;
using namespace std::chrono;

/*-----Define GLU quadric objects, a sphere and a cylinder----*/
//GLUquadricObj* sphere = NULL, * cylind = NULL;

Helicopter helicopter = *(new Helicopter());
Environment environment;

Coordinate lookFrom;
Coordinate lookTo;
Coordinate monitorFrom = { 0, 10, 20 };

void drawAxes() {
    //世界座標x、z軸
    glPolygonMode(GL_FRONT, GL_FILL);
    glPushMatrix();
        glTranslatef(0, 461.78, 0);
        glPushMatrix();
            glScaled(10, 0.2, 1);
            glTranslatef(0, 0, -0.5);
            for each (Face face in cubeFace)
                face.drawAsCustomHSLInc(cubeVertex, 0, 1, 0.7);
        glPopMatrix();
        glRotatef(-90, 0, 1, 0);
        glPushMatrix();
            glScaled(10, 0.2, 1);
            glTranslatef(0, 0, -0.5);
            for each (Face face in cubeFace)
                face.drawAsCustomHSLInc(cubeVertex, 240, 1, 0.7);
        glPopMatrix();
    glPopMatrix();
    //世界座標y軸
    glColor3f(0, 1, 0);
    glPushMatrix();
        glTranslatef(0, 462, 0);
        glRotatef(-90, 1, 0, 0);
        GLUquadricObj* yAxis = gluNewQuadric();
        gluQuadricDrawStyle(yAxis, GLU_FILL);
        gluQuadricNormals(yAxis, GLU_SMOOTH);
        gluCylinder(yAxis, 0.2, 0.2, 47, 24, 3);
    glPopMatrix();
    //眼睛座標三軸
    Coordinate eyePos = monitorFrom + (focusMode ? helicopter.origin_r : Coordinate(0, 462, 0));
    glPushMatrix();
        glTranslatef(eyePos.x, eyePos.y, eyePos.z);
        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(10 * eyeAxisX.x, 10 * eyeAxisX.y, 10 * eyeAxisX.z);
        glColor3f(0, 0.6, 0);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(10 * eyeAxisY.x, 10 * eyeAxisY.y, 10 * eyeAxisY.z);
        glColor3f(0, 0, 1);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(10 * eyeAxisZ.x, 10 * eyeAxisZ.y, 10 * eyeAxisZ.z);
        glEnd();
    glPopMatrix();
}
void drawAuxiliaryLines(int m) {
    Coordinate eyePos = monitorFrom + (focusMode ? helicopter.origin_r : Coordinate(0, 462, 0));
    glPushMatrix();
        glTranslatef(eyePos.x, eyePos.y, eyePos.z);
        float nearHeight = tan(60 / monitorMagnification[0] * PI / 180) * monitorNear;
        float nearWidth = (float)width / height * nearHeight;
        float farWidth = monitorFar / monitorNear * nearWidth;
        float farHeight = monitorFar / monitorNear * nearHeight;
        vector<Coordinate> volumeVertex = {
            {nearWidth, -nearHeight, -monitorNear},
            {-nearWidth, -nearHeight, -monitorNear},
            {-farWidth, -farHeight, -monitorFar},
            {farWidth, -farHeight, -monitorFar},
            {nearWidth, nearHeight, -monitorNear},
            {-nearWidth, nearHeight, -monitorNear},
            {-farWidth, farHeight, -monitorFar},
            {farWidth, farHeight, -monitorFar}
        };
        vector<Face> volumeFace = {
            { {0, 4, 5, 1}, {0, 0, 0, 0.3} },
            { {3, 2, 6, 7}, {0, 0, 0, 0.3} },
            { {0, 1, 2, 3}, {1, 0, 0.0, 0.3} },
            { {7, 6, 5, 4}, {1, 0, 0.0, 0.3} },
            { {0, 3, 7, 4}, {1, 1, 0.0, 0.3} },
            { {1, 5, 6, 2}, {1, 1, 0.0, 0.3} }
        };        
        glPushMatrix();
            rotateSysTo(eyeAxisX, eyeAxisY);
            resetLightIncidence();
            glColor3f(0, 0, 0);
            glBegin(GL_LINES);
            glVertex3f(0, 0, 0); glVertex3f(farWidth, farHeight, -monitorFar);
            glVertex3f(0, 0, 0); glVertex3f(-farWidth, farHeight, -monitorFar);
            glVertex3f(0, 0, 0); glVertex3f(-farWidth, -farHeight, -monitorFar);
            glVertex3f(0, 0, 0); glVertex3f(farWidth, -farHeight, -monitorFar);
            glVertex3f(-nearWidth, -nearHeight, -monitorNear);
            glVertex3f(nearWidth, -nearHeight, -monitorNear);
            glVertex3f(nearWidth, nearHeight, -monitorNear);
            glVertex3f(-nearWidth, nearHeight, -monitorNear);
            glEnd();

            glPolygonMode(GL_FRONT, GL_LINE);
            glPolygonMode(GL_BACK, GL_LINE);
            glBegin(GL_POLYGON);
            glVertex3f(-nearWidth, -nearHeight, -monitorNear);
            glVertex3f(nearWidth, -nearHeight, -monitorNear);
            glVertex3f(nearWidth, nearHeight, -monitorNear);
            glVertex3f(-nearWidth, nearHeight, -monitorNear);
            glEnd();
            glBegin(GL_POLYGON);
            glVertex3f(-farWidth, -farHeight, -monitorFar);
            glVertex3f(farWidth, -farHeight, -monitorFar);
            glVertex3f(farWidth, farHeight, -monitorFar);
            glVertex3f(-farWidth, farHeight, -monitorFar);
            glEnd();
            glPolygonMode(GL_FRONT, GL_FILL);
            glPolygonMode(GL_BACK, GL_FILL);

            vector<int> frontSet, backSet;
            Coordinate normal[6];
            float innProdValue[6];
            normal[0] = { 0, 0, 1 };
            normal[1] = { 0, 0, -1 };
            for (int i = 2; i < 6; i++)
                normal[i] = outerProduct(volumeVertex[volumeFace[i].order[1]] - volumeVertex[volumeFace[i].order[0]],
                                         volumeVertex[volumeFace[i].order[2]] - volumeVertex[volumeFace[i].order[1]]);
            if (m == 1)
                for (int i = 0; i < 6; i++)
                    innProdValue[i] = normal[i].x * eyeAxisX.x + normal[i].y * eyeAxisY.x + normal[i].z * eyeAxisZ.x;
            else if(m==2)
                for (int i = 0; i < 6; i++)
                    innProdValue[i] = normal[i].x * eyeAxisX.y + normal[i].y * eyeAxisY.y + normal[i].z * eyeAxisZ.y;
            else if(m==3)
                for (int i = 0; i < 6; i++)
                    innProdValue[i] = normal[i].x * eyeAxisX.z + normal[i].y * eyeAxisY.z + normal[i].z * eyeAxisZ.z;
            for (int i = 0; i < 6; i++) {
                if(innProdValue[i] > 0) frontSet.push_back(i);
                else backSet.push_back(i);
            }
            for (auto order : backSet) volumeFace[order].drawAsRGB(volumeVertex);
            for (auto order : frontSet) volumeFace[order].drawAsRGB(volumeVertex);
        glPopMatrix();
        
    glPopMatrix();
}

void display() {
    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (viewSysMode == 0) {
        /*----Define the current eye position and the eye-coordinate system---*/
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, width, height);

        if (focusMode == 1) {
            Coordinate& levelProject = sightMode == 3 ? Coordinate(helicopter.origin_r.x - lookFrom.x, 0, helicopter.origin_r.z - lookFrom.z) : Coordinate(helicopter.axisX_r.x, 0, helicopter.axisX_r.z);
            if (sightMode == 1) {
                sightLongitude = includedAngle({ 1,0,0 }, levelProject, { 0,1,0 });
                sightLatitude = includedAngle(helicopter.axisX_r, levelProject) * (helicopter.axisX_r.y >= 0 ? 1 : -1);
            }
            else if (sightMode == 2) {
                sightLongitude = includedAngle({ 1,0,0 }, levelProject, { 0,1,0 });
                sightLatitude = includedAngle(helicopter.axisX_r, levelProject) * (helicopter.axisX_r.y >= 0 ? 1 : -1);
            }
            else if (sightMode == 3) {
                sightLongitude = includedAngle({ 1,0,0 }, levelProject, { 0,1,0 });
                sightLatitude = includedAngle(helicopter.origin_r - lookFrom, levelProject) * (helicopter.origin_r.y >= lookFrom.y ? 1 : -1);
            }
        }
        Coordinate sight = { 1, 0, 0 };
        rotateDir(sight, { 0,1,0 }, sightLongitude);
        rotateDir(sight, outerProduct(sight, { 0,1,0 }), sightLatitude);
        if (sightMode == 1) {
            lookFrom = { helicopter.origin_r.x, helicopter.origin_r.y, helicopter.origin_r.z };
            lookTo = lookFrom + sight;
        }
        else if (sightMode == 2) {
            lookTo = { helicopter.origin_r.x, helicopter.origin_r.y, helicopter.origin_r.z };
            lookFrom = lookTo - lensDistance * sight;
            if (lookFrom.y < 0.1) lookFrom.y = 0.1;
            if (spKeyPressing[R_SHIFT_INT]) {
                viewMagnification *= 1.01;
                if (viewMagnification > 8) viewMagnification = 8;
            }
            if (spKeyPressing[R_CTRL_INT]) {
                viewMagnification /= 1.01;
                if (viewMagnification < 1) viewMagnification = 1;
            }
        }
        else if (sightMode == 3) {
            if (spKeyPressing[UPKEY_INT]) lookFrom.y += 1;
            if (spKeyPressing[DOWNKEY_INT]) lookFrom.y -= 1;
            if (spKeyPressing[LEFTKEY_INT]) lookFrom -= outerProduct(sight, { 0,1,0 }).identity() * 1;
            if (spKeyPressing[RIGHTKEY_INT]) lookFrom += outerProduct(sight, { 0,1,0 }).identity() * 1;
            if (spKeyPressing[R_SHIFT_INT]) lookFrom += sight.identity() * 1;
            if (spKeyPressing[R_CTRL_INT]) lookFrom -= sight.identity() * 1;
            lookTo = lookFrom + sight;
        }

        glFrustum(-1, 1, -1, 1, 10 * viewMagnification, 1.0);
        gluLookAt(lookFrom.x, lookFrom.y, lookFrom.z, lookTo.x, lookTo.y, lookTo.z, 0.0, 1.0, 0.0);

        helicopter.display();
        environment.updateBlock(floor(helicopter.origin_r.x / 110 + 0.5), floor(helicopter.origin_r.z / 110 + 0.5));
        environment.display();
    }
    else {
        if (monitorOperateMode) {
            if (spKeyPressing[UPKEY_INT]) rotateDualAxis(eyeAxisY, eyeAxisZ, cos_1deg, sin_1deg);
            if (spKeyPressing[DOWNKEY_INT]) rotateDualAxis(eyeAxisZ, eyeAxisY, cos_1deg, sin_1deg);
            if (spKeyPressing[LEFTKEY_INT]) rotateDualAxis(eyeAxisZ, eyeAxisX, cos_1deg, sin_1deg);
            if (spKeyPressing[RIGHTKEY_INT]) rotateDualAxis(eyeAxisX, eyeAxisZ, cos_1deg, sin_1deg);
            if (spKeyPressing[R_SHIFT_INT]) rotateDualAxis(eyeAxisY, eyeAxisX, cos_1deg, sin_1deg);
            if (spKeyPressing[R_CTRL_INT]) rotateDualAxis(eyeAxisX, eyeAxisY, cos_1deg, sin_1deg);
        }
        else {
            if (spKeyPressing[UPKEY_INT]) monitorFrom += eyeAxisY;
            if (spKeyPressing[DOWNKEY_INT]) monitorFrom -= eyeAxisY;
            if (spKeyPressing[LEFTKEY_INT]) monitorFrom -= eyeAxisX;
            if (spKeyPressing[RIGHTKEY_INT]) monitorFrom += eyeAxisX;
            if (spKeyPressing[R_SHIFT_INT]) monitorFrom -= eyeAxisZ;
            if (spKeyPressing[R_CTRL_INT]) monitorFrom += eyeAxisZ;
        }

        Coordinate focusOffset = (focusMode ? helicopter.origin_r : Coordinate(0, 462, 0));
        if (monitorMode == 0 || monitorMode == 4) {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(monitorFrom.x + focusOffset.x, monitorFrom.y + focusOffset.y, monitorFrom.z + focusOffset.z,
                monitorFrom.x + focusOffset.x - eyeAxisZ.x, monitorFrom.y + focusOffset.y - eyeAxisZ.y, monitorFrom.z + focusOffset.z - eyeAxisZ.z,
                eyeAxisY.x, eyeAxisY.y, eyeAxisY.z);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(120 / monitorMagnification[0], (double)width / (double)height, monitorNear, monitorFar);
            glMatrixMode(GL_MODELVIEW);
            if(monitorMode == 4) glViewport(width / 2, 0, width / 2, height / 2);
            else glViewport(0, 0, width, height);

            helicopter.display();
            environment.updateBlock(floor(helicopter.origin_r.x / 110 + 0.5), floor(helicopter.origin_r.z / 110 + 0.5));
            environment.display();
            drawAxes();
        }
        for(int m = 1; m <= 3; m++) {
            if (monitorMode != m && monitorMode != 4) continue;
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt((m == 1) * 100 + focusOffset.x, (m == 2) * 100 + focusOffset.y, (m == 3) * 100 + focusOffset.z,
                focusOffset.x, focusOffset.y, focusOffset.z,
                (m == 2), (m != 2), 0);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-60 / monitorMagnification[m], 60 / monitorMagnification[m], -60 / monitorMagnification[m] * (float)height / (float)width,
                60 / monitorMagnification[m] * (float)height / (float)width,
                -0.0, 10000.0);
            glMatrixMode(GL_MODELVIEW);
            if (monitorMode == 4) glViewport(!(m % 2) * width / 2, (m / 2) * height / 2, width / 2, height / 2);
            else glViewport(0, 0, width, height);

            helicopter.display();
            environment.updateBlock(floor(helicopter.origin_r.x / 110 + 0.5), floor(helicopter.origin_r.z / 110 + 0.5));
            environment.display();
            drawAxes();
            drawAuxiliaryLines(m);
        }
    }

    glutSwapBuffers();
    glFlush();
}

void myinit() {
    glClearColor(0.59, 0.79, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, 499, 499);

    /*-----Set a parallel projection mode-----*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15.0, 15.0, -15.0, 15.0, -10.5, 30.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_FLAT);

    glFlush();/*Enforce window system display the results*/
}

void my_reshape(int w, int h) {
    width = w;
    height = h;
    
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w > h)
        glOrtho(-15.0, 15.0, -15.0 * (float)h / (float)w, 15.0 * (float)h / (float)w,
            -10.5, 30.0);
    else
        glOrtho(-15.0 * (float)w / (float)h, 15.0 * (float)w / (float)h, -15.0, 15.0,
            -10.5, 30.0);
}

void keyboard_down(unsigned char key, int x, int y) {
    if (key == 'w') {
        if (lastSysTime - lastTimeSingleW < 250) helicopter.tiltMode = 2;
        lastTimeSingleW = lastSysTime;
    }
    if (key == ' ') {
        if (lastSysTime - lastTimeSingleSpace < 250) helicopter.flightMode = 1;
        lastTimeSingleSpace = lastSysTime;
    }
    if (key == 'v') {
        if (viewSysMode == 1) viewSysMode = 0;
        else {
            sightMode = sightMode % 3 + 1;
            focusMode = sightMode == 3;
            if (sightMode == 2) viewMagnification = 1.5;
        }
    }
    if (key == 'm') {
        if (viewSysMode == 0) viewSysMode = 1;
        else monitorMode = (monitorMode + 1) % 5;
    }
    if (key == 'f') focusMode = !focusMode;
    if (key == 'r') monitorOperateMode = !monitorOperateMode;
    if (key == '[') {
        blockNum -= 2;
        if(blockNum < 3) blockNum = 3;
        environment.initialBuildings();
    }
    if (key == ']') {
        blockNum += 2;
        if (blockNum > 25) blockNum = 25;
        environment.initialBuildings();
    }
    keyPressing[key] = 1;
    if (key >= 'A' && key <= 'Z') keyPressing[key + 32] = 1;
    if (keyPressing['z']) keyPressing['q'] = 1;
    if (keyPressing['c']) keyPressing['e'] = 1;
}

void keyboard_up(unsigned char key, int x, int y) {
    keyPressing[key] = 0;
    if (key >= 'A' && key <= 'Z') keyPressing[key + 32] = 0;
    if (!keyPressing['z']) keyPressing['q'] = 0;
    if (!keyPressing['c']) keyPressing['e'] = 0;
}

void spKeyboard_down(int key, int x, int y) {
    spKeyPressing[key] = 1;
    //cout << key << "\n";
    if (key == L_SHIFT_INT) {
        helicopter.tiltMode = 1;
    }
    if (key == L_CTRL_INT) {
        if (lastSysTime - lastTimeSingleLCtrl < 250) helicopter.flightMode = 0;
        lastTimeSingleLCtrl = lastSysTime;
    }
}

void spKeyboard_up(int key, int x, int y) {
    spKeyPressing[key] = 0;
}

void mouse_func(int button, int state, int x, int y) {
    if (viewSysMode) {
        if (state == GLUT_DOWN) {
            if (button == 1) {
                focusMode = !focusMode;
            }
            int m;
            if (monitorMode == 4) {
                if (x > width / 2 && y > height / 2) m = 0;
                else if (x < width / 2 && y > height / 2) m = 1;
                else if (x > width / 2 && y < height / 2) m = 2;
                else if (x < width / 2 && y < height / 2) m = 3;
            }
            else m = monitorMode;
            if (button == 3) {
                monitorMagnification[m] *= 1.1;
                if (monitorMagnification[m] > 20) monitorMagnification[m] = 20;
            }
            else if (button == 4) {
                monitorMagnification[m] /= 1.1;
                if (monitorMagnification[m] < 1) monitorMagnification[m] = 1;
            }
        }
    }
    else {
        if (state == GLUT_UP) {
            if (button == 2 && focusMode == 2) {
                focusMode = 1;
            }
        }
        if (state == GLUT_DOWN) {
            if (button == 1) {
                focusMode = !focusMode;
            }
            if (button == 2 && focusMode == 1) {
                focusMode = 2;
            }
            if (button == 3) {
                if (sightMode == 2) {
                    lensDistance /= 1.1;
                    if (lensDistance < 8) lensDistance = 8;
                }
                else {
                    viewMagnification *= 1.1;
                    if (viewMagnification > 20) viewMagnification = 20;
                }
            }
            else if (button == 4) {
                if (sightMode == 2) {
                    lensDistance *= 1.1;
                    if (lensDistance > 80) lensDistance = 80;
                }
                else {
                    viewMagnification /= 1.1;
                    if (viewMagnification < 1) viewMagnification = 1;
                }
            }
        }
    }
}

void passive_motion(int x, int y) {
    if (viewSysMode) return;
    if (abs(x - lastMouseX) < 100 && abs(y - lastMouseY) < 100) {
        sightLongitude -= (x - lastMouseX) / 3.0;
        sightLatitude -= (y - lastMouseY) / 3.0;

        if (sightLatitude > 89.9) sightLatitude = 89.9; //因為到90度會不知道上方在哪
        else if (sightLatitude < -89.9) sightLatitude = -89.9;
    }
    lastMouseX = x, lastMouseY = y;
}

void mouse_motion(int x, int y) {
    if (viewSysMode || focusMode != 2) return;
    if (abs(x - lastMouseX) < 100 && abs(y - lastMouseY) < 100) {
        sightLongitude -= (x - lastMouseX) / 3.0;
        sightLatitude -= (y - lastMouseY) / 3.0;

        if (sightLatitude > 89.9) sightLatitude = 89.9; //因為到90度會不知道上方在哪
        else if (sightLatitude < -89.9) sightLatitude = -89.9;
    }
    lastMouseX = x, lastMouseY = y;
}

void idle_func() {
    //int sec_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
    int curSysTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 100000000;
    float dt = (curSysTime - lastSysTime) / 1000.0; //cout << dt << "\n";
    lastSysTime = curSysTime;

    helicopter.evolve(dt);
    
    if (lastMouseX < 30 && lastMouseX != -999) sightLongitude += 120 * dt;
    else if (lastMouseX > width - 31) sightLongitude -= 120 * dt;
    if (lastMouseY < 30 && lastMouseY != -999) sightLatitude += 120 * dt;
    else if (lastMouseY > height - 31) sightLatitude -= 120 * dt;

    if (sightLatitude > 89.9) sightLatitude = 89.9; //因為到90度會不知道上方在哪
    else if (sightLatitude < -89.9) sightLatitude = -89.9;

    /*if (spKeyPressing[R_SHIFT_INT]) lensDistance -= 30 * dt;
    if (spKeyPressing[R_CTRL_INT]) lensDistance += 30 * dt;
    if (lensDistance < 12) lensDistance = 12;
    else if(lensDistance > 100) lensDistance = 100;*/

    display();
}

void main(int argc, char** argv) {

    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow("A Flying Helicopter");

    myinit();      /*---Initialize other state varibales----*/
    
    glutDisplayFunc(display);
    glutIdleFunc(idle_func);
    glutReshapeFunc(my_reshape);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyboard_down);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(spKeyboard_down);
    glutSpecialUpFunc(spKeyboard_up);
    glutMouseFunc(mouse_func);
    glutPassiveMotionFunc(passive_motion);
    glutMotionFunc(mouse_motion);
    //---------------------------------------------------------------
    
    Coordinate a(0.972809, 0, -0.231604), b(0.972809, 0, -0.231604);
    /*rotateDir(a, b, -45);
    a.print();
    cout << includedAngle(a, b) << "\n";
    cout << acos(innerProduct(a, b) / a.getLength() / b.getLength()) << "\n";
    cout << innerProduct(a, b) / a.getLength() / b.getLength() << "\n";*/

    lastSysTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 100000000;
    
    glutMainLoop();
}