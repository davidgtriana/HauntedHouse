#pragma once

#include"Dago.h"

class GhostEntity : public dago::Entity {
public:

	GhostEntity(float x,float y) {
		this->id = "Ghost";
		this->AddComponent<dago::Position>(new dago::Position());
		dago::Position* position = this->getComponent<dago::Position>();
		position->x = x;
		position->y = y;
		this->AddComponent<dago::Velocity>(new dago::Velocity());
		this->AddComponent<dago::AnimatedTexture>(new dago::AnimatedTexture());
	}

	void init(){
		dago::AnimatedTexture* anim = this->getComponent<dago::AnimatedTexture>();
		for(int currentColumn = 0; currentColumn < 6; currentColumn++)
			anim->sprites.push_back(blit3D->MakeSprite(currentColumn * TileMap::TILE_SIZE, 0 * TileMap::TILE_SIZE, TileMap::TILE_SIZE, TileMap::TILE_SIZE, "Media\\ghost_idle.png"));
	}

	void update(double deltaTime){
		dago::AnimatedTexture* anim = this->getComponent<dago::AnimatedTexture>();
		anim->timer += deltaTime;
		if (anim->timer >= 2.f / 20.f)
		{
				anim->frameNumber++;

			if (anim->frameNumber > 5) {
				anim->frameNumber = 0;
			}
			anim->timer -= 4.f / 20.f;
		}
	}

	void draw(){
		dago::Position* position = this->getComponent<dago::Position>();
		dago::AnimatedTexture* anim = this->getComponent<dago::AnimatedTexture>();
		anim->sprites[anim->frameNumber]->Blit((position->x * TileMap::TILE_SIZE) + (TileMap::TILE_SIZE / 2), blit3D->screenHeight - (position->y * TileMap::TILE_SIZE) - (TileMap::TILE_SIZE/2) );

	}
	void doInput(int key, int scancode, int action, int mods){
	}
	void dispose(){
		for (auto pair : this->components)
			delete pair.second;
	}
	void doCursor(double x, double y){
	}
	void doMouseButton(int button, int action, int mods) {

	}
};

