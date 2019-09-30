#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include <math.h>
#include <stdbool.h>
#include <fstream>

using namespace std;

//useful variables
int mouseX = 0;
int mouseY = 0;
int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;
int squareSide = 64;
const int levelX = 50;
const int levelY = 50;
int gameLevel = 1;

void wipeScreen(SDL_Renderer *renderer)
{
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;
	SDL_RenderFillRect(renderer, &rect);
}

struct menuButton
{
	//info for a button represented in ratios of screen height and width
	double posX;
	double posY;
	double sizeX;
	double sizeY;
	bool hasBorder;
	//border color
	int borR;
	int borG;
	int borB;
	//default RGB color
	int defR;
	int defG;
	int defB;
	//new color when hovered over
	int hovR;
	int hovG;
	int hovB;
	//functions
	int trueX()
	{
		return posX * SCREEN_WIDTH;
	}
	int trueY()
	{
		return posY * SCREEN_HEIGHT;
	}
	int trueSizeX()
	{
		return sizeX * SCREEN_WIDTH;
	}
	int trueSizeY()
	{
		return sizeY * SCREEN_HEIGHT;
	}
	SDL_Rect rect()
	{
		SDL_Rect result;
		result.x = trueX();
		result.y = trueY();
		result.w = trueSizeX();
		result.h = trueSizeY();
		return result;
	}
	bool isHovered()
	{
		if(mouseX >= trueX() && mouseX <= trueX() +  trueSizeX() && mouseY >=  trueY() && mouseY <=  trueY() +  trueSizeY())
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	//the filename of the image of the button
	string label;
};

menuButton startGameButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.15;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "Start";
	return result;
}

menuButton quitButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.75;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "Quit";
	return result;
}

menuButton resButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.45;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "Change Resolution";
	return result;
}

menuButton fullscreenButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.60;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "Toggle Fullscreen";
	return result;
}

menuButton ratioButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.30;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "Toggle Ratio";
	return result;
}

SDL_Rect textSubBox(menuButton input)
{
	SDL_Rect rect;
	if(input.label.length() < 7)
	{
		rect.x = input.trueX() + 0.25f * input.trueSizeX();
		rect.y = input.trueY() + 0.25f * input.trueSizeY();
		rect.w = 0.5f * input.trueSizeX();
		rect.h = 0.5f * input.trueSizeY();
	}
	else
	{
		rect.x = input.trueX() + 0.025f * input.trueSizeX();
		rect.y = input.trueY() + 0.25f * input.trueSizeY();
		rect.w = 0.95f * input.trueSizeX();
		rect.h = 0.5f * input.trueSizeY();
	}
	return rect;
}

void drawButton(menuButton theButton, SDL_Renderer *renderer, bool widescreen)
{
	//if widescreen
	if(widescreen)
	{
		theButton.sizeX /= (16.0/9.0)/(4.0/3.0);
	}
	
	//if mouse is over button
	if(theButton.isHovered())
	{
		SDL_SetRenderDrawColor(renderer, theButton.hovR, theButton.hovG, theButton.hovB, 255);
	} 
	else
	{
		SDL_SetRenderDrawColor(renderer, theButton.defR, theButton.defG, theButton.defB, 255);
	}
	
	//rectangle
	SDL_Rect rect = theButton.rect();
	SDL_RenderFillRect(renderer, &rect);
	//border
	if(theButton.hasBorder)
	{
		SDL_SetRenderDrawColor(renderer, theButton.borR, theButton.borG, theButton.borB, 255);
		SDL_RenderDrawRect(renderer, &rect);
	}
	//text
	TTF_Font *font = TTF_OpenFont("Assets/TTF/Ubuntu-R.ttf", 72);
	if(font!=NULL)
	{
		SDL_Color color = {255,255,255};
		SDL_Surface *surface = TTF_RenderText_Solid(font, theButton.label.c_str(), color);
		TTF_CloseFont(font);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		SDL_Rect textRect = textSubBox(theButton);
		SDL_RenderCopy(renderer, texture, NULL, &textRect);
		SDL_DestroyTexture(texture);
	}
}

int getResPos()
{
	if(SCREEN_WIDTH==640 || SCREEN_WIDTH==1280)
	{
		return 0;
	}
	else if(SCREEN_WIDTH==800 || SCREEN_WIDTH==1600)
	{
		return 1;
	}
	else if(SCREEN_WIDTH==1024 || SCREEN_WIDTH==1920)
	{
		return 2;
	}
	return 0;
}

