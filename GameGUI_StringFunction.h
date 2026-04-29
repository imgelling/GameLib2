#pragma once
#include <GameColor.h>

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
