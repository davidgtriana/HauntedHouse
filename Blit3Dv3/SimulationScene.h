#pragma once

#include"Dago.h"
#include"TileMap.h"
#include "GhostEntity.h"


class SimulationScene : public dago::Scene {

public:	
	TileMap* map;
	GhostEntity* dago;

	SimulationScene() {
		entityManager = new dago::EntityManager();
		dago = new GhostEntity(20.f,10.f);
		map = new TileMap();
		entityManager->AddEntity(dago);

	}

	void init() {
		map->init();
		entityManager->init();
	}

	void update(double deltaTime) {
		map->update(deltaTime);
		entityManager->update(deltaTime);
	}

	void draw() {
		map->draw();
		entityManager->draw();
		map->drawLastLayer();
	}

	void doInput(int key, int scancode, int action, int mods) {

	}

	void dispose() {
		entityManager->dispose();
		if (entityManager) delete entityManager;
		map->dispose();
		if (map) delete map;
	}

	void doCursor(double x, double y) {
		
	}

	void doMouseButton(int button, int action, int mods) {
		
	}

};