//return 0 to quit game, 1 to play game, other buttons to be added
int mainMenu(SDL_Window *window, SDL_Renderer *renderer, SDL_Event event)
{
	int buttonCount = 5;
	menuButton buttons[buttonCount];
	buttons[0] = startGameButton();
	buttons[1] = quitButton();
	buttons[2] = resButton();
	buttons[3] = ratioButton();
	buttons[4] = fullscreenButton();
	SDL_Surface *surface = IMG_Load("Assets/Art/UI/menu bg.png");
	SDL_Texture *bg = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	
	//window stuff
	bool fullscreen = 0;
	bool widescreen = 1;
	if(SCREEN_WIDTH / SCREEN_HEIGHT < 1.7f)
		widescreen = 0;
	int resPos = getResPos();
	int standardX[3] = {640,800,1024};
	int standardY[3] = {480,600,768};
	int wideX[3] = {1280,1600,1920};
	int wideY[3] = {720,900,1080};
	
	//begin music
	Mix_Music *bgm = Mix_LoadMUS("Assets/Sound/Music/A Title Screen.wav");
	Mix_Chunk *click = Mix_LoadWAV("Assets/Sound/SFX/click.wav");
	Mix_Chunk *beep = Mix_LoadWAV("Assets/Sound/SFX/beep.wav");
	Mix_PlayMusic(bgm, -1);
	
	int returnValue=-1;
	bool wasHovered[buttonCount]={0};
	while(returnValue==-1)
	{

		//draw graphics
		wipeScreen(renderer);//clear screen
		SDL_RenderCopy(renderer,bg,NULL,NULL);//draw menu bg
		for(int i = 0; i < buttonCount; i++)
		{
			drawButton(buttons[i],renderer,widescreen);
		}
		SDL_RenderPresent(renderer);//draw what's been queued
		
		//events
		SDL_WaitEvent(&event);
		if(event.type == SDL_MOUSEMOTION)//mouse motion
		{
			SDL_GetMouseState(&mouseX, &mouseY);//track the location of the mouse
			for(int i =0; i < buttonCount; i++)
			{
				if(buttons[i].isHovered())
				{
					if(!wasHovered[i])
					{
						Mix_PlayChannel(-1,click,0);//click sound when the mouse is moved over a button
					}
					wasHovered[i] = 1;
				}
				else
				{
					wasHovered[i] = 0;
				}
			}
		}
		else if(event.type == SDL_MOUSEBUTTONDOWN)//on click
		{
			if(event.button.button == SDL_BUTTON_LEFT)
			{
				//check location and perform an action based on that
				for(int i =0; i < buttonCount; i++)
				{
					if(buttons[i].isHovered())
					{
						Mix_PlayChannel(-1,beep,0);
						switch(i)//take the appropriate action per button
						{
							case 0:
								returnValue = 1;
								break;
							case 1:
								returnValue = 0;
								break;
							case 2:
								resPos++;
								if(resPos>2)
									resPos = 0;
								if(widescreen)
								{
									SCREEN_WIDTH = wideX[resPos];
									SCREEN_HEIGHT = wideY[resPos];
								}
								else
								{
									SCREEN_WIDTH = standardX[resPos];
									SCREEN_HEIGHT = standardY[resPos];
								}
								SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
								break;
							case 3:
								if(widescreen)
									widescreen=0;
								else
									widescreen=1;
								if(widescreen)
								{
									SCREEN_WIDTH = wideX[resPos];
									SCREEN_HEIGHT = wideY[resPos];
								}
								else
								{
									SCREEN_WIDTH = standardX[resPos];
									SCREEN_HEIGHT = standardY[resPos];
								}
								SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
								break;
							case 4:
								if(fullscreen)
									fullscreen=0;
								else
									fullscreen=1;
								if(fullscreen)
									SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
								else
									SDL_SetWindowFullscreen(window,0);
								break;
						}
					}
				}
			}
		}
		else if(event.type == SDL_WINDOWEVENT)//window events
		{
			switch(event.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
					returnValue = 0;
					break;
			}
		}
		else
		{
			SDL_FlushEvent(event.type);
		}
	}
	Mix_HaltMusic();
	Mix_FreeChunk(click);
	Mix_FreeChunk(beep);
	Mix_FreeMusic(bgm);
	SDL_DestroyTexture(bg);
	return returnValue;
}

menuButton loadGameButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.15;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "Load Game";
	return result;
}

menuButton newGameButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.3;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "New Game";
	return result;
}

menuButton backButton()
{
	menuButton result;
	//position
	result.posX = 0.4;
	result.posY = 0.45;
	result.sizeX = 0.2;
	result.sizeY = 0.1;
	result.hasBorder = 1;
	//border color
	result.borR = 255;
	result.borG = 255;
	result.borB = 255;
	//default color
	result.defR = 50;
	result.defG = 50;
	result.defB = 50;
	//hover color
	result.hovR = 100;
	result.hovG = 100;
	result.hovB = 100;
	//label
	result.label = "Back";
	return result;
}

