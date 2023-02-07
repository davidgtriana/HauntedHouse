#pragma once

#include "Blit3D.h"
#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
#include <random>
#include <typeindex>

extern Blit3D* blit3D;

namespace dago
{
	// * * CLASS INDEX * * * 
	// - DiceRoller
	// 
	// GAME
	// COMPONENTS
	// ENTITIES
	// SCENES
	
	

	
	// ======================= DiceRoller ==================================
	class DiceRoller
	{
	private:
		std::mt19937 rng;

	public:
		DiceRoller()
		{
			std::random_device rd;
			rng.seed(rd());
		}

		void SeedRNG(unsigned int seedVal) {
			rng.seed(seedVal);
		}

		/**
		* @param An integer N
		* @result Returns a random number between 0 and N
		*/
		int RollIN(int N) {
			return this->RollIN(0, N);
		}

		/**
		* @param two integers
		* @result Returns a random number between in and out
		*/
		int RollIN(int in, int out) {
			std::uniform_int_distribution<unsigned int> randomD(in, out);
			return randomD(rng);
		}

		/**
		* @param A float N
		* @result Returns a random real number between 0 and N
		*/
		float RollFN(float N) {
			return this->RollFN(0, N);
		}

		/**
		* @param two floats
		* @result Returns a random real number between in and out
		*/
		float RollFN(float in, float out) {
			std::uniform_real_distribution<float> randomD(in, out);
			return randomD(rng);
		}

		/**
		* @result Return a summation of 4 dices with 6 faces dismissing the lowest one
		*/
		int Roll4D6DropLowest() {
			int result = 0;
			int lowest = this->RollIN(1, 6);
			int temp = 0;
			for (int i = 1; i <= 3; i++) {
				temp = this->RollIN(1, 6);
				if (temp < lowest) {
					result += lowest;
					lowest = temp;
				}
				else {
					result += temp;
				}
			}
			return result;
		}
	};

	// ============================= GAME ==================================
	//	May have several scenes, and scenes are controlled by a SceneManager

	class GameAdapter {
	public:
		virtual void init() = 0;
		virtual void update(double deltaTime) = 0;
		virtual void draw() = 0;
		virtual void doInput(int key, int scancode, int action, int mods) = 0;
		virtual void dispose() = 0;
		virtual void doCursor(double x, double y) = 0;
		virtual void doMouseButton(int button, int action, int mods) = 0;
	};
	

	class Rectangle {
	public:
		float x;
		float y;
		float w;
		float h;

		Rectangle() {
			this->x = 0;
			this->y = 0;
			this->w = 0;
			this->h = 0;
		}
		Rectangle(float width, float height) {
			this->x = 0;
			this->y = 0;
			this->w = width;
			this->h = height;
		}
		Rectangle(float x, float y, float width, float height) {
			this->x = x;
			this->y = y;
			this->w = width;
			this->h = height;
		}
		Rectangle(Rectangle* rect) {
			Rectangle(rect->x, rect->y, rect->w, rect->h);
		}
		/*
		bool contains(float x, float y) {
			return true;
		}*/
	};

	// ========================= ENTITIES ==================================
	//Every Scene may have several components, and the EntityManager should control the entities and their components

	class EntityManager;
	class Component;
	class Entity : public GameAdapter{
	public:
		std::unordered_map<int, int> otherdummy;
		EntityManager* parent = NULL;
		std::unordered_map<std::type_index, Component*> components;
		std::string id = "default_entity";


		template <typename T> bool addComponent(T* component) {
			if (this->components.count(typeid(T))) {
				std::cout << "cant't add twice the same component "<< std::endl;
				return false;
			}
			component->parent = this;
			this->components[typeid(T)] = component;
			return true;
		}

		template <typename T> T* getComponent() {
			return (T*)this->components[typeid(T)];
		}

	};

	class FSMComponent;
	class State
	{
	public:
		FSMComponent* parent = NULL; //who owns us

		virtual void onEnter() = 0;
		virtual void update() = 0;
		virtual void onExit() = 0;
	};

	class Message
	{
	public:
		std::string senderID;
		std::string receiverID;
		int message;
		Message(std::string sender, std::string receiver, int theMessage) {

		}
	};
	

	class EntityManager : public GameAdapter {
	public:
		std::unordered_map<std::string, Entity*> entities;

		std::vector<Message> messages;

		EntityManager() {
			
		}

