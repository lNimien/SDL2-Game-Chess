#include "Game.h"
#include "SDL.h"
#include "stdio.h"
#include "SDL_image.h"
#include "constants.h"
#include "string"
SDL_Rect* srcR, destR;

ChessPiece* Game::m_pTmpPiece = nullptr;
Cell* Game::m_pTmpCell = nullptr;

enum ColorPieces
{
	WHITE = 0,
	BLACK = 1,
	OVERALL = 2
};

enum TypeTextureLoads
{
	Cells = 0,
	Ball = 1,
	RedCells = 2
};

Game::Game()
{
	bIsRuning = false;
	nCount = 0;

}
Game::~Game()
{

}

void Game::Input()
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		bIsRuning = false;
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (event.button.button == SDL_BUTTON_LEFT) 
		{
			int boardWidth = 8 * SQUARE_SIZE;
			int boardHeight = 8 * SQUARE_SIZE;

			int boardPosX = (WinValueX - boardWidth) / 2;
			int boardPosY = (WinValueY - boardHeight) / 2;

			int cellWidth = boardWidth / 8;
			int cellHeight = boardHeight / 8;

			int row = (event.button.y - boardPosY) / cellHeight;
			int column = (event.button.x - boardPosX) / cellWidth;

			printf("Fila: %d, Columna: %d\n", row, column);
			int TmpLastRow = row;
			int TmpLastColum = column;
			if (row >= 0 && row < 8 && column >= 0 && column < 8)
			{
				Cell* clickedCell = m_pTablero->m_pCells[row][column];
				
				if (clickedCell->pPiece && !clickedCell->bVisibleRedTexture)
				{
					clickedCell->pPiece->GetMoves(row, column, m_pTablero);
					m_pTmpPiece = clickedCell->pPiece;
					m_pTmpCell = clickedCell;
					m_pTmpCell->Row = row;
					m_pTmpCell->Colum = column;
				}
				else
				{
					if (m_pTmpPiece && m_pTmpPiece->CanMove(row, column, m_pTablero, m_pTmpCell->Row, m_pTmpCell->Colum))
					{
						Cell* NewCell = m_pTablero->m_pCells[row][column];

						if (NewCell)
							NewCell->pPiece = m_pTmpPiece;


						Cell* LastCell = m_pTmpCell;
						if (LastCell)
							LastCell->pPiece = nullptr;

					}

					m_pTmpCell = nullptr;
					m_pTmpPiece = nullptr;

					for (int i = 0; i < 8; i++)
					{
						for (int j = 0; j < 8; j++)
						{
							m_pTablero->m_pCells[i][j]->ClearVisibleBallTextures(); // limpiamos todas las casillas
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < 8; i++)
				{
					for (int j = 0; j < 8; j++)
					{
						m_pTablero->m_pCells[i][j]->ClearVisibleBallTextures(); // limpiamos todas las casillas
					}
				}
			}
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			int boardWidth = 8 * SQUARE_SIZE;
			int boardHeight = 8 * SQUARE_SIZE;

			int boardPosX = (WinValueX - boardWidth) / 2;
			int boardPosY = (WinValueY - boardHeight) / 2;

			int cellWidth = boardWidth / 8;
			int cellHeight = boardHeight / 8;

			int row = (event.button.y - boardPosY) / cellHeight;
			int column = (event.button.x - boardPosX) / cellWidth;

			Cell* clickedCell = m_pTablero->m_pCells[row][column];
			clickedCell->bVisibleRedTexture = true;
		}
		break;
	default:
		break;
	}
}

void Game::Render()
{
	SDL_RenderClear(MainRender);
	SDL_RenderCopy(MainRender, backgroundTexture, NULL, NULL);
	m_pTablero->Render(MainRender);
	
	SDL_RenderPresent(MainRender);
}

void Game::Update()
{
	nCount++;
	destR.x = WinValueX;
	destR.y = WinValueY;
	//printf("%d\n", nCount);
}

