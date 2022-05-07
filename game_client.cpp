/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "SDL_Files/SDL_mixer.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <string>
#include <iostream>
#include <bits/stdc++.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080


using namespace std;



Mix_Music *gMusic = NULL;

//Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;

//The dimensions of the level
const int LEVEL_WIDTH = 6400;
const int LEVEL_HEIGHT = 3200;

//Tile constants
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int LAYER1_TOTAL_TILES = 20000;
const int LAYER2_TOTAL_TILES = 20000;

const int TOTAL_TILE_SPRITES = 16384;




int PlayerSpeed = 5;
int player_xpos = 4448;
int player_ypos = 0;



//The different tile sprites



//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The tile
class Tile
{
    public:
		//Initializes position and type
		Tile( int x, int y, int tileType );

		//Shows the tile
		void render( SDL_Rect& camera );

		//Get the tile type
		int getType();

		//Get the collision box
		SDL_Rect getBox();

    private:
		//The attributes of the tile
		SDL_Rect mBox;

		//The tile type
		int mType;
};



//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 16;
		static const int DOT_HEIGHT = 16;
		string name = "";
		
		//static int Score = 0;
		
		//Maximum axis velocity of the dot
		 int DOT_VEL = 5;

		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot and check collision against tiles
		void move( Tile *tiles[] , Tile *tiles2[]);

		//Centers the camera over the dot
		void setCamera( SDL_Rect& camera );

		//Shows the dot on the screen
		void render( SDL_Rect& camera );

    private:
		//Collision box of the dot
		SDL_Rect mBox;

		//The velocity of the dot
		int mVelX, mVelY;
		
		
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia( Tile* tiles[] , Tile* tiles2[]);

//Frees media and shuts down SDL
void close( Tile* tiles[] , Tile* tiles2[] );

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );

//Checks collision box against set of tiles
bool touchesWall( SDL_Rect box, Tile* tiles[] );

//Sets tiles from tile map
bool setTiles( Tile *tiles[], Tile *tiles2[] );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

TTF_Font *gFont = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gTileTexture;
LTexture gTextTexture;
LTexture gStartScreen;
LTexture gInstructionScreen;
LTexture gInputTextTexture;


SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;

SDL_Rect gTileClips[ TOTAL_TILE_SPRITES ];

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}


void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };//xx

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Tile::Tile( int x, int y, int tileType )
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    //Get the tile type
    mType = tileType;
}

void Tile::render( SDL_Rect& camera )
{
    //If the tile is on screen
    if( checkCollision( camera, mBox ) )
    {
        //Show the tile
        gTileTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips[ mType ] );
    }
}

int Tile::getType()
{
    return mType;
}

SDL_Rect Tile::getBox()
{
    return mBox;
}

Dot::Dot()
{
    //Initialize the collision box
    mBox.x = 4448;
    mBox.y = 64;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY = 0; break;
            case SDLK_DOWN: mVelY = 0; break;
            case SDLK_LEFT: mVelX = 0; break;
            case SDLK_RIGHT: mVelX = 0; break;
        }
    }
}
bool hasBoosted = false;
void Dot::move( Tile *tiles[], Tile *tiles2[] )
{
    //Move the dot left or right
    mBox.x += mVelX;
    player_xpos = mBox.x;

    //If the dot went too far to the left or right or touched a wall
    if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > LEVEL_WIDTH ) || touchesWall( mBox, tiles ) )
    {
        //move back
        mBox.x -= mVelX;
    }

    //Move the dot up or down
    mBox.y += mVelY;
    player_ypos = mBox.y;

    //If the dot went too far up or down or touched a wall
    if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_HEIGHT ) || touchesWall( mBox, tiles ) )
    {
        //move back
        mBox.y -= mVelY;
    }
    if(!hasBoosted){
    for (int i=0;i<TOTAL_TILE_SPRITES;i++){
    	if (tiles2[i]->getType() <= 3670 && tiles2[i]->getType() >= 3668){
    		if(checkCollision(mBox,tiles2[i]->getBox())){
    			
    			DOT_VEL = 10;
    			PlayerSpeed = DOT_VEL;
    			hasBoosted  = true;
    		}
    	}
    
    }
    }
    
    
}

