#pragma once
#include"Dago.h"

namespace dago
{
	class AStarNode {
	public:
		AStarNode* parent = NULL;

		int x = 0, y = 0, g = 0, h = 0, f = 0;

		AStarNode(int x, int y, int g, int h, AStarNode* parent) {
			this->x = x;
			this->y = y;
			this->g = g;
			this->h = h;
			this->f = g + h;
			this->parent = parent;
		}
		~AStarNode() {}
		void set(int x, int y, int g, int h, AStarNode* parent) {
			this->x = x;
			this->y = y;
			this->g = g;
			this->h = h;
			this->f = g + h;
			this->parent = parent;
		}
	};

	class PathNode {
	public:
		glm::vec2 goal_position;
		glm::vec2 velocity;
		PathNode(AStarNode* node) {
			this->velocity.x = node->x - node->parent->x;
			this->velocity.y = node->y - node->parent->y;
			this->goal_position = glm::vec2(node->x, node->y);
		}
	};

	class AStarComponent : public Component {
	public:
		float EPSILON = 0.05;
		// Positions
		glm::vec2 start;
		glm::vec2 end;
		
		bool routeFound = false;

		int dx[4] = { 0, 1, 0, -1 }; // possible x-axis movements
		int dy[4] = { -1, 0, 1, 0 }; // possible y-axis movements

		std::vector<bool> collision; // CloseList - BreadCrumb
		std::vector<AStarNode*> openList;

		int currentPathIndex = 0;
		std::vector<PathNode*> path;

		AStarNode* current = NULL;
		AStarNode* previous = NULL;

		AStarComponent() {}
		~AStarComponent() {

		}

		void init() {
			this->start = this->parent->getComponent<Position>()->get();
			this->end = this->parent->getComponent<Position>()->get();
		}
		void update(double deltaTime) {
			if (this->start == this->end) return;
			if (!this->routeFound) run();
			updateOnANode(deltaTime);
		}

		void run() {
			this->collision = this->parent->parent->collisionMap;
			this->openList.push_back(new AStarNode(this->start.x, this->start.y, 0, dist(this->start, this->end), NULL));
			while (openList.size() != 0) {
				//save_collision_map();
  				this->current = LowestF_OpenList();

				// Erase current node from the openList
				this->openList.erase(std::remove(this->openList.begin(), this->openList.end(), current), this->openList.end());
				add_breadCrumb(current->x, current->y);

				if (currentNodeIsTheGoal())
				{
					buildPath();
					this->routeFound = true;
					auto velocity = this->parent->getComponent<dago::Velocity>();
					velocity->set(this->path[this->currentPathIndex]->velocity);
					return;
				}
				
				// Iterate through all possible bounds
				for (int index = 0; index < 4; index++) {
 					int newX = this->current->x + dx[index];
					int newY = this->current->y + dy[index];
					if (!in_bounds(newX, newY) || is_collision(newX, newY)) continue;
					if (!is_in_openList(newX, newY)) {
						this->openList.push_back(new AStarNode(newX, newY, this->current->g + 1, dist(glm::vec2(newX, newY), this->end), this->current));
					}	
					else if (is_better_than_previous(this->current->g + 1, dist(glm::vec2(newX, newY), this->end))){
						this->previous->set(newX, newY, this->current->g + 1, dist(glm::vec2(newX, newY), this->end), this->current);
					}
				}
			}
		}

	private:
		bool updateOnANode(double deltaTime) {
			auto position = this->parent->getComponent<Position>();
			//std::cout << "Current Position X: " << position->get().x << "     Y: " << position->get().y;
			auto distance = glm::length(position->get() - this->path[this->currentPathIndex]->goal_position);
			//std::cout << "   Distance to goal : " << distance << std::endl;
			if (distance < EPSILON) { // Got into a new Node
				//std::cout << "Hey!!! I got into the new Node " << std::endl;
				position->set(this->path[this->currentPathIndex]->goal_position);
				this->currentPathIndex++;
				if (this->currentPathIndex == this->path.size()) {
					this->start = this->end;
					this->routeFound = false;
					this->path.clear();
					this->openList.clear();
					this->currentPathIndex = 0;
					this->parent->getComponent<dago::Position>()->set(this->end);
					this->parent->getComponent<dago::Velocity>()->set(0, 0);
					return false;
				}
				this->parent->getComponent<dago::Velocity>()->set(this->path[this->currentPathIndex]->velocity);
			}
			return true;
		}

		// Manhattan Distance
		int dist(glm::vec2 in, glm::vec2 out) {
			return dist(in.x,in.y,out.x,out.y);
		}
		int dist(int x1, int y1, int x2, int y2) {
			return abs(x1 - x2) + abs(y1 - y2);
		}
		AStarNode* LowestF_OpenList() {
			if (this->openList.size() == 1) return this->openList[0];

			// Find the lowest F Score
			int lowestF = 0, counter = 1;
			for (int currentNode = 1; currentNode < this->openList.size(); currentNode++) {
				if (this->openList[currentNode]->f < this->openList[lowestF]->f) {
					lowestF = currentNode;
					counter = 1;
				}
				if (this->openList[currentNode]->f == this->openList[lowestF]->f) {
					counter++;
				}
			}
			
			if (counter == 1) return this->openList[lowestF];

			// Find the lowest H if there are more than one LowestF
			int lowestH = lowestF;
			for (int currentNode = 0; currentNode < this->openList.size(); currentNode++) {
				if (this->openList[currentNode]->f == this->openList[lowestH]->f) {
					if (this->openList[currentNode]->h < this->openList[lowestH]->h) {
						lowestH = currentNode;
					}
				}
			}
			return this->openList[lowestH];
		}

		bool currentNodeIsTheGoal() {
			return (this->current->x == this->end.x && this->current->y == this->end.y);
		}

		bool in_bounds(int x, int y) {
			return x >= 0 && x < TileMap::COLUMNS && y >= 0 && y < TileMap::ROWS;
		}

		bool is_collision(int x, int y) {
			return collision[y * TileMap::COLUMNS + x];
		}

		bool is_in_openList(int x, int y) {
			for (AStarNode* node : this->openList)
				if (x == node->x && y == node->y) {
					this->previous = node;
					return true;
				}
			return false;
		}
		bool is_better_than_previous(int g, int h) {
			if ((g+h) < this->previous->f) return true;
			return false;
		}

		void add_breadCrumb(int x, int y) {
			collision[y * TileMap::COLUMNS + x] = true;
		}

		void buildPath() {
			while (this->current->parent) {
				path.insert(path.begin(), new PathNode(this->current));
				this->current = this->current->parent;
			}
		}

		void save_collision_map() {
			std::ofstream file;
			file.open("current_collision.txt");

			for (int y = 0; y < TileMap::ROWS; y++)
			{
				for (int x = 0; x < TileMap::COLUMNS; x++)
				{
					file << collision[(TileMap::COLUMNS * y) + x] << " ";
				}
				file << "\n";
			}

		}
	};

}