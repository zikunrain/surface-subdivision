#pragma once

#include "pch.h"
#include "mesh.h"

#include "math.h"
#include "string"
#include "fstream"
#include "sstream"
#include "algorithm"
#include "assert.h"
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define PI 3.14159265

Vec3 Vec3::apply(double n) {
	double nx = n * x;
	double ny = n * y;
	double nz = n * z;
	Vec3 newVertex;
	newVertex.x = nx;
	newVertex.y = ny;
	newVertex.z = nz;
	return newVertex;
}

Vec3 Vec3::add(Vec3 v) {
	double nx = v.x + x;
	double ny = v.y + y;
	double nz = v.z + z;
	Vec3 newVertex;
	newVertex.x = nx;
	newVertex.y = ny;
	newVertex.z = nz;
	return newVertex;
}

bool Mesh::readFile(char* path) {
	ifstream file(path);
	if (!file) {
		cerr << "Error::ObjLoader, could not open obj file:"
			<< path << " for reading." << std::endl;
		return false;
	}
	string line;
	int faceId = 0;
	while (getline(file, line)) {
		if ((line.substr(0, 2) == "vt") || (line.substr(0, 2) == "vn")) {

		} else if (line.substr(0, 1) == "v") {
			// 顶点位置数据
			istringstream s(line.substr(1));
			Vec3 v;
			s >> v.x; s >> v.y; s >> v.z;
			vVertex.push_back(v);
		} else if (line.substr(0, 1) == "f") {
			// 面数据
			Face face;
			istringstream vtns(line.substr(1));
			string vtn;
			while (vtns >> vtn) {
				cout << "faceId: " << faceId << "; vtn: " << vtn << endl;
				// 处理一行中多个顶点属性
				Vertex vertex;
				replace(vtn.begin(), vtn.end(), '/', ' ');
				istringstream ivtn(vtn);
				if (vtn.find("  ") != string::npos) {
					// 没有纹理数据，注意这里是2个空格
					ivtn >> vertex.vertIndex
						>> vertex.normIndex;

					vertex.vertIndex--;     //使得下标从0开始
					vertex.normIndex--;
				} else {
					ivtn >> vertex.vertIndex
						>> vertex.textIndex
						>> vertex.normIndex;

					//cout <<  vertex.vertIndex << "/" << vertex.textIndex << "/" << vertex.normIndex << " ";
					vertex.vertIndex--;
					vertex.textIndex--;
					vertex.normIndex--;
				}
				face.vertex.push_back(vertex);
				face.faceId = faceId;
			}
			vFace.push_back(face);

			int vert1 = face.vertex[0].vertIndex ;
			int vert2 = face.vertex[1].vertIndex;
			int vert3 = face.vertex[2].vertIndex;
			//cout << vert1 << vert2 << vert3 << endl;

			Edge edge1;
			edge1.vertIndex1 = vert1 > vert2 ? vert1 : vert2;
			edge1.vertIndex2 = vert1 > vert2 ? vert2 : vert1;
			edge1.faceId = faceId;

			Edge edge2;
			edge2.vertIndex1 = vert1 > vert3 ? vert1 : vert3;
			edge2.vertIndex2 = vert1 > vert3 ? vert3 : vert1;
			edge2.faceId = faceId;

			Edge edge3;
			edge3.vertIndex1 = vert2 > vert3 ? vert2 : vert3;
			edge3.vertIndex2 = vert2 > vert3 ? vert3 : vert2;
			edge3.faceId = faceId;

			vEdge.push_back(edge1);
			vEdge.push_back(edge2);
			vEdge.push_back(edge3);

			faceId++;
			//cout << endl;
		}
	}

	return true;
}

