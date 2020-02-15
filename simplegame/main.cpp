// Simple Game
// by Dr. Jonathan Metzgar et al
// UAF CS Game Design and Architecture Course
#include <gamelib.hpp>
#include "Game.hpp"

#pragma comment(lib, "gamelib.lib")

//////////////////////////////////////////////////////////////////////
// PROTOTYPES ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void testSprites(GameLib::Context& context,
	int spriteCount,
	int& spritesDrawn,
	SDL_Texture* testPNG,
	SDL_Texture* testJPG);



int main(int argc, char** argv) {
	Game game;
	game.main(argc, argv);
	return 0;
}

void testSprites(GameLib::Context& context,
	int spriteCount,
	int& spritesDrawn,
	SDL_Texture* testPNG,
	SDL_Texture* testJPG) {
	// if (context.keyboard.scancodes[SDL_SCANCODE_ESCAPE]) {
	//    context.quitRequested = true;
	//}

	// An arbitrary number roughly representing 4k at 8 layers, 32x32 sprites
	// constexpr int SpritesToDraw = 128 * 72 * 8;
	// An arbitrary number roughly representing HD at 4 layers, 32x32 sprites
	constexpr int SpritesToDraw = 5;
	// 60 * 34 * 4;
	for (int i = 0; i < SpritesToDraw; i++) {
		GameLib::SPRITEINFO s;
		s.position = { rand() % 1280, rand() % 720 };
		s.center = { 0.0f, 0.0f };
		s.flipFlags = 0;
		s.angle = (float)(rand() % 360);
		// context.drawTexture(0, rand() % spriteCount, s);
		s.position = { rand() % 1280, rand() % 720 };
		context.drawTexture(s.position, 0, rand() % spriteCount);
	}
	spritesDrawn += SpritesToDraw;

	context.drawTexture({ 50, 0 }, { 100, 100 }, testPNG);
	context.drawTexture({ 250, 250 }, { 100, 100 }, testJPG);
}
