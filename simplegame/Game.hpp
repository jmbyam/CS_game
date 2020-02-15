#ifndef GAME_HPP
#define GAME_HPP

#include "Commands.hpp"
#include <gamelib.hpp>

class Game {
public:
	Game() {}
	~Game() {}

	void init();
	void kill();

	virtual void loadData();
	virtual void initLevel(int levelNum);

	// return true if game won, false if game lost
	virtual bool playGame();
	virtual void updateWorld();
	virtual void drawWorld();
	virtual void drawHUD();

	virtual void showIntro();
	virtual void showLostEnding();
	virtual void showWonEnding();

	void main(int argc, char** argv);

protected:
	virtual void startTiming();
	virtual void updateTiming();

	static constexpr float MS_PER_UPDATE = 0.001f;

	GameLib::Context context{ 1280, 720, GameLib::WindowDefault };
	GameLib::Audio audio;
	GameLib::InputHandler input;
	GameLib::Graphics graphics{ &context };
	GameLib::World world;
	GameLib::Font gothicfont{ &context };
	GameLib::Font minchofont{ &context };
	SDL_Color backColor{ GameLib::Azure };

	std::vector<std::string> searchPaths{ "./assets", "../assets" };
	std::string worldPath{ "world.txt" };
	Hf::StopWatch stopwatch;
	double spritesDrawn{ 0 };
	double frames{ 0 };
	float t0{ 0 };
	float t1{ 0 };
	float dt{ 0 };
	float lag{ 0 };
	std::vector<GameLib::ActorPtr> actorPool;

	QuitCommand quitCommand;
	MovementCommand xaxisCommand;
	MovementCommand yaxisCommand;

	virtual void _debugKeys();

	GameLib::ActorPtr _makeActor(float x,
		float y,
		float speed,
		int spriteId,
		GameLib::InputComponentPtr ic,
		GameLib::ActorComponentPtr ac,
		GameLib::PhysicsComponentPtr pc,
		GameLib::GraphicsComponentPtr gc) {
		auto actor = GameLib::makeActor("actor", ic, ac, pc, gc);
		actor->position.x = x;
		actor->position.y = y;
		actor->speed = speed;
		// actor->size = { 0.75f, 0.5f, 1.0f };
		actor->setSprite(0, spriteId);
		actorPool.push_back(actor);
		return actor;
	}
};

#endif
