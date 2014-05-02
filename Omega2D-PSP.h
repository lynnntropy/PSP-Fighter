#include <SDL/SDL.h> // base SDL header
#include <SDL_image.h> // the base for Omega2D::LoadImage()
#include <SDL_ttf.h> // for drawing TTF fonts
#include <string> // for C++ style strings
#include <pspkernel.h> // for PSP kernel calls
#include <pspctrl.h> // for input (not using SDL input)
#include <cstdlib> // C-style stdlib
#include <cstdio> // C-style stdio
#include <vector> // needed to use std::vector
#include <pspdisplay.h> // PSP library for display features (VSync) 
#include <cmath> // C math library
#include <SDL_gfxPrimitives.h>
#include <map>

namespace Omega2D
{
	SDL_Surface *screenSurface = NULL;

	struct Vector2
	{
		float x;
		float y;

		Vector2() {};

		Vector2(float initX, float initY)
		{
			x = initX;
			y = initY;
		}
	}	;

	float Clamp(float value, float min, float max)
	{
		if (value > max)
			return max;
		else if (value < min)
			return min;
		else
			return value;
	}

	float Lerp(float a, float b, float fraction) 
	{
    	return (a * (1.0f - fraction)) + (b * fraction);
	}
	

	int RandomInt(int min, int max)
	{
		int result = rand() % max + min;
		return result;
	}


	int InitInput()
	{		
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

		return 0;
	}

	bool GetButton(int buttonValue)
	{
		SceCtrlData inputSnapshot;
		sceCtrlPeekBufferPositive(&inputSnapshot, 1);

		if (inputSnapshot.Buttons != 0)
		{
			if (inputSnapshot.Buttons & buttonValue)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	int InitGraphics(int screenWidth, int screenHeight, int bitDepth)
	{
			SDL_Init(SDL_INIT_EVERYTHING);
			screenSurface = SDL_SetVideoMode(screenWidth, screenHeight, bitDepth, SDL_SWSURFACE);

			TTF_Init(); // initialize SDL_TTF			
	}

	int ClearScreen(int hexColor)
	{
		SDL_FillRect(screenSurface, NULL, hexColor);
	}

	int UpdateScreen(int frameTime, bool vsync)
	{
		int sdlReturn;

		if (vsync)
		{
			// essentially, enable vsync
			sceDisplayWaitVblankStart();

			sdlReturn = SDL_Flip(screenSurface);			
		}
		else
		{
			SDL_Delay(frameTime);

			sdlReturn = SDL_Flip(screenSurface);			
		}

		return sdlReturn;
	}

	// Supports BMP, PNM (PPM/PGM/PBM), XPM, LBM, PCX, GIF, JPEG, PNG, TGA, and TIFF.
	// Supports alpha transparency, for the formats that provide it.
	SDL_Surface *LoadImage(char *file_path)
	{
		SDL_Surface *image = IMG_Load(file_path); // load image to a surface using SDL_image

		SDL_Surface *convertedImage = SDL_DisplayFormatAlpha(image);

		SDL_FreeSurface(image);
		return convertedImage;
	}

	void DrawImage(int x, int y, SDL_Surface* image)
	{
		SDL_Rect offset;

		offset.x = x;
		offset.y = y;

		SDL_BlitSurface(image, NULL, screenSurface, &offset);
	}

	TTF_Font *LoadFont(char *file_path, int ptSize)
	{
		TTF_Font *font = TTF_OpenFont(file_path, ptSize);
	 	return font;
	}

	void DrawText(TTF_Font *font, char *text, SDL_Color color, int x, int y)
	{
		SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, color);
		SDL_Rect screenOffset = {x, y, 0, 0};

		SDL_BlitSurface(textSurface, NULL, screenSurface, &screenOffset);
		
		SDL_FreeSurface(textSurface); // prevent a memory leak
	}

	bool CircleCollision(Vector2 vec1, Vector2 vec2, int rad1, int rad2)
	{
		int dx = vec1.x - vec2.x;
		int dy = vec1.y - vec2.y;

		int radii = rad1 + rad2;

		if ((dx*dx) + (dy*dy) < radii*radii)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void DrawCircle(Vector2 center, float radius)
	{
		// use SDL_gfx to draw a circle
		// circleRGBA(screenSurface, center.x, center.y, radius, 255, 255, 255, 255);
		circleColor(screenSurface, center.x, center.y, radius, 0xFFFFFFFF);
	}

	void Quit()
	{
		SDL_FreeSurface(screenSurface);
		TTF_Quit();
		SDL_Quit();
	}
}
