#pragma once
#include"Dago.h"


namespace dago
{
	class FSMComponent;
	class FSMState {
	public:
		int id = 0;
		FSMComponent* parent = NULL; //who owns us

		FSMState() {}
		virtual void onEnter() = 0;
		virtual void update() = 0;
		virtual void onExit() = 0;
		virtual ~FSMState() {}

		template <typename T> void changeState() {
			this->parent->previousState = this->parent->currentState;
			this->parent->previousState->onExit();
			this->parent->currentState = this->parent->getState<T>();
			this->parent->currentState->onEnter();
		}
		template <typename T> void changeToBlipState() {
			this->parent->previousState = this->parent->currentState;
			this->parent->currentState = this->parent->getState<T>();
			this->parent->currentState->onEnter();
		}

		bool equals(FSMState* other) {
			return typeid(*this) == typeid(*other);
		}
	};

	class FSMComponent : public Component { // Finite States Machine
	public:
		Entity* parent = NULL;
		FSMState* currentState = NULL;
		FSMState* previousState = NULL;
		std::unordered_map<std::type_index, FSMState*> states;
		std::vector<Message> messages;
		float timer = 0.0f;
		float TIME_TO_RUN = 4.0f;
		bool showID = true;
		int colorText = rlutil::WHITE;

		FSMComponent() { }
		~FSMComponent() {
			for (auto S : states)
				if (S.second) {
					delete S.second;
					S.second = NULL;
				}
		}

		void returnToPreviousState() {
			this->currentState->onExit();
			auto temp = this->currentState;
			this->currentState = this->previousState;
			this->previousState = temp;
		}

		template <typename T> bool addState(T* state) {
			if (this->states.count(typeid(T))) {
				std::cout << "cant't add twice the same state " << std::endl;
				return false;
			}
			state->parent = this;
			state->id = this->states.size();
			this->states[typeid(T)] = state;
			return true;
		}

		template <typename T> T* getState() {
			assert(this->states.count(typeid(T)) != 0, "The state doesn't exists in this map");
			return (T*)this->states[typeid(T)];
		}

		void setState(FSMState* state) {
			this->currentState = state;
			state->onEnter();
		}

		FSMState* getStateById(int id) {
			for (auto pair : states)
				if (pair.second->id == id)
					return pair.second;
		}


		int getStatesSize() {
			return this->states.size();
		}

		void update(double deltaTime) {
			if (timer > 0) timer -= 1.0f * deltaTime;
			if (timer > 0) return;
			timer = TIME_TO_RUN;
			this->currentState->update();
		}

		void setupSubscriptions() {
			this->parent->parent->subscribe(this->parent->id, 0);
		}

		void print(std::string msg) {
			rlutil::setColor(this->colorText);
			std::cout << this->parent->name;
			if(showID) std::cout << " ("<<this->parent->id<< ")";
			rlutil::setColor(rlutil::WHITE);
			std::cout << ": " << msg << std::endl;
		}

	};
	
}