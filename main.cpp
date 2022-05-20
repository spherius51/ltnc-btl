#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>

#include "SDL_utils.h"
#include "Game.h"
#include "Gallery.h"
#include "TextObject.h"



using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
const char WINDOW_TITLE[] = "Snake Game";

const int BOARD_WIDTH = 30;
const int BOARD_HEIGHT = 20;
const int CELL_SIZE = 30;

const SDL_Color BOARD_COLOR = {0, 0, 0};
const SDL_Color LINE_COLOR = {128, 128, 128};
int highScore = 0;
double STEP_DELAY = 0.2;
#define CLOCK_NOW chrono::system_clock::now
typedef chrono::duration<double> ElapsedTime;


void drawWall(SDL_Renderer* renderer, int left, int top);
void drawText(SDL_Renderer* renderer, string str_val, int x, int y, int type);
void renderSplashScreen(SDL_Renderer* renderer, const Game& game, SDL_Event e);
void renderGamePlay(SDL_Renderer*, const Game& game);
void renderGameOver(SDL_Renderer* renderer, const Game& game, SDL_Event e);
void interpretEvent(SDL_Event e, Game& game);
void drawScore(SDL_Renderer* renderer, const Game &game);
void play(SDL_Renderer* renderer, Game& game, SDL_Event e);
void RenderOption(SDL_Renderer* renderer, SDL_Event e);


Gallery* gallery = nullptr; // global picture manager


int main(int argc, char* argv[])
{

    srand(time(0));
    SDL_Window* window;
    SDL_Renderer* renderer;
    initSDL(window, renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);

    gallery = new Gallery(renderer);
    Game game(BOARD_WIDTH , BOARD_HEIGHT);
    SDL_Event e;


    renderSplashScreen(renderer, game, e);
    play(renderer, game, e);

    delete gallery;
    Mix_CloseAudio();
    quitSDL(window, renderer);
    return 0;
}

float generateRandomNumber()
{
    return (float) rand() / RAND_MAX;
}

void renderSplashScreen(SDL_Renderer* renderer, const Game& game, SDL_Event e)
{
    bool running = true;
    //draw picture
    SDL_Texture* menu=loadTexture("demo_start_game.png", renderer);
    SDL_RenderCopy(renderer, menu,NULL,NULL);
    // draw text
    drawText(renderer, "Press 1 to start game", SCREEN_WIDTH - 650, 350, TextObject:: BLACK_TEXT );
    SDL_RenderPresent(renderer);
    //music added
    Mix_Music* music = NULL;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		cout << Mix_GetError();
		exit(2);
	}
    music = Mix_LoadMUS("game_Opening.mp3");
    if (music == NULL)
	{
		cout <<  Mix_GetError();
	}
    if (!Mix_PlayingMusic()){
            Mix_PlayMusic(music, 0);
    }
    //
    while(running){
        if(SDL_WaitEvent(&e)==0) SDL_Delay(100);
        else if(e.key.keysym.sym == SDLK_1){
            Mix_CloseAudio();
            return ;
        }
    }
}

void drawCell(SDL_Renderer* renderer, int left, int top, Position pos, SDL_Texture* texture)
{
	SDL_Rect cell;
	cell.x = left + pos.x * CELL_SIZE + 5;
	cell.y = top + pos.y * CELL_SIZE + 5;
	cell.w = CELL_SIZE-10;
	cell.h = CELL_SIZE-10;
	SDL_RenderCopy(renderer, texture, NULL, &cell);
}
void drawScore(SDL_Renderer* renderer, const Game &game){
        if(TTF_Init()==-1) {
            cout << TTF_GetError();
            exit(2);
        }
        TTF_Font* font_score = TTF_OpenFont("dlxfont_.ttf", 15);
        string str_score = "Score: ";
        string str_val= to_string(game.getScore());
        str_score+=str_val;

        TextObject score_game;
        score_game.SetColor(TextObject:: RED_TEXT);

        score_game.SetText(str_score);
        score_game.LoadFromRenderText(font_score, renderer);
        score_game.RenderText(renderer, SCREEN_WIDTH - 200, 15);
}
void drawCherry(SDL_Renderer* renderer, int left, int top, Position pos)
{
    drawCell(renderer, left, top, pos, gallery->getImage(PIC_CHERRY));
}
void drawBigCherry(SDL_Renderer* renderer, int left, int top, Position pos)
{
    drawCell(renderer, left, top, pos, gallery->getImage(PIC_BIG_CHERRY));
}


