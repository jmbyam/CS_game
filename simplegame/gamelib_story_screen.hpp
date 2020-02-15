#ifndef GAMELIB_STORY_SCREEN_HPP
#define GAMELIB_STORY_SCREEN_HPP

#include <gamelib_actor.hpp>
#include <gamelib_world.hpp>

namespace GameLib {
	// StoryScreen
	// This presents a story screen
	class StoryScreen {
	public:
		StoryScreen();

		void play();

		// writes story screen to output stream
		// format:
		// INT      numberOfActors
		// ARRAY OF
		//   STRING   actorName
		//   STRING   actorImage
		//   FLOAT    actorSizeX % of min(screen W, screen H)
		//   FLOAT    actorSizeY % of min(screen W, screen H)
		// INT      frame duration    If < 0, then no more lines of dialog
		// STRING   actorName
		// INT      imageFlags     1 = HLEFT, 2 = HRIGHT, 3 = HCENTER, 4 = VTOP, 8 = VBOTTOM, 12 = VCENTER
		// INT      textFlags      1 = HLEFT, 2 = HRIGHT, 3 = HCENTER, 4 = COMPLEX
		// INT      animFlags      1 = POSITION, 2 = OFFSET, 2 = SCALE,
		// FLOAT    X1
		// FLOAT    Y1
		// FLOAT    X2
		// FLOAT    Y2
		// INT      OSRL flags     1 = OFFSET, 2 = SCALE, 4 = ROTATION, 8 = LFO
		// FLOAT    OFFSET X1
		// FLOAT    OFFSET Y1
		// FLOAT    OFFSET X2
		// FLOAT    OFFSET Y2
		// FLOAT    SCALE 1
		// FLOAT    SCALE 2
		// FLOAT    SCALE LFO +
		// FLOAT    SCALE LFO *
		// FLOAT    ROTATE ANGLE 1
		// FLOAT    ROTATE ANGLE 2
		// FLOAT    ROTATE LFO +
		// FLOAT    ROTATE LFO *
		// FLOAT    LFO FREQUENCY
		// FLOAT    LFO PHASE OFFSET
		// FLOAT    LFO AMPLITUDE
		// FLOAT    LFO RAMP
		// INT      line count
		// STRING[] lines

		void writeStream(std::ostream& os);

		// returns false if EOF encountered during read
		bool readStream(std::istream& is);

		using string_vector = std::vector<std::string>;

		struct Dialogue {
			static constexpr int NONE = 0;
			static constexpr int OFFSET = 1;
			static constexpr int SCALE = 2;
			static constexpr int ROTATION = 4;
			static constexpr int LFO = 8;

			SDL_Texture* actorSprite{ nullptr };

			std::string actorName;	  // name to display on screen
			int duration;			  // milliseconds
			int imageFlags;			  // 
			int textFlags;			  // 1 = LEFT, 2 = RIGHT, 3 = CENTER
			int animFlags;			  //
			glm::vec2 position1{ 0 }; // position 1 of sprite
			glm::vec2 position2{ 0 }; // position 2 of sprite
			int osrlFlags;			  // offset, scale, rotation, lfo flags
			glm::vec2 offset1;		  // sprite offset 1 for rotation
			glm::vec2 offset2;		  // sprite offset 2 for rotation
			float scale1{ 1 };		  // start scale of sprite
			float scale2{ 1 };		  // end scale of sprite
			float scaleLfoAdd{ 0 };	  // scale + lfoAdd * lfo
			float scaleLfoMul{ 0 };	  // (lfoMul * lfo + 1) * scale
			float rotation1{ 0 };	  // (degrees) start rotation of sprite
			float rotation2{ 0 };	  // (degrees) end rotation of sprite
			float rotLfoAdd{ 0 };	  // rotation + lfoAdd * lfo
			float rotLfoMul{ 0 };	  // (lfoMul * lfo + 1) * rotation
			float lfoAmplitude{ 1 };  // unitless
			float lfoFrequency{ 1 };  // degrees
			float lfoPhase{ 0 };	  // degrees
			float lfoRamp{ 0 };		  // milliseconds
			string_vector lines;
		};

		using dialogue_vector = std::vector<Dialogue>;
		std::map<std::string, ActorPtr> actors;
		dialogue_vector dialogue;
		Dialogue currentFrame;
		Context* context;
		float screenSize;
		float screenWidth;
		float screenHeight;
		bool donePlaying{ false };

		void updateFrame();
		void drawFrame();
	};

} // namespace GameLib

#endif
