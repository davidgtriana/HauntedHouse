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

	// ======================= COMPONENTS ==================================
	//	May have several components, and scenes are controlled by a EntityManager (which is the Scene itslef)
	class Component {
	public:

	};


	class Texture : public Component {
	public:
		Sprite* sprite;

		Texture() {

		}


	};


	class AnimatedTexture : public Component {
	public:
		int frameNumber = 0;
		float timer = 0.f;
		float angle = 0.f;
		std::vector<Sprite*> sprites;

		AnimatedTexture() {
		
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
	};

	class Velocity : public Component {
	public:
		float x;
		float y;

		Velocity() {
			x = 0;
			y = 0;
		}
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
	//	May have several components, and the EntityManager should control the entities and their components

	class Entity : public GameAdapter{
		
	public:
		std::unordered_map<std::type_index, dago::Component*> components;
		std::string id = "default";


		template <typename T> bool AddComponent(T* component) {
			if (this->components.count(typeid(T))) {
				std::cout << "cant't add twice the same component "<< std::endl;
				return false;
			}
			this->components[typeid(T)] = component;
			return true;
		}

		template <typename T> T* getComponent() {
			return (T*)this->components[typeid(T)];
		}

	};
	
	class EntityManager : public GameAdapter {
	public:
		std::unordered_map<std::string, Entity*> entities;


		EntityManager() {
			
		}

		void init() {
			for (auto& pair : entities)
				pair.second->init();
		}
		void update(double deltaTime) {
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

		void AddEntity(Entity* entity) {
			this->entities[entity->id] = entity;
		}


	};



	// ============================ SCENE ==================================
	//	May have several entities, and scenes are controlled by a EntityManager (which is the Scene itslef)
	//	The scenes must know how to handle messages between entities
	// Every Scene is an entity manager

	

	class Scene : public GameAdapter {
	public:
		EntityManager* entityManager;

		/*
		std::vector<dago::Message> messageList; //keeps track of all the mail

		void FSM_SendMessage(FSM_Message message); //pass messages between entities
		void RegisterEntity(FSM_Entity* entity); //how we add entities to our map of entities
		void KillEntity(std::string ID); //removes an entity from the map and de-allocates it.
		void Update(); //updates all entities
		virtual ~FSM_EntityManager(); //frees all entities
		*/

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