void drawSnake(SDL_Renderer* renderer, int left, int top, vector<Position> pos)
{
	// snake's head
	drawCell(renderer, left, top, pos[pos.size()-1], gallery->getImage(PIC_SNAKE_HEAD));

	// snake's body
    for (int i = pos.size() - 2; i >= 0; i--) {
        SDL_Texture* texture = gallery->getImage(
			pos[i].y == pos[i+1].y ? PIC_SNAKE_HORIZONTAL : PIC_SNAKE_VERTICAL);
        drawCell(renderer, left, top, pos[i], texture);
    }
}

void drawVerticalLine(SDL_Renderer* renderer, int left, int top, int cells)
{
    SDL_SetRenderDrawColor(renderer, LINE_COLOR.r, LINE_COLOR.g, LINE_COLOR.b, 0);
    SDL_RenderDrawLine(renderer, left, top, left, top + cells * CELL_SIZE);
}

void drawHorizontalLine(SDL_Renderer* renderer, int left, int top, int cells)
{
    SDL_SetRenderDrawColor(renderer, LINE_COLOR.r, LINE_COLOR.g, LINE_COLOR.b, 0);
    SDL_RenderDrawLine(renderer, left, top, left + cells * CELL_SIZE, top);
}

void renderGamePlay(SDL_Renderer* renderer, const Game& game )
{
    int top = 0, left = 0;
    SDL_SetRenderDrawColor(renderer, BOARD_COLOR.r, BOARD_COLOR.g, BOARD_COLOR.b, 0);
    SDL_RenderClear(renderer);

    drawHorizontalLine(renderer, left, top + 1 * CELL_SIZE, BOARD_WIDTH);
    //drawWall(renderer, left+1, top+1);
    drawCherry(renderer, left, top, game.getCherryPosition());
    drawBigCherry(renderer, left, top, game.getBigCherryPosition());
    drawSnake(renderer, left, top, game.getSnakePositions());
    drawScore(renderer, game);

    string str_high_score = "High Score: ";
    string str_val3 = to_string(highScore);
    str_high_score += str_val3;
    drawText(renderer, str_high_score, SCREEN_WIDTH - 750, 15, TextObject:: RED_TEXT);


    SDL_RenderPresent(renderer);
}
void drawText(SDL_Renderer* renderer, string str_val, int x, int y, int type ){
    if(TTF_Init()==-1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }
    TTF_Font* ourFont=TTF_OpenFont("dlxfont_.ttf", 15);

    TextObject game;
    game.SetColor(type);
    game.SetText(str_val);
    game.LoadFromRenderText(ourFont, renderer);
    game.RenderText(renderer, x, y);

}
void renderGameOver(SDL_Renderer* renderer, Game& game, SDL_Event e)
{
    bool running = true;
    SDL_Texture* gameOver=loadTexture("demo_GameOver3.png", renderer);
    SDL_RenderCopy(renderer, gameOver,NULL,NULL);

    cout << game.getScore() << endl;
    drawText(renderer, "Press 1 for a second chance" , SCREEN_WIDTH - 650, 450, TextObject:: WHITE_TEXT );
    drawText(renderer, "Press 2 to quit game", SCREEN_WIDTH - 650, 500, TextObject:: WHITE_TEXT);
    if(game.getScore() > highScore){
        highScore = game.getScore();
    }
    string str_score = "Score: ";
    string str_va12= to_string(game.getScore());
    str_score+=str_va12;

    string str_high_score = "High Score: ";
    string str_val3 = to_string(highScore);
    str_high_score += str_val3;

    drawText(renderer, str_score, SCREEN_WIDTH - 650, 350, TextObject:: WHITE_TEXT);
    drawText(renderer, str_high_score, SCREEN_WIDTH - 650, 400, TextObject:: WHITE_TEXT);

    SDL_RenderPresent(renderer);
    while(running){
        if(SDL_WaitEvent(&e)==0) SDL_Delay(100);
        else if(e.key.keysym.sym == SDLK_1){
            Game game(BOARD_WIDTH, BOARD_HEIGHT);
            play(renderer, game, e);
        }
        else if (e.key.keysym.sym == SDLK_2 ){
            return;
        }
    }
}
void play(SDL_Renderer* renderer, Game& game, SDL_Event e){
    Mix_Chunk* chunk = NULL;
    Mix_Chunk* boostEffect = NULL;
    Mix_Music* music = NULL;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		cout << Mix_GetError();
		exit(2);
	}

	chunk = Mix_LoadWAV("eating_cherry.mp3");
		if (chunk == NULL)
	{
		cout << Mix_GetError();
		exit(2);
	}
	boostEffect  = Mix_LoadWAV("boost.mp3");
		if (boostEffect  == NULL)
	{
		cout << Mix_GetError();
		exit(2);
	}
    RenderOption(renderer, e);
    music = Mix_LoadMUS("gameOver.mp3");
    if (music == NULL)
	{
		cout <<  Mix_GetError();
	}
    srand(time(0));
    auto start = CLOCK_NOW();
    int temp = game.getScore();
    int temp1 = game.getSpeed();
    renderGamePlay(renderer, game);

    while (game.isGameRunning()) {
        while (SDL_PollEvent(&e)) {
            interpretEvent(e, game);
        }

        if(game.getSpeed() > temp1){
             STEP_DELAY= STEP_DELAY - STEP_DELAY/15;
             if (!Mix_Playing(-1)){
                Mix_PlayChannel(-1, boostEffect, 0);
            }
             temp1 = game.getSpeed();
        }
        if(game.getScore() > temp){
            STEP_DELAY= STEP_DELAY - STEP_DELAY/20;
            if (!Mix_Playing(-1)){
                Mix_PlayChannel(-1, chunk, 0);
            }
            temp = game.getScore();
        }
        auto end = CLOCK_NOW();
        ElapsedTime elapsed = end-start;
        if (elapsed.count() > STEP_DELAY) {
            game.nextStep();
            renderGamePlay(renderer, game);
            start = end;
        }
    }
    SDL_Delay(1);
    if (!Mix_PlayingMusic()){
            Mix_PlayMusic(music, 0);
    }

    renderGameOver(renderer, game, e);
}