void Mesh::generateIndex() {
	for (int ei = 0; ei < vEdge.size(); ei++) {
		Edge e = vEdge[ei];
		int vertIndex1 = e.vertIndex1;
		int vertIndex2 = e.vertIndex2;

		if (mapVertEdges.find(vertIndex1) != mapVertEdges.end()) {
			mapVertEdges[vertIndex1].indices.push_back(ei);
		} else {
			EdgeIndices edgeIndices;
			edgeIndices.indices.push_back(ei);
			mapVertEdges.insert(pair<int, EdgeIndices>(vertIndex1, edgeIndices));
		}

		if (mapVertEdges.find(vertIndex2) != mapVertEdges.end()) {
			mapVertEdges[vertIndex2].indices.push_back(ei);
		}
		else {
			EdgeIndices edgeIndices;
			edgeIndices.indices.push_back(ei);
			mapVertEdges.insert(pair<int, EdgeIndices>(vertIndex2, edgeIndices));
		}

		string key = to_string(vertIndex1) + "_" + to_string(vertIndex2);
		int faceId = e.faceId;
		UniqueEdge myPair = { faceId, -1 };
		if (mapEdgeFace.find(key) != mapEdgeFace.end()) {
			mapEdgeFace[key].facePair[1] = faceId;
			mapEdgeFace[key].edgePair[1] = ei;
		} else {
			mapEdgeFace.insert(pair<string, UniqueEdge>(key, myPair));
			mapEdgeFace[key].edgePair[0] = ei;
		}
	}

	map<string, UniqueEdge>::iterator iter;
	iter = mapEdgeFace.begin();
	while (iter != mapEdgeFace.end()) {
		cout << (iter->second).facePair[0] << "," << (iter->second).facePair[1] <<
		"-" << (iter->second).edgePair[0] << "," << (iter->second).edgePair[1] << endl;

		int vIndex0 = vEdge[(iter->second).edgePair[0]].vertIndex1;
		int vIndex1 = vEdge[(iter->second).edgePair[0]].vertIndex2;
		Vec3 v0 = vVertex[vIndex0];
		Vec3 v1 = vVertex[vIndex1];
		Vec3 v2;
		Vec3 v3;

		int faceId1 = (iter->second).facePair[0];
		vector<Vertex> vertex1 = vFace[faceId1].vertex;
		for (vector<Vertex>::const_iterator iter = vertex1.cbegin(); iter != vertex1.cend(); iter++) {
			int index = (*iter).vertIndex;
			if ((index != vIndex0) && (index != vIndex1)) {
				v2 = vVertex[index];
				break;
			}
		}

		int faceId2 = (iter->second).facePair[1];
		vector<Vertex> vertex2 = vFace[faceId2].vertex;
		for (vector<Vertex>::const_iterator iter = vertex2.cbegin(); iter != vertex2.cend(); iter++) {
			int index = (*iter).vertIndex;
			if ((index != vIndex0) && (index != vIndex1)) {
				v3 = vVertex[index];
				break;
			}
		}

		Vec3 inserted = ((v0.add(v1)).apply(0.375)).add((v2.add(v3)).apply(0.125));
		(iter->second).insertedVert = inserted;
		
		iter++;
	}

	map<int, EdgeIndices>::iterator iter2;
	iter2 = mapVertEdges.begin();
	while (iter2 != mapVertEdges.end()) {
		vector<int> edgeIndices = (iter2->second).indices;
		int vertIndex0 = iter2->first;
		Vec3 v0 = vVertex[vertIndex0];

		int n = edgeIndices.size();
		double beta = (0.625 - (0.375 + cos(2*PI/n) / 4) * (0.375 + cos(2*PI/n) / 4)) / n;
		cout << "beta: " << beta << endl;
		cout << "1 - n * beta: " << 1 - n * beta << endl;
		cout << "0.375 + cos(2*PI/n): " << 0.375 + cos(2 * PI / n) << endl;

		Vec3 surrounding;
		surrounding.x = 0.0;
		surrounding.y = 0.0;
		surrounding.z = 0.0;
		for (vector<int>::const_iterator i = edgeIndices.cbegin(); i != edgeIndices.cend(); i++) {
			int edgeIndex = *i;
			Edge e = vEdge[edgeIndex];
			int vertIndex = e.vertIndex1 == vertIndex0 ? e.vertIndex2 : e.vertIndex1;
			surrounding = surrounding.add(vVertex[vertIndex]);
		}
		surrounding = surrounding.apply(beta);

		Vec3 dominant = v0.apply(1 - n * beta);
		Vec3 modified = dominant.add(surrounding);
		vVertex[vertIndex0].x = modified.x;
		vVertex[vertIndex0].y = modified.y;
		vVertex[vertIndex0].z = modified.z;

		iter2++;
	}

	// 每一条边上都有一个插入点
	// 每一个原来的顶点都已被更新
	// 现在重新生成网格
	// 遍历每一个面片
	vector<Face> vFaceNew;
	int vertexIndexAssigned = vVertex.size();
	int faceId = 0;
	for (int fi = 0; fi < vFace.size(); fi++) {
		Face f = vFace[fi];
		vector<Vertex> vertexes = f.vertex;
		// 原来的
		int vIndex1 = vertexes[0].vertIndex;
		int vIndex2 = vertexes[1].vertIndex;
		int vIndex3 = vertexes[2].vertIndex;

		Vertex v1;
		v1.vertIndex = vIndex1;
		Vertex v2;
		v2.vertIndex = vIndex2;
		Vertex v3;
		v3.vertIndex = vIndex3;

		string edgeKey1 = vIndex1 > vIndex2
			? to_string(vIndex1) + "_" + to_string(vIndex2)
			: to_string(vIndex2) + "_" + to_string(vIndex1);
		Vec3 inserted12 = mapEdgeFace[edgeKey1].insertedVert;
		Vertex vertexNew1;
		vertexNew1.vertIndex = vertexIndexAssigned++;
		vVertex.push_back(inserted12);

		string edgeKey2 = vIndex2 > vIndex3
			? to_string(vIndex2) + "_" + to_string(vIndex3)
			: to_string(vIndex3) + "_" + to_string(vIndex2);
		Vec3 inserted23 = mapEdgeFace[edgeKey2].insertedVert;
		Vertex vertexNew2;
		vertexNew2.vertIndex = vertexIndexAssigned++;
		vVertex.push_back(inserted23);

		string edgeKey3 = vIndex1 > vIndex3
			? to_string(vIndex1) + "_" + to_string(vIndex3)
			: to_string(vIndex3) + "_" + to_string(vIndex1);
		Vec3 inserted13 = mapEdgeFace[edgeKey3].insertedVert;
		Vertex vertexNew3;
		vertexNew3.vertIndex = vertexIndexAssigned++;
		vVertex.push_back(inserted13);

		// f1 由v1， vertexNew1， vertexNew3构成
		Face f1;
		f1.vertex.push_back(v1);
		f1.vertex.push_back(vertexNew1);
		f1.vertex.push_back(vertexNew3);
		f1.faceId = faceId++;
		vFaceNew.push_back(f1);

		// f2 由v2， vertexNew1， vertexNew2构成
		Face f2;
		f2.vertex.push_back(v2);
		f2.vertex.push_back(vertexNew1);
		f2.vertex.push_back(vertexNew2);
		f2.faceId = faceId++;
		vFaceNew.push_back(f2);

		// f3 由v3， vertexNew2， vertexNew3构成
		Face f3;
		f3.vertex.push_back(v3);
		f3.vertex.push_back(vertexNew2);
		f3.vertex.push_back(vertexNew3);
		f3.faceId = faceId++;
		vFaceNew.push_back(f3);

		// f4 由vertexNew1， vertexNew2， vertexNew3构成
		Face f4;
		f4.vertex.push_back(vertexNew1);
		f4.vertex.push_back(vertexNew2);
		f4.vertex.push_back(vertexNew3);
		f4.faceId = faceId++;
		vFaceNew.push_back(f4);
	}

	vFace.clear();
	vFace.assign(vFaceNew.begin(), vFaceNew.end());
}

