#pragma once
#include "dago/Dago.h"
#include "dago/FSMComponent.h"
#include "MarkovChain.h"

extern dago::DiceRoller* dice;

class GhostAttributes : public dago::Component {
public:
	int patience = 1;
	int alert = 1;
	GhostAttributes() {}
	~GhostAttributes() {}
};

class BubbleText : public dago::AnimatedTexture {
public:
	bool visible = false;
	BubbleText() {}
	~BubbleText() {}
};

// States List
class GhostWanderingState;
class GhostThinkingState;
class GhostAlertState;


class GhostThinkingState : public dago::FSMState {
public:
	GhostThinkingState() {}

	void onEnter() {
		this->parent->print("I'm gonna start thinking...");
		this->parent->parent->getComponent<BubbleText>()->visible = true;
	}
	void update() {
		auto& inbox = this->parent->parent->getComponent<dago::Messager>()->inbox;
		if (inbox.size() > 0) {
			auto& msg = inbox[0];
			inbox.pop_back();
			delete msg;
			changeToBlipState<GhostAlertState>();
			return;
		}

		this->parent->print("Thinking, thinking!");
		auto ghost = this->parent->parent->getComponent<GhostAttributes>();

		ghost->patience++;
		if (ghost->patience > 5)
		{
			changeState<GhostWanderingState>();
			return;
		}
	}

	void onExit() {
		this->parent->print("No more thinking");
		this->parent->parent->getComponent<BubbleText>()->visible = false;

	}

};

class GhostAlertState : public dago::FSMState {
public:
	GhostAlertState() {}

	void onEnter() {
		this->parent->parent->getComponent<BubbleText>()->visible = false;
		this->parent->parent->getComponent<dago::Floater>()->active = true;
		this->parent->print("ALERT, ALERT!");
		this->parent->parent->getComponent<dago::Floater>()->setParametersY(
			dice->RollFN(1.0f, 2.0f),
			dice->RollFN(500.0f, 600.0f),
			dice->RollFN(0.1f, 0.2f),
			dice->RollFN(30.0f, 50.0f));
	}
	void update() {
		this->parent->returnToPreviousState();
	}

	void onExit() {
		if (this->parent->previousState->equals(this->parent->getState<GhostThinkingState>()))
		{
			this->parent->parent->getComponent<BubbleText>()->visible = true;
			this->parent->parent->getComponent<dago::Floater>()->active = false;
		}
		this->parent->print("That's weird");
		this->parent->parent->getComponent<dago::Floater>()->setParametersY(
			dice->RollFN(0.1f, 0.3f),
			dice->RollFN(40.0f, 70.0f),
			dice->RollFN(0.1f, 0.2f),
			dice->RollFN(30.0f, 50.0f));	
	}
};



class GhostWanderingState : public dago::FSMState {
public:
	GhostWanderingState() {}

	void onEnter() {
		this->parent->print("I think I'm going to wander somewhere else");
		this->parent->parent->getComponent<dago::Floater>()->active = true;
		this->parent->parent->getComponent<dago::Position>()->set(
			dice->RollFN(6.0f,18.0f),
			dice->RollFN(6.0f, 11.0f));
	}
	void update() {
		this->parent->print("*Floating* *Floating*");
		auto& inbox = this->parent->parent->getComponent<dago::Messager>()->inbox;
		if (inbox.size() > 0) {
			auto& msg = inbox[0];
			inbox.pop_back();
			delete msg;
			changeToBlipState<GhostAlertState>();
			return;
		}
		auto ghost = this->parent->parent->getComponent<GhostAttributes>();
		ghost->patience = dice->RollIN(1, 5);
		if (ghost->patience == 1){
			changeState<GhostThinkingState>();
			return;
		}
		
	}
	void onExit() {
		this->parent->print("Wandering exit");
		this->parent->parent->getComponent<dago::Floater>()->active = false;
		
	}
};


