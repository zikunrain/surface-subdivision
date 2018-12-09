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
		int vertIndex;     // 此顶点在顶点列表中的索引
		int normIndex;     // 顶点的法线索引
		int textIndex;     // 顶点的纹理索引
};

class Face {
	public:
		vector<Vertex> vertex; // 顶点和法线索引组成的列表
		int faceId;

		Face() {}
		~Face() {}
};

class Mesh {
	private:
		vector<Vec3> vVertex;  // 顶点数组
		vector<vec2> vText;    // 纹理数组
		vector<Vec3> vNorm;    // 法线数组
		vector<Face> vFace;    // 面片数组
		vector<Edge> vEdge;    // 边数组
		map<string, UniqueEdge> mapEdgeFace; // 根据边找到面
		map<int, EdgeIndices> mapVertEdges; // 根据点找到边

	public:
		Mesh() {};
		~Mesh() {};

		bool readFile(char* path);
		void drawMesh();
		void generateIndex();
		void refine();
};

