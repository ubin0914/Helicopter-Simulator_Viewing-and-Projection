#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <ctime>

#include "PublicValue.h"

using namespace std;

class Environment {
private:
	int building[25][25][16] = { 0 };
	vector<Coordinate> taipei_101_vertex;
	vector<Face> taipei_101_face;
	float buildingHeight[19] = { 10, 10, 10, 10, 13, 13, 13, 13, 16, 16, 16, 16, 19, 19, 19, 19, 27, 36, 60 };
	int centerBlockX = 0, centerBlockZ = 0;	//以block為單位的x與z
public:
	Environment() {
		pushSquareVertex(64, 0, 64);
		pushSquareVertex(47, 110, 47);
		pushSquareVertex(48, 113.5, 48);
		vector<pair<float, float>> dxdz = {
			make_pair(36, 41),
			make_pair(41, 41),
			make_pair(41, 36),
			make_pair(46, 36),
			make_pair(46, -36)
		};
		pushMultiSquareVertex(dxdz, 113.5);
		pushMultiSquareVertex(dxdz, 117);
		for (int i = 0; i < 8; i++) {
			vector<pair<float, float>> dxdz = {
				make_pair(33, 38),
				make_pair(38, 38),
				make_pair(38, 33),
				make_pair(43, 33),
				make_pair(43, -33)
			};
			pushMultiSquareVertex(dxdz, 117 + i * 34);
			dxdz = {
				make_pair(38, 43),
				make_pair(43, 43),
				make_pair(43, 38),
				make_pair(48, 38),
				make_pair(48, -38)
			};
			pushMultiSquareVertex(dxdz, 117 + (i + 1) * 34);
		}
		pushSquareVertex(24, 389, 24);
		pushSquareVertex(26, 401, 26);
		pushSquareVertex(24, 401, 24);
		pushSquareVertex(24, 407, 24);
		pushSquareVertex(12, 407, 12);
		pushSquareVertex(16, 434, 16);
		pushSquareVertex(15.5, 434, 15.5);
		pushSquareVertex(16.5, 448.5, 16.5);
		pushSquareVertex(9, 448.5, 9);
		pushSquareVertex(6, 457.5, 6);

		pushFourFace(0, 1);
		pushFourFace(4, 1);
		for (int k = 0; k < 5; k++)
			pushFourFace(12 + k, 5, (k == 4));
		for (int i = 0; i < 8; i++)
			for (int k = 0; k < 5; k++)
				pushFourFace(52 + i * 40 + k, 5, (k == 4));
		pushFourFace(372, 1);
		pushFourFace(380, 1);
		pushFourFace(388, 1);
		pushFourFace(396, 1);
		pushFourFace(404, 1);

		pushCeilingFace(8, 4);
		pushCeilingFace(32, 20);
		for(int i = 0; i < 8; i++)
			pushCeilingFace(72 + i * 40, 20);
		pushCeilingFace(376, 4);
		pushCeilingFace(384, 4);
		pushCeilingFace(392, 4);
		pushCeilingFace(400, 4);
		pushCeilingFace(408, 4);

		initialBuildings();
	}
	void initialBuildings() {
		srand(time(NULL));
		for (int i = 0; i < blockNum; i++)
			for (int j = 0; j < blockNum; j++)
				for (int k = 0; k < 16; k++)
					building[i][j][k] = rand() % 19;
	}
	void display() {
		drawGround();
		drawTapei_101();
		
		/*int i = 5, j = 5, k = 0;
		glPushMatrix();
		glTranslated((j - 4) * 100 + (k % 4) * 25 - 37.5, 0, (i - 4) * 100 + (k / 4) * 25 - 37.5);
		glScaled(1, buildingHeight[building[i][j][k]], 1);
		glPolygonMode(GL_FRONT, GL_FILL);
		for each (Face face in buildingFace) {
			face.drawAsRGB(buildingVertex);
		}
		glPopMatrix();*/
		
		glPolygonMode(GL_FRONT, GL_FILL);
		for (int i = -blockNum / 2; i <= blockNum / 2; i++) {
			for (int j = -blockNum / 2; j <= blockNum / 2; j++) {
				float offsetX = (centerBlockX + j) * 110;
				float offsetZ = (centerBlockZ + i) * 110;
				if (!offsetX && !offsetZ) continue;
				int indexX = (centerBlockX + j + 100000 * blockNum) % blockNum;
				int indexZ = (centerBlockZ + i + 100000 * blockNum) % blockNum;
				for (int k = 0; k < 16; k++) {
					float quarterX = (k < 8 ? 25 : -25) + (k % 4 < 2 ? 10 : -10);
					float quarterZ = (k / 4 % 2 ? 20.5 : -20.5) + (k % 2 ? 10 : -10);
					glPushMatrix();
					glTranslated(offsetX + quarterX, 0, offsetZ + quarterZ);
					glScaled(1, buildingHeight[building[indexX][indexZ][k]], 1);
					for each (Face face in buildingFace) {
						face.drawAsRGB(buildingVertex);
					}
					glPopMatrix();
				}
			}
		}
	}
	void updateBlock(int newX, int newZ) {
		if (newX != centerBlockX) {
			int updateX = (centerBlockX + 100000 * blockNum + (newX - centerBlockX == 1 ? -1 : 1) * (blockNum / 2)) % blockNum;
			for (int i = 0; i < blockNum; i++)
				for (int k = 0; k < 16; k++)
					building[updateX][i][k] = rand() % 19;
			centerBlockX = newX;
		}
		if (newZ != centerBlockZ) {
			int updateZ = (centerBlockZ + 100000 * blockNum + (newZ - centerBlockZ == 1 ? -1 : 1) * (blockNum / 2)) % blockNum;
			for (int i = 0; i < blockNum; i++)
				for (int k = 0; k < 16; k++)
					building[i][updateZ][k] = rand() % 19;
			centerBlockZ = newZ;
		}
	}
	void debug() {
		for each (Coordinate vertex in taipei_101_vertex) {
			vertex.print();
		}/*
		for each (Face face in taipei_101_face) {
			for (int i = 0; i < 4; i++)
				cout << face.order[i] << " ";
			cout << "\n";
		}*/
	}
private:
	void drawGround() {
		glColor3f(0.1, 0.1, 0.1);
		glBegin(GL_POLYGON);
		glVertex3f(centerBlockX * 110 + 9999, 0, centerBlockZ * 110 - 9999);
		glVertex3f(centerBlockX * 110 - 9999, 0, centerBlockZ * 110 - 9999);
		glVertex3f(centerBlockX * 110 - 9999, 0, centerBlockZ * 110 + 9999);
		glVertex3f(centerBlockX * 110 + 9999, 0, centerBlockZ * 110 + 9999);
		glEnd();
	}
	void drawTapei_101() {
		glPolygonMode(GL_FRONT, GL_FILL);
		for each (Face face in taipei_101_face) {
			face.drawAsHSLInc(taipei_101_vertex, {-0.6, -1, -0.4});
		}
		glPushMatrix();
			glTranslated(0, 457.5, 0);
			glRotatef(-90, 1, 0, 0);
			glColor3f(0.3, 0.3, 0.4);
			GLUquadricObj* cylind = gluNewQuadric();
			gluQuadricDrawStyle(cylind, GLU_FILL);
			gluQuadricNormals(cylind, GLU_SMOOTH);
			gluCylinder(cylind, 3, 5, 4.5, 24, 3);

			//glDisable(GL_DEPTH_TEST);
			glTranslated(0, 0, 4.5);
			glColor3f(0.4, 0.4, 0.5);
			GLUquadricObj* ceiling = gluNewQuadric();
			gluQuadricDrawStyle(ceiling, GLU_FILL);
			gluDisk(ceiling, 0, 5, 24, 3);
			glTranslated(0, 0, 0.02);
			glColor3f(1, 1, 0.3);
			gluDisk(ceiling, 4, 4.2, 24, 3);

			glRotatef(90, 1, 0, 0);
			glBegin(GL_POLYGON);
			glVertex3f(-1.9, 0, 2.5);
			glVertex3f(-1.9, 0, -2.5);
			glVertex3f(-1.4, 0, -2.5);
			glVertex3f(-1.4, 0, 2.5);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(1.4, 0, 2.5);
			glVertex3f(1.4, 0, -2.5);
			glVertex3f(1.9, 0, -2.5);
			glVertex3f(1.9, 0, 2.5);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(-1.4, 0, 0.25);
			glVertex3f(-1.4, 0, -0.25);
			glVertex3f(1.4, 0, -0.25);
			glVertex3f(1.4, 0, 0.25);
			glEnd();
			//glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
	void pushSquareVertex(float dx, float y, float dz) {
		taipei_101_vertex.push_back({ dx / 2, y, dz / 2 });
		taipei_101_vertex.push_back({ dz / 2, y, -dx /2 });
		taipei_101_vertex.push_back({ -dx / 2, y, -dz / 2 });
		taipei_101_vertex.push_back({ -dz / 2, y, dx / 2 });
	}
	void pushMultiSquareVertex(vector<pair<float, float>> dxdz, float y) {
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ vertex.first / 2, y, vertex.second / 2 });
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ vertex.second / 2, y, -vertex.first / 2 });
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ -vertex.first / 2, y, -vertex.second / 2 });
		for each (pair<float, float> vertex in dxdz)
			taipei_101_vertex.push_back({ -vertex.second / 2, y, vertex.first / 2 });
	}
	void pushFourFace(int start, int interval, bool cross = 1) {
		for (int i = 0; i < 4; i++) {
			int fin = start + 4 * interval;
			taipei_101_face.push_back({ {i * interval + start, 
										(i == 3 && cross ? -1 : i) * interval + start + 1,
										(i == 3 && cross ? -1 : i) * interval + fin + 1,
										i * interval + fin}, 
										{220, 0.2, 0.9} });
		}
	}
	void pushCeilingFace(int start, int cycle) {
		vector<int> order;
		for (int i = 0; i < cycle; i++)
			order.push_back(start + i);
		taipei_101_face.push_back({ order, { 240, 0.2, 0.9 } });
	}

	vector<Coordinate> buildingVertex = {
		{9, 0, 9},
		{9, 0, -9},
		{-9, 0, -9},
		{-9, 0, 9},
		{9, 1, 9},
		{9, 1, -9},
		{-9, 1, -9},
		{-9, 1, 9},
	};
	vector<Face> buildingFace = {
		{{0, 1, 5, 4}, {0.4, 0.4, 0.4}},
		{{1, 2, 6, 5}, {0.2, 0.2, 0.2}},
		{{2, 3, 7, 6}, {0.3, 0.3, 0.3}},
		{{3, 0, 4, 7}, {0.15, 0.15, 0.15}},
		{{4, 5, 6, 7}, {0.6, 0.6, 0.6}},
	};
};