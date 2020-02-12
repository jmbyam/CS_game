#include "pch.h"
#include <gamelib_actor.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
	namespace Tokens {
#define WORLD_TOKENS(ENUM)                                                                                             \
	ENUM(WORLDSIZE)                                                                                                    \
	ENUM(WORLD)                                                                                                        \
	ENUM(DEFINE)                                                                                                       \
	ENUM(COLLIDE)                                                                                                      \
	ENUM(NOCOLLIDE)
#define ENUM_VAL(x) x,
#define ENUM_MAP(x) {#x, Tiles::x},

		enum class Tiles { WORLD_TOKENS(ENUM_VAL) };
		std::map<std::string, Tiles> worldTokens{WORLD_TOKENS(ENUM_MAP)};
		std::map<char, unsigned> mapCell{};
		char nocollide = '.';
	} // namespace Tokens

	World::World() {
		resize(worldSizeX, worldSizeY);
	}

	World::~World() {
		tiles.clear();
		collisionTiles.clear();
		dynamicActors.clear();
		staticActors.clear();
		triggerActors.clear();
	}

	void World::resize(unsigned sizeX, unsigned sizeY) {
		unsigned numTiles = sizeX * sizeY;
		tiles.resize(numTiles);
		worldSizeX = sizeX;
		worldSizeY = sizeY;
		collisionTiles.resize(numTiles * CollisionTileResolution);
	}

	void World::start() {
		for (Actor* a : triggerActors) {
			a->makeTrigger();
			a->beginPlay();
		}
		for (Actor* a : staticActors) {
			a->makeStatic();
			a->beginPlay();
		}
		for (Actor* a : dynamicActors) {
			a->makeDynamic();
			a->beginPlay();
		}
	}

	void World::update(float deltaTime) {
		for (auto& actor : triggerActors) {
			if (!actor->active)
				continue;
			actor->update(deltaTime, *this);
		}
		for (auto& actor : staticActors) {
			if (!actor->active)
				continue;
			actor->update(deltaTime, *this);
		}
		for (auto& actor : dynamicActors) {
			if (!actor->active)
				continue;
			actor->update(deltaTime, *this);
		}
	}

	void World::physics(float deltaTime) {
		for (auto& actor : staticActors) {
			actor->physics(deltaTime, *this);
		}
		for (auto& actor : dynamicActors) {
			if (!actor->active)
				continue;
			actor->physics(deltaTime, *this);
		}
	}

	void World::draw(Graphics& graphics) {
		for (auto& actor : staticActors) {
			if (!actor->active || !actor->visible)
				continue;
			actor->draw(graphics);
		}
		for (auto& actor : dynamicActors) {
			if (!actor->active || !actor->visible)
				continue;
			actor->draw(graphics);
		}
		for (auto& actor : triggerActors) {
			if (!actor->active || !actor->visible)
				continue;
			actor->draw(graphics);
		}
	}

	void World::addDynamicActor(Actor* a) {
		a->makeDynamic();
		dynamicActors.push_back(a);
	}

	void World::addStaticActor(Actor* a) {
		a->makeStatic();
		staticActors.push_back(a);
	}

	void World::addTriggerActor(Actor* a) {
		a->makeTrigger();
		triggerActors.push_back(a);
	}


	void World::setTile(int x, int y, Tile tile) {
		if (x >= worldSizeX || y >= worldSizeY)
			return;
		auto index = y * worldSizeX + x;
		tiles[index] = std::move(tile);
	}

	Tile& World::getTile(int x, int y) {
		static Tile t;
		if (x >= worldSizeX || y >= worldSizeY)
			return t;
		auto index = y * worldSizeX + x;
		return tiles[index];
	}

	const Tile& World::getTile(int x, int y) const {
		static Tile t;
		if (x >= worldSizeX || y >= worldSizeY)
			return t;
		auto index = y * worldSizeX + x;
		return tiles[index];
	}

	int World::getCollisionTile(float x, float y) const {
		int ix = (int)(CollisionTileResolution * x);
		int iy = (int)(CollisionTileResolution * y);
		ix = clamp<int>(ix, 0, collisionSizeX - 1);
		iy = clamp<int>(iy, 0, collisionSizeY - 1);
		int index = iy * collisionSizeX + ix;
		return collisionTiles[index];
	}

	void World::setCollisionTile(float x, float y, int value) {
		int ix = (int)(CollisionTileResolution * x);
		int iy = (int)(CollisionTileResolution * y);
		ix = clamp<int>(ix, 0, collisionSizeX - 1);
		iy = clamp<int>(iy, 0, collisionSizeY - 1);
		int index = iy * collisionSizeX + ix;
		collisionTiles[index] = value;
	}

	std::istream& World::readCharStream(std::istream& s) {
		std::string cmd;
		s >> cmd;
		std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
		if (!Tokens::worldTokens.count(cmd))
			return s;
		Tokens::Tiles token = Tokens::worldTokens[cmd];
		switch (token) {
		case Tokens::Tiles::WORLDSIZE:
			unsigned w, h;
			s >> w;
			s >> h;
			resize(w, h);
			break;
		case Tokens::Tiles::WORLD:
			int row;
			s >> row;
			for (int i = 0; i < worldSizeX; i++) {
				char c;
				s >> c;
				unsigned val = c;
				if (Tokens::mapCell.count(c))
					val = Tokens::mapCell[val];
				setTile(i, row, Tile(val));
				getTile(i, row).flags = (c == Tokens::nocollide) ? Tile::EMPTY : Tile::SOLID;
			}
			break;
		case Tokens::Tiles::NOCOLLIDE:
			s >> Tokens::nocollide;
			break;
		case Tokens::Tiles::DEFINE:
			char c;
			unsigned val;
			s >> c;
			s >> val;
			Tokens::mapCell[c] = val;
			break;
		default:
			HFLOGWARN("cmd '%' not implemented", cmd.c_str());
		}
		return s;
	}

	std::ostream& World::writeCharStream(std::ostream& s) const {
		// TODO: rows should be written out with characters representing background tiles
		// Main Idea is to use a command with parameters
		// Example:
		// # tile is used to remap a character to a sprite index
		// tile # 34
		// tile A 35
		// # actor define type char NAME
		// # actor text TEXT
		// actor define NPC a "Justine Shellock"
		// actor state BossAlive
		// actor text "Howdy, how are you?"
		// actor text "Have you heard about the monolith?"
		// actor state BossDead
		// actor text "Oh thank you! Now I can go back to carpentry!"
		// actor define NPC b "Samson Hallock"
		// actor state BossAlive
		// actor text "It arrived at midnight the day before"
		// actor text "It makes us sad, can you destroy it?"
		// actor state BossDead
		// actor text "The Monolith is dead. We can not be at peace."
		// actor state BossDead
		// actor define BOSS B "Monolith"
		// actor state BossAlive
		// actor text "What? You think you can destroy me?!"
		// actor text "Ha! Ha! Ha! Do your worst!"
		// actor state BossDead
		// actor text "Oh no! What a world!"
		// # world N ... is the information for row N
		// # the space character is the value 0, or no tile
		// # the < character is the player start position
		// # the > character is the player end position
		// worldsize width height
		// world 0 #######################
		// world 1 #      a              #
		// world 2 #  AAAAA     b        #
		// world 3 #<     #  AAAA     B >#
		// world 4 #######################

		std::map<unsigned int, char> cellToChar;
		for (auto& [k, v] : Tokens::mapCell) {
			s << "define " << k << " " << v;
			cellToChar[v] = k;
		}

		s << "worldsize " << worldSizeX << " " << worldSizeY << "\n";
		for (int y = 0; y < worldSizeY; ++y) {
			s << "world " << std::setw(2) << y << " ";
			for (int x = 0; x < worldSizeX; ++x) {
				auto t = getTile(x, y);
				if (cellToChar.count(t.charDesc)) {
					s << cellToChar[t.charDesc];
				} else {
					s << (char)t.charDesc;
				}
			}
			s << "\n";
		}
		return s;
	}
} // namespace GameLib
