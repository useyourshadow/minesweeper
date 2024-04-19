#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#pragma once
class Counter {
private:
    int count;
    std::vector<sf::Sprite> digitSprites;
    sf::Texture digitsTexture;
    int posY;

    // Function to update the digit sprites based on the count
    void updateDigitSprites() {
        int tempCount = count;

        // Clear existing digit sprites
        digitSprites.clear();

        // Handle negative count
        bool isNegative = false;
        if (tempCount < 0) {
            isNegative = true;
            tempCount = -tempCount;
        }

        // Create digit sprites
        do {
            int digit = tempCount % 10; // Extract last digit
            sf::Sprite digitSprite(digitsTexture);
            digitSprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32)); // Set texture rect for current digit
            digitSprites.push_back(digitSprite);
            tempCount /= 10; // Remove last digit
        } while (tempCount > 0);

        // Add '-' sprite if count is negative
        if (isNegative) {
            sf::Sprite minusSprite(digitsTexture);
            minusSprite.setTextureRect(sf::IntRect(10 * 21, 0, 21, 32)); // '-' sprite
            digitSprites.push_back(minusSprite);
        }

        // Reverse the digit sprites vector to position them correctly
        std::reverse(digitSprites.begin(), digitSprites.end());

        // Set positions for digit sprites
        int numDigits = digitSprites.size();
        int digitWidth = 21; // Width of each digit sprite
        int totalWidth = numDigits * digitWidth; // Total width occupied by digits
        int posX = 12; // Starting position for digits

        // Adjust starting position for negative single-digit numbers
        if (isNegative && numDigits == 1) {
            posX = 33; // Move the starting position to the right
        }

        // Position digits correctly
        for (int i = 0; i < numDigits; ++i) {
            digitSprites[i].setPosition(posX, (posY + 0.5) * 32 + 16); // Adjust position for each digit
            posX += digitWidth; // Move to the right for the next digit
        }
    }
public:
    // Constructor
    Counter(int count, int y) : count(count), posY(y) {
        // Load digits texture
        if (!digitsTexture.loadFromFile("./images/digits.png")) {
            std::cerr << "Failed to load digits texture!" << std::endl;
            return;
        }

        // Update digit sprites initially
        updateDigitSprites();
    }

    void setText(sf::Text &text, float x, float y){
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }

    // Function to increase the counter
    void increase() {
        count++;
        updateDigitSprites();
    }

    // Function to decrease the counter
    void decrease() {
        count--;
        updateDigitSprites();
    }

    // Function to get the current count
    int getCount() const {
        return count;
    }

    void setCount(int m)  {
        this->count = m;
        updateDigitSprites();

    }

    // Function to draw the counter digits
    void draw(sf::RenderWindow& window) {
        for (int i = 0; i < digitSprites.size(); ++i) {
            window.draw(digitSprites[i]);
        }
    }
};