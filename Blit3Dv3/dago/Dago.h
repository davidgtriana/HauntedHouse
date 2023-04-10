///////////////////////////////////////////////////////////////////////////////////
/// Dago Library
///
/// @date 2023-02-10
/// @author David Triana (Dago)
///////////////////////////////////////////////////////////////////////////////////



#pragma once

#include "Blit3D.h"

#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
#include <random>
#include <typeindex>
#include "rlutil_fixed.h"

extern Blit3D* blit3D;

namespace dago
{
	// ======================= DiceRoller ==================================
	class DiceRoller
	{
	private:
		std::mt19937 rng;
		unsigned int seed = 0;


	public:
		
		DiceRoller() {
			std::random_device rd;
			this->seed = rd();
			rng.seed(this->seed);
		}
		DiceRoller(unsigned int seed) {
			this->seed = seed;
			rng.seed(this->seed);
		}

		~DiceRoller() {}

		/**
		* @param An integer N
		* @result Returns a random number between 0 (Inclusive) and N (Non-Inclusive)
		*/
		int RollIN(int N) {
			return this->RollIN(0, N);
		}

		/**
		* @param two integers
		* @result Returns a random number between in (Inclusive) and out (Non-Inclusive)
		*/
		int RollIN(int from, int to) {
			std::uniform_int_distribution<unsigned int> randomD(from, to - 1);
			return randomD(rng);
		}

		/**
		* @param A float N
		* @result Returns a random real number between 0 (Inclusive) and N (Inclusive)
		*/
		float RollFN(float N) {
			return this->RollFN(0, N);
		}

		/**
		* @param two floats
		* @result Returns a random real number between 'from' (Inclusive) and 'to' (Inclusive)
		*/
		float RollFN(float from, float to) {
			std::uniform_real_distribution<float> randomD(from, to);
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

		int getSeed() {
			return this->seed;
		}
	};

	// ============================= GAME ==================================
	//	May have several scenes, and scenes are controlled by a SceneManager

	class GameAdapter {
	public:
		virtual void init() = 0;
		virtual void update(double deltaTime) = 0;
		virtual void draw() = 0;
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
		Rectangle(Rectangle* rect) : Rectangle(rect->x, rect->y, rect->w, rect->h) {}

		~Rectangle() {}

	};


	class Message {
	public:
		std::string senderID = "";
		std::string receiverID = "";
		int message = 0;
		Message(std::string sender, std::string receiver, int msg) {
			this->senderID = sender;
			this->receiverID = receiver;
			this->message = msg;
		}
		Message(Message* msg) {
			this->senderID = msg->senderID;
			this->receiverID = msg->receiverID;
			this->message = msg->message;
		}
		~Message() {}
	};

	
	// ======================= COMPONENTS ==================================
	//	Every entity may have several components, and the components are managed by a every entity

	/**
	* The Component class is an abstract class that acts as a base class for
	* different components that can be added to an Entity object. The Component
	* lass has a public member variable parent which is a pointer to the parent
	* Entity object.
	*
	* The Component allows to add features to an entity such as Position, Velocity
	* Texture as well as custom components that may be required for an specific application.
	*/

	class Entity;
	class Component {
	public:
		Entity* parent = NULL;
		virtual ~Component() {}
	};

	class Messager : public Component {
	public:
		std::vector<Message*> inbox;
		std::vector<Message*> outbox;

		Messager() {}

		~Messager() {
			for (auto& msg : inbox) delete msg;
			for (auto& msg : outbox) delete msg;
			inbox.clear();
			outbox.clear();
		}

		void sendMessage(Message* message) {
			outbox.push_back(message);
		}
		void receiveMessage(Message* message) {
			inbox.push_back(message);
		}
	};



	// ========================= ENTITIES ==================================
	// Every Scene may have several components, and the EntityManager should control the entities and their components

	class EntityManager;
	class Entity : public GameAdapter {
	public:
		virtual ~Entity() {}
		EntityManager* parent = NULL;
		std::unordered_map<std::type_index, Component*> components;
		std::string id = "-";
		std::string name = "-";

		template <typename T> bool addComponent(T* component) {
			if (this->components.count(typeid(T))) {
				std::cout << "cant't add twice the same component " << std::endl;
				return false;
			}
			component->parent = this;
			this->components[typeid(T)] = component;
			return true;
		}

		template <typename T> T* getComponent() {
			return (T*)this->components[typeid(T)];
		}

		template <typename T> bool hasComponent() {
			return components.count(typeid(T)) != 0;
		}
	};
	
	class EntityManager : public GameAdapter {
	public:
		std::unordered_map<std::string, Entity*> entities;
		std::unordered_map<std::string, std::vector<std::string> > suscriptions;
		std::vector<Message*> messages;
		std::vector<bool> collisionMap;
		

		EntityManager() {
		}
		~EntityManager() {
			for (auto& pair : this->entities) 
				if (pair.second) {
					delete pair.second;
					pair.second = NULL;
				}
			entities.clear();
		}

