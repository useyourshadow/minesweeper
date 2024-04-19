#include <iostream>
#include <SFML/Graphics.hpp>
#include "Board.cpp"
#include "Timer.cpp"
#include "Tile.cpp"
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "Counter.cpp"
#include <chrono>
#include <utility>


using namespace std;



void updateClockDigits(std::vector<sf::Sprite>& digitSprites, const sf::Texture& digitsTexture, int timeValue, int columns, int rows) {
    int minuteTens = (timeValue / 60) / 10;
    int minuteUnits = (timeValue / 60) % 10;
    int secondTens = (timeValue % 60) / 10;
    int secondUnits = timeValue % 10;

    // Calculate positions for timer digits
    int timerMinutePosX = columns * 32 - 97;
    int timerSecondPosX = columns * 32 - 54;
    int timerPosY = rows * 32 + 16;

    // Set texture rects and positions for minute digits
    digitSprites[0].setTextureRect(sf::IntRect(minuteTens * 21, 0, 21, 32));
    digitSprites[0].setPosition(timerMinutePosX, timerPosY + 17);
    digitSprites[1].setTextureRect(sf::IntRect(minuteUnits * 21, 0, 21, 32));
    digitSprites[1].setPosition(timerMinutePosX + 21, timerPosY+ 17);

    // Set texture rects and positions for second digits
    digitSprites[2].setTextureRect(sf::IntRect(secondTens * 21, 0, 21, 32));
    digitSprites[2].setPosition(timerSecondPosX, timerPosY+ 17);
    digitSprites[3].setTextureRect(sf::IntRect(secondUnits * 21, 0, 21, 32));
    digitSprites[3].setPosition(timerSecondPosX + 21, timerPosY+ 17);
}




void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void setFont(sf::Text &text, sf::Font &font, int characterSize, sf::Color color, sf::String content, float x, float y) {
    text.setFont(font);
    text.setCharacterSize(characterSize);
    text.setFillColor(color);
    text.setString(content);
    setText(text, x, y);
}


vector<string> ReadDataFile(){
    string forData;
    vector<string> data;
    ifstream file("./config.cfg");
    if (file.is_open()){
        while(file>> forData){
            data.push_back(forData);
        }
    }

    return data;
}

std::vector<std::pair<std::string, std::string>> readLeaderboard(const std::string& filename) {
    std::vector<std::pair<std::string, std::string>> entries;
    std::ifstream file(filename);
    std::string line;
    while (getline(file, line)) {
        auto commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            std::string time = line.substr(0, commaPos);
            std::string name = line.substr(commaPos + 1);
            // Convert time from "mm:ss" to seconds
            std::istringstream iss(time);
            int minutes, seconds;
            char colon;
            if (iss >> minutes >> colon >> seconds) {
                int totalSeconds = minutes * 60 + seconds;
                // Format the total seconds back to "mm:ss"
                std::ostringstream oss;
                oss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
                entries.push_back({oss.str(), name});
            } else {
                // Handle invalid time format
                std::cerr << "Invalid time format: " << time << std::endl;
            }
        }
    }
    file.close();
    return entries;
}

