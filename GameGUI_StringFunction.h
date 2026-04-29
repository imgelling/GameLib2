#pragma once
#include <GameColor.h>
#include <GameSpriteFont.h>

namespace game
{
	namespace GUI
	{
		// Structure to hold a text segment and its color
		class ColorTextSegment
		{
		public:
			std::string text;
			Color color; // 0xRRGGBB
		};

		class StringFunction
		{
		public:
			std::vector<game::GUI::ColorTextSegment> segments;

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

			// shouldn't need widthInPixels.
			int32_t NumberTextCharsInAWidth(const game::SpriteFont &font, const std::string & in, const int32_t maxPixels) const
			{
				int32_t count = 0;
				int32_t width = 0;
				int32_t currentX = 0;
				std::string test;
				for (uint8_t c : in)
				{
					test += c;
					width = currentX + font.characterSet->letters[c].xOffset + font.characterSet->letters[c].width;
					if (width >= maxPixels)
						return count;// 
					count++;
					currentX += font.characterSet->letters[c].xAdvance;
				}
				return count;
			}


			void GetSizes(const game::SpriteFont &font, const std::string& string, game::Recti& boundingBox, int32_t& width, int32_t& height)
			{
				boundingBox = BoundingBox(font, string);
				width = boundingBox.right - boundingBox.left;
				height = boundingBox.bottom - boundingBox.top;
			}

			int32_t Height(const game::SpriteFont &font, const std::string& text)
			{
				game::Recti bbox = BoundingBox(font, text);
				return bbox.bottom - bbox.top;
			}

			game::Recti BoundingBox(const game::SpriteFont &font, const std::string& string)
			{
				if (string == "")
				{
					Recti e;
					return e;
				}

				ParseColoredString(string, game::Colors::White);

				Recti destination;
				Recti box(20000, 20000, -20000, -20000);
				int32_t currentX = 0;
				int32_t currentY = 0;
				for (auto& seg : segments)
				{
					const uint64_t size = seg.text.size();
					for (uint64_t i = 0; i < size; ++i)
					{
						const uint8_t letter = string[i];
						const uint32_t widthOfLetter = font.characterSet->letters[letter].width;
						const uint32_t heightOfLetter = font.characterSet->letters[letter].height;

						destination.left = currentX + (font.characterSet->letters[letter].xOffset);
						destination.top = currentY + (font.characterSet->letters[letter].yOffset);
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
						currentX += (font.characterSet->letters[letter].xAdvance);
					}
				}
				return box;
			}


			int32_t GetCursorPositionInText(const game::SpriteFont &font, const int32_t cursorPosition, const std::string& text) const
			{
				int32_t offset = 0;
				int32_t currentX = 0;

				if (cursorPosition <= 0) return 0;

				for (uint64_t i = 0; i < cursorPosition; i++)
				{
					const uint8_t letter = text[i];
					const uint32_t widthOfLetter = font.characterSet->letters[letter].width;
					offset = currentX + font.characterSet->letters[letter].xOffset;
					offset = offset + (widthOfLetter);
					if (letter == ' ') offset += font.characterSet->letters[letter].xAdvance;
					currentX += (font.characterSet->letters[letter].xAdvance);// *_scaleX);
				}
				return offset;
			}

			std::string ColorTagWrap(const std::string& str, const game::Color& color)
			{
				std::string ret;
				ret = "\\|" + color.hexidecimal;// +">";
				ret += str;
				ret += "\\e";
				return ret;
			}

			// Parser function
			void ParseColoredString(const std::string& input, const Color& defaultColor = Colors::White, const bool removeCode = true)
			{
				/*std::vector<ColorTextSegment> segments;*/
				segments.clear();
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
						//ColorTextSegment temp = ;
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
						if (!removeCode) segments.push_back({ input.substr(tagStart + 10), currentColor });
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

				//return segments;
			}

		private:

		};
	}
}
