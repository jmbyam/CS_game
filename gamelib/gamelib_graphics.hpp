#ifndef GAMELIB_GRAPHICS_HPP
#define GAMELIB_GRAPHICS_HPP

#include <gamelib_context.hpp>

namespace GameLib {
	class IGraphics {
	public:
		virtual ~IGraphics() {}

		int getCenterX() const { return getWidth() >> 1; }
		int getCenterY() const { return getHeight() >> 1; }

		virtual int getWidth() const { return 1; }
		virtual int getHeight() const { return 1; }
		virtual int getTileSizeX() const { return 1; }
		virtual int getTileSizeY() const { return 1; }
		virtual void setTileSize(glm::ivec2 size) {}
		virtual glm::ivec2 origin() const { return { 0, 0 }; }
		virtual glm::ivec2 center() const { return { 0, 0 }; }
		virtual glm::ivec2 offset() const { return { 0, 0 }; }
		virtual void setOrigin(glm::ivec2 p) {}
		virtual void setCenter(glm::ivec2 p) {}
		virtual void setOffset(glm::ivec2 p) {}
		virtual glm::ivec2 transform(glm::ivec2 p) { return p; }
		virtual void draw(int tileSetId, int tileId, int x, int y) {}
		virtual void draw(int tileSetId, int tileId, float x, float y) {}
		virtual void draw(int tileSetId, int tileId, int x, int y, int flipFlags) {}
		virtual void draw(int x, int y, int w, int h, SDL_Color color) {}
	};

	class Graphics : public IGraphics {
	public:
		Graphics(Context* ctx);
		virtual ~Graphics();

		int getWidth() const override { return screensize.x; }
		int getHeight() const override { return screensize.y; }
		int getTileSizeX() const override { return tileSize.x; }
		int getTileSizeY() const override { return tileSize.y; }
		void setTileSize(glm::ivec2 size) override { tileSize = size; }
		glm::ivec2 origin() const override { return origin_; }
		glm::ivec2 center() const override { return center_; }
		glm::ivec2 offset() const override { return offset_; }

		void setCenter(glm::ivec2 p) override { center_ = p; }
		void setOrigin(glm::ivec2 p) override { origin_ = p; }
		void setOffset(glm::ivec2 p) override { offset_ = p; }
		void draw(int tileSetId, int tileId, int x, int y) override;
		void draw(int tileSetId, int tileId, float x, float y) override;
		void draw(int tileSetId, int tileId, int x, int y, int flipFlags) override;
		void draw(int x, int y, int w, int h, SDL_Color color) override;

		glm::ivec2 transform(glm::ivec2 p) override { return origin_ + offset_ - center_ + p; }

	private:
		glm::ivec2 origin_{ 0, 0 };	   // location of the center of the screen
		glm::ivec2 offset_{ 0, 0 };	   // amount to move every translation (e.g. screen shaking)
		glm::ivec2 screensize{ 0, 0 }; // size of screen
		glm::ivec2 center_{ 0, 0 };	   // location of the camera attention
		glm::ivec2 tileSize{ 1, 1 };   // size in pixels of the tile grid
		Context* context{ nullptr };
		bool clip(glm::ivec2 p) {
			return (p.x + tileSize.x < 0 || p.y + tileSize.y < 0 || p.x > screensize.x || p.y > screensize.y);
		}
		bool clip(glm::ivec2 p, glm::ivec2 s) {
			return (p.x + s.x < 0 || p.y + s.y < 0 || p.x > screensize.x || p.y > screensize.y);
		}
	};
} // namespace GameLib

#endif
