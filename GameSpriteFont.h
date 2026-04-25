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
		int32_t Height(const std::string& text) const;
		game::Recti BoundingBox(const std::string& string) const;
		void GetSizes(const std::string& string, game::Recti& boundingBox, int32_t& width, int32_t& height) const;
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

		// shouldn't need widthInPixels.
		int32_t NumberTextCharsInAWidth(const std::string& in, const int32_t maxPixels) const
		{
			int32_t count = 0;
			int32_t width = 0;
			int32_t currentX = 0;
			std::string test;
			for (uint8_t c : in)
			{
				test += c;
				width = currentX + characterSet->letters[c].xOffset + characterSet->letters[c].width;
				if (width >= maxPixels)
					return count;// 
				count++;
				currentX += characterSet->letters[c].xAdvance;
			}
			return count;
		}
		std::string ColorTagWrap(const std::string& str, const game::Color& color);

		int32_t GetCursorPositionInText(const int32_t cursorPosition, const std::string& text) const
		{
			int32_t offset = 0;
			int32_t currentX = 0;

			if (cursorPosition <= 0) return 0;

			for (uint64_t i = 0; i < cursorPosition; i++)
			{
				const uint8_t letter = text[i];
				const uint32_t widthOfLetter = characterSet->letters[letter].width;
				offset = currentX + characterSet->letters[letter].xOffset;
				offset = offset + (widthOfLetter);
				if (letter == ' ') offset += characterSet->letters[letter].xAdvance;
				currentX += (characterSet->letters[letter].xAdvance);// *_scaleX);
			}
			return offset;
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

		// Structure to hold a text segment and its color
		class ColorTextSegment
		{
		public:
			std::string text;
			Color color; // 0xRRGGBB
		};

		bool Valid32Hex(const std::string& hex) const
		{
			if (hex.length() < 8) return false;
			for (auto ch : hex)
			{
				if (!std::isxdigit(ch))
				{
					return false;
				}
			}
			return true;
		}

		// Parser function
		std::vector<ColorTextSegment> parseColoredString(const std::string& input, const Color& defaultColor = Colors::White, const bool removeCode = true) const
		{
			std::vector<ColorTextSegment> segments;
			uint64_t pos = 0;
			Color currentColor = defaultColor;
			const uint64_t size = input.size();
			std::string coloredText;
			std::string colorCode;
			uint64_t colorCodeStart = 0;
			uint64_t tagEnd = 0;
			uint64_t closeTag = 0;
			uint64_t tagStart = 0;

			while (pos < size)
			{
				// Color escape = "\|"
				tagStart = input.find("\\|", pos);

				if (tagStart == std::string::npos)
				{
					// No more tags, push remaining text
					segments.push_back({ input.substr(pos), currentColor });
					break;
				}

				// Push text before tag
				if (tagStart > pos)
				{
					segments.push_back({ input.substr(pos, tagStart - pos), currentColor });
				}


				// Find closing '>'
				//tagEnd = input.find('>', colorCodeStart + 8);
				//if (tagEnd == std::string::npos)
				//{
				//	segments.push_back({ input.substr(tagStart), currentColor });
				//	break; // malformed tag
				//}


				tagEnd = tagStart + 8/*color code*/ + 1;
				
				// Parse color code
				colorCodeStart = tagStart + 2; // skip "//|"
				colorCode = input.substr(colorCodeStart, 8);
				if (Valid32Hex(colorCode))
				{
					currentColor.Set(colorCode);
					if (!removeCode)
					{
						segments.push_back({ input.substr(tagStart,10), currentColor });
					}
				}
				else
				{
					segments.push_back({ input.substr(tagStart), currentColor });
					// Invalid hex, just push the rest of string
					break;
				}


				// Find closing \e
				closeTag = input.find("\\e", tagEnd);
				if (closeTag == std::string::npos)
				{
					if (!removeCode) segments.push_back({ input.substr(tagStart+10), currentColor });
					break; // malformed tag
				}

				// Extract colored text
				coloredText = input.substr(tagEnd + 1, closeTag - (tagEnd + 1));
				segments.push_back({ coloredText, currentColor });

				if (!removeCode)
				{
					segments.push_back({ input.substr(closeTag,2), currentColor });
				}

				// Reset color after closing tag
				currentColor = defaultColor;

				// Move position after closing tag
				pos = closeTag + 2; // length of "\e"
			}

			return segments;
		}
	
		bool Load(const std::string &filename, const std::string& texture, const TextureFilterType filter = TextureFilterType::Point);
		void UnLoad();
		std::shared_ptr<Charset> characterSet;
		bool isLoaded;


		Texture2D texture;
	private:
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

	inline bool SpriteFont::Load(const std::string &fileName, const std::string& texture, const TextureFilterType filter)
	{

		std::string line;
		std::string read;
		std::string key;
		std::string value;
		uint64_t index;
		std::ifstream stream;

		if (!characterSet) characterSet = std::make_shared<Charset>();

		this->texture.isMipMapped = true;
		this->texture.filterType = filter;// TextureFilterType::Trilinear;
		if (!enginePointer->geLoadTexture(texture, this->texture))
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


	inline game::Recti SpriteFont::BoundingBox(const std::string& string) const
	{
		if (string == "")
		{
			Recti e;
			return e;
		}

		auto segments = parseColoredString(string, game::Colors::White);

		Recti destination;
		Recti box(20000, 20000, -20000, -20000);
		int32_t currentX = 0;
		int32_t currentY = 0;
		for (auto &seg : segments)
		{
			const uint64_t size = seg.text.size();
			for (uint64_t i = 0; i < size; ++i)
			{
				const uint8_t letter = string[i];
				const uint32_t widthOfLetter = characterSet->letters[letter].width;
				const uint32_t heightOfLetter = characterSet->letters[letter].height;

				destination.left = currentX + (characterSet->letters[letter].xOffset);
				destination.top = currentY + (characterSet->letters[letter].yOffset);
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
				currentX += (characterSet->letters[letter].xAdvance);
			}
		}
		return box;
	}

	inline void SpriteFont::GetSizes(const std::string& string, game::Recti& boundingBox, int32_t& width, int32_t& height) const
	{
		boundingBox = BoundingBox(string);
		width = boundingBox.right - boundingBox.left;
		height = boundingBox.bottom - boundingBox.top;
	}

	inline int32_t SpriteFont::Height(const std::string& text) const
	{
		game::Recti bbox = BoundingBox(text);
		return bbox.bottom - bbox.top;
	}

	inline std::string SpriteFont::ColorTagWrap(const std::string& str, const game::Color& color)
	{
		std::string ret;
		ret = "\\|" + color.hexidecimal;// +">";
		ret += str;
		ret += "\\e";
		return ret;
	}

}
