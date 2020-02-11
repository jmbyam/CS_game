#ifndef GAMELIB_STORY_SCREEN_HPP
#define GAMELIB_STORY_SCREEN_HPP

#include <gamelib_world.hpp>

namespace GameLib {
	class StoryScreen {
	public:
		StoryScreen();

		std::vector<Actor*> actors;
	};

} // namespace GameLib

#endif