void Dot::setCamera( SDL_Rect& camera )
{
	//Center the camera over the dot
	camera.x = ( mBox.x + DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
	camera.y = ( mBox.y + DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if( camera.x < 0 )
	{ 
		camera.x = 0;
	}
	if( camera.y < 0 )
	{
		camera.y = 0;
	}
	if( camera.x > LEVEL_WIDTH - camera.w )
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if( camera.y > LEVEL_HEIGHT - camera.h )
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Dot::render( SDL_Rect& camera )
{
    //Show the dot
  
    
	gDotTexture.render( mBox.x - camera.x, mBox.y - camera.y );
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_EVERYTHING < 0 ))
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "MAZEMANIA", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
        if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gScreenSurface = SDL_GetWindowSurface( gWindow );
			
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia( Tile* tiles[] , Tile* tiles2[])
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	gFont = TTF_OpenFont( "Fonts/AngryBirds.ttf", 20);
	
	
	/*gHelloWorld = SDL_LoadBMP( "startscreen.bmp" );
	if( gHelloWorld == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", "startscreen.bmp", SDL_GetError() );
		success = false;
	}*/
	

	if(!gStartScreen.loadFromFile("Images/start.jpg")){
		printf("Failed to load Start screen Texture!\n");
		success = false;
	}
	
	if(!gInstructionScreen.loadFromFile("Images/instructions.jpg")){
		printf("Failed to load Start screen Texture!\n");
		success = false;
	}
	
	if( !gDotTexture.loadFromFile( "Player/playern2.png" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}
	 gMusic = Mix_LoadMUS( "Sounds/Fluffing-a-Duck.wav" );
    if( gMusic == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    if( gFont == NULL )
    {
        printf( "Failed to load Angry birds font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
   
    /*if( !gStartScreen.loadFromFile("startscreen.jpg"))
    {
    	printf( "Failed to load Start Screen texture!\n" );
		success = false;
    }*/

	//Load tile texture
	if( !gTileTexture.loadFromFile( "Tileset/mytileset.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	//Load tile map
	if( !setTiles( tiles , tiles2) )
	{
		printf( "Failed to load tile set!\n" );
		success = false;
	}

	return success;
}

void close( Tile* tiles[], Tile* tiles2[] )
{
	//Deallocate tiles
	for( int i = 0; i < LAYER1_TOTAL_TILES; ++i )
	{
		 if( tiles[ i ] != NULL )
		 {
			delete tiles[ i ];
			tiles[ i ] = NULL;
		 }
	}
	
	for( int i = 0; i < LAYER2_TOTAL_TILES; ++i )
	{
		 if( tiles2[ i ] != NULL )
		 {
			delete tiles2[ i ];
			tiles2[ i ] = NULL;
		 }
	}

	//Free loaded images
	gTextTexture.free();
	gInputTextTexture.free();
	TTF_CloseFont( gFont );
    gFont = NULL;
    
	gDotTexture.free();
	gStartScreen.free();
	gInstructionScreen.free();
	gTileTexture.free();
	Mix_FreeMusic( gMusic );
    gMusic = NULL;
    
   //SDL_FreeSurface( gHelloWorld );
	//gHelloWorld = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool setTiles( Tile* tiles[] , Tile* tiles2[])
{
	//Success flag
	bool tilesLoaded = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream map1( "Map/xxm1.map" );

    //If the map couldn't be loaded
    if( map1.fail() )
    {
		printf( "Unable to load map file!\n" );
		tilesLoaded = false;
    }
	else
	{
		//Initialize the tiles
		for( int i = 0; i < LAYER1_TOTAL_TILES; ++i )
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map1 >> tileType;

			//If the was a problem in reading the map
			if( map1.fail() )
			{
				//Stop loading map
				printf( "Error loading map: Unexpected end of file!\n" );
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
			{
				tiles[ i ] = new Tile( x, y, tileType );
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf( "Error loading map: Invalid tile type at %d!\n", i );
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if( x >= LEVEL_WIDTH )
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		
		//Close the file
		
	}	
		map1.close();
		x = 0, y = 0;
		std::ifstream map2( "Map/xxm2.map" );
		
		if( map2.fail() )
    {
		printf( "Unable to load map file!\n" );
		tilesLoaded = false;
    }
	else
	{
		//Initialize the tiles
		for( int i = 0; i < LAYER2_TOTAL_TILES; ++i )
		{
			//Determines what kind of tile will be made
			int tile2Type = -1;

			//Read tile from map file
			map2 >> tile2Type;

			//If the was a problem in reading the map
			if( map2.fail() )
			{
				//Stop loading map
				printf( "Error loading map: Unexpected end of file!\n" );
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if( ( tile2Type >= 0 ) && ( tile2Type < TOTAL_TILE_SPRITES ) )
			{
				tiles2[ i ] = new Tile( x, y, tile2Type );
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf( "Error loading map: Invalid tile type at %d!\n", i );
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if( x >= LEVEL_WIDTH )
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		
		//Close the file
		map2.close();		
		
		//Clip the sprite sheet
		
	}
	
	if( tilesLoaded )
		{
			for (int i=1;i<TOTAL_TILE_SPRITES;i++){
				gTileClips[i].x=32*((i-1)%128);
				gTileClips[i].y=32*(i/128);
				gTileClips[i].w = TILE_WIDTH;
				gTileClips[i].h = TILE_HEIGHT;
			}
			

		}

    

    //If the map was loaded fine
    return tilesLoaded;
}

bool touchesWall( SDL_Rect box, Tile* tiles[] )
{
    //Go through the tiles
   for( int i = 0; i < LAYER1_TOTAL_TILES; ++i )
    {
        //If the tile is a wall type tile
        if( ( tiles[ i ]->getType() ==  513)  )
        {
            //If the collision box touches the wall tile
            if( checkCollision( box, tiles[ i ]->getBox() ) )
            {
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
}
string inputText = "NAME";
bool isPlaying = false;
bool isInstructions = false;
bool isGameQuit = false;
bool isBackToStart  = false;

void play (Tile* tileSet[], Tile* tileSet2[],Dot dot1, Dot dot2, int sock, char buffer[], int valread) {

	bool quit = false;
	
	SDL_Event e;

			//The dot that will be moving around on the screen
	

			//Level camera
	SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	while(!quit){
		while( SDL_PollEvent( &e ) != 0 )
		{
				
				
			//User requests quit
			if(Mix_PlayingMusic()==0){
				Mix_PlayMusic(gMusic, -1);
			}
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
			if(e.type == SDL_KEYDOWN){
			

				if(e.key.keysym.sym == SDLK_ESCAPE){
						quit = true;
				}
				
				if(e.key.keysym.sym == SDLK_s){
						if( Mix_PausedMusic() == 1 )
		                {
		                    //Resume the music
		                    Mix_ResumeMusic();
		                }
		                //If the music is playing
		               
					}
				}
			else if(e.key.keysym.sym == SDLK_m){
				 Mix_HaltMusic();
			}

			//Handle input for the dot
			dot1.handleEvent( e );
		}
			
			

			//Move the dot
		dot1.move( tileSet, tileSet2 );
		dot1.setCamera( camera );

			//Clear screen
			
			
		SDL_RenderClear( gRenderer );
			
			
			

			//Render level
			
			
		for( int i = 0; i < LAYER1_TOTAL_TILES; ++i )
		{
			tileSet[ i ]->render( camera );
		}
			
		for( int i = 0; i < LAYER2_TOTAL_TILES; ++i )
		{
			if(tileSet2[i]->getType()==0){
				continue;
			}
			else tileSet2[ i ]->render( camera );
		}
			
			
			 

			//Render dot
		dot1.render( camera );
					
				//SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_Color textColor = { 255,255, 255};
		
		//string s = "Score: " + to_string(dot.Score);
		//cout << Score << endl;
		gTextTexture.loadFromRenderedText( "ESC - Quit", textColor );

		gTextTexture.render( 1100,  0 );
		
		gTextTexture.loadFromRenderedText( "S - Sound", textColor );

		gTextTexture.render( 1100,  25 );

		gTextTexture.loadFromRenderedText( "M - Mute", textColor );

		gTextTexture.render( 1100,  50);
		
		string s = "SPEED : " + to_string(PlayerSpeed);
		
		gTextTexture.loadFromRenderedText( s, textColor );

		gTextTexture.render( 5,  25);
		
		s = "X : " + to_string(player_xpos);
		
		gTextTexture.loadFromRenderedText( s, textColor );

		gTextTexture.render( 5,  50);
		
		s = "Y : " + to_string(player_ypos);
		
		gTextTexture.loadFromRenderedText( s, textColor );

		gTextTexture.render( 5,  75);
		
		s = inputText;
		gTextTexture.loadFromRenderedText( s, textColor );

		gTextTexture.render( 5,  0);
		//Update screen
		
		//sending data over sockets
		
		
		if(isPlaying){
			const char* data_name = s.c_str();
			send(sock, data_name, strlen(data_name),0);
			
			valread = read(sock, buffer, 1024);
			string data_name1 = buffer;
			memset(buffer,0,1024);
			
			gTextTexture.loadFromRenderedText( data_name1, textColor );
			gTextTexture.render( 5,  150);
			
			const char* data_speed = to_string(PlayerSpeed).c_str();
			send(sock, data_speed, strlen(data_speed),0);

			/*
			
			*/
			
			// recieving data over the socket
			
			
			
			valread = read(sock, buffer, 1024);
			string data_speed1 = buffer;
			memset(buffer,0,1024);
			
			gTextTexture.loadFromRenderedText( "SPEED : ", textColor );
			gTextTexture.render( 5,  175);
			
			gTextTexture.loadFromRenderedText( data_speed1, textColor );
			gTextTexture.render( 70,  175);
			
			const char* data_x = to_string(player_xpos).c_str();
			send(sock, data_x, strlen(data_x), 0);
			
			valread = read(sock, buffer, 1024);
			string data_x1 = buffer;
			memset(buffer,0,1024);
			
			gTextTexture.loadFromRenderedText( "X : ", textColor );
			gTextTexture.render( 5,  200);
			
			gTextTexture.loadFromRenderedText( data_x1, textColor );
			gTextTexture.render( 30,  200);
			
			const char* data_y = to_string(player_ypos).c_str();
			send(sock, data_y, strlen(data_y), 0);
			
			valread = read(sock, buffer, 1024);
			string data_y1 = buffer;
			memset(buffer,0,1024);

			gTextTexture.loadFromRenderedText( "Y : ", textColor );
			gTextTexture.render( 5,  225);
			
			gTextTexture.loadFromRenderedText( data_y1, textColor );
			gTextTexture.render( 30,  225);
		}
		

		
		
		
		
		SDL_RenderPresent( gRenderer );	
	}
	
}




void start(Dot dot){
	
	bool wasMusic = false;
	SDL_Event e;
	isBackToStart = false;
	
	
	SDL_Color textColor = { 255, 255, 255, 0xFF };
	gInputTextTexture.loadFromRenderedText( inputText.c_str(), textColor );
	SDL_StartTextInput();
	
	while(1){
	
		bool renderText = false;
		
		while( SDL_PollEvent( &e ) != 0 )
		{
			if(Mix_PlayingMusic()==0 && !wasMusic){
				Mix_PlayMusic(gMusic, -1);
				wasMusic = true;
			}
			if( e.type == SDL_QUIT )
			{
				isGameQuit = true;
				return;
			}	
			if(e.type == SDL_KEYDOWN){
			
				if( e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0 )
				{
					//lop off character
					inputText.pop_back();
					renderText = true;
				}
				//Handle copy
				else if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
				{
					SDL_SetClipboardText( inputText.c_str() );
				}
				//Handle paste
				else if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
				{
					inputText = SDL_GetClipboardText();
					renderText = true;
				}
				
				if(e.key.keysym.sym == SDLK_ESCAPE){
						isGameQuit = true;
						return;
				}
				
				if(e.key.keysym.sym == SDLK_SPACE){
					isPlaying = true;
					return;
				}
				
				if(e.key.keysym.sym == SDLK_i){
					isInstructions = true;
					return;
				}
				
				if(e.key.keysym.sym == SDLK_s){
						if( Mix_PausedMusic() == 1 )
		                {
		                    //Resume the music
		                    Mix_ResumeMusic();
		                }
		                //If the music is playing
		               
					}
				}

			else if(e.key.keysym.sym == SDLK_m){
				 Mix_HaltMusic();
				 wasMusic = false;
			}
								else if( e.type == SDL_TEXTINPUT )
					{
						//Not copy or pasting
						if( !( SDL_GetModState() & KMOD_CTRL && ( e.text.text[ 0 ] == 'c' || e.text.text[ 0 ] == 'C' || e.text.text[ 0 ] == 'v' || e.text.text[ 0 ] == 'V' ) ) )
						{
							//Append character
							inputText += e.text.text;
							renderText = true;
						}
					}
		}
		
		if( renderText )
		{
			//Text is not empty
			if( inputText != "" )
			{
				//Render new text
				gInputTextTexture.loadFromRenderedText( inputText.c_str(), textColor );
			}
			//Text is empty
			else
			{
				//Render space texture
				gInputTextTexture.loadFromRenderedText( " ", textColor );
			}
		}		
		/*SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );

			//Update the surface
		SDL_UpdateWindowSurface( gWindow );*/
		SDL_Rect clip = {0,0,1200,904};

		gStartScreen.render(0,0, &clip);
		
		//SDL_Rect inputClip = {0,0};
		
		gInputTextTexture.render(525,350);
		
		SDL_RenderPresent( gRenderer );	

		
	}
	dot.name = inputText;
	SDL_StopTextInput();
	
}

void instructions() {
	bool wasMusic = false;
	SDL_Event e;
	
	while(1)
	{
		while( SDL_PollEvent( &e ) != 0 )
		{
			if(Mix_PlayingMusic()==0 && !wasMusic){
				Mix_PlayMusic(gMusic, -1);
				wasMusic = true;
			}
			
			if( e.type == SDL_QUIT )
			{
				isGameQuit = true;
				return;
			}
			if(e.type == SDL_KEYDOWN){
		

				if(e.key.keysym.sym == SDLK_ESCAPE){
						isGameQuit = true;
						return;
				}
				
				if(e.key.keysym.sym == SDLK_BACKSPACE ){
					isBackToStart = true;
					return;
				}
							
				if(e.key.keysym.sym == SDLK_s){
						if( Mix_PausedMusic() == 1 )
			            {
			                //Resume the music
			                Mix_ResumeMusic();
			            }
			            //If the music is playing
			           
					}
				}
			else if(e.key.keysym.sym == SDLK_m){
				 Mix_HaltMusic();
				 wasMusic = false;
			}
				
				
				
			}
			
			SDL_Rect clip = {0,0,1200,904};

			gInstructionScreen.render(0,0, &clip);
		
			SDL_RenderPresent( gRenderer );	
		}
		
}

int main( int argc, char* args[] )
{

	//Socket Implementation
	
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char* hello = "Hello from client";
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
	//send(sock, hello, strlen(hello), 0);
	//printf("Hello message sent\n");
	//valread = read(sock, buffer, 1024);
	//printf("%s\n", buffer);	
		
	
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//The level tiles
		Tile* tileSet[ LAYER1_TOTAL_TILES ];
		Tile* tileSet2[ LAYER2_TOTAL_TILES ];

		//Load media
		if( !loadMedia( tileSet , tileSet2) )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			
			//Main loop flag
			
			Dot dot1, dot2;
			
			while(!isPlaying && !isGameQuit){
			
				start(dot1);
				
				if(isInstructions && !isGameQuit && !isPlaying){
					instructions();
				}
			}
			
			
			if(isPlaying && !isGameQuit){
				
				play(tileSet, tileSet2, dot1, dot2, sock , buffer, valread);
				
			}
			
			
		}
		
		//Free resources and close SDL
		close( tileSet , tileSet2);
		
		
	}

	return 0;
}
