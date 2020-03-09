#include "Game.hpp"
#include "DungeonActorComponent.hpp"
#include "NewtonPhysicsComponent.hpp"
#include <gamelib_story_screen.hpp>

constexpr int SOUND_BLIP = 6;

void Game::init() {
	GameLib::Locator::provide(&context);
	if (context.audioInitialized())
		GameLib::Locator::provide(&audio);
	GameLib::Locator::provide(&input);
	GameLib::Locator::provide(&graphics);
	GameLib::Locator::provide(&world);
	GameLib::Locator::provide(&box2d);

	box2d.init();

	audio.setVolume(0.2f);

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
	input.start = &shakeCommand;
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
	graphics.setTileSize({ 32, 32 });
	int spriteCount = context.loadTileset(0, 32, 32, "Pilot.png");
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
	context.loadAudioClip(SOUND_BLIP, "blip.wav");
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
	actor = _makeActor(cx + 2, cy, 16, 0, NewInput(), NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addDynamicActor(actor);

	actor = _makeActor(10, 10, 4, 4, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
	world.addTriggerActor(actor);
    
    actor = _makeActor(31, 10, 4, 4, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
    world.addTriggerActor(actor);
    
    actor = _makeActor(56, 2, 4, 4, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
    world.addTriggerActor(actor);
    
    actor = _makeActor(74, 8, 4, 4, nullptr, NewDungeonActor(), NewPhysics(), NewGraphics());
    world.addTriggerActor(actor);

}


void Game::showIntro() {
	
}


void Game::showWonEnding() {}


void Game::showLostEnding() {
	
}


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


void Game::shake() {
	if (nextShakeTime > endShakeTime)
		return;
	nextShakeTime += shakeDt;
	using GameLib::random;
	glm::ivec2 screenShake{ random.between(-shakeAmount, shakeAmount), random.between(-shakeAmount, shakeAmount) };
	graphics.setOffset(screenShake);
}


void Game::shake(int amount, float timeLength, float dt) {
	if (amount == 0) {
		graphics.setOffset({ 0, 0 });
		return;
	}
	shakeAmount = amount;
	endShakeTime = t1 + timeLength;
	nextShakeTime = t1 + dt;
	shakeDt = dt;
	shake();
}


bool Game::playGame() {
	stopwatch.start();
	startTiming();
	world.start(t0);
	graphics.setCenter(graphics.origin());
	bool gameWon = false;
	bool gameOver = false;
	while (!context.quitRequested && !gameOver) {
		updateTiming();

		context.getEvents();
		input.handle();
		_debugKeys();

		context.clearScreen(backColor);
		world.drawTiles(graphics);
		while (lag >= Game::MS_PER_UPDATE) {
			updateWorld();
			lag -= Game::MS_PER_UPDATE;
		}

		shake();
		updateCamera();
		drawWorld();
		drawHUD();

		context.swapBuffers();
		frames++;
		std::this_thread::yield();
	}

	return gameWon;
}


void Game::updateCamera() {
	glm::ivec2 xy = world.dynamicActors[0]->pixelCenter(graphics);
	glm::ivec2 center = graphics.center();
	center.x = GameLib::clamp(center.x, xy.x - 100, xy.x + 100);
	center.y = GameLib::clamp(center.y, xy.y - 100, xy.y + 100);
	//center.y = std::min(graphics.getCenterY(), center.y);
	graphics.setCenter(center);
}


void Game::updateWorld() {
	world.update(Game::MS_PER_UPDATE);
	world.physics(Game::MS_PER_UPDATE);
}


void Game::drawWorld() {
	world.draw(graphics);
}


void Game::drawHUD() {
	minchofont.draw(0, 0, "Hello, world!", GameLib::Red, GameLib::Font::SHADOWED);
	gothicfont.draw(
		(int)graphics.getWidth(),
		0,
		"Hello, world!",
		GameLib::Blue,
		GameLib::Font::HALIGN_RIGHT | GameLib::Font::SHADOWED);

	int x = (int)graphics.getCenterX();
	int y = (int)graphics.getCenterY() >> 1;
	float s = GameLib::wave(t1, 1.0f);
	SDL_Color c = GameLib::MakeColorHI(7, 4, s, false);
	minchofont.draw(
		x,
		y,
		"Collisions",
		c,
		GameLib::Font::SHADOWED | GameLib::Font::HALIGN_CENTER | GameLib::Font::VALIGN_CENTER | GameLib::Font::BOLD |
			GameLib::Font::ITALIC);

	minchofont.draw(
		0,
		(int)graphics.getHeight() - 2,
		"HP: 56",
		GameLib::Gold,
		GameLib::Font::VALIGN_BOTTOM | GameLib::Font::SHADOWED);

	char fpsstr[64] = { 0 };
	snprintf(fpsstr, 64, "%3.2f", 1.0f / dt);
	minchofont.draw(
		(int)graphics.getWidth(),
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

	if (shakeCommand.checkClear()) {
		shake(4, 5, 50 * MS_PER_UPDATE);
	}
}