//options: new game, load game, back
int fileMenu(SDL_Window *window, SDL_Renderer *renderer, SDL_Event event)
{
	FILE *save = fopen("Data/playerdata.sav","r");
	
	int buttonCount = 3;
	menuButton buttons[buttonCount];
	buttons[0] = loadGameButton();
	buttons[1] = newGameButton();
	buttons[2] = backButton();
	if(save == NULL)
	{
		buttons[0].posY = -0.5;
		buttons[1].posY = 0.15;
		buttons[2].posY = 0.3;
	}
	bool widescreen = 1;
	if(SCREEN_WIDTH / SCREEN_HEIGHT < 1.7f)
		widescreen = 0;
	bool wasHovered[buttonCount] = {0};
	
	SDL_Surface *surface = IMG_Load("Assets/Art/UI/static.png");
	SDL_Texture *bg = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	Mix_Music *bgm = Mix_LoadMUS("Assets/Sound/Music/File Management.wav");
	Mix_Chunk *click = Mix_LoadWAV("Assets/Sound/SFX/click.wav");
	Mix_Chunk *beep = Mix_LoadWAV("Assets/Sound/SFX/beep.wav");
	Mix_PlayMusic(bgm, -1);
	
	int returnValue = -1;
	while(returnValue == -1)
	{
		//draw graphics
		wipeScreen(renderer);//clear screen
		SDL_RenderCopy(renderer,bg,NULL,NULL);//draw
		for(int i = 0; i < buttonCount; i++)
		{
			drawButton(buttons[i],renderer,widescreen);
		}
		SDL_RenderPresent(renderer);//draw what's been queued
		
		//events
		SDL_WaitEvent(&event);
		if(event.type == SDL_MOUSEMOTION)//mouse motion
		{
			SDL_GetMouseState(&mouseX, &mouseY);//track the location of the mouse
			for(int i =0; i < buttonCount; i++)
			{
				if(buttons[i].isHovered())
				{
					if(!wasHovered[i])
					{
						Mix_PlayChannel(-1,click,0);//click sound when the mouse is moved over a button
					}
					wasHovered[i] = 1;
				}
				else
				{
					wasHovered[i] = 0;
				}
			}
		}
		else if(event.type == SDL_MOUSEBUTTONDOWN)//on click
		{
			if(event.button.button == SDL_BUTTON_LEFT)
			{
				//check location and perform an action based on that
				for(int i =0; i < buttonCount; i++)
				{
					if(buttons[i].isHovered())
					{
						Mix_PlayChannel(-1,beep,0);
						returnValue = i;
					}
				}
			}
		}
		else if(event.type == SDL_WINDOWEVENT)//window events
		{
			switch(event.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
					returnValue = -2;
					break;
			}
		}
		else
		{
			SDL_FlushEvent(event.type);
		}
	}
	Mix_FreeChunk(click);
	Mix_FreeChunk(beep);
	Mix_HaltMusic();
	Mix_FreeMusic(bgm);
	SDL_DestroyTexture(bg);
	if(returnValue == 0)
	{
		fread(&gameLevel,sizeof(int),1,save);
		fclose(save);
	}
	else
	{
		gameLevel = 1;
	}
	return returnValue;
}

void fadeBetweenImages(SDL_Renderer *renderer, string filename1, string filename2)
{
	SDL_Surface *s1 = IMG_Load(filename1.c_str());
	SDL_Surface *s2 = IMG_Load(filename2.c_str());
	SDL_Texture *t1 = SDL_CreateTextureFromSurface(renderer,s1);
	SDL_Texture *t2 = SDL_CreateTextureFromSurface(renderer,s2);
	SDL_FreeSurface(s1);
	SDL_FreeSurface(s2);
	int interval = 10;
	SDL_Rect rect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
	for(int i = 0; i <=255; i+=interval)
	{
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, t1, NULL, NULL);
		SDL_SetRenderDrawColor(renderer,0,0,0,i);
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderPresent(renderer);
	}
	for(int i = 255; i >=0; i-=interval)
	{
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, t2, NULL, NULL);
		SDL_SetRenderDrawColor(renderer,0,0,0,i);
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderPresent(renderer);
	}
	SDL_DestroyTexture(t1);
	SDL_DestroyTexture(t2);
}

int introScene(SDL_Window *window, SDL_Renderer *renderer,SDL_Event event)
{
	int lineCount = 12;
	string linesOfText[lineCount] = {"Current Date: September 30th, 2059","Last Login: December 17th, 2023","------------------","Solar Charge Complete - Current Battery 100%"," ","........................"," ", " ", "Objective : Return to Lunar Base"," "," ","[Press any key to continue]"};
	int drawX = 10;
	int drawY = 10;
	TTF_Font *font = TTF_OpenFont("Assets/TTF/Ubuntu-R.ttf", 72);
	SDL_Surface *surface;
	SDL_Texture *texture;
	Mix_Music *bgm = Mix_LoadMUS("Assets/Sound/Music/static.wav");
	Mix_PlayMusic(bgm, -1);
	Mix_Chunk *click = Mix_LoadWAV("Assets/Sound/SFX/click.wav");
	if(font == NULL) return 0;
	
	//make screen black
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer,0,0,0,255);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;
	SDL_RenderFillRect(renderer, &rect);
	
	for(int currentLine = 0; currentLine < lineCount; currentLine++)
	{
		for(unsigned int currentIndex = 0; currentIndex < linesOfText[currentLine].length(); currentIndex++)
		{
			SDL_PollEvent(&event);
			if(event.type == SDL_WINDOWEVENT)//window events
			{
				if(event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return -1;
				}
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym==SDLK_ESCAPE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return 0;
				}
			}
			
			SDL_Rect rect = {drawX, drawY, 10,20};
			SDL_Color color = {255,255,255};
			char character[1];
			character[0] = linesOfText[currentLine].c_str()[currentIndex];
			surface = TTF_RenderText_Solid(font, character , color);
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_RenderCopy(renderer,texture,NULL,&rect);
			SDL_RenderPresent(renderer);
			SDL_Delay(32);
			drawX+=12;
			Mix_PlayChannel(-1,click,0);
		}
		for(int i =0; i<60; i++)
		{
			SDL_PollEvent(&event);
			if(event.type == SDL_WINDOWEVENT)//window events
			{
				if(event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return -1;
				}
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym==SDLK_ESCAPE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return 0;
				}
			}
			SDL_Delay(16);
		}
		drawX=10;
		drawY+=40;
	}
	while(1)
	{
		SDL_PollEvent(&event);
		if(event.type == SDL_WINDOWEVENT)//window events
		{
			if(event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				TTF_CloseFont(font);
				SDL_FreeSurface(surface);
				SDL_DestroyTexture(texture);
				Mix_FreeChunk(click);
				Mix_FreeMusic(bgm);
				return -1;
			}
		}
		else if(event.type == SDL_KEYDOWN)
		{
			TTF_CloseFont(font);
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
			Mix_FreeChunk(click);
			Mix_FreeMusic(bgm);
			return 0;
		}
	}
	TTF_CloseFont(font);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	Mix_FreeChunk(click);
	Mix_FreeMusic(bgm);
	return 0;
}