void displayLeaderboard(sf::RenderWindow& window, const std::vector<std::pair<std::string, std::string>>& leaderboard) {
    sf::Font lf;
    if (!lf.loadFromFile("./font.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return; // Exit the function if the font fails to load
    }

    window.clear(sf::Color::Blue); // Clear the window with a blue background

    // Title Text Configuration
    sf::Text title("LEADERBOARD", lf, 20); // Create title text with size 20 pixels
    title.setStyle(sf::Text::Bold | sf::Text::Underlined); // Set style to bold and underlined
    title.setFillColor(sf::Color::White); // Set color to white

    // Calculate the position of the title
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    title.setPosition(window.getSize().x / 2.0f, 50.0f);

    window.draw(title); // Draw the title to the window

    // Leaderboard Entries Configuration
    float startY = 100.0f; // Starting Y position for names
    float startX = 100.0f; // Start X to position ranks and times
    int rank = 1;

    // Draw each entry
    for (const auto& entry : leaderboard) {
        std::stringstream ss;
        ss <<rank<< ". "<< entry.first << " " <<  entry.second;

        sf::Text text(ss.str(), lf, 18); // Create text with size 18 pixels
        text.setStyle(sf::Text::Bold); // Set style to bold
        text.setFillColor(sf::Color::White); // Set color to white

        // Calculate position of each text
        text.setPosition(startX, startY);

        window.draw(text); // Draw the text to the window

        startY += 25.0f; // Move down for the next entry
        rank++; // Increment rank
    }

    window.display(); // Display everything drawn to the window
}


bool updateLeaderboard(const std::string& filename, const std::string& playerName, const std::string& playerTime) {
    std::vector<std::pair<std::string, std::string>> leaderboard;
    std::ifstream readFile(filename);
    if (!readFile.is_open()) {
        std::cerr << "Unable to open file for reading." << std::endl;
        return false;
    }

    // Read data from the file
    std::string line;
    while (std::getline(readFile, line)) {
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            std::string timeAndName = line.substr(0, commaPos);
            std::string name = line.substr(commaPos + 1);

            leaderboard.emplace_back(timeAndName, name);
        }
    }
    readFile.close();

    // Convert playerTime from "mm:ss" to string
    std::string playerTimeString = playerTime;
    std::istringstream timeStream(playerTime);
    int minutes, seconds;
    char colon;
    if (timeStream >> minutes >> colon >> seconds) {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
        playerTimeString = oss.str();
    } else {
        cout << playerTimeString;

    }
    // Find or add player's entry
    bool playerNameFound = false;
    for (auto& entry : leaderboard) {
        if (entry.second == playerName) {
            playerNameFound = true;
            // Compare times directly as strings
            if (playerTimeString < entry.first) {
                entry.first = playerTimeString; // Update time if player's time is better
                cout << entry.first;
            }

        }
    }

    if (!playerNameFound) { // If player's name not found, add new entry
        leaderboard.push_back({playerTimeString, playerName});
    }

    // Write updated leaderboard to file
    std::ofstream writeFile(filename);
    if (!writeFile.is_open()) {
        std::cerr << "Unable to open file for writing." << std::endl;
        return false;
    }

    for (const auto& entry : leaderboard) {
        writeFile << entry.first << ":"<< entry.first << "," << entry.second << "\n";
    }

    writeFile.close();
    return true;
}

    void removeStarsFromLeaderboard(const std::string& filename) {
    // Read the existing leaderboard entries
    std::ifstream readFile(filename);
    if (!readFile.is_open()) {
        std::cerr << "Unable to open leaderboard file for reading." << std::endl;
        return;
    }

    std::vector<std::pair<std::string, std::string>> leaderboardEntries;
    std::string line;
    while (std::getline(readFile, line)) {
        std::string time, name;
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            time = line.substr(0, commaPos);
            name = line.substr(commaPos + 1);
            // Remove asterisks from the name
            name.erase(std::remove(name.begin(), name.end(), '*'), name.end());
            leaderboardEntries.emplace_back(time, name);
        }
    }
    readFile.close();

    // Write the updated leaderboard back to the file
    std::ofstream writeFile(filename);
    if (!writeFile.is_open()) {
        std::cerr << "Unable to open leaderboard file for writing." << std::endl;
        return;
    }

    for (const auto& entry : leaderboardEntries) {
        writeFile << entry.first << "," << entry.second << "\n";
    }
    writeFile.close();
}





