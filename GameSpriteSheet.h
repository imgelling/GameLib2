#pragma once

#include <cstdint>
#include <Game_Assert.h>
#include "GameMath.h"
#include "GameTexture2D.h"
#include "Game_SpriteSubSheet.h"

namespace game
{
	class SpriteSheet
	{
	public:
		SpriteSheet();
		SpriteSheet(const Texture2D& texture, const int width, const int height);
		void Initialize(const Texture2D &texture, const int width, const int height) noexcept;
		void Initialize(game::SpriteSubSheet& subSheet, const std::string& subName, const int32_t width, const int32_t height);
		void Initialize(const Texture2D &texture, const Pointi &size) noexcept;
		Recti GetRectFromId(int id) const noexcept;
		uint32_t tileWidth;
		uint32_t tileHeight;
	private:
		uint32_t _tilesPerRow;
		game::Pointi _textureOffset;
	};

	inline SpriteSheet::SpriteSheet()
	{
		tileWidth = 0;
		tileHeight = 0;
		_tilesPerRow = 0;
	}
	
	SpriteSheet::SpriteSheet(const Texture2D& texture, const int width, const int height)
	{
		SpriteSheet();
		if (!width || !height) return;
		tileWidth = width;
		tileHeight = height;
		_tilesPerRow = texture.width / tileWidth;
	}

	void SpriteSheet::Initialize(game::SpriteSubSheet& subSheet, const std::string& subName, const int32_t width, const int32_t height)
	{
		GAME_ASSERT(!(subSheet.subTextureRegistry.find(subName) == subSheet.subTextureRegistry.end()));

		tileWidth = width;
		tileHeight = height;
		int32_t textureWidth = subSheet.subTextureRegistry[subName].right;
		_tilesPerRow = textureWidth / tileWidth;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const int width, const int height) noexcept
	{
		tileWidth = width;
		tileHeight = height;
		_tilesPerRow = texture.width / tileWidth;
	}

	inline void SpriteSheet::Initialize(const Texture2D &texture, const Pointi &size) noexcept
	{
		Initialize(texture, size.width, size.height);
	}

	inline Recti SpriteSheet::GetRectFromId(int id) const noexcept
	{
		Recti rectangle;

		rectangle.top = (id / _tilesPerRow * tileHeight) + _textureOffset.y;
		rectangle.left = (id % _tilesPerRow * tileWidth) + _textureOffset.x;
		rectangle.bottom = rectangle.top + tileHeight;
		rectangle.right = rectangle.left + tileWidth;

		return rectangle;
	}
}