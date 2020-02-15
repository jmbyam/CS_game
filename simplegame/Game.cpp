#include "Game.hpp"
#include "DungeonActorComponent.hpp"
#include "NewtonPhysicsComponent.hpp"

void Game::init() {
	GameLib::Locator::provide(&context);
	if (context.audioInitialized())
		GameLib::Locator::provide(&audio);
	GameLib::Locator::provide(&input);
	GameLib::Locator::provide(&graphics);
	GameLib::Locator::provide(&world);

	PlaySoundCommand play0(0, false);
	PlaySoundCommand play1(1, false);
	PlaySoundCommand play2(2, false);
	PlaySoundCommand play3(3, false);
	PlayMusicCommand playMusic1(0);
	PlayMusicCommand playMusic2(1);
	PlayMusicCommand playMusic3(2);

	input.back = &quitCommand;
	input.key1 = &play0;
	input.key2 = &play1;
	input.key3 = &play2;
	input.key4 = &play3;
	input.key5 = &playMusic1;
	input.key6 = &playMusic2;
	input.key7 = &playMusic3;
	input.axis1X = &xaxisCommand;
	input.axis1Y = &yaxisCommand;
}


void Game::kill() {
	double totalTime = stopwatch.stop_s();
	HFLOGDEBUG("Sprites/sec = %5.1f", spritesDrawn / totalTime);
	HFLOGDEBUG("Frames/sec = %5.1f", frames / totalTime);

	actorPool.clear();
}


void Game::main(int argc, char** argv) {
	init();
	loadData();
	showIntro();
	initLevel(1);
	if (playGame()) {
		showWonEnding();
	} else {
		showLostEnding();
	}
	kill();
}


void Game::loadData() {
	for (auto sp : searchPaths) {
		context.addSearchPath(sp);
	}
	SDL_Texture* testPNG = context.loadImage("godzilla.png");
	SDL_Texture* testJPG = context.loadImage("parrot.jpg");
	graphics.setTileSize(32, 32);
	int spriteCount = context.loadTileset(0, 32, 32, "Tiles32x32.png");
	if (!spriteCount) {
		HFLOGWARN("Tileset not found");
	}
	context.loadTileset(GameLib::LIBXOR_TILESET32, 32, 32, "LibXORColors32x32.png");

	context.loadAudioClip(0, "starbattle-bad.wav");
	context.loadAudioClip(1, "starbattle-dead.wav");
	context.loadAudioClip(2, "starbattle-endo.wav");
	context.loadAudioClip(3, "starbattle-exo.wav");
	context.loadAudioClip(4, "starbattle-ok.wav");
	context.loadAudioClip(5, "starbattle-pdead.wav");
	context.loadMusicClip(0, "starbattlemusic1.mp3");
	context.loadMusicClip(1, "starbattlemusic2.mp3");
	context.loadMusicClip(2, "distoro2.mid");

	gothicfont.load("fonts-japanese-gothic.ttf", 36);
	minchofont.load("fonts-japanese-mincho.ttf", 36);

	worldPath = context.findSearchPath(worldPath);
	if (!world.load(worldPath)) {
		HFLOGWARN("world.txt not found");
	}
}