		void init() {
			for (auto& pair : entities)
				pair.second->init();
		}
		void update(double deltaTime) {
			/*
			//SEND ALL MESSAGES IN THE QUEUE

			//loop backwards so we can add messages as we delete others without messing up our
			//place in the list
			for (int i = messageList.size() - 1; i >= 0; --i)
			{
				//is this a broadcast message?
				if (messageList[i].receiverID == "BROADCAST")
				{
					//this is broadcast
					//send message to every entity on the appropriate subscription list.
					// EXCEPT the original sender (for now, this condition can be omitted if we wish 
					//for different behaviour i.e. entities send themselves messages as part of their operational logic)

					subsIter = subscriptionLists.find((messageList[i].message));
					if (subsIter == subscriptionLists.end())
					{
						assert(false);
					}
					else
					{

						for (auto subscriber : subsIter->second)
						{
							if (subscriber != messageList[i].senderID)
							{
								//look up the entity by ID
								it = entityMap.find(subscriber);
								assert(it != entityMap.end());

								//send this essage to this subscriber
								it->second->ReceiveMessage(messageList[i]);
							}
						}
					}
				}
				else
				{
					it = entityMap.find(messageList[i].receiverID);
					assert(it != entityMap.end()); //could remove this for some applications, if sending to a non-existant entity is ok
					if (it != entityMap.end())
					{
						it->second->ReceiveMessage(messageList[i]);
					}
				}

				messageList.erase(messageList.begin() + i);
			}*/
			for (auto& pair : entities)
				pair.second->update(deltaTime);
		}

		void draw() {
			for (auto& pair : entities)
				pair.second->draw();
		}
		void doInput(int key, int scancode, int action, int mods) {

		}
		void doCursor(double x, double y) {

		}

		virtual void doMouseButton(int button, int action, int mods) {

		}

		void dispose() {
			for (auto& pair : this->entities) {
				pair.second->dispose();
				delete pair.second;
			}	
		}

		void addEntity(Entity* entity) { //Registry Entity
			entity->parent = this;
			this->entities[entity->id] = entity;
		}
		void sendMessage(Message message) {
			this->messages.push_back(message);
		}

		void subscribe(std::string subscriberID, int messageType) {
			
		}


	};

	// ======================= COMPONENTS ==================================
	//	Every entity may have several components, and the components are managed by a every entity

	class Component {
	public:
		Entity* parent = NULL;
		virtual void dispose() = 0;
	};


	class Texture : public Component {
	public:
		Sprite* sprite;

		Texture() {

		}

		void dispose() {

		}

	};


	class AnimatedTexture : public Component {
	public:
		int frameNumber = 0;
		float timer = 0.f;
		float angle = 0.f;
		std::vector<Sprite*>* sprites;

		AnimatedTexture() {
			sprites = new std::vector<Sprite*>();
		}

		void dispose() {
			delete sprites;
		}
	};


	class Position : public Component {
	public:
		float x;
		float y;

		Position() {
			x = 0;
			y = 0;
		}

		void dispose() {

		}
	};

	class Velocity : public Component {
	public:
		float x;
		float y;

		Velocity() {
			x = 0;
			y = 0;
		}

		void dispose() {

		}
	};

	class FSMComponent : public Component { // Finite States Machine
	public:
		Entity* parent = NULL;
		State* currentState = NULL;
		State* previousState = NULL;
		std::unordered_map<std::type_index, State*>* states;
		std::vector<Message>* messages;

		FSMComponent(){
			states = new std::unordered_map<std::type_index, State*>();
			messages = new std::vector<Message>();
		}

		template <typename T> void changeState() {
			this->previousState = this->currentState;
			previousState->onExit();
			this->currentState = getState<T>();
			this->currentState->onEnter();
		}
		template <typename T> void changeToBlipState() {
			this->previousState = this->currentState;
			this->currentState = getState<T>();
			this->currentState->onEnter();
		}
		void returnToPreviousState() {
			this->currentState->onExit();
			auto temp = this->currentState;
			this->currentState = this->previousState;
			this->previousState = temp;
		}
		template <typename T> bool addState(T* state) {
			if (this->states->count(typeid(T))) {
				std::cout << "cant't add twice the same state " << std::endl;
				return false;
			}
			state->parent = this;
			this->states->operator[](typeid(T)) = state;
			return true;
		}

		template <typename T> T* getState() {
			return (T*)this->states->operator[](typeid(T));
		}

		void sendMessage(Message message) {
			this->parent->parent->sendMessage(message);
		}
		void receiveMessage(Message message) {

		}
		void setupSubscriptions() {
			this->parent->parent->subscribe(this->parent->id, 0);
		}
		void print(std::string msg) {
			std::cout << this->parent->id << ": " << msg << std::endl;
		}

		void dispose() {
			for (auto& S : *states) delete S.second;
			delete states;
			delete messages;
		}
	};



	// ============================ SCENE ==================================
	//	May have several entities, and scenes are controlled by a SceneManager
	//	The scenes must know how to handle messages between entities

	

	class Scene : public GameAdapter {
	public:
		EntityManager* entityManager;

	};

	class SceneManager : public GameAdapter {

	public:

		Scene* scene;

		
		SceneManager() {}
		void init() { scene->init(); }
		void update(double deltaTime) { scene->update(deltaTime); }
		void draw() { scene->draw(); }
		void dispose() { scene->dispose(); delete scene; }
		void doInput(int key, int scancode, int action, int mods) { scene->doInput(key, scancode, action, mods); }
		void doCursor(double x, double y) { scene->doCursor(x, y); }
		void doMouseButton(int button, int action, int mods) { scene->doMouseButton(button, action, mods); }
		Scene* get() { return scene; }
		void set(Scene* scn) {
			if (!scene) dispose();
			scene = scn;
			scene->init();
		}

	};


	



}