void interpretEvent(SDL_Event e,  Game& game)
{
    if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
        	case SDLK_UP: game.processUserInput(UP); break;
        	case SDLK_DOWN: game.processUserInput(DOWN); break;
        	case SDLK_LEFT: game.processUserInput(LEFT); break;
        	case SDLK_RIGHT: game.processUserInput(RIGHT); break;
        }
    }
}
void RenderOption(SDL_Renderer* renderer, SDL_Event e){
    SDL_Texture* option=loadTexture("demo_option.png", renderer);
    SDL_RenderCopy(renderer, option,NULL,NULL);


    drawText(renderer, "Game Mode ", SCREEN_WIDTH - 650, 150, TextObject:: BLACK_TEXT );
    drawText(renderer, "Press a to choose easy mode ", SCREEN_WIDTH - 650, 200, TextObject:: BLACK_TEXT);
    drawText(renderer, "Press b to choose normal mode ", SCREEN_WIDTH - 650, 250, TextObject:: BLACK_TEXT);
    drawText(renderer, "Press c to choose hard mode ", SCREEN_WIDTH - 650, 300, TextObject:: BLACK_TEXT);
    SDL_RenderPresent(renderer);

    bool running = true;
    while(running){
        if(SDL_WaitEvent(&e)==0) SDL_Delay(100);
        else if(e.key.keysym.sym == SDLK_b){
            return;

        }
        else if (e.key.keysym.sym == SDLK_c){
            STEP_DELAY = 0.1;
            return;
        }
        else if (e.key.keysym.sym == SDLK_a){
            STEP_DELAY = 0.3;
            return;
        }
    }
}