int main() {

    bool leaderboardWindowOpen = false;
    //Reading Data
    vector <string> data;
    data = ReadDataFile();
    float width = stof(data[0]);
    float height = stof(data[1]);
    float numMines = stof(data[2]);

    //Setting up Font
    sf::Text Welcome, Enter, playerText, reader;

    sf::Font font;
    font.loadFromFile("./font.ttf");
    //Adding welcome text
    setFont(Welcome, font, 24, sf::Color::White, "WELCOME TO MINESWEEPER", width*32/2, height*32/2-150);
    setFont(Enter, font, 20, sf::Color::White, "Enter your name:", width*32/2, height*32/2-75);
    setFont(playerText, font, 18, sf::Color::White, "", width*32/2, height*32/2-45);
    setFont(reader, font, 20, sf::Color::White, "|", width*32/2+2, height*32/2-45);
    sf::String playerInput;

    //Window
    sf::RenderWindow window;
    window.create(sf::VideoMode((width*32), (height*32)+100), "Welcome");

    setText(playerText,width*32/2,height*32/2-45);
    setText(reader,width*32/2+2,height*32/2-45);
    bool gameWon = false;
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {

           setText(playerText,width*32/2,height*32/2-45);
            setText(reader,width*32/2+2,height*32/2-45);



            if (event.type == sf::Event::TextEntered) {

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                    if (playerInput.getSize() >= 1) {
                        window.close(); // Close the main window

                        bool showLeader = false;
                        // Open the game window
                        sf::RenderWindow Game, Leader;
                        Game.create(sf::VideoMode((width * 32), (height * 32) + 100), "Game");
                        Board board(width,height,numMines);

                        //Creating Buttons and setting sprites
                        sf::Sprite rest,wi,los,dig,debu,paus,ply,ldb;
                        sf::Texture restart,win,lose,digits,debug,pause,play,leaderboard;
                        restart.loadFromFile("./images/face_happy.png");
                        win.loadFromFile("./images/face_win.png");
                        lose.loadFromFile("./images/face_lose.png");
                        digits.loadFromFile("./images/digits.png");
                        debug.loadFromFile("./images/debug.png");
                        pause.loadFromFile("./images/pause.png");
                        play.loadFromFile("./images/play.png");

                        leaderboard.loadFromFile("./images/leaderboard.png");
                        rest.setTexture(restart);
                        wi.setTexture(win);
                        los.setTexture(lose);
                        dig.setTexture(digits);
                        debu.setTexture(debug);
                        paus.setTexture(pause);
                        ply.setTexture(play);
                        ldb.setTexture(leaderboard);
                        rest.setPosition(width/2*32-32,32*(height+.5));
                        wi.setPosition(width/2*32-32,32*(height+.5));
                        los.setPosition(width/2*32-32,32*(height+.5));
                        debu.setPosition(width/2*32+100,32*(height+.5));
                        paus.setPosition(width/2*32+100+ 64,32*(height+.5));
                        ply.setPosition(width/2*32+100+ 64,32*(height+.5));
                        bool leftButtonDebounced = false;
                        bool rightButtonDebounced = false;
                        auto lastLeftClickTime = std::chrono::high_resolution_clock::now();
                        auto lastRightClickTime = std::chrono::high_resolution_clock::now();
                        const int debounceDuration = 200;  // debounce duration in milliseconds
                        ldb.setPosition(width/2*32+100 + 64*2    ,32*(height+.5));
                        std::chrono::time_point<std::chrono::high_resolution_clock> pausedTime; // Store the time when the game is paused

                        std::vector<sf::Sprite> digitSprites;
                        for (int i = 0; i <= 9; ++i) {
                            sf::Sprite digitSprite(digits);
                            digitSprite.setTextureRect(sf::IntRect(i * 21, 0, 21, 32)); // Set texture rect for current digit
                            digitSprites.push_back(digitSprite);
                        }

                        sf::Sprite minusSprite(digits);
                        minusSprite.setTextureRect(sf::IntRect(10 * 21, 0, 21, 32));


                        bool timerPaused = false;
                        auto startTime = std::chrono::high_resolution_clock::now();
                        Counter counter(numMines, height);
                        std::vector<std::pair<std::string, std::string>> leaderboardz = readLeaderboard("leaderboard.txt");

                        bool gamepaused = false;
                        // Game loop
                        while (Game.isOpen()) {

                            sf::Event gameEvent;
                            while (Game.pollEvent(gameEvent)) {
                                if (gameEvent.type == sf::Event::Closed) {
                                    Game.close();
                                }
                            }

                            if (board.isWin() && !gameWon) {
                                std::string playerName = playerInput;
                                auto currentTime = std::chrono::high_resolution_clock::now();
                                auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
                                int timeValue = duration.count();
                                std::string playerTime = std::to_string(timeValue);

                                // Update the leaderboard
                                bool inserted = updateLeaderboard("./leaderboard.txt", playerName, playerTime);
                                // Set the flag to true to indicate that the block has been executed
                                gameWon = true;

                                leaderboardWindowOpen = true;
                                Leader.create(sf::VideoMode(width * 16, height * 16 + 50),
                                              "Leaderboard");
                                std::vector<std::pair<std::string, std::string>> leaderboard = readLeaderboard(
                                        "leaderboard.txt");
                                displayLeaderboard(Leader, leaderboard);
                            }

                            if (!leaderboardWindowOpen) {
                                if (gameEvent.type == sf::Event::MouseButtonPressed) {
                                    auto now = std::chrono::high_resolution_clock::now();

                                    sf::Vector2i pixelPos = sf::Mouse::getPosition(Game);
                                    sf::Vector2f worldPos = Game.mapPixelToCoords(pixelPos);

                                    float tileSize = 32.0f;
                                    int xIndex = static_cast<int>(worldPos.x / tileSize);
                                    int yIndex = static_cast<int>(worldPos.y / tileSize);

                                    if (gameEvent.mouseButton.button == sf::Mouse::Left) {

                                        sf::Vector2i mousePos = sf::Mouse::getPosition(Game);
                                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLeftClickTime).count();
                                        if (!leftButtonDebounced || duration > debounceDuration) {
                                            leftButtonDebounced = true;
                                            lastLeftClickTime = now;
                                            if (paus.getGlobalBounds().contains(static_cast<float>(mousePos.x),
                                                                                static_cast<float>(mousePos.y))) {
                                                if (!gamepaused) {
                                                    // Pause the timer

                                                    timerPaused = true;
                                                    gamepaused = true;
                                                    pausedTime = std::chrono::high_resolution_clock::now(); // Store the current time when pausing
                                                } else {
                                                    // Unpause the timer

                                                    timerPaused = false;
                                                    gamepaused = false;
                                                    // Calculate the duration of the pause
                                                    auto now = std::chrono::high_resolution_clock::now();
                                                    auto pauseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
                                                            now - pausedTime);
                                                    // Adjust the start time to subtract the duration of the pause
                                                    startTime += pauseDuration;
                                                }


                                            }

                                            if (debu.getGlobalBounds().contains(static_cast<float>(mousePos.x),
                                                                                static_cast<float>(mousePos.y))) {
                                                board.revealAllMines();

                                            } else if (rest.getGlobalBounds().contains(
                                                    static_cast<float>(mousePos.x),
                                                    static_cast<float>(mousePos.y))) {
                                                board.setAllTilesHidden();
                                                board.removeAll();
                                                board.placeMines();
                                                board.winner(false);
                                                gameWon = false;
                                                board.game_over(false);
                                                counter.setCount(numMines);
                                                startTime = std::chrono::high_resolution_clock::now();

                                                board.start();
                                                timerPaused = false;
                                                gamepaused = false;

                                            } else if (ldb.getGlobalBounds().contains(
                                                    static_cast<float>(mousePos.x),
                                                    static_cast<float>(mousePos.y))) {
                                                leaderboardWindowOpen = true;
                                                Leader.create(sf::VideoMode(width * 16, height * 16 + 50),
                                                              "Leaderboard");
                                                std::vector<std::pair<std::string, std::string>> leaderboard = readLeaderboard(
                                                        "leaderboard.txt");
                                                displayLeaderboard(Leader, leaderboard);

                                            } else if (xIndex >= 0 && xIndex < width && yIndex >= 0 &&
                                                       yIndex < height) {
                                                if (!board.Isgame_over()) {
                                                    if (!gamepaused) {
                                                        board.revealTile(yIndex, xIndex);

                                                    }

                                                }
                                            }
                                        }

                                    }

                                    if (!gamepaused) {

                                        if (gameEvent.mouseButton.button == sf::Mouse::Right) {
                                            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRightClickTime).count();
                                            if (!rightButtonDebounced || duration > debounceDuration) {
                                                rightButtonDebounced = true;
                                                lastRightClickTime = now;
                                            if (!board.Isgame_over()) {
                                                if (xIndex >= 0 && xIndex < width && yIndex >= 0 && yIndex < height) {
                                                    board.setFlag(yIndex, xIndex, counter);

                                                }
                                            }
                                            }
                                        }
                                    }

                                }
                            }




                            if (Leader.isOpen()) {
                                sf::Event leaderboardEvent;
                                while (Leader.pollEvent(leaderboardEvent)) {
                                    if (leaderboardEvent.type == sf::Event::Closed) {
                                        leaderboardWindowOpen = false;
//                                        removeStarsFromLeaderboard("./leaderboard.txt");
                                        Leader.close();

                                    }
                                }

                                Leader.clear();

                                // Update the leaderboard
                                leaderboardz = readLeaderboard("leaderboard.txt");
                                displayLeaderboard(Leader, leaderboardz);
                            }
                            if (!timerPaused) {
                                // Update the timer only if it's not paused
                                auto currentTime = std::chrono::high_resolution_clock::now();
                                auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
                                int timeValue = duration.count();
                                updateClockDigits(digitSprites, digits, timeValue, width, height);
                            }
                            Game.clear(sf::Color::White);

                            for (int i = 0; i < digitSprites.size(); ++i) {
                                Game.draw(digitSprites[i]);
                            }
                            if(board.Isgame_over() && !board.isWin()){
                                los.setPosition(width/2*32-32,32*(height+.5));
                                Game.draw(los);
                                timerPaused = true;
                            }

                            else if(board.isWin()){

                                wi.setPosition(width/2*32-32,32*(height+.5));
                                Game.draw(wi);
                                timerPaused = true;


                            }
                            else{
                                Game.draw(rest);

                            }

                            if (!gamepaused) {
                                Game.draw(paus);
                            }
                            else{

                                Game.draw(ply);
                            }
                            Game.draw(ldb);
                            Game.draw(debu);

                            board.draws(Game);
                            counter.draw(Game);
                            Game.display();



                        }
                    }
                }


                //For backspace
                if (event.text.unicode == 8) {
                    if (!playerInput.isEmpty()) {
                        playerInput = playerInput.substring(0, playerInput.getSize() - 1);
                    }
                }
                //Adding player input
                else if (event.text.unicode < 128) {
                    if (playerInput.getSize() <= 9) {
                        if (isalpha(event.text.unicode)) {
                            playerInput += event.text.unicode;

                        }
                    }
                }

                //To capitalize first element
                if(playerInput.getSize()==1)
                {
                    playerInput[0] = toupper(playerInput[0]);
                }
                if(playerInput.getSize()>1)
                {
                    for (int j = 1; j < playerInput.getSize(); j++)
                    {
                        playerInput[j] = tolower(playerInput[j]);

                    }
                }
                playerText.setString(playerInput);
                setText(playerText,width*32/2,height*32/2-45);
            }


            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }



        // Makes the "|" follow the texts
        window.clear(sf::Color::Blue);

        sf::Vector2f textPos = playerText.findCharacterPos(playerInput.getSize());
        reader.setPosition(textPos.x, height*32/2-45);

        window.draw(reader);
        window.draw(playerText);
        window.draw(Welcome);
        window.draw(Enter);
        window.display();
    }


    return 0;
}