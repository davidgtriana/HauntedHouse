#pragma once
#include <algorithm>
#include"Dago.h"

class GhostThinkingState : public dago::State {
public:
	GhostThinkingState() {}

	void onEnter() {
		this->parent->print("Thinking enter");
	}
	void update() {
		this->parent->print("Thinking update");
	}
	
	void onExit() {
		this->parent->print("Thinking exit");
	}

};

class GhostWanderingState : public dago::State {
public:
	GhostWanderingState() {}

	void onEnter() {
		this->parent->print("Wandering enter");
	}
	void update() {
		this->parent->print("Wandering update");
	}
	void onExit() {
		this->parent->print("Wandering exit");
	}

};


class GhostEntity : public dago::Entity {
public:
	int crazyness;

	GhostEntity(float x, float y) {
		this->id = "Ghost";
		this->crazyness = 10;

		this->addComponent<dago::Position>(new dago::Position());
		this->addComponent<dago::FSMComponent>(new dago::FSMComponent());
		this->addComponent<dago::AnimatedTexture>(new dago::AnimatedTexture());
		
		auto fsm = this->getComponent<dago::FSMComponent>();
		fsm->addState<GhostThinkingState>(new GhostThinkingState());
		fsm->addState<GhostWanderingState>(new GhostWanderingState());
		fsm->currentState = fsm->getState<GhostThinkingState>();
		
		auto position = this->getComponent<dago::Position>();
		position->x = x;
		position->y = y;
	}
	virtual ~GhostEntity() {
		for (auto pair : this->components) delete pair.second;
		this->components.clear();
	}

	void init() {
		auto anim = this->getComponent<dago::AnimatedTexture>();
		for (int currentColumn = 0; currentColumn < 6; currentColumn++)
			anim->sprites.push_back(blit3D->MakeSprite(currentColumn * TileMap::TILE_SIZE, 0 * TileMap::TILE_SIZE, TileMap::TILE_SIZE, TileMap::TILE_SIZE, "Media\\ghost_idle.png"));
	}

	void update(double deltaTime) {
		auto anim = this->getComponent<dago::AnimatedTexture>();
		anim->timer += deltaTime;
		if (anim->timer >= 2.f / 20.f){
			anim->frameNumber++;
			if (anim->frameNumber > 5) {
				anim->frameNumber = 0;
			}
			anim->timer -= 4.f / 20.f;
		}
	}

	void draw() {
		auto position = this->getComponent<dago::Position>();
		auto anim = this->getComponent<dago::AnimatedTexture>();
		anim->sprites[anim->frameNumber]->Blit((position->x * TileMap::TILE_SIZE) + (TileMap::TILE_SIZE / 2), blit3D->screenHeight - (position->y * TileMap::TILE_SIZE) - (TileMap::TILE_SIZE/2) );
	}
	void doInput(int key, int scancode, int action, int mods) {

	}

	void doCursor(double x, double y) {
	}
	void doMouseButton(int button, int action, int mods) {

	}

};