class GhostEntity : public dago::Entity {
public:
	GhostEntity(std::string name) : GhostEntity(name, 0, 10.0f, 10.0f) {}
	GhostEntity(float x, float y) : GhostEntity("_default_", 0, x, y) {}
	GhostEntity(std::string name, float x, float y) : GhostEntity(name, 0, x, y) {}
	GhostEntity(std::string name, int colorText, float x, float y) {
		this->id = "Ghost";

		this->name = name;
		if (this->name == "random") {
			MarkovChain markov;
			markov.ParseFile("MaleNames.txt");
			this->name = markov.toCapitalCase(markov.GenerateChain(dice->RollIN(3, 8)));
		}

		this->addComponent<GhostAttributes>(new GhostAttributes());
		this->addComponent<dago::Position>(new dago::Position(dice->RollIN(30, 34), dice->RollIN(4, 8)));
		this->addComponent<dago::Floater>(new dago::Floater());
		this->addComponent<dago::FSMComponent>(new dago::FSMComponent());
		this->addComponent<dago::AnimatedTexture>(new dago::AnimatedTexture());
		this->addComponent<dago::Messager>(new dago::Messager());
		this->addComponent<BubbleText>(new BubbleText());

		auto floater = this->getComponent<dago::Floater>();
		floater->setParametersX(
			dice->RollFN(0.1f, 0.3f),
			dice->RollFN(40.0f, 70.0f),
			dice->RollFN(0.1f, 0.2f),
			dice->RollFN(90.0f, 120.0f));
		
		floater->setParametersY(
			dice->RollFN(0.1f, 0.3f),
			dice->RollFN(40.0f, 70.0f),
			dice->RollFN(0.1f, 0.2f),
			dice->RollFN(30.0f, 50.0f));

		auto fsm = this->getComponent<dago::FSMComponent>();
		fsm->colorText = colorText;
		if (fsm->colorText == 0)
			fsm->colorText = dice->RollIN(1, 15);
		fsm->addState<GhostThinkingState>(new GhostThinkingState());
		fsm->addState<GhostWanderingState>(new GhostWanderingState());
		fsm->addState<GhostAlertState>(new GhostAlertState());
		fsm->setState(fsm->getStateById(dice->RollIN(fsm->getStatesSize()-1)));

		
	}
	~GhostEntity() {
		for (auto pair : this->components)
			if (pair.second) {
				delete pair.second;
				pair.second = NULL;
			}
		this->components.clear();
	}

	void init() {
		auto ts = TileMap::TILE_SIZE;
		auto anim = this->getComponent<dago::AnimatedTexture>();
		for (int currentColumn = 0; currentColumn < 6; currentColumn++)
			anim->sprites.push_back(blit3D->MakeSprite(
				currentColumn * ts,
				0 * ts,
				ts, ts,
				"Media\\ghost_idle.png"));

		auto bubbleAnim = this->getComponent<BubbleText>();
		for (int currentColumn = 0; currentColumn < 4; currentColumn++)
			bubbleAnim->sprites.push_back(blit3D->MakeSprite(
				currentColumn * 40,
				0,
				40, 25,
				"Media\\text_balloon.png"));
	}

	void update(double deltaTime) {
		// Updating Animation Component
		auto anim = this->getComponent<dago::AnimatedTexture>();
		anim->timer += (float)deltaTime;
		if (anim->timer >= 2.f / 20.f){
			anim->frameNumber++;
			if (anim->frameNumber > 5) {
				anim->frameNumber = 0;
			}
			anim->timer -= 4.f / 20.f;
		}
		auto bubbleAnim = this->getComponent<BubbleText>();
		bubbleAnim->timer += (float)deltaTime;
		if (bubbleAnim->timer >= 2.f / 20.f) {
			bubbleAnim->frameNumber++;
			if (bubbleAnim->frameNumber > 3) {
				bubbleAnim->frameNumber = 0;
			}
			bubbleAnim->timer -= 3.f / 20.f;
		}

		// Updating FSM Component
		this->getComponent<dago::FSMComponent>()->update(deltaTime);
		this->getComponent<dago::Floater>()->update(deltaTime);
	}

	void draw() {
		auto position = this->getComponent<dago::Position>();
		auto floater = this->getComponent<dago::Floater>()->offset;
		auto anim = this->getComponent<dago::AnimatedTexture>();
		float x = position->x + floater->x;
		float y = position->y + floater->y;
		for (Sprite* sprite : anim->sprites)
			sprite->alpha = anim->alpha;
		anim->sprites[anim->frameNumber]->Blit(TileMap::getXTileMap(x), TileMap::getYTileMap(y));
		auto bubbleAnim = this->getComponent<BubbleText>();
		if (bubbleAnim->visible) {
			bubbleAnim->sprites[bubbleAnim->frameNumber]->Blit(TileMap::getXTileMap(x + 0.7), TileMap::getYTileMap(y - 0.7));
		}
	}
};