void Mesh::refine() {
	int count = vVertex.size();
	cout << endl;
	for (int i = 0; i < count; i++) {
		cout << "id: " << i
			<< "; x: " << vVertex[i].x
			<< "; y: " << vVertex[i].y
			<< "; z: " << vVertex[i].z << endl;
	}
	cout << endl;

	int countf = vFace.size();
	for (int i = 0; i < countf; i++) {
		Face f = vFace[i];
		vector<Vertex> vertex = f.vertex;
		cout << "id: " << f.faceId
			<< "; 0: " << vertex[0].vertIndex
			<< "; 1: " << vertex[1].vertIndex
			<< "; 2: " << vertex[2].vertIndex << endl;
	}
	cout << endl;
}

void Mesh::drawMesh() {
	if (vFace.empty())
		return;

	for (int f = 0; f < vFace.size(); f++)  // 绘制每个面片
	{
		int n = vFace[f].vertex.size();    // 面的顶点数
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLES);
		for (int v = 0; v < n; v++)
		{
			int it = vFace[f].vertex[v].textIndex;
			glTexCoord2f(0, 0);

			int iv = vFace[f].vertex[v].vertIndex;
			glVertex3f(vVertex[iv].x, vVertex[iv].y, vVertex[iv].z);
		}
		glEnd();
	}
}
