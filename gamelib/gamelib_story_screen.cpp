#include "pch.h"
#include <algorithm>
#include <cmath>
#include <gamelib_locator.hpp>
#include <gamelib_story_screen.hpp>

namespace GameLib {
	StoryScreen::StoryScreen() {
		context = Locator::getContext();
		screenWidth = (float)context->screenWidth;
		screenHeight = (float)context->screenHeight;
		screenSize = std::min(screenWidth, screenHeight);
		ptsize = (int)(screenSize / 25.0f);
		for (auto i = 0; i < MAX_FONTS; ++i) {
			fonts[i] = std::make_unique<Font>(context);
		}
	}


	void StoryScreen::setFont(const std::string& path, int which, float size) {
		if (which < 0 || which >= MAX_FONTS)
			return;
		if (!fonts[which]->load(path, (int)size * ptsize)) {
			HFLOGERROR("Font '%s' not found", path.c_str())
		}
		curFont_ = which;
	}


	void StoryScreen::play() {
		curframe = 0;
		Hf::StopWatch stopwatch;

		InputCommand abutton;
		InputCommand enterButton;
		InputCommand escapeButton;
		InputHandler input;
		input.buttonA = &abutton;
		input.start = &enterButton;
		input.back = &escapeButton;
		GameLib::InputHandler* oldinput = Locator::getInput();
		Locator::provide(&input);

		float t0 = stopwatch.stop_msf();
		tickCount = 0;
		_advanceFrame(0);
		while (!donePlaying) {
			context->getEvents();
			input.handle();
			float t1 = stopwatch.stop_sf();
			float dt = t1 - t0;
			lag += dt;
			t0 = t1;
			framePct = tickCount / (float)dialogue[curframe].duration;
			while (lag >= MS_PER_UPDATE) {
				_updateFrame();
				lag -= MS_PER_UPDATE;
				ticksLeft--;
				tickCount++;
			}
			_drawFrame();
			bool a = abutton.checkClear();
			bool b = enterButton.checkClear();
			bool c = escapeButton.checkClear();
			bool anykey = a || b || c;
			if (ticksLeft < 0 || anykey) {
				_advanceFrame(curframe + 1);
				tickCount = 0.0f;
			}
			context->swapBuffers();
		}

		Locator::provide(oldinput);
	}


	bool StoryScreen::load(const std::string& path) {
		std::string foundPath = context->findSearchPath(path);
		std::ifstream fin(foundPath);
		if (!fin)
			return false;
		return readStream(fin);
	}


	bool StoryScreen::save(const std::string& path) {
		std::ofstream fout(path);
		if (!fout)
			return false;
		return writeStream(fout);
	}


	void StoryScreen::newFrame(int duration, int backColor, int textColor) {
		Dialogue d;
		d.duration = duration;
		d.backColor = backColor;
		d.textColor = textColor;
		dialogue.push_back(d);
	}


	void StoryScreen::frameHeader(const std::string& text, int color, int flags, int font) {
		Dialogue& d = dialogue.back();
		d.headerText = text;
		d.headerColor = color;
		d.headerFlags = flags;
		d.headerFont = clamp(font, 0, MAX_FONTS - 1);
	}


	void StoryScreen::frameLine(const std::string& line) {
		Dialogue& d = dialogue.back();
		d.textFont = curFont_;
		d.textFlags = Font::SHADOWED;
		d.lines.push_back(line);
	}


	void StoryScreen::_advanceFrame(int frame) {
		curframe = frame;
		if (curframe >= dialogue.size()) {
			donePlaying = true;
			HFLOGDEBUG("no more frames");
			return;
		}
		ticksLeft = dialogue[curframe].duration;
		Dialogue& d = dialogue[curframe];
		HFLOGDEBUG("Story frame %d for %d ticks", frame, ticksLeft);
		HFLOGDEBUG("Story frame # %s", d.headerText.c_str());
		_reflowText(d);
	}


