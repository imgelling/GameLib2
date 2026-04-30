#pragma once
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

#include "GameEngine.h"
#include "GameErrors.h"
#include "GameTexture2D.h"
#include "GameMath.h"
#include "GameColor.h"
#include "Game_SpriteSubSheet.h"
//#include "GameGUI_StringFunction.h"

namespace game
{
	extern Engine* enginePointer;
	extern GameError lastError;
	struct FontDescriptor
	{
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
		int16_t xOffset;
		int16_t yOffset;
		uint16_t xAdvance;
		//uint16_t page;

		FontDescriptor()
		{
			y = 0;
			x = 0;
			width = 0;
			height = 0;
			xOffset = 0;
			yOffset = 0;
			xAdvance = 0;
			//page = 0;
		}
	};

	struct Charset
	{
		uint16_t lineHeight = 0;
		uint16_t base = 0;
		uint16_t width = 0;
		uint16_t height = 0;
		//uint16_t pages = 0;
		//uint16_t size = 0;
		FontDescriptor letters[256];
	};

	class SpriteFont
	{
	public:
		SpriteFont();
		~SpriteFont();

		inline int32_t WidthOfTextInPixels(const std::string& string) const
		{
			//const uint64_t size = string.size();
			int32_t currentX = 0;
			int32_t width = 0; 
			for (uint16_t i : string) // = 0; i < size; ++i)
			{
				//const uint8_t letter = string[i];
				const uint32_t widthOfLetter = characterSet->letters[i].width;
				// this can be cleaned up.
				width = currentX + (characterSet->letters[i].xOffset) + (widthOfLetter);
				currentX += (characterSet->letters[i].xAdvance);
			}
			return width;
		}

		void ScaleBoundingBoxOnCenter(game::Rectf& rect, float scaleFactorX, float scaleFactorY)
		{
			// Negative scaling bad m'kay!
			scaleFactorX = scaleFactorX < 0 ? 0 : scaleFactorX;
			scaleFactorY = scaleFactorY < 0 ? 0 : scaleFactorY;

			// Find center before scaling
			float halfWidth = (rect.right - rect.left) * 0.5f;
			float halfHeight = (rect.bottom - rect.top) * 0.5f;
			float centerX = rect.left + halfWidth;// / 2.0f;
			float centerY = rect.top + halfHeight;// / 2.0f;

			// translate to center around the origin
			// and find the center again
			rect.left -= centerX;
			rect.top -= centerY;
			centerX = rect.left + halfWidth;// / 2.0f;
			centerY = rect.top + halfHeight;// / 2.0f;

			// Scale width and height
			halfWidth *= scaleFactorX;
			halfHeight *= scaleFactorY;

			// Recalculate top-left so center stays the same
			rect.left = centerX - halfWidth;// / 2.0f;
			rect.top = centerY - halfHeight;// / 2.0f;
			rect.right = centerX + halfWidth;// / 2.0f;
			rect.bottom = centerY + halfHeight;// / 2.0f;
		}
	
		bool Load(const std::string &filename, const std::string& textureIn, const TextureFilterType filter = TextureFilterType::Point);
		void UnLoad();
		std::shared_ptr<Charset> characterSet;
		bool isLoaded;


		Texture2D texture;
	private:
		//game::GUI::StringFunction stringFunction;
	};

	inline SpriteFont::SpriteFont()
	{
		isLoaded = false;
		characterSet = std::make_shared<Charset>();
	}

	inline SpriteFont::~SpriteFont()
	{
		UnLoad();
	}

	inline bool SpriteFont::Load(const std::string &fileName, const std::string& textureIn, const TextureFilterType filter)
	{

		std::string line;
		std::string read;
		std::string key;
		std::string value;
		uint64_t index;
		std::ifstream stream;

		if (!characterSet) characterSet = std::make_shared<Charset>();

		if (textureIn != "")
		{
			this->texture.isMipMapped = true;
			this->texture.filterType = filter;// TextureFilterType::Trilinear;
			if (!enginePointer->geLoadTexture(textureIn, this->texture))
			{
				lastError = { GameErrors::GameContent, "Could not load \"" + textureIn + "\" for SpriteFont." };
				return false;
			}
		}

		stream.open(fileName.c_str());
		if (!stream.is_open())
		{
			lastError = { GameErrors::GameContent, "Could not load \"" + fileName + "\" for SpriteFont." };
			return false;
		}

		std::stringstream lineStream;
		while (!stream.eof())
		{
			getline(stream, line);
			lineStream.str("");
			lineStream.clear();
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
						Converter >> characterSet->lineHeight; // line size of font
					else if (key == "base")
						Converter >> characterSet->base;
					else if (key == "scaleW")
						Converter >> characterSet->width;
					else if (key == "scaleH")
						Converter >> characterSet->height;
					//else if (key == "pages")
					//	Converter >> characterSet->pages;
					//else if (key == "size")
					//	Converter >> characterSet->size;
				}
			}
			else if (read == "char")
			{
				//this is data for a specific char
				uint16_t charID = 0;

				while (!lineStream.eof())
				{
					if (charID > 255)
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
						Converter >> charID;
					else if (key == "x")
						Converter >> characterSet->letters[charID].x;
					else if (key == "y")
						Converter >> characterSet->letters[charID].y;
					else if (key == "width")
						Converter >> characterSet->letters[charID].width;
					else if (key == "height")
						Converter >> characterSet->letters[charID].height;
					else if (key == "xoffset")
						Converter >> characterSet->letters[charID].xOffset;
					else if (key == "yoffset")
						Converter >> characterSet->letters[charID].yOffset;
					else if (key == "xadvance")
						Converter >> characterSet->letters[charID].xAdvance;
					//else if (key == "page")
					//	Converter >> characterSet->letters[CharID].page;
				}
			}
		}
		stream.close();
		isLoaded = true;
		return true;
	}

	inline void SpriteFont::UnLoad()
	{
		enginePointer->geUnLoadTexture(texture);
		characterSet = nullptr;
		isLoaded = false;
	}
}
