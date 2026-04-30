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
#include "Game_Assert.h"

namespace game
{
#include "Game.h"
	class SpriteSubSheet
	{
	public:
		std::unordered_map<std::string, game::Recti> subTexture;
		game::Texture2D texture;
		~SpriteSubSheet()
		{
			if (texture.isLoaded) Unload();
		}
		game::Recti RectOf(const std::string& name)
		{
			GAME_ASSERT(!(subTexture.find(name) == subTexture.end()));
			game::Recti ret;
			ret.top = 0;
			ret.left = 0;
			ret.bottom = subTexture.at(name).bottom;
			ret.right = subTexture.at(name).right;
			return ret;
		}
		void Unload()
		{
			if (enginePointer)
			{
				if (texture.isLoaded) enginePointer->geUnLoadTexture(texture);
			}
		}
		bool Load(std::string filename)
		{
			std::ifstream f(filename + ".txt");

			if (!f.is_open()) 
			{
				return false;
			}
			else
			{
				std::string s;

				std::string filenameInput;
				game::Recti rect;
				//std::stringstream  ss;
				while (getline(f, s))
				{
					std::stringstream ss(s);
					// filename
					getline(ss, s, ',');
					filenameInput = s;
					// x,y,width,height
					getline(ss, s, ',');
					rect.left = std::stoul(s);

					getline(ss, s, ',');
					rect.top = std::stoul(s);

					getline(ss, s, ',');
					rect.right = std::stoul(s);

					getline(ss, s, ',');
					rect.bottom = std::stoul(s);

					subTexture[filenameInput] = rect;

					std::cout << filenameInput << "," << rect.left << "," << rect.top << "," << rect.right << "," << rect.bottom << std::endl;
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