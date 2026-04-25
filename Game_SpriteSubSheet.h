#pragma once

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <Game.h>
#include "GameMath.h"
#include "GameTexture2D.h"

namespace game
{

	class SpriteSubSheet
	{
	public:
		std::unordered_map<std::string, game::Recti> subTexture;
		game::Texture2D texture;
		bool Load(std::string filename)
		{
			// Open the text file named 
	// "textFile.txt"
			std::ifstream f(filename + ".txt");

			// Check if the file is 
			// successfully opened
			if (!f.is_open()) {
				return false;
			}
			else
			{
				std::string s;

				std::string filename;
				game::Recti rect;
				std::stringstream  ss;
				while (getline(f, s))
				{
					std::stringstream ss(s);
					// filename
					getline(ss, s, ',');
					filename = s;
					// x,y,width,height
					getline(ss, s, ',');
					rect.left = std::stoul(s);

					getline(ss, s, ',');
					rect.top = std::stoul(s);

					getline(ss, s, ',');
					rect.right = std::stoul(s);

					getline(ss, s, ',');
					rect.bottom = std::stoul(s);

					subTexture[filename] = rect;

					std::cout << filename << "," << rect.left << "," << rect.top << "," << rect.right << "," << rect.bottom << std::endl;
				}

				// Close the file
				f.close();
			}
			//texture.isMipMapped = true;
			//texture.filterType = TextureFilterType::Trilinear;
			if (!game::enginePointer->geLoadTexture(filename + ".png", texture))
			{
				return false;
			}
			return true;
		}

	};
}