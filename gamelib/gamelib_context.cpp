#include "pch.h"

#ifdef __unix__
#if __cplusplus >= 201703L && __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif
#else
#include <filesystem>
namespace filesystem = std::filesystem;
#endif

#include <gamelib.hpp>

namespace GameLib {
    //////////////////////////////////////////////////////////////////
    // CONSTRUCTOR/DESTRUCTOR ////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    Context::Context(int width, int height, int windowFlags) {
        if (!_init())
            return;
        if (!_initScreen(width, height, windowFlags))
            return;
        initialized_ = true;
    }

    Context::~Context() { _kill(); }

    //////////////////////////////////////////////////////////////////
    // ERROR HANDLING ////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    bool Context::hadError() const {
        bool retError = hadError_;
        hadError_ = false;
        return retError;
    }

    void Context::_setError(std::string&& errorString) {
        errorString_ = std::move(errorString);
        HFLOGWARN("Error detected: %s", errorString.c_str());
        hadError_ = true;
    }

    //////////////////////////////////////////////////////////////////
    // INITIALIZATION / SHUTDOWN /////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    bool Context::_init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            HFLOGERROR("SDL not initialized");
            return false;
        }

        if (!_initSubsystems()) {
            _setError("one or more subsystems not initialized");
        }

        keyboard.scancodes.resize(SDL_NUM_SCANCODES);

        _openGameControllers();

        return true;
    }

    bool Context::_initSubsystems() {
        bool result = true;
        int flags = IMG_INIT_JPG | IMG_INIT_PNG;
        int initFlags = IMG_Init(flags);
        if (initFlags != flags) {
            if (~initFlags & IMG_INIT_JPG)
                _setError("JPEG not supported");
            if (~initFlags & IMG_INIT_PNG)
                _setError("PNG not supported");
            result = false;
        }

        if (TTF_Init() < 0) {
            _setError("TTF support not available");
            result = false;
        }

        flags = MIX_INIT_MP3 | MIX_INIT_OGG;
        initFlags = Mix_Init(flags);
        if (initFlags != flags) {
            if (~initFlags & MIX_INIT_MP3)
                _setError("MP3 not supported");
            if (~initFlags & MIX_INIT_OGG)
                _setError("OGG not supported");
            result = false;
        }

		constexpr int frequency = 48000;
		constexpr int channels = 2;
        if (Mix_OpenAudio(frequency, MIX_DEFAULT_FORMAT, channels, 4096) != 0) {
            HFLOGERROR("Failed to open audio: %s", SDL_GetError());
        } else {
            HFLOGINFO("Audio Device:      %s", SDL_GetAudioDeviceName(0, 0));
            HFLOGINFO("Audio initialized: %dHz %d channels", frequency, channels);
        }
        return result;
    }

    bool Context::_initScreen(int width, int height, int windowFlags) {
        return SDL_CreateWindowAndRenderer(width, height, windowFlags, &window_, &renderer_) == 0;
    }

    void Context::_kill() {
        _closeGameControllers();
        freeImages();
        freeTilesets();
        freeAudioClips();
        Mix_CloseAudio();
        SDL_Quit();
    }

    //////////////////////////////////////////////////////////////////
    // GAME CONTROLLERS //////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    void Context::_openGameControllers() {
        joystickCount = std::min<int>(MaxJoysticks, SDL_NumJoysticks());

        for (int i = 0; i < MaxJoysticks; i++) {
            JOYSTICKSTATE& j = joysticks[i];
            if (SDL_IsGameController(i)) {
                // do not open controllers already opened
                if (j.controller)
                    continue;
                j.controller = SDL_GameControllerOpen(i);
                // reset this joystick if it wasn't opened
                if (!j.controller) {
                    j = JOYSTICKSTATE();
                } else {
                    j.name = SDL_GameControllerNameForIndex(i);
                    HFLOGINFO("Joystick %i '%s' connected", i, j.name.c_str());
                }
                j.enabled = j.controller != nullptr;
            } else if (j.enabled) {
                HFLOGINFO("Joystick %i '%s' disconnected", i, j.name.c_str());
                SDL_GameControllerClose(j.controller);
                j = JOYSTICKSTATE();
            }
        }
    }

    void Context::_closeGameControllers() {
        for (int i = 0; i < MaxJoysticks; i++) {
            JOYSTICKSTATE& j = joysticks[i];
            if (!j.controller)
                continue;
            HFLOGINFO("Joystick %i '%s' closed", i, j.name.c_str());
            SDL_GameControllerClose(j.controller);
            j = JOYSTICKSTATE();
        }
    }

    void Context::_updateGameControllers() {
        for (unsigned i = 0; i < joystickCount; i++) {
            JOYSTICKSTATE& j = joysticks[i];
            if (!j.enabled)
                continue;
        }
    }

    //////////////////////////////////////////////////////////////////
    // EVENT HANDLING ////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    int Context::getEvents() {
        static int checkForGameControllers = 100;

        if (--checkForGameControllers <= 0) {
            checkForGameControllers = 100;
            _openGameControllers();
        }

        int eventCount{ 0 };
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_APP_TERMINATING:
            case SDL_QUIT:
                quitRequested = 1;
                break;
            case SDL_KEYDOWN:
                keyboard.keys[e.key.keysym.sym] = 1;
                keyboard.scancodes[e.key.keysym.scancode] = 1;
                keyboard.mod = e.key.keysym.mod;
                break;
            case SDL_KEYUP:
                keyboard.keys[e.key.keysym.sym] = 0;
                keyboard.scancodes[e.key.keysym.scancode] = 0;
                keyboard.mod = e.key.keysym.mod;
                break;
            default:
                break;
            }
            ++eventCount;
        }
        return eventCount;
    }

    //////////////////////////////////////////////////////////////////
    // DRAWING ///////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    int Context::drawTexture(glm::vec2 position, glm::vec2 size, SDL_Texture* texture) {
        if (!texture)
            return -1;
        SDL_Rect dstrect{ (int)position.x, (int)position.y, (int)size.x, (int)size.y };
        return SDL_RenderCopy(renderer_, texture, nullptr, &dstrect);
    }

    int Context::drawTexture(glm::vec2 position, int tilesetId, int tileId) {
#ifndef _DEBUG
        TILEIMAGE* t = getTileFast(tilesetId, tileId);
#else
        TILEIMAGE* t = getTile(tilesetId, tileId);
#endif
        if (!t)
            return -1;
        SDL_Rect dstrect{ (int)position.x, (int)position.y, t->w, t->h };
        return SDL_RenderCopy(renderer_, t->texture, nullptr, &dstrect);
    }

    int Context::drawTexture(int tilesetId, int tileId, SPRITEINFO& spriteInfo) {
#ifndef _DEBUG
        TILEIMAGE* t = getTileFast(tilesetId, tileId);
#else
        TILEIMAGE* t = getTile(tilesetId, tileId);
#endif
        if (!t)
            return -1;
        SDL_Rect dstrect{ (int)spriteInfo.position.x, (int)spriteInfo.position.y, t->w, t->h };
        SDL_Point center{ (int)spriteInfo.center.x, (int)spriteInfo.center.y };
        SDL_RendererFlip flip = (spriteInfo.flipFlags & 1) ? SDL_FLIP_HORIZONTAL : (spriteInfo.flipFlags & 2) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
        return SDL_RenderCopyEx(renderer_, t->texture, nullptr, &dstrect, spriteInfo.angle, &center, flip);
    }

    void Context::clearScreen(glm::u8vec4 color) {
        SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer_);
    }

    void Context::swapBuffers() { SDL_RenderPresent(renderer_); }

    //////////////////////////////////////////////////////////////////
    // SEARCH PATHS //////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    void Context::addSearchPath(const std::string& path) {
        if (path.empty()) {
            searchPaths_.push_back("./");
            return;
        }

        // normalize path to use '/' characters
        std::string search_path{ path };
        for (auto& c : search_path) {
            if (c == '\\')
                c = '/';
        }
        if (search_path.back() != '/')
            search_path.push_back('/');
        // add path if it exists and is a folder?
        if (filesystem::is_directory(search_path)) {
            searchPaths_.push_back(search_path);
        } else {
            HFLOGWARN("'%s' is not a directory", search_path.c_str());
        }
    }

    void Context::clearSearchPaths() { searchPaths_.clear(); }

    std::string Context::findSearchPath(const std::string& filename) const {
        std::string path;
        if (filesystem::is_regular_file(filename)) {
            path = filename;
        } else
            for (auto& dir : searchPaths_) {
                std::string p{ dir + filename };
                if (filesystem::is_regular_file(p)) {
                    path = p;
                    break;
                }
            }
        return path;
    }

    //////////////////////////////////////////////////////////////////
    // IMAGES ////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    SDL_Texture* Context::loadImage(const std::string& filename) {
        std::string p = findSearchPath(filename);
        if (p.empty())
            return nullptr;
        filesystem::path path = p;
        std::string resourceName = std::move(path.filename().string());
        if (images_.count(resourceName)) {
            SDL_DestroyTexture(images_[resourceName].texture);
        }
        SDL_Surface* img = IMG_Load(p.c_str());
        if (!img) {
            HFLOGERROR("'%s' not found", resourceName.c_str());
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, img);
        images_[resourceName].texture = texture;
        SDL_FreeSurface(img);
        HFLOGINFO("loaded '%s'", filename.c_str());
        return texture;
    }

    void Context::freeImages() {
        for (auto& [k, v] : images_) {
            SDL_DestroyTexture(v.texture);
            v.texture = nullptr;
        }
        images_.clear();
    }

    bool Context::imageLoaded(const std::string& resourceName) const { return images_.count(resourceName); }

    SDL_Texture* Context::getImage(const std::string& resourceName) const {
        if (images_.count(resourceName)) {
            return images_.at(resourceName).texture;
        }
        return nullptr;
    }

    //////////////////////////////////////////////////////////////////
    // TILESET ///////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    std::vector<TILEIMAGE>& Context::_initTileset(int id) {
        if (!tilesets_.count(id)) {
            auto& tileset = tilesets_[id];
            tileset.clear();
        } else {
            auto& tileset = tilesets_[id];
            for (auto& t : tileset) {
                if (t.texture) {
                    SDL_DestroyTexture(t.texture);
                }
                t = TILEIMAGE();
            }
            tilesets_[id].clear();
        }
        return tilesets_[id];
    }

    int Context::_addTile(int tilesetId, SDL_Surface* surface) {
        auto& tileset = tilesets_[tilesetId];
        TILEIMAGE t;
        t.texture = SDL_CreateTextureFromSurface(renderer_, surface);
        if (!t.texture)
            return 0;
        t.tileId = (int)tileset.size();
        t.tilesetId = tilesetId;
        t.w = surface->w;
        t.h = surface->h;
        tileset.push_back(t);
        return t.tileId;
    }

    int Context::loadTileset(int tilesetId, int w, int h, const std::string& filename) {
        std::string p = findSearchPath(filename);
        if (p.empty())
            return 0;
        SDL_Surface* surface = IMG_Load(p.c_str());
        if (!surface)
            return 0;
        SDL_Surface* tile = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
        if (!tile) {
            SDL_FreeSurface(surface);
            return 0;
        }
        int tileCount = 0;
        SDL_Rect dstrect{ 0, 0, w, h };
        auto tileset = _initTileset(tilesetId);
        for (int y = 0; y < surface->h; y += h) {
            for (int x = 0; x < surface->w; x += w) {
                SDL_Rect srcrect{ x, y, w, h };
                SDL_BlitSurface(surface, &srcrect, tile, &dstrect);
                _addTile(tilesetId, tile);
                tileCount++;
            }
        }
        HFLOGINFO("loaded '%s'", filename.c_str());
        return tileCount;
    }

    void Context::freeTilesets() {
        for (auto& [k, v] : tilesets_) {
            for (auto& t : v) {
                SDL_DestroyTexture(t.texture);
                t.texture = nullptr;
            }
        }
    }

    TILEIMAGE* Context::getTile(int tilesetId, int tileId) {
        if (!tilesets_.count(tilesetId))
            return nullptr;
        auto& tileset = tilesets_.at(tilesetId);
        if (tileId >= tileset.size())
            return nullptr;
        return &tileset[tileId];
    }

    //////////////////////////////////////////////////////////////////
    // AUDIO LOADING /////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////

    AUDIOINFO* Context::initAudioClip(int clipId) {
        if (audioClips_[clipId]) {
            audioClips_[clipId].free();
            audioClips_[clipId] = AUDIOINFO();
        }
        return &audioClips_[clipId];
    }

    AUDIOINFO* Context::getAudioClip(int clipId) {
        if (audioClips_.count(clipId))
            return &audioClips_[clipId];
        return nullptr;
    }

    void Context::freeAudioClips() {
        for (auto& [k, v] : audioClips_) {
            v.free();
        }
        audioClips_.clear();
    }

    AUDIOINFO* Context::loadAudioClip(int clipId, const std::string& filename) {
        std::string p = findSearchPath(filename);
        if (p.empty())
            return nullptr;
        AUDIOINFO& audio = *initAudioClip(clipId);

        Mix_Chunk* chunk = Mix_LoadWAV(p.c_str());
        if (!chunk) {
            HFLOGWARN("Unable to load '%s'", filename.c_str());
            HFLOGWARN("Mix_LoadWAV returned '%s'", Mix_GetError());
            return nullptr;
        }
        filesystem::path path = p;
        audio.chunk = chunk;
        audio.name = path.filename().string();
        HFLOGINFO("loaded '%s'", filename.c_str());
        return &audio;
    }

    int Context::playAudioClip(int clipId, int channel) {
        AUDIOINFO* audio = getAudioClip(clipId);
        if (!audio)
            return -1;
        return Mix_PlayChannel(-1, audio->chunk, 0);
    }
}
