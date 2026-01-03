#pragma once
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

#include "GameEngine.h"
#include "GameErrors.h"
#include "GameTexture2D.h"
#include "GameMath.h"
#include "GameColor.h"

namespace game
{
	extern Engine* enginePointer;
	extern GameError lastError;
	struct FontDescriptor
	{
		unsigned short x, y;
		unsigned short width, height;
		short xOffset, yOffset;
		unsigned short xAdvance;
		unsigned short page;

		FontDescriptor()
		{
			y = x = 0;
			width = height = 0;
			xOffset = yOffset = 0;
			xAdvance = 0;
			page = 0;
		}
	};

	struct Charset
	{
		unsigned short lineHeight = 0;
		unsigned short base = 0;
		unsigned short width = 0;
		unsigned short height = 0;
		unsigned short pages = 0;
		FontDescriptor letters[256];
	};

	class SpriteFont
	{
	public:
		SpriteFont();
		~SpriteFont();
		uint32_t Length(const std::string &text) const;
		uint32_t Height(const std::string& text) const;
		game::Recti BoundingBox(const std::string& string) const;
		std::string ColorTagWrap(const std::string& str, const game::Color& color);
		bool Load(const std::string &filename, const std::string& texture);
		void UnLoad();
		Texture2D Texture() const;
		Charset _characterSet;


	private:
		Texture2D _texture;
	};

	inline SpriteFont::SpriteFont()
	{

	}

	inline SpriteFont::~SpriteFont()
	{
		UnLoad();
	}

	inline bool SpriteFont::Load(const std::string &fileName, const std::string& texture)
	{

		std::string line;
		std::string read, key, value;
		std::size_t index;
		std::ifstream stream;

		_texture.isMipMapped = true;
		//_texture.filterType = TextureFilterType::Point;
		if (!enginePointer->geLoadTexture(texture, _texture))
		{
			lastError = { GameErrors::GameContent, "Could not load \"" + texture + "\" for SpriteFont." };
			return false;
		}

		stream.open(fileName.c_str());
		if (!stream.is_open())
		{
			lastError = { GameErrors::GameContent, "Could not load \"" + fileName + "\" for SpriteFont." };
			return false;
		}

		while (!stream.eof())
		{
			std::stringstream lineStream;
			getline(stream, line);
			lineStream << line;

			//read the line's type
			lineStream >> read;
			if (read == "common")
			{
				//this holds common data
				while (!lineStream.eof())
				{
					std::stringstream Converter;
					lineStream >> read;
					index = read.find('=');
					key = read.substr(0, index);
					value = read.substr(index + 1);

					//assign the correct value
					Converter << value;
					if (key == "lineHeight")
						Converter >> _characterSet.lineHeight;
					else if (key == "base")
						Converter >> _characterSet.base;
					else if (key == "scaleW")
						Converter >> _characterSet.width;
					else if (key == "scaleH")
						Converter >> _characterSet.height;
					else if (key == "pages")
						Converter >> _characterSet.pages;
				}
			}
			else if (read == "char")
			{
				//this is data for a specific char
				unsigned short CharID = 0;

				while (!lineStream.eof())
				{
					if (CharID > 255)
					{
						stream.close();
						lastError = { GameErrors::GameContent, "File error in \"" + fileName + "\"." };
						return false;
					}
					std::stringstream Converter;
					lineStream >> read;
					index = read.find('=');
					key = read.substr(0, index);
					value = read.substr(index + 1);

					//assign the correct value
					Converter << value;
					if (key == "id")
						Converter >> CharID;
					else if (key == "x")
						Converter >> _characterSet.letters[CharID].x;
					else if (key == "y")
						Converter >> _characterSet.letters[CharID].y;
					else if (key == "width")
						Converter >> _characterSet.letters[CharID].width;
					else if (key == "height")
						Converter >> _characterSet.letters[CharID].height;
					else if (key == "xoffset")
						Converter >> _characterSet.letters[CharID].xOffset;
					else if (key == "yoffset")
						Converter >> _characterSet.letters[CharID].yOffset;
					else if (key == "xadvance")
						Converter >> _characterSet.letters[CharID].xAdvance;
					else if (key == "page")
						Converter >> _characterSet.letters[CharID].page;
				}
			}
		}
		stream.close();

		return true;
	}

	inline void SpriteFont::UnLoad()
	{
		enginePointer->geUnLoadTexture(_texture);
	}


	inline game::Recti SpriteFont::BoundingBox(const std::string& string) const
	{
		Recti destination;
		const uint64_t size = string.size();
		Recti box(20000, 20000, -20000, -20000);
		int32_t currentX = 0;
		int32_t currentY = 0;
		for (uint64_t i = 0; i < size; ++i)
		{
			const uint8_t letter = string[i];
			const uint32_t widthOfLetter = _characterSet.letters[letter].width;
			const uint32_t heightOfLetter = _characterSet.letters[letter].height;

			destination.left = currentX + (_characterSet.letters[letter].xOffset);
			destination.top = currentY + (_characterSet.letters[letter].yOffset);
			destination.right = destination.left + (widthOfLetter);
			destination.bottom = destination.top + (heightOfLetter);

			// Find the bounding box
			if (destination.left < box.left)
				box.left = destination.left;
			if (destination.top < box.top)
				box.top = destination.top;
			if (destination.bottom > box.bottom)
				box.bottom = destination.bottom;
			if (destination.right > box.right)
				box.right = destination.right;
			currentX += (_characterSet.letters[letter].xAdvance);
		}
		return box;
	}

	inline uint32_t SpriteFont::Length(const std::string& text) const
	{
		game::Recti bbox = BoundingBox(text);
		return bbox.right - bbox.left;
	}

	inline uint32_t SpriteFont::Height(const std::string& text) const
	{
		game::Recti bbox = BoundingBox(text);
		return bbox.bottom - bbox.top;
	}

	inline std::string SpriteFont::ColorTagWrap(const std::string& str, const game::Color& color)
	{
		std::string ret;
		ret = "<color=" + color.hexidecimal + ">";
		ret += str;
		ret += "</color>";
		return ret;
	}

	inline Texture2D SpriteFont::Texture() const
	{
		return _texture;
	}
}