	void StoryScreen::_updateFrame() {}


	void StoryScreen::_drawFrame() {
		int nextframe = curframe + 1;
		if (nextframe >= dialogue.size())
			nextframe = curframe;

		float fgMix = 1.0f;
		if (framePct < 0.1f)
			fgMix = framePct * 10.0f;
		if (framePct > 0.9f)
			fgMix = 1.0f - (framePct - 0.9f) * 10.0f;
		fgMix = clamp(fgMix, 0.0f, 1.0f);
		float bgMix = framePct > 0.9f ? 1.0f - fgMix : 0.0f;

		Dialogue& d1 = dialogue[curframe];
		Dialogue& d2 = dialogue[nextframe];
		SDL_Color color1 = MakeColor(d1.backColor);
		SDL_Color color2 = MakeColor(d2.backColor);
		SDL_Color backColor = mix(color1, color2, bgMix);
		context->clearScreen(backColor);

		if (!d1.headerText.empty()) {
			SDL_Color fg = mix(backColor, MakeColor(d1.headerColor), fgMix);

			int x = 0;
			int y = 0;
			if (d1.headerFlags & Font::HALIGN_RIGHT)
				x = context->screenWidth;
			if (d1.headerFlags & Font::HALIGN_CENTER)
				x = context->screenWidth >> 1;
			int which = d1.headerFont;
			int flags = d1.headerFlags & 0xFF;
			if (fgMix < 1.0f && flags & Font::SHADOWED)
				flags = flags & ~Font::SHADOWED;
			fonts[which]->draw(x, y, dialogue[curframe].headerText.c_str(), fg, flags);
		}

		// story text
		if (framePct > 0.1f) {
			float pct = 1.0f;
			// if (framePct < 0.2f)
			//	pct = framePct - 0.1f;
			if (framePct > 0.9f)
				pct = fgMix;
			Font& f = *fonts[d1.textFont].get();

			SDL_Color fg = mix(backColor, MakeColor(d1.textColor), pct);

			// N characters per tick
			const int ticksPerChar = 50;
			int adjTickCount = tickCount - d1.duration * 0.1f;
			size_t charsDrawn = 0;
			size_t maxChars = (size_t)(adjTickCount / ticksPerChar);
			int xleft = context->screenWidth >> 2;
			int x = xleft;
			int y = context->screenHeight >> 1;
			int spacew = f.calcWidth(" ");
			int fh = f.calcHeight();
			for (auto line : reflow) {
				if (line == "\n") {
					y += fh;
					x = xleft;
					continue;
				}
				size_t possibleChars = charsDrawn + line.size();
				if (possibleChars < maxChars) {
					int w = f.calcWidth(line.c_str());
					f.draw(x, y, line.c_str(), fg, d1.textFlags & 0xFF);
					charsDrawn += line.size();
					x += spacew + w;
				} else if (maxChars > charsDrawn) {
					std::string sub = line.substr(0, maxChars - charsDrawn);
					f.draw(x, y, sub.c_str(), fg, d1.textFlags & 0xFF);
					charsDrawn += sub.size();
				}
			}
		}
	}


	void StoryScreen::_reflowText(Dialogue& d) {
		Font& f = *fonts[d.textFont].get();
		int fh = f.height();
		reflow.clear();
		reflow.reserve(200);
		int maxWidth = context->screenWidth >> 1;
		int x = 0;
		int spacew = f.calcWidth(" ");
		for (auto& line : d.lines) {
			std::istringstream is(line);
			while (is) {
				std::string s;
				is >> s;
				if (s.empty())
					continue;
				int w = f.calcWidth(s.c_str());
				if (x + w > maxWidth) {
					reflow.push_back("\n");
					x = 0;
				}
				reflow.push_back(s);
				x += w + spacew;
			}
			reflow.push_back("\n");
		}
	}


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


	bool StoryScreen::writeStream(std::ostream& os) {
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
		return true;
	}
} // namespace GameLib
