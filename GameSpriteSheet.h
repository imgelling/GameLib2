#pragma once

#include <cstdint>
#include <string>
#include "Game_Assert.h"
#include "GameMath.h"
#include "GameTexture2D.h"
#include "Game_SpriteSubSheet.h"

namespace game
{
	class SpriteSheet
	{
	public:
		SpriteSheet() noexcept;
		SpriteSheet(const Texture2D& texture, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0) noexcept;
		void Initialize(const Texture2D &texture, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0) noexcept;
		void Initialize(const SpriteSubSheet& subSheet, const std::string& subName, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0) noexcept;
		void Initialize(const Texture2D& texture, const Pointi& size, const game::Pointi &margin = { 0,0 }) noexcept;
		void Initialize(const SpriteSubSheet& subSheet, const std::string& subName, const Pointi& size, const game::Pointi &margin = { 0,0 }) noexcept;
		Recti GetRectFromId(const uint32_t id) const noexcept;
		uint32_t GetTileWidth() const noexcept { return _tileWidth; };
		uint32_t GetTileHeight() const noexcept { return _tileHeight; };
		uint32_t GetMaxTiles() const noexcept { return _maxTiles; };
	private:
		uint32_t _tileWidth;
		uint32_t _tileHeight;
		uint32_t _maxTiles;
		uint32_t _tileWidthWithMargin;
		uint32_t _tileHeightWithMargin;
		uint32_t _tilesPerRow;
		Pointi _textureOffset;
	};

	inline SpriteSheet::SpriteSheet() noexcept
	{
		_tileWidth = 0;
		_tileHeight = 0;
		_maxTiles = 0;
		_tilesPerRow = 0;
		_tileWidthWithMargin = 0;
		_tileHeightWithMargin = 0;
	}
	
	SpriteSheet::SpriteSheet(const Texture2D& texture, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY) noexcept
	{
		GAME_ASSERT(width > 0);
		GAME_ASSERT(height > 0);
		_tileWidth = width;
		_tileHeight = height;
		_tileWidthWithMargin = _tileWidth + marginX;
		_tileHeightWithMargin = _tileHeight + marginY;
		_tilesPerRow = texture.width / (_tileWidthWithMargin);
		const uint32_t tilesPerCol = texture.height / (_tileHeightWithMargin);
		_maxTiles = _tilesPerRow * tilesPerCol;
	}

	inline void SpriteSheet::Initialize(const SpriteSubSheet& subSheet, const std::string& subName, const Pointi& size, const game::Pointi &margin) noexcept
	{
		Initialize(subSheet, subName, size.width, size.height, margin.x, margin.y);
	}

	inline void SpriteSheet::Initialize(const SpriteSubSheet& subSheet, const std::string& subName, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY) noexcept
	{
		GAME_ASSERT(!(subSheet.subTextureRegistry.find(subName) == subSheet.subTextureRegistry.end()));
		GAME_ASSERT(width > 0);
		GAME_ASSERT(height > 0);
		_tileWidth = width;
		_tileHeight = height;
		_tileWidthWithMargin = _tileWidth + marginX;
		_tileHeightWithMargin = _tileHeight + marginY;
		_tilesPerRow = subSheet.subTextureRegistry.at(subName).right / (_tileWidthWithMargin);
		const uint32_t tilesPerCol = subSheet.subTextureRegistry.at(subName).bottom / (_tileHeightWithMargin);
		_maxTiles = _tilesPerRow * tilesPerCol;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY) noexcept
	{
		GAME_ASSERT(width > 0);
		GAME_ASSERT(height > 0);
		_tileWidth = width;
		_tileHeight = height;
		_tileWidthWithMargin = _tileWidth + marginX;
		_tileHeightWithMargin = _tileHeight + marginY;
		_tilesPerRow = texture.width / (_tileWidthWithMargin);
		const uint32_t tilesPerCol = texture.height / (_tileHeightWithMargin);
		_maxTiles = _tilesPerRow * tilesPerCol;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const Pointi &size, const game::Pointi &margin) noexcept
	{
		Initialize(texture, size.width, size.height, margin.x, margin.y);
	}

	inline Recti SpriteSheet::GetRectFromId(const uint32_t id) const noexcept
	{
		Recti rectangle;

		rectangle.top = (id / _tilesPerRow * (_tileHeightWithMargin)) + _textureOffset.y;
		rectangle.left = (id % _tilesPerRow * (_tileWidthWithMargin)) + _textureOffset.x;
		rectangle.bottom = rectangle.top + _tileHeight;
		rectangle.right = rectangle.left + _tileWidth;

		return rectangle;
	}
}