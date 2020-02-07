#include "Font.hpp"

namespace GameLib {
    Font::Font(Context* context)
        : context_(context) {}

    Font::~Font() {
        if (font_) {
            TTF_CloseFont(font_);
            font_ = nullptr;
        }
    }

    bool Font::load(const std::string& filename, int ptsize) {
        std::string path = context_->findSearchPath(filename);
        font_ = TTF_OpenFont(path.c_str(), ptsize);
        return font_ != nullptr;
    }

    void Font::newRender() {
        if (texture_) {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        if (surface_) {
            SDL_FreeSurface(surface_);
            surface_ = nullptr;
        }
    }

    int Font::calcWidth(const char* text) {
        int w{ 0 };
        if (font_)
            TTF_SizeUTF8(font_, text, &w, nullptr);
        return w;
    }

    SDL_Texture* Font::render(const char* text, SDL_Color fg) {
        if (!font_)
            return nullptr;
        newRender();
        surface_ = TTF_RenderText_Blended(font_, text, fg);
        if (surface_) {
            rect_.w = surface_->w;
            rect_.h = surface_->h;
            texture_ = SDL_CreateTextureFromSurface(context_->renderer(), surface_);
        }
        return texture_;
    }

    void Font::draw(int x, int y) {
        rect_.x = x;
        rect_.y = y;
        SDL_Renderer* renderer_ = context_->renderer();
        SDL_RenderCopy(renderer_, texture_, nullptr, &rect_);
    }

    void Font::draw(int x, int y, const char* text, SDL_Color fg, int flags) {
        if (flags & HALIGN_CENTER) {
            x -= calcWidth(text) >> 1;
        } else if (flags & HALIGN_RIGHT) {
            x -= calcWidth(text);
        }
        if (flags & VALIGN_TOP) {
        } else if (flags & VALIGN_CENTER) {
            y -= calcHeight() >> 1;
        } else if (flags & VALIGN_BOTTOM) {
            y -= calcHeight();
        }
        if (flags & SHADOWED) {
            render(text, Black);
            draw(x + 2, y + 2);
        }
        render(text, fg);
        draw(x, y);
    }
}