void Game::Init(const char* strTitle, int x, int y, int widht, int heigh, bool fullscreen)
{
	auto flag = 0;
	if (fullscreen)
	{
		flag = SDL_WINDOW_FULLSCREEN;
	}
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		MainWindow = SDL_CreateWindow(strTitle, x, y, widht, heigh, flag);

		if (MainWindow)
		{
			printf("Window Creada");
		}
		MainRender = SDL_CreateRenderer(MainWindow, -1, 0);

		if (MainRender)
		{
			printf("Todo ok");
		}
		bIsRuning = true;
	}
	else
	{
		bIsRuning = false;
	}

	LoadTextures();
}

void Game::Clean()
{
	SDL_DestroyWindow(MainWindow);
	SDL_DestroyRenderer(MainRender);
	SDL_Quit();
}

void Game::LoadTextures()
{
	SDL_Surface* tmpSurface = IMG_Load("Assets/background.png");
	backgroundTexture = SDL_CreateTextureFromSurface(MainRender, tmpSurface);
	SDL_FreeSurface(tmpSurface);
}


Tablero::Tablero()
{
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			m_pCells[j][i] = nullptr;
		}
	}
}

Tablero::~Tablero()
{
}

void Tablero::Init(SDL_Renderer* Render)
{
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			m_pCells[y][x] = new Cell(); 
			m_pCells[y][x]->LoadTextureCell(Render, ((x + y) % 2 == 0) ? "assets/white.png" : "assets/dark.png", Cells);
			m_pCells[y][x]->LoadTextureCell(Render, "assets/bolita.png",Ball);
			m_pCells[y][x]->LoadTextureCell(Render, "assets/red.png",RedCells);

			m_pCells[y][x]->Color = ((x + y) % 2 == 0) ? WHITE : BLACK;

			if (y == 0) 
			{
				switch (x) 
				{
					//esquinas
				case 0:
				case 7:
					m_pCells[y][x]->pPiece = new Rook(IMG_LoadTexture(Render,"assets/b_rook.png"), BLACK);
					break;
					//caballos
				case 1:
				case 6:
					m_pCells[y][x]->pPiece = new Knight(IMG_LoadTexture(Render, "assets/b_knight.png"), BLACK);
					break;
					//alfiles
				case 2:
				case 5:
					m_pCells[y][x]->pPiece = new Bishop(IMG_LoadTexture(Render, "assets/b_bishop.png"), BLACK);
					break;
				case 3:
					m_pCells[y][x]->pPiece = new Queen(IMG_LoadTexture(Render, "assets/b_Queen.png"), BLACK);
					break;
				case 4:
					m_pCells[y][x]->pPiece = new King(IMG_LoadTexture(Render, "assets/b_king.png"), BLACK);
					break;
				}
			}
			else if (y == 1) 
				m_pCells[y][x]->pPiece = new Pawn(IMG_LoadTexture(Render, "assets/b_pawn.png"), BLACK);
			else if (y == 6) 
				m_pCells[y][x]->pPiece = new Pawn(IMG_LoadTexture(Render, "assets/w_pawn.png"), WHITE);
			else if (y == 7) 
			{
				switch (x) 
				{
				case 0:
				case 7:
					m_pCells[y][x]->pPiece = new Rook(IMG_LoadTexture(Render, "assets/w_rook.png"), WHITE);
					break;
				case 1:
				case 6:
					m_pCells[y][x]->pPiece = new Knight(IMG_LoadTexture(Render, "assets/w_knight.png"), WHITE);
					break;
				case 2:
				case 5:
					m_pCells[y][x]->pPiece = new Bishop(IMG_LoadTexture(Render, "assets/w_bishop.png"), WHITE);
					break;
				case 3:
					m_pCells[y][x]->pPiece = new Queen(IMG_LoadTexture(Render, "assets/w_queen.png"), WHITE);
					break;
				case 4:
					m_pCells[y][x]->pPiece = new King(IMG_LoadTexture(Render, "assets/w_king.png"), WHITE);
					break;
				}
			}
		
		}
	}
}

