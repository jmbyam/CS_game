#ifndef GAMELIB_FONT_HPP
#define GAMELIB_FONT_HPP

#include <gamelib_context.hpp>

namespace GameLib {
	class Font {
	public:
		static constexpr int HALIGN_LEFT = 0;
		static constexpr int HALIGN_CENTER = 1 << 0;
		static constexpr int HALIGN_RIGHT = 1 << 1;
		static constexpr int VALIGN_TOP = 1 << 2;
		static constexpr int VALIGN_CENTER = 1 << 3;
		static constexpr int VALIGN_BOTTOM = 1 << 4;
		static constexpr int SHADOWED = 1 << 5;
		static constexpr int BOLD = 1 << 6;
		static constexpr int ITALIC = 1 << 7;

		// initialize new font using the specified context
		Font(Context* context);

		// destructor
		~Font();

		// loads font from disk using specified point size
		bool load(const std::string& path, int ptsize);

		// renders text using color
		SDL_Texture* render(const char* text, SDL_Color fg);

		// return texture from last render
		SDL_Texture* lastRender();

		// prepares for new render
		void newRender();

		// calculates the width of the string text
		int calcWidth(const char* text);

		// calculates the height of the loaded font
		int calcHeight() const;

		// returns the height of the rendered string
		int height() const {
			return rect_.h;
		}
		// returns the width of the renderered string
		int width() const {
			return rect_.w;
		}

		// draw prerendered text to screen
		void draw(int x, int y);

		// draw text to screen
		void draw(int x, int y, const char* text, SDL_Color fg, int flags);

	private:
		Context* context_{ nullptr };
		TTF_Font* font_{ nullptr };
		SDL_Texture* texture_{ nullptr };
		SDL_Surface* surface_{ nullptr };
		SDL_Rect rect_;
		int style_ = TTF_STYLE_NORMAL;
	};
} // namespace GameLib

#endif
