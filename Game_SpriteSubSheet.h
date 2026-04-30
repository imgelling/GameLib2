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
		std::unordered_map<std::string, game::Recti> subTextureRegistry;
		game::Texture2D texture;
		~SpriteSubSheet()
		{
			if (texture.isLoaded) Unload();
		}

		Pointi SizeOf(const std::string& name)
		{
			GAME_ASSERT(!(subTextureRegistry.find(name) == subTextureRegistry.end()));
			Pointi size;
			size.height = subTextureRegistry.at(name).bottom;
			size.width = subTextureRegistry.at(name).right;
			return size;
		}

		game::Recti RectOf(const std::string& name)
		{
			GAME_ASSERT(!(subTextureRegistry.find(name) == subTextureRegistry.end()));
			game::Recti ret;
			ret.top = 0;
			ret.left = 0;
			ret.bottom = subTextureRegistry.at(name).bottom;
			ret.right = subTextureRegistry.at(name).right;
			return ret;
		}

		void Unload()
		{
			GAME_ASSERT(enginePointer);
			//if (enginePointer)
			{
				if (texture.isLoaded) enginePointer->geUnLoadTexture(texture);
			}
			subTextureRegistry.clear();
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

					subTextureRegistry[filenameInput] = rect;

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