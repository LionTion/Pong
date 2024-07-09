#include "Pong.h"

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 50;

const int MINBALLSPEED = 4;

int score[] = { 0, 0 };

bool limitPaddleMovement(SDL_Rect* paddle) {
	int paddleMaxHeight = SCREEN_HEIGHT - paddle->h;
	if (paddle->y < 0) {
		paddle->y = 0;
		return true;
	}
	else if (paddle->y > paddleMaxHeight) {
		paddle->y = paddleMaxHeight;
		return true;
	}
	return false;
}

int moveDelta(float direction, double velocity, double delta) {
	return ((double)direction * velocity) * delta;
}

float ballvel[2] = { 0, 0 };
void resetBall(SDL_Rect* ball) {
	srand(SDL_GetTicks());
	int x = 0, y = 0;
	switch (rand() % 2) {
		case 0:
			x = 1;
			break;
		case 1:
			x = -1;
			break;
	}
	ballvel[0] = x;
	ballvel[1] = (100 - (rand() % 200)) / 100.0f;
	ball->x = SCREEN_WIDTH / 2 - 10;
	ball->y = SCREEN_HEIGHT / 2 - 10;
}

void resetPaddle(SDL_Rect* paddle) {
	paddle->y = SCREEN_HEIGHT / 2 - paddle->h / 2;
}

int checkWindowBorders(SDL_Rect* rect) {
	int maxX = SCREEN_WIDTH - rect->w;
	int maxY = SCREEN_HEIGHT - rect->h;

	bool ly = rect->y < 0;
	bool hy = rect->y > maxY;
	bool lx = rect->x < 0;
	bool hx = rect->x > maxX;

	if (lx) return -1;
	if (hx) return 1;

	if (ly) {
		rect->y = 0;
	}
	else if (hy) {
		rect->y = maxY;
	}

	if (ly || hy) {
		ballvel[1] = -ballvel[1];
	}
	return 0;
}

bool InRect(int x, int y, int* rect) {
	return x >= rect[0] && y >= rect[1] && x <= rect[2] && y <= rect[3];
}
bool collideBall(SDL_Rect ball, SDL_Rect paddle) {
	int rect[] = { paddle.x, paddle.y, paddle.x + paddle.w, paddle.y + paddle.h };

	int x = ball.x;
	int y = ball.y;
	int x2 = ball.x + ball.w;
	int y2 = ball.y + ball.h;

	return InRect(x, y, rect) || InRect(x, y2, rect) || InRect(x2, y, rect) || InRect(x2, y2, rect);
}

SDL_Rect scorePositions[2] = { {100, 100, 0, 0}, {SCREEN_WIDTH-125, 100, 0, 0} };
Text scoreTexts[2];

