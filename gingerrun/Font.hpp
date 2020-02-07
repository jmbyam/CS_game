#pragma once

#include <gamelib.hpp>

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

        Font(Context* context);
        ~Font();

        bool load(const std::string& path, int ptsize);
        SDL_Texture* render(const char* text, SDL_Color fg);
        SDL_Texture* lastRender() { return texture_; }
        void newRender();

        // calculates the width of the string text
        int calcWidth(const char* text);

        // calculates the height of the loaded font
        int calcHeight() const { return TTF_FontHeight(font_); }

        // returns the height of the rendered string
        int height() const { return rect_.h; }
        // returns the width of the renderered string
        int width() const { return rect_.w; }

        // draws previously rendered text to screen
        void draw(int x, int y);
        // rerenders text and draws it to the screen
        void draw(int x, int y, const char* text, SDL_Color fg, int flags);

    private:
        Context* context_{ nullptr };
        TTF_Font* font_{ nullptr };
        SDL_Texture* texture_{ nullptr };
        SDL_Surface* surface_{ nullptr };
        SDL_Rect rect_{ 0, 0, 0, 0 };
    };
}
