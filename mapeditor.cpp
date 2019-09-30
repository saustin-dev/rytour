#include <iostream>
#include <fstream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdbool.h>
#include <stdlib.h>

//useful variables
int mouseX = 0;
int mouseY = 0;
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 1024;
int squareSide = SCREEN_HEIGHT/50;

using namespace std;

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

struct mapArray
{
	int map[50][50];
	void initialize()
	{
		for (int i =0; i<50; i++)
		{
			for (int j =0; j<50; j++)
			{
				map[i][j]=0;
			}
		}
	}
};

void drawTiles(SDL_Renderer *renderer, mapArray values, SDL_Texture ** textures)
{
	SDL_SetRenderDrawColor(renderer,0,0,0,255);//black for outlines
	for(int y = 0; y < 50; y++)
	{
		for(int x = 0; x < 50; x++)
		{
			SDL_Rect rect = {x*squareSide,y*squareSide,squareSide,squareSide};
			SDL_RenderCopy(renderer,textures[values.map[x][y]],NULL,&rect);
			SDL_RenderDrawRect(renderer,&rect);
		}
	}
	
}

void saveFile(mapArray values, string fileName)
{
	//make a big string containing the info
	string saveData = "";
	for(int y =0; y<50; y++)
	{
		for(int x =0; x<50; x++)
		{
			saveData+=to_string(values.map[x][y])+",";
		}
	}
	saveData+="space bg 1.png";
	//then put it in a file
	ofstream output(fileName.c_str());
	output << saveData;
	output.close();
}

mapArray loadFile(mapArray values, string fileName)
{
	string readData[50][50]={"0"};
	ifstream input(fileName.c_str());
	for(int y =0; y<50; y++)
	{
		for(int x =0; x<50; x++)
		{
			getline(input,readData[x][y],',');
			//cout << "Read a " << readData[x][y] << " at " << to_string(x) << "," << to_string(y) <<endl;
		}
	}
	for(int y =0; y<50; y++)
	{
		for(int x =0; x<50; x++)
		{
			values.map[x][y] = stoi(readData[x][y]);
			//cout << "Set " <<  to_string(x) << "," << to_string(y) << " to " << readData[x][y] <<endl;
		}
	}
	input.close();
	return values;
}

//recursive flood fill
//usually causes a segfault unless you divvy up the area yourself beforehand
//handle with care
mapArray changeNeighbors(mapArray input, int x, int y, bool checked[50][50])
{
	if(checked[x][y] || input.map[x][y] != 0)
	{
		return input;
	}

	input.map[x][y]=6;
	checked[x][y]=1;
	
	if(x-1 >-1 && input.map[x-1][y] == 0)
	{
		input = changeNeighbors(input,x-1,y,checked);
	}
	if(x+1 <50 && input.map[x+1][y] == 0)
	{
		input = changeNeighbors(input,x+1,y,checked);
	}
	if(y-1 >-1 && input.map[x][y-1] == 0)
	{
		input = changeNeighbors(input,x,y-1,checked);
	}
	if(y+1 <50 && input.map[x][y+1] == 0)
	{
		input = changeNeighbors(input,x,y+1,checked);
	}
	return input;
}

//TODO (all done)
//-----
//draw a 50x50 grid with side lengths of SCREEN_WIDTH/50
//get the appropriate texture based off the value in the appropriate cell
//when a cell is clicked, increment that value by 1
//if it goes over the max, set it to zero
//when the window is closed, write all this into a file and save with the name given as input