void Tablero::Render(SDL_Renderer* Render)
{
	//tablero
	int boardWidth = 8 * SQUARE_SIZE;
	int boardHeight = 8 * SQUARE_SIZE;

	int startX = (WinValueX - boardWidth) / 2;
	int startY = (WinValueY - boardHeight) / 2;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			int x = startX + j * SQUARE_SIZE;
			int y = startY + i * SQUARE_SIZE;
			Cell* pCell = m_pCells[i][j];
			if (pCell) 
			{
				SDL_Rect destRect = { x, y, SQUARE_SIZE, SQUARE_SIZE };
				if (pCell->Texture)
				{
					SDL_RenderCopy(Render, pCell->Texture, NULL, &destRect);
				}
				if (pCell->BallTexture && pCell->bVisibleBallTexture) 
				{
					SDL_Rect ballDestRect = { x + (SQUARE_SIZE - BALL_SIZE) / 2, y + (SQUARE_SIZE - BALL_SIZE) / 2, BALL_SIZE, BALL_SIZE };
					SDL_RenderCopy(Render, pCell->BallTexture, NULL, &ballDestRect);
				}
				if (pCell->RedTexture && pCell->bVisibleRedTexture)
				{
					SDL_RenderCopy(Render, pCell->RedTexture, NULL, &destRect);
				}
				if (pCell->pPiece)
				{
					int pieceX = x + (SQUARE_SIZE - 46) / 2; // Ajusta el ancho de la pieza
					int pieceY = y + (SQUARE_SIZE - 46) / 2; // Ajusta el alto de la pieza
					SDL_Rect PieceDestRect = { pieceX, pieceY, 46, 46 };
					SDL_RenderCopy(Render, pCell->pPiece->Texture, NULL, &PieceDestRect);
				}
			}
		}
	}
}

Cell::Cell()
{
	Color = OVERALL;
	Row = 0;
	Colum = 0;
	bVisibleBallTexture = false;
	bVisibleRedTexture = false;
}

Cell::~Cell()
{
}


void Cell::ClearVisibleBallTextures()
{
	bVisibleBallTexture = false;
	bVisibleRedTexture = false;
}

void Cell::LoadTextureCell(SDL_Renderer* Render, const std::string& filePath, int bType)
{
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	if (!surface)
	{
		printf("Error al cargar la imagen: %s", IMG_GetError());
		return;
	}

	switch (bType)
	{
	case 0:
		Texture = SDL_CreateTextureFromSurface(Render, surface);
		if (!Texture)
		{
			printf("Error al crear la textura: %s", SDL_GetError());
			SDL_FreeSurface(surface);
			return;
		}
		break;
	case 1:
		BallTexture = SDL_CreateTextureFromSurface(Render, surface);
		if (!BallTexture)
		{
			printf("Error al crear la textura: %s", SDL_GetError());
			SDL_FreeSurface(surface);
			return;
		}
		break;
	case 2:
		RedTexture = SDL_CreateTextureFromSurface(Render, surface);
		if (!RedTexture)
		{
			printf("Error al crear la textura: %s", SDL_GetError());
			SDL_FreeSurface(surface);
			return;
		}
		break;
	default:
		break;
	}
		

	// Liberar la superficie
	SDL_FreeSurface(surface);
}

ChessPiece::ChessPiece(SDL_Texture* texture, int color)
	: Texture(texture), Color(color)
{

}

ChessPiece::~ChessPiece()
{
}

void ChessPiece::Clear(Tablero* MainTablero)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			MainTablero->m_pCells[i][j]->ClearVisibleBallTextures(); // limpiamos todas las casillas
		}
	}
}



Pawn::Pawn(SDL_Texture* texture, int color)
	: ChessPiece(texture, color) 
{

}

Pawn::~Pawn()
{
}

void Pawn::GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero)
{
	
	//Clear squares textures
	Clear(MainTablero);

	RemarcarDiagonalCells(-1, InitialRow, InitialColum, MainTablero); // Diagonal izquierda
	RemarcarDiagonalCells(1, InitialRow, InitialColum, MainTablero); // Diagonal derecha


	int direction = (Color == WHITE) ? -1 : 1;
	int startRow = (Color == WHITE) ? 6 : 1;
	int nextRow1 = InitialRow + direction;
	Cell* nextCell1 = MainTablero->m_pCells[nextRow1][InitialColum];


	if (nextRow1 >= 0 && nextRow1 < 8)
	{
		if (!nextCell1->pPiece) // next vacia
		{
			nextCell1->bVisibleBallTexture = true;
		}
	}

	//caso especial peon al inicio
	MovimientoInicial(InitialRow, InitialColum, MainTablero);
	
}