void Game::initLevel(int levelNum) {
	auto NewDungeonActor = []() { return std::make_shared<GameLib::DungeonActorComponent>(); };
	auto NewInput = []() { return std::make_shared<GameLib::SimpleInputComponent>(); };
	auto NewRandomInput = []() { return std::make_shared<GameLib::RandomInputComponent>(); };
	auto NewActor = []() { return std::make_shared<GameLib::ActorComponent>(); };
	auto NewPhysics = []() { return std::make_shared<GameLib::SimplePhysicsComponent>(); };
	auto NewNewtonPhysics = []() { return std::make_shared<GameLib::NewtonPhysicsComponent>(); };
	auto NewGraphics = []() { return std::make_shared<GameLib::SimpleGraphicsComponent>(); };
	auto NewDebugGraphics = []() { return std::make_shared<GameLib::DebugGraphicsComponent>(); };

	float cx = world.worldSizeX * 0.5f;
	float cy = world.worldSizeY * 0.5f;
	float speed = (float)graphics.getTileSizeX();

	GameLib::ActorPtr actor;
	actor = _makeActor(cx - 6, cy, 4, 2, NewInput(), NewDungeonActor(), NewNewtonPhysics(), NewGraphics());
	world.addDynamicActor(actor);

	actor = _makeActor(cx + 6, cy + 4, 4, 32, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addStaticActor(actor);

	actor = _makeActor(cx + 10, cy - 4, 4, 32, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addStaticActor(actor);

	actor = _makeActor(16, 6, 4, 100, NewRandomInput(), NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addDynamicActor(actor);

	actor = _makeActor(6, 6, 4, 101, NewRandomInput(), NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addDynamicActor(actor);

	actor = _makeActor(10, 10, 4, 102, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addTriggerActor(actor);

	actor = _makeActor(19, 10, 4, 103, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addTriggerActor(actor);

	// Trace Curtis Actors

	// Some extras
	auto randomPlayer1 = _makeActor(cx - 3,
		cy,
		speed,
		1,
		std::make_shared<GameLib::RandomInputComponent>(),
		std::make_shared<GameLib::DungeonActorComponent>(),
		std::make_shared<GameLib::TraceCurtisDynamicActorComponent>(),
		std::make_shared<GameLib::SimpleGraphicsComponent>());
	world.addDynamicActor(randomPlayer1);
	randomPlayer1->rename("randomPlayer1");

	auto randomPlayer2 = _makeActor(cx - 6,
		cy,
		speed,
		3,
		std::make_shared<GameLib::RandomInputComponent>(),
		std::make_shared<GameLib::DainNickJosephWorldCollidingActorComponent>(),
		std::make_shared<GameLib::DainNickJosephWorldPhysicsComponent>(),
		std::make_shared<GameLib::SimpleGraphicsComponent>());
	world.addDynamicActor(randomPlayer2);
	randomPlayer2->rename("randomPlayer2");

	auto randomPlayer3 = _makeActor(cx - 6,
		cy - 3,
		speed,
		4,
		std::make_shared<GameLib::RandomInputComponent>(),
		std::make_shared<GameLib::ActorComponent>(),
		std::make_shared<GameLib::SimplePhysicsComponent>(),
		std::make_shared<GameLib::SimpleGraphicsComponent>());
	world.addDynamicActor(randomPlayer3);
	randomPlayer3->rename("randomPlayer3");

	auto Tailon = _makeActor(cx,
		cy - 9,
		speed,
		300,
		std::make_shared<GameLib::InputComponentForDynamic>(),
		std::make_shared<GameLib::TailonsDynamicCollidingActorComponent>(),
		std::make_shared<GameLib::TailonsDynamicPhysicsComponent>(),
		std::make_shared<GameLib::SimpleGraphicsComponent>());
	world.addDynamicActor(Tailon);
	Tailon->rename("Tailon");

	auto Tailon2 = _makeActor(cx - 8,
		cy - 9,
		speed,
		30,
		std::make_shared<GameLib::InputComponentForStatic>(),
		std::make_shared<GameLib::TailonsStaticCollidingActorComponent>(),
		std::make_shared<GameLib::TailonsStaticPhysicsComponent>(),
		std::make_shared<GameLib::SimpleGraphicsComponent>());
	world.addStaticActor(Tailon2);
	Tailon2->rename("Tailon2");
}


void Game::showIntro() {}


void Game::showWonEnding() {}


void Game::showLostEnding() {}


void Game::startTiming() {
	t0 = stopwatch.stop_sf();
	lag = 0.0f;
}


void Game::updateTiming() {
	t1 = stopwatch.stop_sf();
	dt = t1 - t0;
	t0 = t1;
	GameLib::Context::deltaTime = dt;
	GameLib::Context::currentTime_s = t1;
	GameLib::Context::currentTime_ms = t1 * 1000;
	lag += dt;
}


bool Game::playGame() {
	stopwatch.start();
	startTiming();
	world.start(t0);
	bool gameWon = false;
	bool gameOver = false;
	while (!context.quitRequested && !gameOver) {
		updateTiming();

		context.getEvents();
		input.handle();
		_debugKeys();

		while (lag >= Game::MS_PER_UPDATE) {
			updateWorld();
			lag -= Game::MS_PER_UPDATE;
		}

		context.clearScreen(backColor);
		drawWorld();
		drawHUD();

		context.swapBuffers();
		frames++;
		std::this_thread::yield();
	}

	return gameWon;
}


void Game::updateWorld() {
	world.update(Game::MS_PER_UPDATE);
	world.physics(Game::MS_PER_UPDATE);
}


void Game::drawWorld() {
	for (int x = 0; x < world.worldSizeX; x++) {
		for (int y = 0; y < world.worldSizeY; y++) {
			GameLib::SPRITEINFO s;
			s.position = { x * 32, y * 32 };
			auto t = world.getTile(x, y);
			context.drawTexture(s.position, 0, t.charDesc);
		}
	}

	world.draw(graphics);
}


void Game::drawHUD() {
	minchofont.draw(0, 0, "Hello, world!", GameLib::Red, GameLib::Font::SHADOWED);
	gothicfont.draw((int)graphics.getWidth(),
		0,
		"Hello, world!",
		GameLib::Blue,
		GameLib::Font::HALIGN_RIGHT | GameLib::Font::SHADOWED);

	int x = (int)graphics.getCenterX();
	int y = (int)graphics.getCenterY() >> 1;
	float s = GameLib::wave(t1, 1.0f);
	SDL_Color c = GameLib::MakeColorHI(7, 4, s, false);
	minchofont.draw(x,
		y,
		"Collisions",
		c,
		GameLib::Font::SHADOWED | GameLib::Font::HALIGN_CENTER | GameLib::Font::VALIGN_CENTER | GameLib::Font::BOLD |
			GameLib::Font::ITALIC);

	minchofont.draw(0,
		(int)graphics.getHeight() - 2,
		"HP: 56",
		GameLib::Gold,
		GameLib::Font::VALIGN_BOTTOM | GameLib::Font::SHADOWED);

	char fpsstr[64] = { 0 };
	snprintf(fpsstr, 64, "%3.2f", 1.0f / dt);
	minchofont.draw((int)graphics.getWidth(),
		(int)graphics.getHeight() - 2,
		fpsstr,
		GameLib::Gold,
		GameLib::Font::HALIGN_RIGHT | GameLib::Font::VALIGN_BOTTOM | GameLib::Font::SHADOWED);
}


void Game::_debugKeys() {
	if (context.keyboard.checkClear(SDL_SCANCODE_F5)) {
		if (!world.load(worldPath)) {
			HFLOGWARN("world.txt not found");
		}
	}
}
