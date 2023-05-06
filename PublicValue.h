#pragma once

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

#define L_SHIFT_INT 112
#define R_SHIFT_INT 113
#define L_CTRL_INT 114
#define R_CTRL_INT 115
#define L_ALT_INT 116
#define UPKEY_INT 101
#define DOWNKEY_INT 103
#define LEFTKEY_INT 100
#define RIGHTKEY_INT 102

int width = 1520, height = 790;
int lastSysTime;
float PI = 3.14159265;
float gravity = 9.80665;
bool keyPressing[128] = { 0 }, spKeyPressing[128] = { 0 };
float lastTimeSingleW = 0;
float lastTimeSingleLCtrl = 0;
float lastTimeSingleSpace = 0;
int lastMouseX = -999, lastMouseY = -999;
int sightMode = 2;
int focusMode = 0;
float sightLongitude = 135, sightLatitude = -5;
float lensDistance = 20;
float viewMagnification = 1.5;
int blockNum = 5;
int viewSysMode = 1;
int monitorMode = 4;
int monitorOperateMode = 0;
float monitorMagnification[4] = { 1.5, 1.5, 1.5, 1.5 };
float cos_1deg = cos(1 * PI / 180);
float sin_1deg = sin(1 * PI / 180);
float monitorNear = 5, monitorFar = 50;

//Coordinate operator+ (const Coordinate&, const Coordinate&);
//Coordinate operator- (const Coordinate&, const Coordinate&);
//Coordinate operator* (int, const Coordinate&);
//float innerProduct(const Coordinate&, const Coordinate&);

class Coordinate {
public:
	float x, y, z;

