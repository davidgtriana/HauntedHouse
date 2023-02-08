#pragma once
#include"Dago.h"

#define T_BACKGROUND 0
#define T_FORE1 1
#define T_CHARACTER 2
#define T_FORE2 3
#define T_EMPTY 15


struct Tile{
	int layers[4];

	Tile() {
		this->layers[T_BACKGROUND] = 48;
		this->layers[T_FORE1] = T_EMPTY;
		this->layers[T_CHARACTER] = T_EMPTY;
		this->layers[T_FORE2] = T_EMPTY;
	}
	Tile(int layer, int value) {
		Tile();
		this->layers[layer] = value;
	}
};

class TileMap : public dago::GameAdapter{
public:

	// WHOLE MAP SIZE
	int ROWS = 22; // HEIGHT
	int COLUMNS = 40; // WIDTH

	// CURRENT MAP CANVAS SIZE
	int MAX_ROWS = 20;
	int MAX_COLUMNS = 20;

	// TileSheet Size
	int TS_WIDTH = 512;
	int TS_HEIGHT = 512;

	// TILE
	static const int TILE_SIZE = 32;

	// Offset
	int xOffset = 0;
	int yOffset = 0;

	// Maximum Possible Value of the cursor
	int xMaxCursor = COLUMNS;
	int yMaxCursor = ROWS;

	int xSelected = 0;
	int ySelected = 0;

	std::vector<Tile*> tiles;
	std::unordered_map<int,Sprite*> tileSheet;
	std::unordered_set<int> ignoredTiles;

	TileMap() {
		loadTileSheetIgnoreList();
		loadTileSheet();
	}
	~TileMap() {
		std::cout << "hola ome" << std::endl;
		for (auto tile : tiles) {
			if (tile) {
				delete tile;
			}
		}
	}

	void init() {
		tiles.clear();
		int currentRow, currentColumn;
		for (currentRow = 0; currentRow < ROWS; currentRow++)
			for (currentColumn = 0; currentColumn < COLUMNS; currentColumn++)
				tiles.push_back(new Tile());
		load("house.txt");
	}
	void update(double deltaTime) {

	}
	void draw() {
		int id_tile, currentRow, currentColumn, currentLayer;
		for (currentRow = 0; currentRow < ROWS; currentRow++) {
			for (currentColumn = 0; currentColumn < COLUMNS; currentColumn++) {
				for (currentLayer = T_BACKGROUND; currentLayer <= T_FORE1; currentLayer++) {
					id_tile = tiles[currentColumn + currentRow * COLUMNS]->layers[currentLayer];
					tileSheet[id_tile]->Blit(currentColumn * TILE_SIZE + TILE_SIZE / 2, blit3D->screenHeight - (currentRow * TILE_SIZE + TILE_SIZE / 2));
				}
			}
		}
	}
	void drawLastLayer() {
		int id_tile, currentRow, currentColumn;
		for (currentRow = 0; currentRow < ROWS; currentRow++) {
			for (currentColumn = 0; currentColumn < COLUMNS; currentColumn++) {
				id_tile = tiles[currentColumn + currentRow * COLUMNS]->layers[T_FORE2];
				tileSheet[id_tile]->Blit(currentColumn * TILE_SIZE + TILE_SIZE / 2, blit3D->screenHeight - (currentRow * TILE_SIZE + TILE_SIZE / 2));
			}
		}
	}
	void doInput(int key, int scancode, int action, int mods) {

	}


	void doCursor(double x, double y) {

	}
	void doMouseButton(int button, int action, int mods) {

	}

	void loadTileSheetIgnoreList() {
		std::ifstream fileRead;
		std::string id_tile;

		fileRead.open("Media\\ignore_tiles.txt");
		if (!fileRead) {
			std::cout << "ERROR: It couldn't be opened" << std::endl;
		}
		while (getline(fileRead, id_tile)) {
			ignoredTiles.insert(stoi(id_tile));
		}
	}

	void loadTileSheet() {
		int id_tile = -1, currentRow, currentColumn;
		for (currentRow = 0; currentRow < TS_HEIGHT / TILE_SIZE; currentRow++) {
			for (currentColumn = 0; currentColumn < TS_WIDTH / TILE_SIZE; currentColumn++) {
				id_tile++;
				if (isIgnored(id_tile)) continue;
				tileSheet[id_tile] = blit3D->MakeSprite(currentColumn * TILE_SIZE, currentRow * TILE_SIZE, TILE_SIZE, TILE_SIZE, "Media\\interior.png");
			}
		}
	}

	bool isIgnored(int id) {
		return ignoredTiles.find(id) != ignoredTiles.end();
	}

	void load(std::string path) {
		std::ifstream fileRead;
		std::string number;
		fileRead.open("Media\\" + path);
		if (!fileRead) {
			std::cout << "ERROR: It couldn't be opened" << std::endl;
		}
		int id_tile, currentRow, currentColumn;
		for (currentRow = 0; currentRow < ROWS; currentRow++)
			for (currentColumn = 0; currentColumn < COLUMNS; currentColumn++)
				fileRead >> tiles[currentColumn + currentRow * COLUMNS]->layers[T_BACKGROUND];
		fileRead >> number;//go to next layer

		for (currentRow = 0; currentRow < ROWS; currentRow++)
			for (currentColumn = 0; currentColumn < COLUMNS; currentColumn++)
				fileRead >> tiles[currentColumn + currentRow * COLUMNS]->layers[T_FORE1];

		fileRead >> number;//go to next layer
		for (currentRow = 0; currentRow < ROWS; currentRow++)
			for (currentColumn = 0; currentColumn < COLUMNS; currentColumn++)
				fileRead >> tiles[currentColumn + currentRow * COLUMNS]->layers[T_FORE2];
		fileRead.close();
	}

};