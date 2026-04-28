#pragma once

#include <cstdint>
#include "GameMath.h"
#include "GameTexture2D.h"
#include "Game_SpriteSubSheet.h"

namespace game
{
	class SpriteSheet
	{
	public:
		SpriteSheet();
		SpriteSheet(Texture2D& texure, const int width, const int height);
		void Initialize(Texture2D &texure, const int width, const int height) noexcept;
		void Initialize(game::SpriteSubSheet& subSheet, const std::string& subName, const int32_t width, const int32_t height);
		void Initialize(Texture2D &texure, const Pointi &size) noexcept;
		//void SetTexture(Texture2D& texture) { _texture = &texture; };
		Recti GetRectFromId(int id) noexcept;
		uint32_t tileWidth;
		uint32_t tileHeight;
	private:
		uint32_t _tilesPerRow;
		//Texture2D* _texture;
		game::Pointi _textureOffset;
	};

	inline SpriteSheet::SpriteSheet()
	{
		tileWidth = 0;
		tileHeight = 0;
		_tilesPerRow = 0;
		//_texture = nullptr;
	}
	
	SpriteSheet::SpriteSheet(Texture2D& texture, const int width, const int height)
	{
		SpriteSheet();
		if (!width || !height) return;
		//_texture = &texture;
		tileWidth = width;
		tileHeight = height;
		_tilesPerRow = texture.width / tileWidth;
	}

	void SpriteSheet::Initialize(game::SpriteSubSheet& subSheet, const std::string& subName, const int32_t width, const int32_t height)
	{
		auto it = subSheet.subTexture.find(subName);
		if (it == subSheet.subTexture.end())
			return;

		//_texture = &subSheet.texture;
		tileWidth = width;
		tileHeight = height;
		int32_t textureWidth = subSheet.subTexture[subName].right - subSheet.subTexture[subName].left;
		//int32_t textureHeight = subSheet.subTexture[subName].bottom - subSheet.subTexture[subName].top;
		_tilesPerRow = textureWidth / tileWidth;
		//_textureOffset.y = subSheet.subTexture[subName].top;
		//_textureOffset.x = subSheet.subTexture[subName].left;
	}

	inline void SpriteSheet::Initialize(Texture2D &texture, const int width, const int height) noexcept
	{
		//_texture = &texture;
		tileWidth = width;
		tileHeight = height;
		_tilesPerRow = texture.width / tileWidth;
	}

	inline void SpriteSheet::Initialize(Texture2D &texture, const Pointi &size) noexcept
	{
		Initialize(texture, size.width, size.height);
	}

	inline Recti SpriteSheet::GetRectFromId(int id) noexcept
	{
		Recti rectangle;
		//if (!_texture)
		//{
		//	return rectangle;
		//}

		rectangle.top = (id / _tilesPerRow * tileHeight) + _textureOffset.y;
		rectangle.left = (id % _tilesPerRow * tileWidth) + _textureOffset.x;
		rectangle.bottom = rectangle.top + tileHeight;
		rectangle.right = rectangle.left + tileWidth;

		return rectangle;
	}
}