int outroScene(SDL_Window *window, SDL_Renderer *renderer,SDL_Event event)
{
	int lineCount = 9;
	string linesOfText[lineCount] = {"Base entry - login successful", "","Objective Complete","","","-----------------------","","Congratulations!","[Press any key to exit the game]"};
	int drawX = 10;
	int drawY = 10;
	TTF_Font *font = TTF_OpenFont("Assets/TTF/Ubuntu-R.ttf", 72);
	SDL_Surface *surface;
	SDL_Texture *texture;
	Mix_Music *bgm = Mix_LoadMUS("Assets/Sound/Music/static.wav");
	Mix_PlayMusic(bgm, -1);
	Mix_Chunk *click = Mix_LoadWAV("Assets/Sound/SFX/click.wav");
	if(font == NULL) return 0;
	
	//make screen black
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer,0,0,0,255);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = SCREEN_WIDTH;
	rect.h = SCREEN_HEIGHT;
	SDL_RenderFillRect(renderer, &rect);
	
	for(int currentLine = 0; currentLine < lineCount; currentLine++)
	{
		for(unsigned int currentIndex = 0; currentIndex < linesOfText[currentLine].length(); currentIndex++)
		{
			SDL_PollEvent(&event);
			if(event.type == SDL_WINDOWEVENT)//window events
			{
				if(event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return -1;
				}
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym==SDLK_ESCAPE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return 0;
				}
			}
			
			SDL_Rect rect = {drawX, drawY, 10,20};
			SDL_Color color = {255,255,255};
			char character[1];
			character[0] = linesOfText[currentLine].c_str()[currentIndex];
			surface = TTF_RenderText_Solid(font, character , color);
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_RenderCopy(renderer,texture,NULL,&rect);
			SDL_RenderPresent(renderer);
			SDL_Delay(32);
			drawX+=12;
			Mix_PlayChannel(-1,click,0);
		}
		for(int i =0; i<60; i++)
		{
			SDL_PollEvent(&event);
			if(event.type == SDL_WINDOWEVENT)//window events
			{
				if(event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return -1;
				}
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym==SDLK_ESCAPE)
				{
					TTF_CloseFont(font);
					SDL_FreeSurface(surface);
					SDL_DestroyTexture(texture);
					Mix_FreeChunk(click);
					Mix_FreeMusic(bgm);
					return 0;
				}
			}
			SDL_Delay(16);
		}
		drawX=10;
		drawY+=40;
	}
	while(1)
	{
		SDL_PollEvent(&event);
		if(event.type == SDL_WINDOWEVENT)//window events
		{
			if(event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				TTF_CloseFont(font);
				SDL_FreeSurface(surface);
				SDL_DestroyTexture(texture);
				Mix_FreeChunk(click);
				Mix_FreeMusic(bgm);
				return -1;
			}
		}
		else if(event.type == SDL_KEYDOWN)
		{
			TTF_CloseFont(font);
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
			Mix_FreeChunk(click);
			Mix_FreeMusic(bgm);
			return 0;
		}
	}
	TTF_CloseFont(font);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	Mix_FreeChunk(click);
	Mix_FreeMusic(bgm);
	return 0;
}

struct levelMap
{
	string bgFilename;
	string bgmFilename;
	void clearMap()
	{
		for(int i =0; i< levelX; i++)
		{
			for(int j = 0; j < levelY; j++)
			{
				map[i][j]=0;
			}
		}
	}
	int map[levelX][levelY];
	
};

levelMap loadMap(int levelNumber)
{
	levelMap result;
	result.clearMap();
	result.bgFilename = "Assets/Art/World/";
	result.bgmFilename = "Assets/Sound/Music/";
	
	string fileName = "Data/Maps/" + to_string(levelNumber) + ".map";
	
	string readData[50][50]={"0"};
	ifstream input(fileName.c_str());
	if(!input.good()) return result;
	for(int y =0; y<50; y++)
	{
		for(int x =0; x<50; x++)
		{
			getline(input,readData[x][y],',');
		}
	}
	
	for(int y =0; y<50; y++)
	{
		for(int x =0; x<50; x++)
		{
			result.map[x][y] = stoi(readData[x][y]);
			//cout << to_string(result.map[x][y])<<endl;
		}
	}
	
	string bg = "";
	getline(input,bg,',');
	result.bgFilename+=bg;
	string bgm = "";
	getline(input,bgm,',');
	result.bgFilename+=bgm;
	input.close();
	
	return result;
}

struct movingEntity
{
	double xpos;
	double ypos;
	long double xvel;
	double yvel;
	double xacc;
	double yacc;
	int lastDir; //last facing direction, left (0) or right (1)
	string name;
	void initialize()
	{
		xpos =0;
		ypos =0;
		xvel =0;
		yvel =0;
		xacc =0;
		yacc =0;
		lastDir = 1;
	}
	void updateMovement(double deltaT)
	{
		xvel += xacc * deltaT;
		yvel += yacc * deltaT;
		xpos += xvel * deltaT;
		ypos += yvel * deltaT;
	}
	int getSpriteIndex()
	{
		int result=0;
		if(yvel!=0) result+=2;
		if(lastDir) result+=1;
		return result;
	}
};