		void init() {
			for (auto& pair : entities) pair.second->init();
		}
		void update(double deltaTime) {

			updateMessages();

			for (auto& pair : entities)
				pair.second->update(deltaTime);
		}

		void draw() {
			for (auto& pair : entities)
				pair.second->draw();
		}
		void addEntity(Entity* entity) { //Registry Entity
			entity->id += updateSuffix(entity->id);
			entity->parent = this;
			this->entities[entity->id] = entity;
		}

		void addEntity(std::vector<Entity*> entities) {
			for (auto& entity : entities) addEntity(entity);
		}

		

		void subscribe(std::string subscriberID, int messageType) {
			
		}

		void updateCollisionMap(std::vector<bool> map) {
			this->collisionMap = map;
		}


	private:
		bool startsWith(const std::string& str, const std::string& prefix) {
			return str.substr(0, prefix.length()) == prefix;
		}

		std::string updateSuffix(std::string id) {
			std::string suffix = "";
			int count = 0;
			for (auto& pair : this->entities)
				if (startsWith(pair.first, id))
					count++;
			return count > 0 ? "_" + std::to_string(count) : "";
		}

		void print(std::string text) {
			rlutil::setColor(rlutil::YELLOW);
			std::cout<<" - " << text << std::endl;
			rlutil::setColor(rlutil::WHITE);
		}

		void updateMessages() {

			// Get messages from entity list
			for (auto& pair : this->entities) {
				if (!pair.second->hasComponent<Messager>()) continue;
				auto& outbox = pair.second->getComponent<Messager>()->outbox;
				if (outbox.size() == 0) continue;
				for (auto& msg : outbox)
					this->messages.push_back(msg);
				outbox.clear();
			}

			// Send message to their destinataire
			for (int i = messages.size() - 1; i >= 0; --i)
			{
				auto& msg = messages[i];

				if(msg->receiverID == "BROADCAST") {
					print(msg->senderID + " has sent a message to EVERYONE");
					for (auto& pair : this->entities) {
						if (!pair.second->hasComponent<Messager>()) continue;
						if (pair.second->id == msg->senderID) continue;
						Message* copy = new Message(msg);
						pair.second->getComponent<Messager>()->receiveMessage(copy);
						print(pair.second->id + " has received the message");
					}
					messages.pop_back();
					delete msg;
				}

				else {
					// If the destinataire doesn't exits
					if (this->entities.count(msg->receiverID) == 0)
					{
						print("Entity " + msg->receiverID + " doesn't exist");
						messages.pop_back();
						delete msg;
						continue;
					}

					// If the destinataire doesn't have a Messager Component
					if (!this->entities[msg->receiverID]->hasComponent<Messager>())
					{
						print("Entity " + msg->receiverID + " can't receive messages... Add a Messager Component and try again");
						messages.pop_back();
						delete msg;
						continue;
					}

					// Send message to this individual
					this->entities[msg->receiverID]->getComponent<Messager>()->receiveMessage(msg);
					print(msg->senderID + " has sent a message to " + msg->receiverID);
					messages.pop_back();
				}
			}
			this->messages.clear();
		}
	};


	// Other Components
	class Texture : public Component {
	public:
		Sprite* sprite = NULL;

		Texture() {}

		~Texture() {}

	};


	class AnimatedTexture : public Component {
	public:
		std::vector<Sprite*> sprites;
		int currentFrame = 0;
		int frameAmount = 0;
		float deltaFrame = 0.0f;
		float timer = 0.f;
		float angle = 0.f;
		float alpha = 1.0f;
		bool visible = true;

		AnimatedTexture() {	}
		~AnimatedTexture() { }
		void update(double deltaTime) {
			this->timer += (float)deltaTime;
			if (this->timer >= 2.f / 20.f) {
				this->currentFrame++;
				if (this->currentFrame == this->frameAmount) {
					this->currentFrame = 0;
				}
				this->timer -= this->deltaFrame / 20.f;
			}
		}

		void getTexture(std::string path, int w, int h) {
			for (int currentColumn = 0; currentColumn < this->frameAmount; currentColumn++)
				this->sprites.push_back(blit3D->MakeSprite(
					currentColumn * w,
					0 * h,
					w, h,
					path));
		}
		void draw(float x, float y) {
			if (visible) this->sprites[this->currentFrame]->Blit(x, y);
		}
	};
	class Velocity : public Component {
	public:
		float magnitude = 0.6f;
		glm::vec2 velocity = glm::vec2(0,0);

		Velocity() : Velocity(0, 0) {}
		Velocity(float x, float y) {
			this->velocity.x = x;
			this->velocity.y = y;
		}
		~Velocity() { }

		void set(float x, float y) {
			this->velocity.x = x * this->magnitude;
			this->velocity.y = y * this->magnitude;
		}
		void set(glm::vec2 vector) {
			set(vector.x,vector.y);
		}
		glm::vec2 get() {
			return this->velocity;
		}
	};

	class Position : public Component {
	public:
		glm::vec2 position = glm::vec2(0, 0);

		Position() : Position(0, 0) {}
		Position(glm::vec2 vector) : Position(vector.x, vector.y) {}
		Position(float x, float y) {
			this->position.x = x;
			this->position.y = y;
		}
		~Position() { }

