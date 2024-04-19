#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Tile.cpp"
#include "Counter.cpp"
using namespace std;

class Board{

private:
int width, height, numMine, flaggedMines;
vector<vector<Tile>> tiles;
bool gameover, win;

public:
    Board(int width, int height, int numMine) : width(width), height(height), numMine(numMine) {
        tiles.resize(height, vector<Tile>(width));
        flaggedMines = numMine;
        game_over(false);
        placeMines();
        winner(false);
        setAllTilesHidden();
        start();

    }

    void start(){
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                tiles[i][j].LoadTexture(); // Load texture for each tile
            }
        }
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        int nx = i + dx;
                        int ny = j + dy;
                        if (nx >= 0 && nx < height && ny >= 0 && ny < width) {
                            tiles[i][j].addNeighbor(&tiles[nx][ny]);
                        }
                    }
                }
                tiles[i][j].numNeighborMines(); // Calculate the number of neighboring mines

            }
        }
    }
    void setAllTilesHidden() {
        int pp = 0;
        for (auto& row : tiles) {
            for (auto& tile : row) {
                tile.setIsRevealed(false);
                pp++;
            }
        }
    }

    void removeAll(){
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                tiles[i][j].setHasMine(false);
                tiles[i][j].clearNeighbors();
                tiles[i][j].setIsFlagged(false);
                tiles[i][j].LoadTexture(); // Load texture for each tile
            }
        }
    }
    void placeMines() {
        srand(time(nullptr));
        int placedMine = 0;
        while (placedMine < numMine) {
            int y = rand() % width;
            int x = rand() % height;
            if (!tiles[x][y].getHasMine() ) {
                tiles[x][y].setHasMine(true);
                placedMine++;
            }
        }
    }

    void setFlag(int x, int y,Counter& counter)
    {

        Tile& tile = tiles[x][y];
        if (tile.getIsFlagged()){
            tile.setIsFlagged(false);

            counter.increase();
            }

        else {
                tile.setIsFlagged(true);
            counter.decrease();
        }
        tile.updateSprite();


    }

    void revealTile(int x, int y) {
        if (x < 0 || x >= height || y < 0 || y >= width) {
            return;
        }

        Tile& tile = tiles[x][y];
        if (tile.getIsRevealed() || tile.getIsFlagged()) {
            return;
        }

        if (tile.getHasMine()) {
            game_over(true);
            winner(false);
            revealAllMines();
            return;
        }


        if (tile.numNeighborMines() > 0) {
            tile.setIsRevealed(true);
            tile.updateSprite();
            checkwin();

            return;
        }
        revealSurrounding(x, y);
        checkwin();


    }


    void checkwin(){

        int pp = 0;

        for (auto& row : tiles) {
            for (auto& tile : row) {
                if(tile.getIsRevealed()) {
                    pp++;
                }

            }
        }
        pp = height*width-pp;
        if( pp == numMine){
            for (auto& row : tiles) {
                for (auto& tile : row) {
                    if(tile.getHasMine()) {
                        tile.setIsFlagged(true);
                        tile.updateSprite();

                    }

                }
            }

            winner(true);
            game_over(true);
        }


    }
    void revealSurrounding(int x, int y) {
        Tile& currentTile = tiles[x][y];

        currentTile.setIsRevealed(true);
        currentTile.updateSprite();

        for (int i = max(0, x - 1); i <= min(height - 1, x + 1); ++i) {
            for (int j = max(0, y - 1); j <= min(width - 1, y + 1); ++j) {
                if (!tiles[i][j].getIsRevealed()) {
                    revealTile(i, j);
                }
            }
        }
    }




    void revealAllMines() {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (tiles[i][j].getHasMine()) {
                    tiles[i][j].setIsRevealed(true);
                    tiles[i][j].updateSprite();
                }
            }
        }
    }

    bool game_over(bool input){
        gameover = input;
        return gameover;
    }

    bool Isgame_over(){
        return gameover;
    }

    bool winner(bool input){
        win = input;
        return win;
    }

    bool isWin(){
        return win;
    }

    void draws(sf::RenderWindow& window)
    {
        int tileSize = 32;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                tiles[i][j].updateSprite();
                tiles[i][j].setPosition(j * tileSize, i * tileSize);
                tiles[i][j].draw(window);

            }
        }
    }

};