bool Pawn::CanMove(int DestinationRow, int DestinationColumn, Tablero* MainTablero, int InitialRow, int InitialColumn)
{
	int direction = (Color == WHITE) ? -1 : 1;
	int nextRow = InitialRow + direction; 

	Cell* ClickedCell = MainTablero->m_pCells[DestinationRow][DestinationColumn];

	if (nextRow >= 0 && nextRow < 8 && !ClickedCell->pPiece)
	{
		if (InitialColumn != DestinationColumn && DestinationRow != InitialRow) // si se mueve en diagonal y no hay pieza en esa casilla
		{
			return false;
		}
		if ((InitialRow == 6 && Color == WHITE) || (InitialRow == 1 && Color == BLACK))
		{
			if (abs(nextRow - DestinationRow) < 2) 
			{
				return true;
			}
		}
		else
		{
			if (nextRow - DestinationRow == 0 ) // se mueve una casilla solo
			{
				return true;
			}
		}
	}
	else if (nextRow >= 0 && nextRow < 8)
	{
		if (ClickedCell->pPiece && ClickedCell->pPiece->Color != Color)
			return true;
	}

	return false;
}

void Pawn::RemarcarDiagonalCells(int rowOffset, int InitialRow, int InitialColumn, Tablero* MainTablero) 
{
	int direction = (Color == WHITE) ? -1 : 1;
	int targetRow = InitialRow + direction;
	int targetColumn = InitialColumn + rowOffset;

	if (targetRow >= 0 && targetRow < 8 && targetColumn >= 0 && targetColumn < 8) 
	{
		Cell* diagonalCell = MainTablero->m_pCells[targetRow][targetColumn];
		if (diagonalCell->pPiece && diagonalCell->pPiece->Color != Color) 
			diagonalCell->bVisibleRedTexture = true;
	}
}

void Pawn::MovimientoInicial(int InitialRow, int InitialColum, Tablero* MainTablero)
{
	int direction = (Color == WHITE) ? -1 : 1;
	int startRow = (Color == WHITE) ? 6 : 1;

	if (startRow == InitialRow) 
	{
		int nextRow2 = InitialRow + 2 * direction;
		Cell* nextCell2 = MainTablero->m_pCells[nextRow2][InitialColum];
		if (!nextCell2->pPiece) 
			nextCell2->bVisibleBallTexture = true;
	}
}

Rook::Rook(SDL_Texture* texture, int color)
	: ChessPiece(texture, color)
{
}

Rook::~Rook()
{
}

void Rook::GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero)
{
}

bool Rook::CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum)
{
	return false;
}

Knight::Knight(SDL_Texture* texture, int color)
	: ChessPiece(texture, color)
{
}

Knight::~Knight()
{
}

void Knight::GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero)
{
}

bool Knight::CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum)
{
	return false;
}

Bishop::Bishop(SDL_Texture* texture, int color)
	: ChessPiece(texture, color)
{
}

Bishop::~Bishop()
{
}

