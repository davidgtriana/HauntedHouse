#pragma once
#include "dago/Dago.h"
#include "dago/FSMComponent.h"
#include "dago/PathfindComponent.h"
#include "MarkovChain.h"

extern dago::DiceRoller* dice;

class GhostAttributes : public dago::Component {
public:
	int patience = 1;
	int maxPatience = dice->RollIN(3, 8);
	int alert = 1;
	GhostAttributes() {}
	~GhostAttributes() {}
};

class BubbleText : public dago::AnimatedTexture {
public:
	BubbleText() {}
	~BubbleText() {}
};

// States List
class GhostWanderingState;
class GhostThinkingState;
class GhostAlertState;
glm::vec2 getRandomPosition();

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
		if (ghost->patience > ghost->maxPatience)
		{
			ghost->patience = 0;
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
	glm::vec2 goal;
	GhostWanderingState() {}

	void onEnter() {
		this->parent->print("I think I'm going to wander somewhere else");
		this->parent->parent->getComponent<BubbleText>()->visible = false;
		this->parent->parent->getComponent<dago::Floater>()->active = true;
		this->goal = getRandomPosition();
		this->parent->parent->getComponent<dago::AStarComponent>()->end = glm::vec2(this->goal);
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
		if (this->parent->parent->getComponent<dago::Position>()->get() == this->goal) {
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
	GhostEntity(std::string name) : GhostEntity(name, 0, getRandomPosition()) {}
	GhostEntity(glm::vec2 vector) : GhostEntity("_default_", 0, vector) {}
	GhostEntity(std::string name, glm::vec2 vector) : GhostEntity(name, 0, vector) {}
	GhostEntity(std::string name, int colorText, glm::vec2 position) {
		this->id = "Ghost";
		this->name = name;
		if (this->name == "random") {
			MarkovChain markov;
			markov.ParseFile("MaleNames.txt");
			this->name = markov.toCapitalCase(markov.GenerateChain(dice->RollIN(3, 8)));
		}

		this->addComponent<GhostAttributes>(new GhostAttributes());
		this->addComponent<dago::Position>(new dago::Position(position));
		this->addComponent<dago::Velocity>(new dago::Velocity(0,0));
		this->addComponent<dago::AStarComponent>(new dago::AStarComponent());
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
		fsm->setState(fsm->getStateById(0));
		//fsm->setState(fsm->getStateById(dice->RollIN(fsm->getStatesSize()-1)));

		
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
		anim->frameAmount = 6;
		anim->deltaFrame = 4.0f;
		anim->getTexture("Media\\ghost_idle.png", ts,ts);

		anim = this->getComponent<BubbleText>();
		anim->frameAmount = 4;
		anim->deltaFrame = 3.0f;
		anim->getTexture("Media\\text_balloon.png", 40, 25);

		this->getComponent<dago::AStarComponent>()->init();
	}

	void update(double deltaTime) {
		// Updating FSM Component
		this->getComponent<dago::FSMComponent>()->update(deltaTime);

		// Updating Position
		this->getComponent<dago::AStarComponent>()->update(deltaTime);
		this->getComponent<dago::Position>()->update(deltaTime);


		// Updating Animation Component
		this->getComponent<dago::AnimatedTexture>()->update(deltaTime);
		this->getComponent<BubbleText>()->update(deltaTime);
		this->getComponent<dago::Floater>()->update(deltaTime);
	}

	void draw() {
		auto position = this->getComponent<dago::Position>()->get() + this->getComponent<dago::Floater>()->offset;
		this->getComponent<dago::AnimatedTexture>()->draw(TileMap::getXTileMap(position.x), TileMap::getYTileMap(position.y));
		this->getComponent<BubbleText>()->draw(TileMap::getXTileMap(position.x + 0.7), TileMap::getYTileMap(position.y - 0.7));
	}
};

glm::vec2 getRandomPosition() {
	glm::vec2 position = glm::vec2(0, 0);
	int room = dice->RollIN(5);
	switch (room) {
		// Room
	case 0:
		position.x = dice->RollIN(6, 18);
		position.y = dice->RollIN(6, 11);
		break;
		// Study Room
	case 1:
		position.x = dice->RollIN(30, 34);
		position.y = dice->RollIN(4, 8);
		break;
		// Hall
	case 2:
		position.x = dice->RollIN(20, 34);
		position.y = dice->RollIN(8, 11);
		break;
		// Kitchen
	case 3:
		position.x = dice->RollIN(8, 16);
		position.y = dice->RollIN(16, 21);
		break;
		// Living Room
	case 4:
		position.x = dice->RollIN(18, 34);
		position.y = dice->RollIN(15, 20);
		break;
	}
	return position;
}



