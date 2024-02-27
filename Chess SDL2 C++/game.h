#include "SDL.h"
#include <vector>
#include "string"
class Cell;
class ChessPiece;
class Pawn;

struct Vector2
{
    float x;
    float y;
};


class Tablero
{
public:
    Tablero();
    ~Tablero();

    void Render(SDL_Renderer* Render);
    void Init(SDL_Renderer* Render);
    Cell* m_pCells[8][8];
private:

};

class Game
{
public:
    Game();
    ~Game();

public:
    virtual void Input();
    virtual void Render();
    virtual void Update();
    virtual void Clean();
    void Init(const char* strTitle, int x, int y, int widht, int heigh, bool fullscreen);
    bool IsRuning() { return bIsRuning; }
    void LoadTextures();
    int nCount;

    Tablero* m_pTablero;
    static ChessPiece* m_pTmpPiece;
    static Cell* m_pTmpCell;
    static ChessPiece* GetTmpPiece() { return m_pTmpPiece; }
    static Cell* GetTmpCell() { return m_pTmpCell; }

    SDL_Renderer* GetRender() { return MainRender; }
    SDL_Texture* backgroundTexture;
private:
    bool bIsRuning;
    SDL_Window* MainWindow;
    SDL_Renderer* MainRender;
};

class Cell
{
public:
    Cell();
    ~Cell();
    int Row;
    int Colum;
    void ClearVisibleBallTextures();
    void LoadTextureCell(SDL_Renderer* Render, const std::string& filePath, int bType);
    SDL_Texture* Texture;
    SDL_Texture* BallTexture;
    SDL_Texture* RedTexture;
    Vector2 vPostion;
    ChessPiece* pPiece;
    int Color;
    bool bVisibleBallTexture;
    bool bVisibleRedTexture;
private:

};

struct Color
{
    int WHITE = 0;
    int BLACK = 1;
};
class ChessPiece
{
public:
    ChessPiece(SDL_Texture* texture, int color);
    virtual ~ChessPiece();

    int Color;
    virtual void GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero) = 0;
    virtual bool CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum) = 0;
    virtual void Clear(Tablero* MainTablero);
    void Render(SDL_Renderer* render);
    SDL_Texture* LoadTexturePiece(SDL_Renderer* renderer, const std::string& filePath);
    SDL_Texture* Texture;

};

class Pawn : public ChessPiece
{
public:
    Pawn(SDL_Texture* texture, int color); 
    ~Pawn();

    void GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero) override;
    bool CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum) override;
    void RemarcarDiagonalCells(int rowOffset, int InitialRow, int InitialColumn, Tablero* MainTablero);
    void MovimientoInicial(int InitialRow, int InitialColum, Tablero* MainTablero);
};

class Rook : public ChessPiece
{
public:
    Rook(SDL_Texture* texture, int color);
    ~Rook();

    void GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero) override;
    bool CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum) override;
};

class Knight : public ChessPiece
{
public:
    Knight(SDL_Texture* texture, int color);
    ~Knight();

    void GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero) override;
    bool CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum) override;
};

class Bishop : public ChessPiece
{
public:
    Bishop(SDL_Texture* texture, int color);
    ~Bishop();

    void GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero) override;
    bool CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum) override;
};

class Queen : public ChessPiece
{
public:
    Queen(SDL_Texture* texture, int color);
    ~Queen();

    void GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero) override;
    bool CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum) override;
};

class King : public ChessPiece
{
public:
    King(SDL_Texture* texture, int color);
    ~King();

    void GetMoves(int InitialRow, int InitialColum, Tablero* MainTablero) override;
    bool CanMove(int InitialRow, int InitialColum, Tablero* MainTablero, int TmpLastRow, int TmpLastColum) override;
};