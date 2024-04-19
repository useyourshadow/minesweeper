#include <SFML/Graphics.hpp>
#pragma once
#include <Vector>
#include <iostream>
using namespace std;

class Tile{
private:
    bool isHidden, isFlagged, hasMine, isRevealed;
    int  x,y;
    sf::Sprite sprite;
    sf::Texture hidden, revealed, flagged, mine, one, two, three, four, five, six, seven, eight;
    std::vector<Tile*> neighbors;



public:
    bool getIsRevealed() const {
        return isRevealed;
    }

    void setIsRevealed(bool revealed) {
        isRevealed = revealed;
    }

    bool getHasMine() const {
        return hasMine;
    }

    void setHasMine(bool mine) {
        hasMine = mine;
    }

    bool getIsFlagged() const {
        return isFlagged;
    }

    void setIsFlagged(bool flagged) {
        isFlagged = flagged;
    }

    void clearNeighbors() {
        neighbors.clear();
    }

    std::vector<Tile*>& getNeighbors() {
        return neighbors;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    void LoadTexture()
    {
        hidden.loadFromFile("./images/tile_hidden.png");
        revealed.loadFromFile("./images/tile_revealed.png");
        flagged.loadFromFile("./images/flag.png");
        mine.loadFromFile("./images/mine.png");
        one.loadFromFile("./images/number_1.png");
        two.loadFromFile("./images/number_2.png");
        three.loadFromFile("./images/number_3.png");
        four.loadFromFile("./images/number_4.png");
        five.loadFromFile("./images/number_5.png");
        six.loadFromFile("./images/number_6.png");
        seven.loadFromFile("./images/number_7.png");
        eight.loadFromFile("./images/number_8.png");

    }



    void addNeighbor(Tile* neighbor) {
        neighbors.push_back(neighbor);
    }

    int numNeighborMines() const {
        int count = 0;
        for (Tile* neighbor : neighbors) {
            if (neighbor->getHasMine()) {
                count++;
            }
        }
        return count;
    }


    void updateSprite() {
        if (isFlagged) {
            sprite.setTexture(flagged);
        } else if (isRevealed) {
            if (hasMine) {
                sprite.setTexture(mine);
            }

            else if (numNeighborMines() == 1){
                sprite.setTexture(one);
            }

            else if (numNeighborMines() == 2){
                sprite.setTexture(two);
            }

            else if (numNeighborMines() == 3){
                sprite.setTexture(three);
            }

            else if (numNeighborMines() == 4){
                sprite.setTexture(four);
            }

            else if (numNeighborMines() == 5){
                sprite.setTexture(five);
            }

            else if (numNeighborMines() == 6){
                sprite.setTexture(six);
            }

            else if (numNeighborMines() == 7){
                sprite.setTexture(seven);
            }
            else if (numNeighborMines() == 8){
                sprite.setTexture(eight);
            }

            else {
                sprite.setTexture(revealed);
            }
        }
        else {
            sprite.setTexture(hidden);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }


    void setPosition(float x, float y)
    {
        sprite.setPosition(x,y);
    }

};