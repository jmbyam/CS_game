#include "gamelib_story_screen.hpp"
#include <algorithm>
#include <cmath>
#include <gamelib_locator.hpp>

namespace GameLib {
	StoryScreen::StoryScreen() {
		Context* context = Locator::getContext();
		screenWidth = (float)context->screenWidth;
		screenHeight = (float)context->screenHeight;
		screenSize = std::min(screenWidth, screenHeight);
	}


	void StoryScreen::play() {
		bool donePlaying = false;
		while (!donePlaying) {
			updateFrame();
			drawFrame();
		}
	}


	void StoryScreen::updateFrame() {}


	void StoryScreen::drawFrame() {}


	bool StoryScreen::readStream(std::istream& is) {
		int actorCount;
		is >> actorCount;
		actors.clear();

		for (int i = 0; i < actorCount; i++) {
			std::string actorName;
			std::getline(is, actorName);
			actors[actorName] = makeActor(actorName,
				std::make_shared<InputComponent>(),
				std::make_shared<ActorComponent>(),
				std::make_shared<PhysicsComponent>(),
				std::make_shared<GraphicsComponent>());
			auto actor = actors[actorName];

			// get name of image (a PNG or JPG)
			std::getline(is, actor->imageName);
			if (!context->loadImage(actor->imageName)) {
				HFLOGERROR("Can't load image '%s'", actor->imageName.c_str());
				exit(0);
			}

			std::string line;
			std::getline(is, line);
			std::istringstream istr(line);
			istr >> actor->size.x;
			istr >> actor->size.y;
			actor->size *= screenSize;
		}

		dialogue.clear();
		int duration = 0;
		while (is) {
			int duration;
			is >> duration;
			if (duration < 0)
				break;
			Dialogue d;
			d.duration = duration;
			std::getline(is, d.actorName);

			int size;
			is >> size;
			d.lines.resize(size);
			for (std::string& line : d.lines) {
				std::getline(is, line);
			}
			dialogue.push_back(d);
		}
		return is.eof();
	}


	void StoryScreen::writeStream(std::ostream& os) {
		os << actors.size() << "\n";
		for (auto& [k, actor] : actors) {
			os << actor->name() << "\n";
			os << actor->imageName << "\n";
			os << std::setprecision(4) << actor->size.x << "\n";
			os << std::setprecision(4) << actor->size.y << "\n";
		}
		for (auto d : dialogue) {
			os << d.duration << "\n";
			os << d.actorName << "\n";
			for (auto line : d.lines) {
				os << line << "\n";
			}
		}
		os << "-1\n";
	}
} // namespace GameLib