void writeScore(Text* scoreText, int score, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer, SDL_Rect* rect) {
	string str = std::to_string(score);
	SDL_Surface* surface = TTF_RenderText_Solid(font, str.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	scoreText->surface = surface;
	scoreText->texture = texture;
	rect->w = surface->w;
	rect->h = surface->h;
}
void freeScore(Text* scoreText) {
	SDL_DestroyTexture(scoreText->texture);
	SDL_FreeSurface(scoreText->surface);
}

int main(int argc, char* args[])
{
	cout << "Initializing..." << endl;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("Nerd emoji > %s\n", SDL_GetError());
		return 1;
	}
	if (TTF_Init() != 0) {
		printf("errrmmm no text! > %s\n", SDL_GetError());
		return 1;
	}
	cout << "We be runnin'" << endl;

	TTF_Font* font;
	font = TTF_OpenFont("font.ttf", 32);
	if (!font) {
		printf("Why no font.ttf file??? %s\n", TTF_GetError());
		return 1;
	}

	SDL_Window* window;
	SDL_Surface* surface;

	window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_KEYBOARD_GRABBED);
	if (window == NULL) {
		printf("No window nerd > %s\n", SDL_GetError());
		return 1;
	}

	surface = SDL_GetWindowSurface(window);
	SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);

	SDL_Rect leftpaddle = {
		32,
		SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2,
		PADDLE_WIDTH,
		PADDLE_HEIGHT
	};

	SDL_Rect rightpaddle = {
		SCREEN_WIDTH - 42,
		SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2,
		PADDLE_WIDTH,
		PADDLE_HEIGHT
	};
	SDL_Rect ball = {
		SCREEN_WIDTH / 2 - 10,
		SCREEN_HEIGHT / 2 - 10,
		20,
		20
	};

	Uint32 bgColor = SDL_MapRGB(surface->format, 0x0, 0x0, 0x7);
	Uint32 fgColor = SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF);
	Uint32 lostColor = SDL_MapRGB(surface->format, 0xFF, 0x0, 0x0);
	SDL_Color textColor = { 128, 128, 128 };

	int leftInputDirection = 0;
	int rightInputDirection = 0;
	Uint64 lastTicks = SDL_GetTicks64();
	Uint64 lastMoveTicks = SDL_GetTicks64();
	Uint64 maxFPS = 1000/60;
	double delta = 1.0f;

	double startTimer = lastTicks;
	bool hasStarted = false;
	bool isBallTurning = false;

	writeScore(&scoreTexts[0], score[0], font, textColor, renderer, &scorePositions[0]);
	writeScore(&scoreTexts[1], score[1], font, textColor, renderer, &scorePositions[1]);

	resetBall(&ball);
	double ballSpeed = MINBALLSPEED;

	SDL_Event e;
	while (true) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					goto quit;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							goto quit;
						case SDLK_DOWN:
							rightInputDirection = 1;
							break;
						case SDLK_UP:
							rightInputDirection = -1;
							break;
						case SDLK_w:
							leftInputDirection = -1;
							break;
						case SDLK_s:
							leftInputDirection = 1;
							break;
					}
					break;
				case SDL_KEYUP:
					switch (e.key.keysym.sym) {
						case SDLK_DOWN:
							if (rightInputDirection != 1) break;
							rightInputDirection = 0;
							break;
						case SDLK_UP:
							if (rightInputDirection != -1) break;
							rightInputDirection = 0;
							break;
						case SDLK_w:
							if (leftInputDirection != -1) break;
							leftInputDirection = 0;
							break;
						case SDLK_s:
							if (leftInputDirection != 1) break;
							leftInputDirection = 0;
							break;
					}
					break;
			}
		}

		Uint64 currentTicks = SDL_GetTicks64();
		Uint64 delta = currentTicks - lastTicks;
		double control = (double)delta / (double)maxFPS;
		if (!hasStarted && (currentTicks - startTimer) > 1500) {
			hasStarted = true;
		}
		if (delta >= maxFPS) {
			int leftVel = moveDelta(leftInputDirection, 8, control);
			int rightVel = moveDelta(rightInputDirection, 8, control);
			leftpaddle.y += leftVel;
			rightpaddle.y += rightVel;
			limitPaddleMovement(&leftpaddle);
			limitPaddleMovement(&rightpaddle);

			if (hasStarted) {
				ball.x += moveDelta(ballvel[0], ballSpeed, control);
				ball.y += moveDelta(ballvel[1], ballSpeed, control);
				int scoreCheck = checkWindowBorders(&ball);
				if (scoreCheck != 0) {
					resetBall(&ball);
					hasStarted = false;
					startTimer = currentTicks;
					ballSpeed = MINBALLSPEED;
					resetPaddle(&leftpaddle);
					resetPaddle(&rightpaddle);
					int x = -1;
					switch (scoreCheck) {
					case -1:
						x = 1;
						break;
					case 1:
						x = 0;
						break;
					}
					freeScore(&scoreTexts[x]);
					writeScore(&scoreTexts[x], ++score[x], font, textColor, renderer, &scorePositions[x]);
				}
				else {
					bool ballcolliding = collideBall(ball, leftpaddle) || collideBall(ball, rightpaddle);
					if (ballcolliding && !isBallTurning) {
						ballvel[0] = -ballvel[0];
						ballvel[1] = (100 - (rand() % 200)) / 100.0f;
						isBallTurning = true;
						ballSpeed += 0.1;
					}
					else if (!ballcolliding && isBallTurning) {
						isBallTurning = false;
					}
				}
			}

			SDL_FillRect(surface, NULL, bgColor);
			SDL_RenderCopy(renderer, scoreTexts[0].texture, NULL, &scorePositions[0]);
			SDL_RenderCopy(renderer, scoreTexts[1].texture, NULL, &scorePositions[1]);
			SDL_FillRect(surface, &leftpaddle, fgColor);
			SDL_FillRect(surface, &rightpaddle, fgColor);
			SDL_FillRect(surface, &ball, (ball.x < 20 || ball.x+ball.w > SCREEN_WIDTH - 20) ? lostColor : fgColor);
			SDL_RenderPresent(renderer);
			SDL_UpdateWindowSurface(window);
			lastTicks = currentTicks;
		}
	}
quit:
	printf("We be quittin'\n");

	for (int i = 0; i < 2; ++i) {
		Text score = scoreTexts[i];
		freeScore(&score);
	}
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