	Coordinate() {}
	Coordinate(const Coordinate& v) { x = v.x, y = v.y, z = v.z; }
	Coordinate(float point[3]) {
		this->x = point[0];
		this->y = point[1];
		this->z = point[2];
	}
	Coordinate(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	float getLength() const {
		return sqrtf(x * x + y * y + z * z);
	}
	void identify() {
		float length = sqrtf(x * x + y * y + z * z);
		if (length) { x /= length; y /= length; z /= length; }
	}
	Coordinate identity() const {
		Coordinate v(*this);
		v.identify();
		return v;
	}
	void print() const {
		cout << "(" << x << " " << y << " " << z << ") ";
	}
	void println() const {
		cout << x << " " << y << " " << z << "\n";
	}
};

Coordinate lightIncidence = { 0, -1, 0 };
Coordinate eyeAxisX = { 1, 0, 0 };
Coordinate eyeAxisY = { 0, 1, 0 };
Coordinate eyeAxisZ = { 0, 0, 1 };

Coordinate operator+ (const Coordinate& a, const Coordinate& b) { return Coordinate(a.x + b.x, a.y + b.y, a.z + b.z); }
Coordinate operator- (const Coordinate& a, const Coordinate& b) { return Coordinate(a.x - b.x, a.y - b.y, a.z - b.z); }
Coordinate operator- (const Coordinate& a) { return Coordinate(-a.x, -a.y, -a.z); }
Coordinate operator* (const float c, const Coordinate& v) { return Coordinate(c * v.x, c * v.y, c * v.z); }
Coordinate operator* (const Coordinate& v, const float c) { return Coordinate(c * v.x, c * v.y, c * v.z); }
bool operator== (const Coordinate& a, const Coordinate& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
bool operator!= (const Coordinate& a, const Coordinate& b) { return a.x != b.x || a.y != b.y || a.z != b.z; }
void operator+= (Coordinate& a, const Coordinate& b) { a.x += b.x, a.y += b.y, a.z += b.z; }
void operator-= (Coordinate& a, const Coordinate& b) { a.x -= b.x, a.y -= b.y, a.z -= b.z; }
void operator*= (Coordinate& a, const float b) { a.x *= b, a.y *= b, a.z *= b; }

float innerProduct(const Coordinate& a, const Coordinate& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Coordinate outerProduct(const Coordinate& a, const Coordinate& b) {
	return Coordinate(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Coordinate project(const Coordinate& projectand, const Coordinate& projector) {
	return innerProduct(projectand, projector) / projector.getLength() * projector;
}

float includedAngle(const Coordinate& a, const Coordinate& b, const Coordinate& c = { 0, 0, 0 }) {
	if (a == b) return 0;
	int direct = 1;
	if (c.x || c.y || c.z) direct = (innerProduct(outerProduct(a, b), c) >= 0 ? 1 : -1);
	float angle_cos = innerProduct(a, b) / a.getLength() / b.getLength();
	if (angle_cos > 1) angle_cos = 1;
	else if (angle_cos < -1) angle_cos = -1;
	return direct * 180 / PI * acos(angle_cos);
}

//任意方向旋轉，轉一向量
void rotateDir(Coordinate& dir, Coordinate axis, float angle) {
	axis.identify();
	float th = (float)angle / 180 * PI;
	dir = dir * cos(th) + outerProduct(axis, dir) * sin(th) + axis * innerProduct(axis, dir) * (1 - cos(th));
}
//只在平面上旋轉，用這個較快，並一次轉兩軸
void rotateDualAxis(Coordinate& x, Coordinate& y, float ang) {
	float cosV = cos(ang * PI / 180), sinV = sin(ang * PI / 180);
	Coordinate newX = cosV * x + sinV * y;
	Coordinate newY = cosV * y - sinV * x;
	x = newX, y = newY;
}
//若已有三角函數值，再簡化計算三角函數時間
void rotateDualAxis(Coordinate& x, Coordinate& y, float cosV, float sinV) {
	Coordinate newX = cosV * x + sinV * y;
	Coordinate newY = cosV * y - sinV * x;
	x = newX, y = newY;
}

void rotateSysTo(const Coordinate& newAxisX, const Coordinate& newAxisY) {
	if (newAxisX.y || newAxisX.z) {
		if (newAxisY.x || newAxisY.z) {
			Coordinate temp = newAxisX - project(newAxisX, { 1,0,0 });
			float angle = includedAngle({ 0,0,1 }, temp, { 1,0,0 });
			glRotatef(angle, 1, 0, 0);					rotateDir(lightIncidence, { 1,0,0 }, -angle);
			temp = outerProduct(temp, { 1,0,0 });
			angle = includedAngle({ 1,0,0 }, newAxisX, temp);
			glRotatef(angle, 0, 1, 0);					rotateDir(lightIncidence, { 0,1,0 }, -angle);
			angle = includedAngle(temp, newAxisY, newAxisX);
			//if (angle * 0 != 0) cout << angle << "\n";
			glRotatef(angle, 1, 0, 0);					rotateDir(lightIncidence, { 1,0,0 }, -angle);
		}
		else {
			float angle = includedAngle({ 1,0,0 }, newAxisX, { 0,1,0 });
			glRotatef(angle, 0, 1, 0);					rotateDir(lightIncidence, { 0,1,0 }, -angle);
		}
	}
	else {
		float angle = includedAngle({ 0,1,0 }, newAxisY, { 1,0,0 });
		glRotatef(angle, 1, 0, 0);					rotateDir(lightIncidence, { 1,0,0 }, -angle);
	}
}

//直接返回世界座標系，沒有實作popMatrix時的處理
void resetLightIncidence() {
	lightIncidence = { 0, -1, 0 };
}

void drawText(const pair<int, int>& vertex, const string& text) {
	int print_x = vertex.first, print_y = vertex.second;
	for (int i = 0; i < text.size(); i++) {
		char key = text[i];
		if (key == '\r') {
			print_x = vertex.first;
			print_y += 20;
		}
		else {
			glRasterPos2i(print_x, height - print_y);
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)key);
			print_x += 10;
		}
	}
}

class Face {
public:
	vector<int> order;
	vector<float> color;
	int type;

	Face() {}
	Face(vector<int> vertex, vector<float> color) {
		this->order = vertex;
		this->color = color;
		if (color.size() == 3) this->color.push_back(1);
		type = 1;
	}
	void drawAsRGB(const vector<Coordinate>& cord) {
		glColor4f(color[0], color[1], color[2], color[3]);
		glBegin(GL_POLYGON);
		for each (int index in order) {
			glVertex3f(cord[index].x, cord[index].y, cord[index].z);
		}
		glEnd();
	}
	void drawAsHSLInc(const vector<Coordinate>& cord, Coordinate incidence = lightIncidence, Coordinate ground = { 0, 1, 0 }) {
		drawAsCustomHSLInc(cord, color[0], color[1], color[2], incidence, ground);
	}
	void drawAsCustomHSLInc(const vector<Coordinate>& cord, float h, float s, float l, Coordinate incidence = lightIncidence, Coordinate ground = { 0, 1, 0 }) {
		if (type == 1) {
			Coordinate normal = outerProduct(cord[order[1]] - cord[order[0]], cord[order[2]] - cord[order[1]]);
			float reflectivity = -(innerProduct(normal, incidence) / normal.getLength() / incidence.getLength());
			if (reflectivity < (1 + reflectivity) * 0.2 + 0.15) reflectivity = (1 + reflectivity) * 0.2 + 0.15;

			float H = h, S = s, L = reflectivity * l;
			float c = (1 - fabs(2 * L - 1)) * S;
			float x = c * (1 - fabs(H / 60 - int(H) / 120 * 2 - 1));
			float m = L - c / 2;
			float r = 0, g = 0, b = 0;
			switch (int(H) / 60) {
			case 0: r = c; g = x; b = 0; break;
			case 1: r = x; g = c; b = 0; break;
			case 2: r = 0; g = c; b = x; break;
			case 3: r = 0; g = x; b = c; break;
			case 4: r = x; g = 0; b = c; break;
			case 5: r = c; g = 0; b = x; break;
			}
			glColor3f(r + m, g + m, b + m);
			//glPolygonMode(GL_FRONT, GL_FILL);
			glBegin(GL_POLYGON);
			for each (int index in order) {
				glVertex3f(cord[index].x, cord[index].y, cord[index].z);
			}
			glEnd();
		}
	}
};

vector<Coordinate> cubeVertex = {
	{0.0, 0.0, 0.0},
	{1.0, 0.0, 0.0},
	{1.0, 0.0, 1.0},
	{0.0, 0.0, 1.0},
	{0.0, 1.0, 0.0},
	{1.0, 1.0, 0.0},
	{1.0, 1.0, 1.0},
	{0.0, 1.0, 1.0}
};
vector<Face> cubeFace = {
	{ {0, 1, 2, 3}, {0, 0, 0} },
	{ {7, 6, 5, 4}, {0, 0, 0} },
	{ {0, 4, 5, 1}, {0, 0, 0} },
	{ {1, 5, 6, 2}, {0, 0, 0} },
	{ {3, 2, 6, 7}, {0, 0, 0} },
	{ {0, 3, 7, 4}, {0, 0, 0} }
};