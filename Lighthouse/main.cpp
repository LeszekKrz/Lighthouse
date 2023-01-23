#include "SDL.h"
#include <iostream>
#include <cmath>
#include <WindowsNumerics.h>
#include <fstream>
#include <string>
#include "Import.h"
#include "Rendering.h"

const int width = 800;
const int height = 800;

using namespace Windows::Foundation::Numerics;

int main(int argc, char* argv[])
{

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Lighthouse", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, width, height);

	unsigned char* pixels = (unsigned char*)malloc(width * height * 3);
	memset(pixels, 255, width * height * 3);

	
	object objects[4];
	object currObjects[4];
	object worldObjects[4];
	ImportScene(objects);

	//camera view = { float3(150, 50, 0), float3(0, 0, 0), float3(0,0,0), float3(0,0,0), 1, 1000, M_PI / 4, 1 };
	camera view = { float3(0, 120, 120), float3(0, 0, 0), float3(0,0,0), float3(0,0,0), 50, 200, M_PI / 4, 1 };

	SDL_Event event;

	bool running = true;

	float step = M_PI / 720;
	float angle = 0;
	float currStep = step;

	int cameraType = 0;
	int shadingType = 0;

	bool paused = false;

	Uint32 start, end;
	//float fps;

	point reflector;
	
	while (running)
	{
		//start = SDL_GetTicks();
		SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_k:
				std::cout << "k\n";
				cameraType = (cameraType + 1) % 3;
				break;
			case SDLK_p:
				std::cout << "p\n";
				if (paused) paused = false;
				else paused = true;
				break;
			case SDLK_s:
				std::cout << "s\n";
				shadingType = (shadingType + 1) % 3;
				switch (shadingType)
				{
				case 0:
					step = M_PI / 720;
					break;
				case 1:
					step = M_PI / 360;
					break;
				case 2:
					step = M_PI / 90;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}

		default:
			break;
		}
		if (paused) currStep = 0;
		else currStep = step;
		angle += currStep;
		memset(pixels, 0, width * height * 3);
		TransformObjects(objects, currObjects, worldObjects, view, reflector, angle, cameraType);
		DrawObjects(currObjects, worldObjects, view, reflector, pixels, shadingType);

		SDL_UpdateTexture(texture, NULL, pixels, width * 3);
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		SDL_RenderPresent(renderer);

		//end = SDL_GetTicks();
		//fps = 1000.0f / (end - start);
		//std::cout << fps << std::endl;
		//std::cout << end - start << std::endl;
	}

	free(pixels);
	std::cout << "Koniec\n";
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);



	return 0;
}