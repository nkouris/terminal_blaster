#include "Game.hpp"

const int	Game::GAME_WINDOW_WIDTH = 40;
const int	Game::GAME_WINDOW_HEIGHT = 40;
const int	Game::INFO_WINDOW_WIDTH = 40;
const int	Game::INFO_WINDOW_HEIGHT = 40;

Game::Game(void)
{
	std::srand(time(0));
	setlocale(LC_ALL, "");
	initscr();
	raw();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	noecho();
	curs_set(0);
	start_color();
	refresh();

	this->_gameWindow = newwin(GAME_WINDOW_HEIGHT + 2, GAME_WINDOW_WIDTH + 2, 0, 0);
	this->_infoWindow = newwin(INFO_WINDOW_HEIGHT + 2, INFO_WINDOW_WIDTH + 2, 0, GAME_WINDOW_WIDTH + 3);
	
	this->_board = new Board(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
	this->_sceneBoard = new Board(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
	
	this->_player = new Player(GAME_WINDOW_WIDTH / 2, GAME_WINDOW_HEIGHT * 3 / 4, this);
	this->_board->setCell(this->_player->getPosX(), this->_player->getPosY(), this->_player);
	
//	this->_board->setCell(GAME_WINDOW_WIDTH / 2 + 1, 5, new EnemyTypeB(GAME_WINDOW_WIDTH / 2 + 1, 5, '%', 3, this));

//	EnemyFace *e = new EnemyFace(GAME_WINDOW_WIDTH / 2, GAME_WINDOW_HEIGHT / 2, this);
//	this->_board->setCell(e->getPosX(), e->getPosY(), e);
	
	this->_isGameOver = false;
	this->_score = 0;

	_start = std::chrono::steady_clock::now();

	static std::string bgm[23] =
	{
		"sounds/Hotline_Miami_OST/ANewMorning.mp3",
		"sounds/Hotline_Miami_OST/Crush.mp3",
		"sounds/Hotline_Miami_OST/Crystals.mp3",
		"sounds/Hotline_Miami_OST/Daisuke.mp3",
		"sounds/Hotline_Miami_OST/DeepCover.mp3",
		"sounds/Hotline_Miami_OST/ElectricDreams.mp3",
		"sounds/Hotline_Miami_OST/Flatline.mp3",
		"sounds/Hotline_Miami_OST/HorseSteppin.mp3",
		"sounds/Hotline_Miami_OST/Hotline.mp3",
		"sounds/Hotline_Miami_OST/Hydrogen.mp3",
		"sounds/Hotline_Miami_OST/InnerAnimal.mp3",
		"sounds/Hotline_Miami_OST/ItsSafeNow.mp3",
		"sounds/Hotline_Miami_OST/Knock.mp3",
		"sounds/Hotline_Miami_OST/Miami2.mp3",
		"sounds/Hotline_Miami_OST/Musikk2.mp3",
		"sounds/Hotline_Miami_OST/Paris2.mp3",
		"sounds/Hotline_Miami_OST/Perturbator.mp3",
		"sounds/Hotline_Miami_OST/Release.mp3",
		"sounds/Hotline_Miami_OST/SilverLights.mp3",
		"sounds/Hotline_Miami_OST/Static.mp3",
		"sounds/Hotline_Miami_OST/ToTheTop.mp3",
		"sounds/Hotline_Miami_OST/TurfIntro.mp3",
		"sounds/Hotline_Miami_OST/TurfMain.mp3"
	};

	Game::playSound(bgm[std::rand() % 23]);
}

Game::~Game(void)
{
	if (this->_gameWindow) delwin(this->_gameWindow);
	if (this->_infoWindow) delwin(this->_infoWindow);
	endwin();

	delete this->_sceneBoard;
	delete this->_board;
	std::system("pkill -f afplay");
}

void				Game::playSound(std::string soundFile)
{
	std::string		command = "afplay " + soundFile + " &";
	std::system(command.c_str());
}

void				Game::run(void)
{
	while (this->_isGameOver == false)
	{
		this->_processInput();
		this->_update();	// disabled for debugging; press 't' to run update once
		this->_render();
		usleep(25000);
	}
	Game::playSound("sounds/youLose.wav");
	nodelay(stdscr, FALSE);
	getch();
}

void				Game::addScore(unsigned long long inc)
{
	this->_score += inc;
}

Board *				Game::getBoard(void) const { return (this->_board); }
Board *				Game::getSceneBoard(void) const { return (this->_sceneBoard); }
Player *			Game::getPlayer(void) const { return (this->_player); }

void				Game::setGameOver(void) { this->_isGameOver = true; }

// -----------------------------------------------------------------------------------
// PROCESS_INPUT
// -----------------------------------------------------------------------------------

void				Game::_processInput(void)
{
	switch (getch())
	{
		case (KEY_UP):
			this->_player->move(this->_player->getPosX(), this->_player->getPosY() - 1, this->_board);
			break;
		case (KEY_DOWN):
			this->_player->move(this->_player->getPosX(), this->_player->getPosY() + 1, this->_board);
			break;
		case (KEY_LEFT):
			this->_player->move(this->_player->getPosX() - 1, this->_player->getPosY(), this->_board);
			break;
		case (KEY_RIGHT):
			this->_player->move(this->_player->getPosX() + 1, this->_player->getPosY(), this->_board);
			break;
		case (' '):
			this->_player->shoot();
			break;
		case ('b'):
		case ('B'):
			this->_player->bomb();
			break;
		case ('q'):
		case ('Q'):
			this->_isGameOver = true;
			break;
		case ('t'):
		case ('T'):
			this->_update();
			break;
		default:
			break;
	}
}


// -----------------------------------------------------------------------------------
// UPDATE
// -----------------------------------------------------------------------------------

void				Game::_update(void)
{
	this->_generateScenery();
	this->_generateEnemies();
	this->_sceneBoard->updateAllCells();
	this->_board->updateAllCells();
}

void				Game::_generateScenery(void)
{
	//Scene			*temp = NULL;
	int				x, nobjects;

	nobjects = std::rand() % 4;
	x = std::rand() % this->GAME_WINDOW_WIDTH;
//	moveSpeed = std::rand() % 20;

	while (nobjects--)
	{
		//temp = new Scene(x, 0, 'A', moveSpeed);
		Scene *temp = new Scene(x, 0);
		this->_sceneBoard->setCell(temp->getPosX(), temp->getPosY(), temp);
	}
}

void				Game::_generateEnemies(void)
{
/* switch statement for difficulty selection */


	// spawn EnemyCop section
	if (std::rand() % 100 < 2)
	{
		int x = GAME_WINDOW_WIDTH - 1;
		int y = std::rand() % GAME_WINDOW_HEIGHT;

		if (this->_board->getCell(x, y) == NULL)
		{
			EnemyCop *e = new EnemyCop(x, y, this);
			this->_board->setCell(x, y, e);
		}
	}


	
	// spawn EnemyFace section
	if (std::rand() % 100 < 5)
	{
		int				x, y;
		if (std::rand() % 2)
		{
			// at top or bottom border
			x = std::rand() % GAME_WINDOW_WIDTH;
			y = (std::rand() % 2) ? 0 : GAME_WINDOW_HEIGHT - 1;
		}
		else
		{
			// at left or right border
			x = (std::rand() % 2) ? 0 : GAME_WINDOW_WIDTH - 1;
			y = std::rand() % GAME_WINDOW_HEIGHT;
		}

		if (this->_board->getCell(x, y) == NULL)
		{
			EnemyFace *e = new EnemyFace(x, y, this);
			this->_board->setCell(x, y, e);
		}
	}
	
	
}


// -----------------------------------------------------------------------------------
// RENDER
// -----------------------------------------------------------------------------------

void				Game::_render(void) const
{
	this->_updateGameWindow();
	this->_updateInfoWindow();
}

void				Game::_updateGameWindow(void) const
{
	wclear(this->_gameWindow);

	this->_sceneBoard->renderAllCells(this->_gameWindow);
	this->_board->renderAllCells(this->_gameWindow);
	
	wborder(this->_gameWindow, '|', '|', '-', '-', '+', '+', '+', '+');
	wrefresh(this->_gameWindow);
}

void				Game::_updateInfoWindow(void) const
{
	wclear(this->_infoWindow);
	std::chrono::steady_clock::time_point mark = std::chrono::steady_clock::now();

	std::string minutesTime =
		std::to_string(std::chrono::duration_cast<std::chrono::minutes>(mark - _start).count());
	std::string secondsTime =
		std::to_string(std::chrono::duration_cast<std::chrono::seconds>(mark - _start).count());

//	this->_board->debugAllCells(this->_infoWindow);

	mvwprintw(this->_infoWindow, 1, 1, "Time:\t%02s:%02s", minutesTime.data(), secondsTime.data());
	mvwprintw(this->_infoWindow, 2, 1, "Score:\t%llu", this->_score);
	//mvwprintw(this->_infoWindow, 4, 1, "Lives:\t%u", this->_isGameOver ? 0 : this->_player->getLives());
	mvwprintw(this->_infoWindow, 4, 1, "Lives:\t");
	for (int i = 0; i < this->_player->getLives(); i++)
		wprintw(this->_infoWindow, "%lc ", L'💖');
	//mvwprintw(this->_infoWindow, 5, 1, "Bombs:\t%u", this->_isGameOver ? 0 : this->_player->getBombs());
	mvwprintw(this->_infoWindow, 5, 1, "Bombs:\t");
	for (int i = 0; i < this->_player->getBombs(); i++)
		wprintw(this->_infoWindow, "%lc ", L'💣');

	if (this->_isGameOver)
	{
		mvwprintw(this->_infoWindow, 7, 1, "GAME OVER");
		mvwprintw(this->_infoWindow, 9, 1, "Press R to restart");
		mvwprintw(this->_infoWindow, 10, 1, "Press Q to quit");
	}

	wborder(this->_infoWindow, '|', '|', '-', '-', '+', '+', '+', '+');
	wrefresh(this->_infoWindow);
}























