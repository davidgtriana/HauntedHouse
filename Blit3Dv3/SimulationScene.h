#pragma once
#include "TileMap.h"
#include "dago/Dago.h"
#include "GhostEntity.h"
#include "NPCEntity.h"


class SimulationScene : public dago::Scene {

public:	
	TileMap* map;
	std::vector<dago::Entity*> ghosts;
	dago::Entity* dago;

	SimulationScene() {
		map = new TileMap();
		entityManager = new dago::EntityManager();

		dago = new NPCEntity("Dago", rlutil::WHITE, 31.0f, 21.f);
		for(int i = 0; i < 2; i++)
			ghosts.push_back(new GhostEntity("random"));
		
		entityManager->addEntity(dago);
		entityManager->addEntity(ghosts);

	}
	~SimulationScene() {
		if (map) {
			delete map;
			map = NULL;
		}
		if (entityManager) {
			delete entityManager;
			entityManager = NULL;
		}
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

	void doCursor(double x, double y) {
		
	}

	void doMouseButton(int button, int action, int mods) {
		
	}

};