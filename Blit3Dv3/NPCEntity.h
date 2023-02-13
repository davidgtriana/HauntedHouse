#pragma once
#include "dago/Dago.h"
#include "dago/FSMComponent.h"

extern dago::DiceRoller* dice;

class NPCAttributes : public dago::Component {
public:
	int scaryness = 1;

	NPCAttributes() {}
	~NPCAttributes() {}
};

// States List
class NPCScaredState;
class NPCThinkingState;
class NPCWanderingState;

class NPCWanderingState : public dago::FSMState {
public:
	NPCWanderingState() {}
	~NPCWanderingState() {}

	void onEnter() {
		this->parent->parent->getComponent<dago::Position>()->set(dice->RollIN(18,34), dice->RollIN(15, 20));
		this->parent->parent->getComponent<dago::AnimatedTexture>()->active = true;
		this->parent->print("I wonder what can I find over here");
	}
	void update() {
		auto npc = this->parent->parent->getComponent<NPCAttributes>();

		this->parent->print("Walking, Walking!");
		npc->scaryness++;
		if (npc->scaryness > 5)
		{
			auto msg = new dago::Message(this->parent->parent->id, "BROADCAST", 0);
			this->parent->parent->getComponent<dago::Messager>()->sendMessage(msg);
			changeToBlipState<NPCScaredState>();
			return;
		}
		if (dice->RollIN(100) < 10) changeState<NPCThinkingState>();
	}

	void onExit() {
		this->parent->print("No more walking");
	}

};

class NPCScaredState : public dago::FSMState {
public:
	NPCScaredState() {}
	~NPCScaredState() {}

	void onEnter() {
		this->parent->print("AAHHHH");
		this->parent->parent->getComponent<dago::AnimatedTexture>()->active = true;
	}
	void update() {
		auto npc = this->parent->parent->getComponent<NPCAttributes>();
		npc->scaryness-=2;
		if (npc->scaryness > 1)
		{
			npc->scaryness = 1;
			this->parent->returnToPreviousState();
			return;
		}
		this->parent->print("*Scared* *Scared*");
	}
	void onExit() {
		this->parent->print("Ufffff");
	}
};

class NPCThinkingState : public dago::FSMState {
public:
	NPCThinkingState() {}
	~NPCThinkingState() {}

	void onEnter() {
		this->parent->print("I'm going to start thinking");
		this->parent->parent->getComponent<dago::AnimatedTexture>()->active = false;
	}
	void update() {
		auto npc = this->parent->parent->getComponent<NPCAttributes>();
		npc->scaryness++;
		if (npc->scaryness > 5)
		{
			auto msg = new dago::Message(this->parent->parent->id, "Ghost_1", 0);
			this->parent->parent->getComponent<dago::Messager>()->sendMessage(msg);
			changeToBlipState<NPCScaredState>();
			return;
		}
		if (dice->RollIN(10) % 2 == 0) changeState<NPCWanderingState>();
		this->parent->print("*Thinking* *Thinking*");
	}
	void onExit() {
		this->parent->print("Enough thinking");
	}
};




class NPCEntity : public dago::Entity {
public:
	NPCEntity(std::string name) : NPCEntity(name, rlutil::WHITE, 10.0f, 10.0f) {}
	NPCEntity(float x, float y) : NPCEntity("_default_", rlutil::WHITE, x, y) {}
	NPCEntity(std::string name, float x, float y) : NPCEntity(name, dice->RollIN(1,15), x, y) {}
	NPCEntity(std::string name, int colorText, float x, float y) {
		this->id = "NPC";
		this->name = name;

		this->addComponent<NPCAttributes>(new NPCAttributes());
		this->addComponent<dago::Position>(new dago::Position(x, y));
		this->addComponent<dago::FSMComponent>(new dago::FSMComponent());
		this->addComponent<dago::AnimatedTexture>(new dago::AnimatedTexture());
		this->addComponent<dago::Messager>(new dago::Messager());

		auto fsm = this->getComponent<dago::FSMComponent>();
		fsm->colorText = colorText;
		fsm->addState<NPCThinkingState>(new NPCThinkingState());
		fsm->addState<NPCWanderingState>(new NPCWanderingState());
		fsm->addState<NPCScaredState>(new NPCScaredState());
		fsm->setState(fsm->getStateById(0));

	}
	~NPCEntity() {
		for (auto pair : this->components)
			if (pair.second) {
				delete pair.second;
				pair.second = NULL;
			}
		this->components.clear();
	}

	void init() {
		auto anim = this->getComponent<dago::AnimatedTexture>();
		auto ts = TileMap::TILE_SIZE;
		for (int currentColumn = 0; currentColumn < 11; currentColumn++)
			anim->sprites.push_back(blit3D->MakeSprite(
				currentColumn * ts,
				0 * ts,
				ts, ts,
				"Media\\dago.png"));
	}

	void update(double deltaTime) {
		// Updating FSM Component
		this->getComponent<dago::FSMComponent>()->update(deltaTime);
		// Updating Animation Component
		auto anim = this->getComponent<dago::AnimatedTexture>();
		if (!anim->active) {
			anim->frameNumber = 3;
			return;
		}
		anim->timer += (float)deltaTime;
		if (anim->timer >= 2.f / 20.f) {
			anim->frameNumber++;
			if (anim->frameNumber > 3) {
				anim->frameNumber = 0;
			}
			anim->timer -= 4.f / 20.f;
		}

		
	}

	void draw() {
		auto position = this->getComponent<dago::Position>();
		auto anim = this->getComponent<dago::AnimatedTexture>();
		anim->sprites[anim->frameNumber]->Blit(TileMap::getXTileMap(position->x), TileMap::getYTileMap(position->y));
	}
};




