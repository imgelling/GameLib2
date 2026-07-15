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
		SpriteSheet();
		SpriteSheet(const Texture2D& texture, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0);
		void Initialize(const Texture2D &texture, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0) noexcept;
		void Initialize(SpriteSubSheet& subSheet, const std::string& subName, const uint32_t width, const uint32_t height, const uint32_t marginX = 0, const uint32_t marginY = 0);
		void Initialize(const Texture2D& texture, const Pointi& size, const game::Pointi &margin = { 0,0 }) noexcept;
		void Initialize(SpriteSubSheet& subSheet, const std::string& subName, const Pointi& size, const game::Pointi &margin = { 0,0 });
		Recti GetRectFromId(const int32_t id) const noexcept;
		uint32_t tileWidth;
		uint32_t tileHeight;
		int32_t maxTiles;
	private:
		uint32_t _tileWidthWithMargin;
		uint32_t _tileHeightWithMargin;
		uint32_t _tilesPerRow;
		Pointi _textureOffset;
		//Pointi _margins;
	};

	inline SpriteSheet::SpriteSheet()
	{
		tileWidth = 0;
		tileHeight = 0;
		maxTiles = 0;
		_tilesPerRow = 0;
		_tileWidthWithMargin = 0;
		_tileHeightWithMargin = 0;
	}
	
	SpriteSheet::SpriteSheet(const Texture2D& texture, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY)
	{
		SpriteSheet();
		GAME_ASSERT(width);
		GAME_ASSERT(height);
		//if (!width || !height) return;
		tileWidth = width;
		tileHeight = height;
		_tileWidthWithMargin = tileWidth + marginX;
		_tileHeightWithMargin = tileHeight + marginY;
		_tilesPerRow = texture.width / (_tileWidthWithMargin);
		const uint32_t tilesPerCol = texture.height / (_tileHeightWithMargin);
		maxTiles = _tilesPerRow * tilesPerCol;
		//_margins.x = marginX;
		//_margins.y = marginY;
	}

	void SpriteSheet::Initialize(SpriteSubSheet& subSheet, const std::string& subName, const Pointi& size, const game::Pointi &margin)
	{
		Initialize(subSheet, subName, size.width, size.height, margin.x, margin.y);
	}

	void SpriteSheet::Initialize(SpriteSubSheet& subSheet, const std::string& subName, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY)
	{
		GAME_ASSERT(!(subSheet.subTextureRegistry.find(subName) == subSheet.subTextureRegistry.end()));
		GAME_ASSERT(width);
		GAME_ASSERT(height);
		tileWidth = width;
		tileHeight = height;
		_tileWidthWithMargin = tileWidth + marginX;
		_tileHeightWithMargin = tileHeight + marginY;
		//int32_t textureWidth = subSheet.subTextureRegistry[subName].right;
		_tilesPerRow = subSheet.subTextureRegistry[subName].right / (_tileWidthWithMargin);
		const uint32_t tilesPerCol = subSheet.subTextureRegistry[subName].bottom / (_tileHeightWithMargin);
		maxTiles = _tilesPerRow * tilesPerCol;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const uint32_t width, const uint32_t height, const uint32_t marginX, const uint32_t marginY) noexcept
	{
		GAME_ASSERT(width);
		GAME_ASSERT(height);
		tileWidth = width;
		tileHeight = height;
		_tileWidthWithMargin = tileWidth + marginX;
		_tileHeightWithMargin = tileHeight + marginY;
		_tilesPerRow = texture.width / (_tileWidthWithMargin);
		uint32_t tilesPerCol = texture.height / (_tileHeightWithMargin);
		maxTiles = _tilesPerRow * tilesPerCol;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const Pointi &size, const game::Pointi &margin) noexcept
	{
		Initialize(texture, size.width, size.height, margin.x, margin.y);
	}

	inline Recti SpriteSheet::GetRectFromId(const int32_t id) const noexcept
	{
		Recti rectangle;

		rectangle.top = (id / _tilesPerRow * (_tileHeightWithMargin)) + _textureOffset.y;
		rectangle.left = (id % _tilesPerRow * (_tileWidthWithMargin)) + _textureOffset.x;
		rectangle.bottom = rectangle.top + tileHeight;
		rectangle.right = rectangle.left + tileWidth;

		return rectangle;
	}
}