void drawMap(SDL_Renderer *renderer, levelMap map, SDL_Texture **tiles, movingEntity player, SDL_Texture **playerSprites)
{
	//percentage of where in the screen the player is centered
	double xMaxPercent = 0.5f;
	double yMaxPercent = 0.75f;
	
	//draw the whole map, with entities, but such that only the part we care about is in the screen
	//first get the center of the screen in terms of game pixels
	//	this will be at the center of the player
	int cameraCenterX;
	int cameraCenterY;
	cameraCenterX = player.xpos + squareSide/2;
	cameraCenterY = player.ypos + squareSide/2;
	
	//use this to find an offset
	//the offset should be such that
	/**
	 * . < game zero
	 * 
	 *   screen  0 ______________
	 *            |      center  |
	 *            |      .       |
	 *            |______________|
	 *  __________
	 * offset
	 */
	//thus offset is equal to:
	//centerx - width/2 for x
	//centery - height/2 for x
	
	int offsetX;
	int offsetY;
	double xPercent = (double)((double)cameraCenterX / (double)SCREEN_WIDTH);
	double yPercent = (double)((double)cameraCenterY / (double)SCREEN_HEIGHT);
	if(xPercent > xMaxPercent)
		xPercent = xMaxPercent;
	if(yPercent > yMaxPercent)
		yPercent = yMaxPercent;
	
	if(cameraCenterX+SCREEN_WIDTH/2 > 50*squareSide)//if out of bounds would be shown
	{
		//xPercent must be set such that cameraCenterX + xPercent * SCREEN_WIDTH == 50*squareSide
		//xPercent * SCREEN_WIDTH == 50*squareSide - cameraCenterX
		//xPercent == (50*squareSide - cameraCenterX) / SCREEN_WIDTH
		
		
		xPercent = 1.0f-(double)((double)(50*squareSide - cameraCenterX) / SCREEN_WIDTH);
		if(xPercent < xMaxPercent)
			xPercent = xMaxPercent;
	}
	if(cameraCenterY+SCREEN_HEIGHT/2 > 50*squareSide)
	{
		yPercent = 1.0f-(double)((double)(50*squareSide - cameraCenterY) / SCREEN_HEIGHT);
		if(yPercent < yMaxPercent)
			yPercent = yMaxPercent;
	}
	
	offsetX = cameraCenterX - SCREEN_WIDTH * xPercent;
	offsetY = cameraCenterY - SCREEN_HEIGHT * yPercent;
	
	
	//now we gotta use this to draw everything
	//this is simple: use nested for loops from 0 to < 50
	//the rect of drawing will be {x*squareSide - offsetX, y*squareSide - offsetY, squareSide, squareSide}
	//then draw it there
	
	for(int y = 0; y < 50; y++)
	{
		for(int x = 0; x < 50; x++)
		{
			SDL_Rect rect = {x*squareSide - offsetX, y*squareSide - offsetY, squareSide, squareSide};
			if(map.map[x][y]>0)
			{
				SDL_RenderCopy(renderer,tiles[map.map[x][y]],NULL,&rect);
			}
		}
	}
	
	//lastly, draw the player to be centered
	SDL_Rect playerRect = {(int)(SCREEN_WIDTH*xPercent) - squareSide/2, (int)(SCREEN_HEIGHT*yPercent) - squareSide/2, squareSide, squareSide};
	SDL_RenderCopy(renderer, playerSprites[player.getSpriteIndex()],NULL,&playerRect);
}

movingEntity playerEnt(int xpos, int ypos)
{
	movingEntity result;
	result.initialize();
	result.xpos = xpos;
	result.ypos = ypos;
	result.name = "robot";
	return result;
}

//checks collision
movingEntity checkCollision(movingEntity player, levelMap map, double deltaT)
{
	int extraPixels = 3;
	int checkX;
	int checkY;
	int xIndex;
	int yIndex;
	//up
	if(player.yvel < 0)
	{
		checkX = player.xpos;
		checkY = player.ypos + extraPixels;
		xIndex = checkX/squareSide;
		yIndex = checkY/squareSide;
		if(xIndex <50 && xIndex >-1 && yIndex <50 && yIndex >-1)
		{
			if(map.map[xIndex][yIndex]>0 && map.map[xIndex][yIndex]<16)
			{
				player.yvel = 0;
				player.ypos+=3;
			}
		}
		checkX = player.xpos+squareSide;
		xIndex = checkX/squareSide;
		if(xIndex <50 && xIndex >-1 && yIndex <50 && yIndex >-1)
		{
			if(map.map[xIndex][yIndex]>0 && map.map[xIndex][yIndex]<16)
			{
				player.yvel = 0;
				player.ypos+=3;
			}
		}
	}
	
	//left
	if(player.xvel < 0  && abs(player.xacc)>10)//player.lastDir == 0)
	{
		checkX = player.xpos - extraPixels;
		checkY = player.ypos;
		xIndex = checkX/squareSide;
		yIndex = checkY/squareSide;
		if(xIndex <50 && xIndex >-1 && yIndex <50 && yIndex >-1)
		{
			if(map.map[xIndex][yIndex]>0 && map.map[xIndex][yIndex]<16)
			{
				player.xvel = 0;
				player.xpos +=1;
			}
		}
		checkY = player.ypos+squareSide;
		xIndex = checkX/squareSide;
		if(xIndex <50 && xIndex >-1 && yIndex <50 && yIndex >-1)
		{
			if(map.map[xIndex][yIndex]>0 && map.map[xIndex][yIndex]<16)
			{
				player.xvel = 0;
				player.xpos +=1;
			}
		}
		
	}
	
	//right
	if(player.xvel > 0  && abs(player.xacc)>10)//player.lastDir == 1)
	{
		checkX = player.xpos + squareSide + extraPixels;
		checkY = player.ypos;
		xIndex = checkX/squareSide;
		yIndex = checkY/squareSide;
		if(xIndex <50 && xIndex >-1 && yIndex <50 && yIndex >-1)
		{
			if(map.map[xIndex][yIndex]>0 && map.map[xIndex][yIndex]<16)
			{
				player.xvel = 0;
				player.xpos -=1;
			}
		}
		checkY = player.ypos+squareSide;
		xIndex = checkX/squareSide;
		if(xIndex <50 && xIndex >-1 && yIndex <50 && yIndex >-1)
		{
			if(map.map[xIndex][yIndex]>0 && map.map[xIndex][yIndex]<16)
			{
				player.xvel = 0;
				player.xpos -=1;
			}
		}
	}
	if(player.xvel==0) player.xacc *= -1;
	return player;
}

