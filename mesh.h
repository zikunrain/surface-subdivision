#pragma once

#include "vector"
#include "iostream"
#include "map"

using namespace std;

class Vec3 {
	public:
		double x, y, z;
		Vec3 apply(double n);
		Vec3 add(Vec3 v);
};

struct vec2 {
	double x, y;
};

struct Edge {
	int faceId, nextFaceId, vertIndex1, vertIndex2;
};

struct UniqueEdge {
	int facePair[2];
	int edgePair[2];
	Vec3 insertedVert;
};

struct EdgeIndices {
	vector<int> indices;
};

class Vertex {
	public:
		int vertIndex;     // �˶����ڶ����б��е�����
		int normIndex;     // ����ķ�������
		int textIndex;     // �������������
};

class Face {
	public:
		vector<Vertex> vertex; // ����ͷ���������ɵ��б�
		int faceId;

		Face() {}
		~Face() {}
};

class Mesh {
	private:
		vector<Vec3> vVertex;  // ��������
		vector<vec2> vText;    // ��������
		vector<Vec3> vNorm;    // ��������
		vector<Face> vFace;    // ��Ƭ����
		vector<Edge> vEdge;    // ������
		map<string, UniqueEdge> mapEdgeFace; // ���ݱ��ҵ���
		map<int, EdgeIndices> mapVertEdges; // ���ݵ��ҵ���

	public:
		Mesh() {};
		~Mesh() {};

		bool readFile(char* path);
		void drawMesh();
		void generateIndex();
		void refine();
};