void Bishop::GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero)
{
	//Clear squares textures
	Clear(MainTablero);


	int direction = (Color == WHITE) ? -1 : 1;

	std::vector<Cell*> vDiagonales;
	Cell* ActualCell = MainTablero->m_pCells[InitialRow][InitialColum];

	int newRow, newColumn;
	for (int offset = 0; offset < 8; offset++) // esta dentro del tablero
	{
		//Diagonal Superior Izquierda
		newRow = InitialRow + offset * direction;
		newColumn = InitialColum - offset;



		if (newRow >= 0 && newRow < 8 && newColumn >= 0 && newColumn < 8)
		{
			if (ActualCell->pPiece == MainTablero->m_pCells[newRow][newColumn]->pPiece) // no chequeamos la misma row no tiene sentido
			{
				continue;
			}

			if (MainTablero->m_pCells[newRow][newColumn]->pPiece && MainTablero->m_pCells[newRow][newColumn]->pPiece->Color != Color)
			{
				MainTablero->m_pCells[newRow][newColumn]->bVisibleRedTexture = true; break;
			}
			else if (MainTablero->m_pCells[newRow][newColumn]->pPiece)
			{
				break;
			}
			else
				vDiagonales.push_back(MainTablero->m_pCells[newRow][newColumn]);
		}
	}

	for (int offset = 0; offset < 8; offset++) // esta dentro del tablero
	{


		//Diagonal Superior derecha
		newRow = InitialRow + offset * direction;
		newColumn = InitialColum + offset;
		if (newRow >= 0 && newRow < 8 && newColumn >= 0 && newColumn < 8)
		{
			if (ActualCell->pPiece == MainTablero->m_pCells[newRow][newColumn]->pPiece) // no chequeamos la misma row no tiene sentido
			{
				continue;
			}
			if (MainTablero->m_pCells[newRow][newColumn]->pPiece && MainTablero->m_pCells[newRow][newColumn]->pPiece->Color != Color)
			{
				MainTablero->m_pCells[newRow][newColumn]->bVisibleRedTexture = true; break;
			}
			else if (MainTablero->m_pCells[newRow][newColumn]->pPiece)
			{
				break;
			}
			else
				vDiagonales.push_back(MainTablero->m_pCells[newRow][newColumn]);
		}
	}

	for (int offset = 0; offset < 8; offset++) // esta dentro del tablero
	{
		//Diagonal inferior izquierda
		newRow = InitialRow - offset * direction;
		newColumn = InitialColum - offset;
		if (newRow >= 0 && newRow < 8 && newColumn >= 0 && newColumn < 8)
		{
			if (ActualCell->pPiece == MainTablero->m_pCells[newRow][newColumn]->pPiece) // no chequeamos la misma row no tiene sentido
			{
				continue;
			}

			if (MainTablero->m_pCells[newRow][newColumn]->pPiece && MainTablero->m_pCells[newRow][newColumn]->pPiece->Color != Color)
			{
				MainTablero->m_pCells[newRow][newColumn]->bVisibleRedTexture = true; break;
			}
			else if (MainTablero->m_pCells[newRow][newColumn]->pPiece)
			{
				break;
			}
			else
				vDiagonales.push_back(MainTablero->m_pCells[newRow][newColumn]);
		}
	}

	for (int offset = 0; offset < 8; offset++) // esta dentro del tablero
	{
		//Diagonal inferior derecha
		newRow = InitialRow - offset * direction;
		newColumn = InitialColum + offset;
		if (newRow >= 0 && newRow < 8 && newColumn >= 0 && newColumn < 8)
		{
			if (ActualCell->pPiece == MainTablero->m_pCells[newRow][newColumn]->pPiece) // no chequeamos la misma row no tiene sentido
			{
				continue;
			}
			if (MainTablero->m_pCells[newRow][newColumn]->pPiece && MainTablero->m_pCells[newRow][newColumn]->pPiece->Color != Color)
			{
				MainTablero->m_pCells[newRow][newColumn]->bVisibleRedTexture = true; break;
			}
			else if (MainTablero->m_pCells[newRow][newColumn]->pPiece)
			{
				break;
			}
			else
				vDiagonales.push_back(MainTablero->m_pCells[newRow][newColumn]);
		}

	}

	for (auto cell : vDiagonales)
	{
		cell->bVisibleBallTexture = true;
	}


	vDiagonales.clear();
}

bool Bishop::CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum)
{
	return false;
}

Queen::Queen(SDL_Texture* texture, int color)
	: ChessPiece(texture, color)
{
}

Queen::~Queen()
{
}

void Queen::GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero)
{
}

bool Queen::CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum)
{
	return false;
}

King::King(SDL_Texture* texture, int color)
	: ChessPiece(texture, color)
{
}

King::~King()
{
}

void King::GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero)
{
}

bool King::CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum)
{
	return false;
}