bool checkForFlag(movingEntity player, levelMap map)
{
	int xIndex = (player.xpos+squareSide/2) / squareSide;
	int yIndex = (player.ypos+squareSide/2) / squareSide;
	if(xIndex>-1&&xIndex<50&&yIndex>-1&&yIndex<50)
	{
		if(map.map[xIndex][yIndex]==16)
		{
			return 1;
		}
	}
	return 0;
}

bool checkForDeath(movingEntity player, levelMap map)
{
	int xIndex = (player.xpos+squareSide/2) / squareSide;
	int yIndex = (player.ypos+squareSide/2) / squareSide;
	if(xIndex>-1&&xIndex<50&&yIndex>-1&&yIndex<50)
	{
		if(map.map[xIndex][yIndex]==17)
		{
			return 1;
		}
	}
	if(player.ypos > squareSide*51)
		return 1;
	return 0;
}

//will be a subset of the above
//check if there is a tile underneath the player
bool isGrounded(movingEntity player, levelMap map)
{
	bool result = 0;
	int feetPosX = player.xpos;
	int feetPosY = player.ypos + squareSide;
	if(feetPosX/squareSide >= 50 || feetPosX/squareSide < 0 || feetPosY/squareSide >= 50 || feetPosY/squareSide < 0)
	{
		return result;
	}
	if(map.map[feetPosX/squareSide][feetPosY/squareSide]!=0 && map.map[feetPosX/squareSide][feetPosY/squareSide]<16 )
	{
		return 1;
	}
	feetPosX+=squareSide;
	if(feetPosX/squareSide >= 50 || feetPosX/squareSide < 0 || feetPosY/squareSide >= 50 || feetPosY/squareSide < 0)
	{
		return result;
	}
	if(map.map[feetPosX/squareSide][feetPosY/squareSide]!=0  && map.map[feetPosX/squareSide][feetPosY/squareSide]<16 )
	{
		return 1;
	}
	return result;
}

movingEntity playerMovement(movingEntity player, bool left, bool right, bool grounded, double deltaT, levelMap map)
{
	if(left)
	{
		player.xacc=-500;
		player.lastDir=0;
	}
	if(right)
	{
		player.xacc=500;
		player.lastDir=1;
	}
	if(!left && !right)
	{
		if(player.xvel>0)
		{
			player.xacc=-10*player.xvel;
		}
		else if(player.xvel<0)
		{
			player.xacc=-10*player.xvel;
		}
		else
		{
			player.xacc = 0;
		}
		if(abs(player.xvel) < 20)
		{
			player.xvel = 0;
		}
	}
	if(!grounded)
	{
		player.yacc=1500;
	}
	else
	{
		player.yacc=0;
		if(player.yvel>0)
			player.yvel=0;
	}
	player=checkCollision(player, map, deltaT);
	player.updateMovement(deltaT);
	if(abs(player.xvel)>500)
		player.xvel = 500*(player.xvel/abs(player.xvel));
	if(abs(player.yvel)>3000)
		player.yvel = 2200*(player.yvel/abs(player.yvel));
	return player;
}



movingEntity getStartPos(movingEntity player, levelMap map)
{
	for(int y = 0; y<50; y++)
	{
		for(int x = 0; x<50; x++)
		{
			if(map.map[x][y]==15)
			{
				player.xpos=x*squareSide;
				player.ypos=y*squareSide;
			}
		}
	}
	return player;
}

levelMap removeStartPoint(levelMap map)
{
	for(int y = 0; y<50; y++)
	{
		for(int x = 0; x<50; x++)
		{
			if(map.map[x][y]==15)
			{
				map.map[x][y]=0;
			}
		}
	}
	return map;
}