int main(int argc, char *argv[])
{
	//get the filename based on command line arguments
	string fileName = "";
	if(argc>1) fileName+=argv[1];
	else fileName = "untitledMap";
	fileName+=".map";
	
	
	//make an int 2d array and initialize it to 0
	mapArray values;
	values.initialize();
	//attempt to load map if exists
	fileName = "Data/Maps/" + fileName;
	string windowTitle = fileName;
	FILE *fp = fopen(fileName.c_str(),"r");
	if(fp!=NULL)//can open file
	{
		//cout <<"File found, opening" << endl;
		fclose(fp);
		values = loadFile(values, fileName);
	}
	else
	{
		windowTitle += " (new file)";
	}
	
	//start SDL
	SDL_Init(0);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window *window = SDL_CreateWindow(windowTitle.c_str(),SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,0);
	SDL_Renderer *renderer  = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
	SDL_Event event;
	
	
	//load our tile textures
	const int textureCount = 18;
	SDL_Texture *textures[textureCount];
	for(int i = 1; i<textureCount-2; i++)
	{
		//load moon textures (1 to 9) (0 is air)
		string filename = "Assets/Art/World/moon";
		filename+=to_string(i)+".png";
		SDL_Surface *buf = IMG_Load(filename.c_str());
		textures[i] = SDL_CreateTextureFromSurface(renderer, buf);
		SDL_FreeSurface(buf);
	}
	SDL_Surface *buf2 = IMG_Load("Assets/Art/Character/robotR.png");
	textures[15] = SDL_CreateTextureFromSurface(renderer, buf2);
	SDL_FreeSurface(buf2);
	buf2 = IMG_Load("Assets/Art/World/flag.png");
	textures[16] = SDL_CreateTextureFromSurface(renderer, buf2);
	buf2 = IMG_Load("Assets/Art/World/spike.png");
	textures[17] = SDL_CreateTextureFromSurface(renderer, buf2);
	SDL_FreeSurface(buf2);
	
	bool shouldRun = 1;
	
	while(shouldRun)
	{
		//GRAPHICS
		wipeScreen(renderer);//clear screen
		drawTiles(renderer, values, textures);
		SDL_RenderPresent(renderer);//draw what's been queued
		
		//EVENTS
		SDL_WaitEvent(&event);
		if(event.type == SDL_MOUSEMOTION)//mouse motion
		{
			SDL_GetMouseState(&mouseX, &mouseY);//track the location of the mouse
		}
		else if(event.type == SDL_MOUSEBUTTONDOWN)//on click
		{
			if(event.button.button == SDL_BUTTON_LEFT)
			{
				//check which cell was clicked
				int clickedX = mouseX/squareSide;
				int clickedY = mouseY/squareSide;
				if(clickedX < 50 && clickedY < 50)
				{
					values.map[clickedX][clickedY]++;
					if(values.map[clickedX][clickedY] >= textureCount)
					{
						values.map[clickedX][clickedY]=0;
					}
				}
			}
		}
		else if(event.type == SDL_KEYDOWN)
		{
			int newValue = -1;
			switch(event.key.keysym.sym)
			{
				case SDLK_0:
					newValue=0;
					break;
				case SDLK_1:
					newValue=1;
					break;
				case SDLK_2:
					newValue=2;
					break;
				case SDLK_3:
					newValue=3;
					break;
				case SDLK_4:
					newValue=4;
					break;
				case SDLK_5:
					newValue=5;
					break;
				case SDLK_6:
					newValue=6;
					break;
				case SDLK_7:
					newValue=7;
					break;
				case SDLK_8:
					newValue=8;
					break;
				case SDLK_9:
					newValue=9;
					break;
				case SDLK_q:
					newValue=10;
					break;
				case SDLK_w:
					newValue=11;
					break;
				case SDLK_e:
					newValue=12;
					break;
				case SDLK_r:
					newValue=13;
					break;
				case SDLK_t:
					newValue=14;
					break;
				case SDLK_s:
					newValue=15;
					break;
				case SDLK_f:
					newValue=16;
					break;
				case SDLK_y:
					newValue=17;
					break;
				case SDLK_l:
					bool checked[50][50] = {0};
					values = changeNeighbors(values,mouseX/squareSide,mouseY/squareSide,checked);
					break;
			}
			if(newValue!=-1)
			{
				int clickedX = mouseX/squareSide;
				int clickedY = mouseY/squareSide;
				if(clickedX < 50 && clickedY < 50)
				{
					values.map[clickedX][clickedY]=newValue;
				}
			}
		}
		else if(event.type == SDL_WINDOWEVENT)//window events
		{
			switch(event.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
					shouldRun = 0;
					break;
			}
		}
		else
		{
			SDL_FlushEvent(event.type);
		}
	}
	
	saveFile(values,fileName);
	
	//clean up
	for(int i=0; i< textureCount; i++)
	{
		SDL_DestroyTexture(textures[i]);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}


