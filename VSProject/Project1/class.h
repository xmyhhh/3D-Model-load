#pragma once
#include<vector>
#include<iostream>

class Model;

class Mesh {
public:
	Model* model;

	Mesh(Model* pModel) {
		model = pModel;
	}
	void drawmesh();

};


class Model {
public:
	std::vector<Mesh> meshes;
	
	int draw_material = 5555;

	Model(){
		meshes.push_back(Mesh(this));
	}

	void drawmodel() {
		meshes[0].drawmesh();
	}

};

inline void Mesh::drawmesh() {

	std::cout << model->draw_material << std::endl;
}