int pauseMenu(SDL_Window *window, SDL_Renderer *renderer, SDL_Event event, SDL_Texture *bg)
{
	int buttonCount = 2;
	menuButton buttons[buttonCount];
	buttons[0] = newGameButton();
	buttons[1] = backButton();
	buttons[0].posY-=0.15;
	buttons[0].label="Resume";
	buttons[1].posY-=0.15;
	buttons[1].label = "Back to Menu";
	bool widescreen = 1;
	if(SCREEN_WIDTH / SCREEN_HEIGHT < 1.7f)
		widescreen = 0;
	bool wasHovered[buttonCount] = {0};
	
	Mix_Chunk *click = Mix_LoadWAV("Assets/Sound/SFX/click.wav");
	Mix_Chunk *beep = Mix_LoadWAV("Assets/Sound/SFX/beep.wav");
	
	int returnValue = -1;
	while(returnValue == -1)
	{
		//draw graphics
		wipeScreen(renderer);//clear screen
		SDL_RenderCopy(renderer,bg,NULL,NULL);//draw
		for(int i = 0; i < buttonCount; i++)
		{
			drawButton(buttons[i],renderer,widescreen);
		}
		SDL_RenderPresent(renderer);//draw what's been queued
		
		//events
		SDL_WaitEvent(&event);
		if(event.type == SDL_MOUSEMOTION)//mouse motion
		{
			SDL_GetMouseState(&mouseX, &mouseY);//track the location of the mouse
			for(int i =0; i < buttonCount; i++)
			{
				if(buttons[i].isHovered())
				{
					if(!wasHovered[i])
					{
						Mix_PlayChannel(-1,click,0);//click sound when the mouse is moved over a button
					}
					wasHovered[i] = 1;
				}
				else
				{
					wasHovered[i] = 0;
				}
			}
		}
		else if(event.type == SDL_MOUSEBUTTONDOWN)//on click
		{
			if(event.button.button == SDL_BUTTON_LEFT)
			{
				//check location and perform an action based on that
				for(int i =0; i < buttonCount; i++)
				{
					if(buttons[i].isHovered())
					{
						Mix_PlayChannel(-1,beep,0);
						returnValue = i;
					}
				}
			}
		}
		else if(event.type == SDL_WINDOWEVENT)//window events
		{
			switch(event.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
					returnValue = -2;
					break;
			}
		}
		else
		{
			SDL_FlushEvent(event.type);
		}
	}
	Mix_FreeChunk(click);
	Mix_FreeChunk(beep);
	
	return returnValue;
}

struct gameplayReturn
{
	int levelNumber;
	int returnValue;
};

gameplayReturn gameplay(SDL_Window *window, SDL_Renderer *renderer, SDL_Event event, int levelNumber)
{
	int returnValue = -1;
	//load textures
	int textureCount = 18;
	SDL_Texture *textures[textureCount];
	for(int i = 1; i<textureCount-2; i++)
	{
		//load moon textures (1 to 14) (0 is air)
		string filename = "Assets/Art/World/moon";
		filename+=to_string(i)+".png";
		SDL_Surface *buf = IMG_Load(filename.c_str());
		textures[i] = SDL_CreateTextureFromSurface(renderer, buf);
		SDL_FreeSurface(buf);
	}
	//15 is start, 16 is flag, 17 is death
	SDL_Surface *buf3;
	if(levelNumber==16)
	{
		buf3 = IMG_Load("Assets/Art/World/door.png");
	}
	else
	{
		buf3 = IMG_Load("Assets/Art/World/flag.png");
	}
	textures[16] = SDL_CreateTextureFromSurface(renderer,buf3);
	SDL_FreeSurface(buf3);
	buf3 = IMG_Load("Assets/Art/World/spike.png");
	textures[17] = SDL_CreateTextureFromSurface(renderer,buf3);
	SDL_FreeSurface(buf3);
	//load player textures
	SDL_Texture *playerSprites[4];
	SDL_Surface *buf2 = IMG_Load("Assets/Art/Character/robotL.png");
	playerSprites[0] = SDL_CreateTextureFromSurface(renderer, buf2);
	buf2 = IMG_Load("Assets/Art/Character/robotR.png");
	playerSprites[1] = SDL_CreateTextureFromSurface(renderer, buf2);
	buf2 = IMG_Load("Assets/Art/Character/robotJL.png");
	playerSprites[2] = SDL_CreateTextureFromSurface(renderer, buf2);
	buf2 = IMG_Load("Assets/Art/Character/robotJR.png");
	playerSprites[3] = SDL_CreateTextureFromSurface(renderer, buf2);
	SDL_FreeSurface(buf2);
	
	//state variables
	bool left = 0;
	bool right = 0;
	bool grounded = 0;
	
	//test
	levelMap testMap;
	testMap.clearMap();
	movingEntity testPlayer = playerEnt(0,0);//make the player at 0,0
	
	
	
	testMap = loadMap(levelNumber);
	testPlayer = getStartPos(testPlayer, testMap);
	testMap = removeStartPoint(testMap);
	SDL_Surface *bgSur = IMG_Load(testMap.bgFilename.c_str());
	SDL_Texture *bgTex = SDL_CreateTextureFromSurface(renderer,bgSur);
	SDL_FreeSurface(bgSur);
	
	bool loadNext = 0;
	bool shouldReset = 0;
	int lastTime = SDL_GetTicks();
	int lastPauseTime = SDL_GetTicks();
	
	while(returnValue == -1)
	{
		//update time
		int newTime = SDL_GetTicks();
		double deltaT = (newTime - lastTime)/1000.0;
		lastTime = newTime;
		
		//check if need to load level
		if(checkForDeath(testPlayer,testMap)) shouldReset=1;
		if (checkForFlag(testPlayer,testMap)) loadNext=1;
		if (shouldReset)
		{
			levelNumber--;
			loadNext=1;
		}
		if (loadNext)
		{
			levelNumber++;
			returnValue = 0;
		}
		
		//draw graphics
		wipeScreen(renderer);//clear screen
		if(bgTex!=NULL)
		{
			SDL_RenderCopy(renderer, bgTex, NULL, NULL);
		}
		//things to draw goes here
		drawMap(renderer, testMap, textures, testPlayer, playerSprites);
		//then draw entities
		SDL_RenderPresent(renderer);//draw what's been queued
		
		//calculate movement for entities
		testPlayer = playerMovement(testPlayer,left,right,grounded,deltaT,testMap);
		grounded = isGrounded(testPlayer, testMap);
		//cout << "Vel: " << to_string(testPlayer.xvel) << "Acc: " << to_string(testPlayer.xacc) << endl;
		
		
		//event handling
		SDL_PollEvent(&event);
		
		if(event.type == SDL_KEYDOWN)//key presses
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_a:
					left = 1;
					break;
				case SDLK_LEFT:
					left = 1;
					break;
				case SDLK_d:
					right = 1;
					break;
				case SDLK_RIGHT:
					right = 1;
					break;
				case SDLK_SPACE:
					if(grounded)
					{
						grounded = 0;
						testPlayer.yvel =-1000;
					}
					break;
				case SDLK_r:
					shouldReset=1;
					break;
				case SDLK_ESCAPE:
					if(SDL_GetTicks() - lastPauseTime > 250)
					{
						int menuTime = SDL_GetTicks();
						//do menu things
						int menuResult = pauseMenu(window,renderer,event,bgTex);
						if(menuResult==1)
						{
							returnValue=-3;
						}
						menuTime = SDL_GetTicks()-menuTime;
						lastTime+=menuTime;
						lastPauseTime = SDL_GetTicks();
					}
					break;
			}
		}
		else if(event.type == SDL_KEYUP)//key releases
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_a:
					left = 0;
					break;
				case SDLK_LEFT:
					left = 0;
					break;
				case SDLK_d:
					right = 0;
					break;
				case SDLK_RIGHT:
					right = 0;
					break;
			}
		}
		else if(event.type == SDL_WINDOWEVENT)//window events
		{
			switch(event.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
					returnValue = -2;
					break;
			}
		}
	}
	//to do: destroy textures
	for(int i =1; i<17; i++)
	{
		SDL_DestroyTexture(textures[i]);
	}
	for(int i =0; i<4; i++)
	{
		SDL_DestroyTexture(playerSprites[i]);
	}
	SDL_DestroyTexture(bgTex);
	gameplayReturn result;
	result.returnValue = returnValue;
	result.levelNumber = levelNumber;
	return result;
}

