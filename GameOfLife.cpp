//подключим SDL и stdio
#include <SDL.h>  //x86
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <math.h>



class Lifes
{

private:
	//Некоторые константы нашего окна
	static const int SCREEN_WIDTH = 1166;
	static const int SCREEN_HEIGHT = 568;

	SDL_Window* window;
	SDL_Renderer* ren;
	void (Lifes::* initializeLifes[3]) () = { &Lifes::InitLifesOne ,&Lifes::InitLifesTwo, &Lifes::InitLifesThree };
	//два массива для точек - текущее состояние и новое 
	int lifes[SCREEN_WIDTH][SCREEN_HEIGHT];
	int lifesNew[SCREEN_WIDTH][SCREEN_HEIGHT];
	//Инициализация SDL, окна и рендера
	bool InitSDL()
	{
		bool res = true;
		//инициализируем SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			res = false;
		}
		//создаем окно (имя окна, координаты (x,y), размеры окна, флаги)
		//int request = SDL_GetDesktopDisplayMode(0, &displayMode);
		//window = SDL_CreateWindow("Hello World!", 0, 0, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN);
		window = SDL_CreateWindow("Hello World!", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == nullptr) {
			std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
			res = false;
		}

		//рендер (окно, индекс драйвера (-1 - первый подходящий), флаги (аппаратное ускорение и вертикальная синхронизация)
		ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (ren == nullptr) {
			std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
			res = false;
		}
		return res;
	}
	//дестрой SDL, окна и рендера
	void QuitSDL()
	{
		SDL_DestroyWindow(window);
		window = NULL;

		SDL_DestroyRenderer(ren);
		ren = NULL;

		SDL_Quit();
	}

	//методы начальной отрисовки точек
	void InitLifesOne()
	{
		int xx[] = { rand() % (SCREEN_WIDTH - 1) + 1, rand() % (SCREEN_WIDTH - 1) + 1 };
		int yy[] = { rand() % (SCREEN_HEIGHT - 1) + 1, rand() % (SCREEN_HEIGHT - 1) + 1 };
		for (int i = 0; i < SCREEN_WIDTH; i++)
			for (int j = 0; j < SCREEN_HEIGHT; j++)
			{
				/*lifes[i][j] = rand() % 2;
				lifesNew[i][j] = lifes[i][j];*/
				lifes[i][j] = 0;
				lifesNew[i][j] = lifes[i][j];
				if ((i == xx[0]) | (i == xx[1]) | (j == yy[0]) | (j == yy[1]))
				{
					lifes[i][j] = 1;
					lifesNew[i][j] = lifes[i][j];
				}
			}
	}

	void InitLifesTwo()
	{
		int xx = SCREEN_WIDTH / 8 + rand() % 100;
		int xx2 = SCREEN_WIDTH / 6 + rand() % 100;
		for (int i = 0; i < SCREEN_WIDTH; i++)
			for (int j = 0; j < SCREEN_HEIGHT; j++)
			{
				/*lifes[i][j] = rand() % 2;
				lifesNew[i][j] = lifes[i][j];*/
				lifes[i][j] = 0;
				lifesNew[i][j] = lifes[i][j];
				if ((i - xx == j) | (i + j + xx == SCREEN_WIDTH + 1) | (i - xx2 == j) | (i + j + xx2 == SCREEN_WIDTH + 1))
				{
					lifes[i][j] = 1;
					lifesNew[i][j] = lifes[i][j];
				}
			}
	}

	void InitLifesThree()
	{
		int xx = SCREEN_WIDTH / 2 - 100 + rand() % 200;
		int kk = 100 + rand() % 100;
		int zz = 100000;
		int yy = SCREEN_HEIGHT / 2 - 100 + rand() % 200;
		int yy2 = SCREEN_HEIGHT / 3 - 100 + rand() % 200;
		for (int i = 0; i < SCREEN_WIDTH; i++)
			for (int j = 0; j < SCREEN_HEIGHT; j++)
			{
				/*lifes[i][j] = rand() % 2;
				lifesNew[i][j] = lifes[i][j];*/
				lifes[i][j] = 0;
				lifesNew[i][j] = lifes[i][j];
				if (((int)pow(i - xx, 2) * yy / zz == j) | ((int)pow(i - xx, 2) * yy / zz - 1 == j) | ((int)pow(i - xx, 2) * yy / zz + 1 == j) | (j == yy) | (j == yy2))
				{
					lifes[i][j] = 1;
					lifesNew[i][j] = lifes[i][j];
				}
			}
	}

	//случайным образом выбираем вариант отрисовки
	void InitLifes()
	{
		int k = rand() % (sizeof(initializeLifes) / sizeof(initializeLifes[0]));
		(this->*initializeLifes[k])();
		//(this->*initializeLifes[2])();
		Paint();
		SDL_Delay(500);
	}

	/*определяем новое состояние точек по правилам:
	  точка живая и рядом <2 живых - точка умирает (вырождение)
	  точка живая и рядом 2-3 живых - точка живет дальше
	  точка живая и рядом >3 живых - точка умирает (перенаселение)
	  точка мертва и рядом ровно 3 живых - точка оживает
	  идея взята отсюда - https://www.youtube.com/watch?v=C2vgICfQawE&feature=emb_logo
	*/
	void NewLifes()
	{
		int k = 0;
		for (int i = 1; i < SCREEN_WIDTH - 1; i++)
			for (int j = 1; j < SCREEN_HEIGHT - 1; j++)
			{
				k = 0;
				k += lifes[i - 1][j];
				k += lifes[i + 1][j];
				k += lifes[i - 1][j - 1];
				k += lifes[i + 1][j - 1];
				k += lifes[i - 1][j + 1];
				k += lifes[i + 1][j + 1];
				k += lifes[i][j - 1];
				k += lifes[i][j + 1];
				if ((lifes[i][j] == 1) & ((k < 2) | (k > 3)))
					lifesNew[i][j] = 0;
				else if ((lifes[i][j] == 0) & (k == 3))
					lifesNew[i][j] = 1;
			}
	}

	//копируем полученные значения точек во второй массив
	void CopyLifes()
	{
		for (int i = 1; i < SCREEN_WIDTH - 1; i++)
			for (int j = 1; j < SCREEN_HEIGHT - 1; j++)
				lifes[i][j] = lifesNew[i][j];
	}

	//отрисовка точек на экране
	void Paint()
	{

		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0x00);
		for (int i = 1; i < SCREEN_WIDTH - 1; i++)
			for (int j = 1; j < SCREEN_HEIGHT - 1; j++)
				if (lifes[i][j] == 1)
					SDL_RenderDrawPoint(ren, i, j);

		SDL_RenderPresent(ren);
	}


public:
	Lifes()
	{

	}

	~Lifes()
	{
		QuitSDL();
	}

	bool Initialize()
	{
		if (!InitSDL())
		{
			QuitSDL();
			return false;
		}
		else
		{
			InitLifes();//инициализация массива данных
			Paint();
		}
		return true;
	}

	//вызываем функции обновления состояния точек и пререрисовки экрана, пока не закроем окно
	void GamesLifes()
	{
		SDL_Event event;
		bool quit = false;
		bool newInitLifes = false;
		while (!quit)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
					quit = true;
				if (event.type == SDL_MOUSEBUTTONDOWN)
					newInitLifes = true;
			}
			if ((newInitLifes) & (!quit))
			{
				InitLifes();//инициализация массива данных
				newInitLifes = false;
			}
			NewLifes();
			CopyLifes();
			Paint();
		}
	}

};


int main(int argc, char* args[])
{

	srand(time(0));
	Lifes* life = new Lifes();
	if (life->Initialize())
		life->GamesLifes();
	//getchar();
	delete life;

	return 0;
}