		void set(float x, float y) {
			this->position.x = x;
			this->position.y = y;
		}
		void set(glm::vec2 vector) {
			set(vector.x, vector.y);
		}
		glm::vec2 get() {
			return this->position;
		}
		void update(double deltaTime) {
			// This will throw an error if the entity doesn't have a velocity
			auto velocity = this->parent->getComponent<Velocity>()->get();
			this->position += velocity * (float)deltaTime;
		}

	};


	/**
	* This class called Floater, is a subclass of the Component class and is used
	* to calculate and update the position of an object. It has various attributes that represen
	* the parameters of a mathematical equation that determines the offset of the position
	* of the object being updated. The position of the object is stored in the 'offset' object
	* which is an instance of the Position class. The Floater class contains methods that
	* update the position of the object and set the parameters used in the mathematical equation.
	* The update method calculates the angle of the object and updates the x and y position
	* of the object based on the mathematical equation using the sin and cos functions in the
	* glm library. The setParametersX and setParametersY methods allow the parameters of the
	* mathematical equation to be set. The Floater class also has a destructor method that properly
	* deallocates memory and nullifies the 'offset' object when the instance of the Floater
	* class is destroyed.
	*/
	class Floater : public Component {
	public:
		glm::vec2 offset;
		bool active = false;
		float angle = 0;
		float Ax1 = 0.0f; //xScale1
		float Ax2 = 0.0f; //xScale2
		float Bx1 = 0.0f; //xSpeed1
		float Bx2 = 0.0f; //xSpeed2

		float Ay1 = 0.0f; //yScale1
		float Ay2 = 0.0f; //yScale2
		float By1 = 0.0f; //ySpeed1
		float By2 = 0.0f; //ySpeed2

		Floater() {
			this->offset = glm::vec2(0, 0);
		}
		~Floater() {}

		void update(double deltaTime) {
			if (!active) return;
			angle += deltaTime;
			if (angle >= 360) angle -= 360;

			// Mathematical Function https://bit.ly/3DXNLOc
			// A -> Amplitude
			// B -> Period
			// Offset = A1 + trig (B1 * theta) + A2 + trig (B2 * theta)
			this->offset.x = Ax1 * cos(glm::radians(Bx1 * angle)) + Ax2 * cos(glm::radians(Bx2 * angle));
			this->offset.y = Ay1 * sin(glm::radians(By1 * angle)) + Ay2 * sin(glm::radians(By2 * angle));
		}
		void setParametersX(float A1, float B1, float A2, float B2) {
			this->Ax1 = A1;
			this->Bx1 = B1;
			this->Ax2 = A2;
			this->Bx2 = B2;
		}
		void setParametersY(float A1, float B1, float A2, float B2) {
			this->Ay1 = A1;
			this->By1 = B1;
			this->Ay2 = A2;
			this->By2 = B2;
		}
	};



	// ============================ SCENE ==================================
	//	May have several entities, and scenes are controlled by a SceneManager

	/**
	* The Scene class is a base class for scenes in a game application. It serves
	* as a parent class for different scenes in the game. It provides basic functionality
	* for managing entities in the scene and handling user input events.
	* 
	* The class is designed as an abstract class and defines several abstract
	* methods that need to be overridden by concrete implementations of the class.
	* The abstract methods are for handling input events such as keyboard, mouse
	* cursor, and mouse button events.
	* 
	* The Scene class also contains a pointer to an EntityManager object, which
	* is responsible for managing entities in the scene.
	*/
	class Scene : public GameAdapter {
	public:
		EntityManager* entityManager;
		virtual ~Scene() {}
		virtual void doInput(int key, int scancode, int action, int mods) = 0;
		virtual void doCursor(double x, double y) = 0;
		virtual void doMouseButton(int button, int action, int mods) = 0;

	};

	/**
	* The SceneManager class is a GameAdapter class that manages different scenes in
	* a game. It holds a pointer to the current scene and provides the necessary methods
	* for managing its initialization, update, draw, input, cursor and mouse button events.
	* The class also has methods for getting the current scene, setting a new scene and
	* freeing the memory of the old scene. The SceneManager class ensures that the current
	* scene is always initialized and updated correctly.
	*/
	class SceneManager : public GameAdapter {

	public:

		Scene* scene = NULL;
		SceneManager() {}
		~SceneManager() {
			if (scene) {
				delete scene;
				scene = NULL;
			}
		}
		void init() { scene->init(); }
		void update(double deltaTime) { scene->update(deltaTime); }
		void draw() { scene->draw(); }
		void doInput(int key, int scancode, int action, int mods) { scene->doInput(key, scancode, action, mods); }
		void doCursor(double x, double y) { scene->doCursor(x, y); }
		void doMouseButton(int button, int action, int mods) { scene->doMouseButton(button, action, mods); }
		Scene* get() { return scene; }
		void set(Scene* scn) {
			if (scene) {
				delete scene;
				scene = NULL;
			}
			scene = scn;
			scene->init();
		}
	};

}