string songNameFromLevel(int levelNumber)
{
	string result = "";
	if(levelNumber<=5)
	{
		result = "Assets/Sound/Music/Awakening Looped.wav";
	}
	else if(levelNumber<=10)
	{
		result = "Assets/Sound/Music/Setting Out Looped.wav";
	}
	else if(levelNumber<=15)
	{
		result = "Assets/Sound/Music/Jumping Looped.wav";
	}
	else
	{
		result = "Assets/Sound/Music/In Sight Looped.wav";
	}
	return result;
}

int main()
{
	//start it up
	SDL_Init(SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
	//create the basic objects
	SDL_Window *window = SDL_CreateWindow("Rytour",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0);
	SDL_Renderer *renderer  = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	SDL_Event event;
	
	bool runProgram = 1; //setting to 0 ends the loop
	while(runProgram)
	{
		int mainResult = mainMenu(window,renderer,event);
		if (!mainResult)
			break;
		fadeBetweenImages(renderer,"Assets/Art/UI/menu bg.png","Assets/Art/UI/static.png");
		int	fileResult =  fileMenu(window,renderer,event);
		if(fileResult == -2)
		{
			break;
		}
		else if (fileResult != 2)
		{
			int currentLevel = gameLevel;
			if(fileResult==1)//new game
			{
				int introResult = introScene(window,renderer,event);
				currentLevel=1;
				if(introResult == -1)
				{
					runProgram = 0;
					break;
				}
			}
			bool keepGaming = 1;
			
			string levelSong = songNameFromLevel(currentLevel);
			Mix_Music *bgm = Mix_LoadMUS(levelSong.c_str());
			Mix_PlayMusic(bgm, -1);
			while(keepGaming)
			{
				//game stuff
				int oldLevel = currentLevel;
				if(currentLevel==17)
				{
					//do end stuff
					outroScene(window,renderer,event);
					runProgram=0;
					remove("Data/playerdata.sav");
					break;
				}
				gameplayReturn gameResult = gameplay(window, renderer, event,currentLevel);
				currentLevel = gameResult.levelNumber;
				if(currentLevel!=oldLevel)
				{
					//check if new music
					levelSong = songNameFromLevel(oldLevel);
					string newLevelSong = songNameFromLevel(currentLevel);
					if(newLevelSong != levelSong)
					{
						bgm = Mix_LoadMUS(newLevelSong.c_str());
						Mix_PlayMusic(bgm, -1);
					}
				}
				if(gameResult.returnValue == -2)
				{
					runProgram=0;
					break;
				}
				else if(gameResult.returnValue == -3)
				{
					break;
				}
				else if(gameResult.returnValue == 0)
				{
					
				}
				remove("Data/playerdata.sav");
				FILE *save = fopen("Data/playerdata.sav","w");
				fwrite(&currentLevel,sizeof(int),1,save);
				fclose(save);
			}
			Mix_HaltMusic();
			Mix_FreeMusic(bgm);
		}
		
		fadeBetweenImages(renderer,"Assets/Art/UI/static.png","Assets/Art/UI/menu bg.png");
	}
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
