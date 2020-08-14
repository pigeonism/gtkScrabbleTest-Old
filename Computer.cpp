#include "Computer.h"
#include <iostream>
#include <Board.h>
#include <Hand.h>
#include <Combinations.h>
#include <algorithm>
#include <random>
#include "Scoring.h"

using namespace std;

Computer::Computer() {}
Computer::Computer( Board* board, Scoring *gameScoreC)
{
    gameScoreComputer = gameScoreC;
    COMPUTER_PLAYER_TAG = "computer";
    VERTICAL_ORIENTATION = 0;
    HORIZONTAL_ORIENTATION = 1;
    // The computer player shares the same board with every other player.
    gameBoardMapComp = board;
    goodMovesScore = 0;

    // For now, if a row block is bigger than 4 in size, this will make sure to reduce it to 4 (to increase speed).
    minDepth = true;
}

void Computer::printBlockHorizontal(string block)
{
    cout << "(Computer::printBlockHorizontal) COMPUTER: printing block" << endl;
    cout << block;
    cout << endl;
}

// Get coordinates from a string then place them into a vector containing a small integer vector of{x,y};
vector<vector<int>> Computer::getCoordinatesFromString(string coords)
{
    vector<vector<int>>fullCoordArray;
    // coords [7,0][7,1][7,2][7,3][7,4][7,5]
    // coords [7,0][7,1][7,2][7,3][7,4][7,5]
    for(int i = 0; i < coords.length(); i++)
    {
        if(coords[i] == '[')
        {
            //leftBracketAmount++;
            coords[i] = '#';
        }
    }
    std::replace( coords.begin(), coords.end(), ']', '#');
    coords = "#" + coords + "#";

    // Split the 'coords' string into tokens.
    std::string delimiter = "##";
    size_t pos = 0;
    std::string token;

    while ((pos = coords.find(delimiter)) != string::npos)
    {
        token = coords.substr(0, pos);
        if (token.length())
        {
            //std::cout << "dtoken"<<token << std::endl;
            int comma = token.find(",",1);
            string tempX = token.substr(0, comma);
            string tempY = token.substr(comma+1, -1);
            //cout << "COMPUTER: tokensub x: "<<tempX << " y: "<< tempY<<  endl;
            vector<int>tempSingleCoord{std::stoi( tempX ),std::stoi( tempY) };

            fullCoordArray.push_back(tempSingleCoord);
        }
        coords.erase(0, pos + delimiter.length());
    }
    return fullCoordArray;
}

// For every letter in a hand, use that letter alone in a 'checkValidMove' test with a coordinate.
void Computer::buildSingleMoves(vector<vector<int>>coordinates,string hand, int round)
{
    vector<vector<string>>computerMoves;
    vector<string> oneMove{"",""};
    string positionX;
    string positionY;
    string pos;
    string letter;

    for(int i = 0; i < hand.size(); i++ )
    {
        string letterOfHand = hand.substr(i,1);
        //cout << "letter " << letter<<endl;
        for(int j = 0; j < coordinates.size(); j++)
        {
            //vector<string> oneMove{"",""};
            positionX = to_string( coordinates[j][0] );         // {x, -}
            positionY = to_string( coordinates[j][1] );         // {-, y}
            pos = "["+  positionX + "," + positionY + "]";
            letter = letterOfHand;
            oneMove[0] = pos;
            oneMove[1] = letter;
            computerMoves.push_back(oneMove);
        }
        // If the move is valid retain the score temporarily to compare to the next score.
        if(gameBoardMapComp->checkValidMove(computerMoves, round, COMPUTER_PLAYER_TAG) == true)
        {
            int score = gameScoreComputer->getBoardScore(COMPUTER_PLAYER_TAG);

            if(score > goodMovesScore)
            {
                goodMoves = computerMoves;
                goodMovesScore = score;
            }
            gameScoreComputer->resetScore(COMPUTER_PLAYER_TAG);
            //cout << "COMPUTER: played a scoring hand: " << saveVector << endl;
        }
        computerMoves.clear();
    }
}

// For every letter combination of a computer hand, which may vary in size, test it with 'checkValidMove' by passing that function the moves vector.
// checkValidMove splits the vector into coordinates and letters for testing.
void Computer::buildMove(vector<vector<int>>coordinates, vector<string> comboVector, string hand, int round)
{
    //cout << "(Computer::buildMove) coordsize array: " << coordinates.size() << endl;
    vector<vector<string>>computerMoves;
    vector<string> oneMove{"",""};
    string positionX;
    string positionY;
    string pos;
    string letter;

    for(int i = 0; i < comboVector.size(); i++ )
    {

        for(int j = 0; j <comboVector[i].size(); j++)
        {
            //vector<string> oneMove{"",""};
            positionX = to_string( coordinates[j][0] ); // {x, -}
            positionY = to_string( coordinates[j][1] ); // {-, y}
            pos = "["+  positionX + "," + positionY + "]";
            letter = comboVector[i].substr(j,1);
            oneMove[0] = pos;
            oneMove[1] = letter;
            computerMoves.push_back(oneMove);
        }
        // If the move is valid retain the score temporarily to compare to the next score.
        if(gameBoardMapComp->checkValidMove(computerMoves, round, COMPUTER_PLAYER_TAG) == true)
        {
            int score = gameScoreComputer->getBoardScore(COMPUTER_PLAYER_TAG);

            if(score > goodMovesScore)
            {
                goodMoves = computerMoves;
                goodMovesScore = score;
            }
            gameScoreComputer->resetScore(COMPUTER_PLAYER_TAG);

            //cout << "COMPUTER: played a scoring hand: " << saveVector << endl;
        }
        computerMoves.clear();
    }
}

// To lower the search scope, check if a {row, col} position array is surrounded by letters, returning false if it is.
bool Computer::allLettersSurroundingTest(vector<vector<int>>coordinates, int whichTest)
{
    // It's possible to change which amount of CounterLetters to ignore to yield more results (and tests).
    if (whichTest == HORIZONTAL_ORIENTATION)
    {

        int countLettersAbove = 0;
        int countLettersBelow = 0;
        int rowXvalue = coordinates[0][0];

        if(rowXvalue >= 0 && rowXvalue <= 14)
        {
            // Check above and below of each part.
            for(int i = 0; i < coordinates.size(); i++)
            {
                if(rowXvalue != 0)
                {
                    if(gameBoardMapComp->getInfoFromSquare(coordinates[i][0] -1, coordinates[i][1] ) != "-")  // above
                    {
                        countLettersAbove++;
                    }
                }
                if(rowXvalue != 14)
                {
                    if(gameBoardMapComp->getInfoFromSquare(coordinates[i][0] +1, coordinates[i][1] ) != "-")  // below
                    {
                        countLettersBelow++;
                    }
                }
            }
            if(countLettersAbove > 1 || countLettersBelow > 1)
            {
                return true;
            }
        }
    }

    if (whichTest == VERTICAL_ORIENTATION)
    {
        // If the 'whichTest' parameter is vertical...
        int countLettersLeft = 0;
        int countLettersRight = 0;
        int rowYvalue = coordinates[0][1];

        if(rowYvalue >= 0 && rowYvalue <= 14)
        {
            for(int i = 0; i < coordinates.size(); i++)
            {
                if(rowYvalue != 0)
                {
                    if(gameBoardMapComp->getInfoFromSquare(coordinates[i][0], coordinates[i][1] -1 ) != "-")
                    {
                        countLettersLeft++;
                    }
                }
                if(rowYvalue != 14)
                {
                    if(gameBoardMapComp->getInfoFromSquare(coordinates[i][0], coordinates[i][1] +1) != "-")
                    {
                        countLettersRight++;
                    }
                }
            }

            if(countLettersLeft > 1 || countLettersRight > 1)
            {
                return true;
            }
        }
    }
    return false;
}

// This method decides which length of combinations to try.
bool Computer::moveSelector(vector<vector<int>>moveCoordinates,int howBig,Combinations combo, string hand, int round, int orientation)
{
    // Start by checking if the vector array surrounded by an array of letters.
    if(moveCoordinates.size() > 1)
    {
        if(allLettersSurroundingTest(moveCoordinates, orientation) == true)
        {
            return false;
        }
    }

    // The moveSelector should only pick one branch at a time after finding no score, rather than doing all combinations.
    if(howBig ==1)
    {
        buildSingleMoves(moveCoordinates,hand,round);
        // When the vector has been shrunk down to a size of 1, it is time to exit the moveSelector.
        return true;
    }

    if(howBig == 2)
    {
        buildMove(moveCoordinates, combo.pairsVector, hand, round);
        // if(goodMovesScore == 0)
        //{
        moveCoordinates.pop_back();
        moveSelector(moveCoordinates, moveCoordinates.size(), combo, hand, round,orientation);
        //}else {
        //    return true;
        //}
    }
    if(howBig == 3)
    {
        buildMove(moveCoordinates, combo.threeVector, hand, round);
        //if(goodMovesScore == 0)
        //{
        moveCoordinates.pop_back();
        moveSelector(moveCoordinates, moveCoordinates.size(), combo, hand, round,orientation);
        //}else {
        //    return true;
        //}
    }
    if(howBig == 4)
    {
        buildMove(moveCoordinates, combo.fourVector, hand, round);
        //if(goodMovesScore == 0)
        // {
        moveCoordinates.pop_back();
        moveSelector(moveCoordinates, moveCoordinates.size(), combo, hand, round,orientation);
        //}else {
        //    return true;
        //}
    }
    if(howBig == 5)
    {
        if(minDepth == false)
        {
            buildMove(moveCoordinates, combo.fiveVector, hand, round);
        }
        //if(goodMovesScore == 0)
        //{
        moveCoordinates.pop_back();
        moveSelector(moveCoordinates, moveCoordinates.size(), combo, hand, round,orientation);
        // }else {
        //    return true;
        // }
    }
    if(howBig == 6)
    {
        if(minDepth == false)
        {
            buildMove(moveCoordinates, combo.sixVector, hand, round);
        }
        //if(goodMovesScore == 0)
        //{
        moveCoordinates.pop_back();
        moveSelector(moveCoordinates, moveCoordinates.size(), combo, hand, round,orientation); // 5 will be called next
        //} else {
        //    return true;
        //}
    }
    if(howBig == 7)
    {
        if(minDepth == false)
        {
            buildMove(moveCoordinates, combo.sevenVector, hand, round);
        }
        // Only jump down to the next lowest combo length if no goodMovesScore (good word found) set.
        // 'goodMovesScore' is updated by 'buildMove' which aims for the best score.
        // if(goodMovesScore == 0)     // Even with mindepth set, 'goodMovesScore' should always be initially 0.
        // {
        moveCoordinates.pop_back();
        moveSelector(moveCoordinates, moveCoordinates.size(), combo, hand, round,orientation);
        //} else {
        //    return true;
        //}
    }
}

// Check moves here. Because the player moves first each game, there should be rows of empty blocks to check.
// TODO (euser#1#03/03/19): Reducing string operations would be ideal here. Even though the scope of a search
// is reduced by ignoring blank rows, the process of creating a vector of strings just to split a row into groups
// could be avoided if there was an alternative that split vectors of integer into a group. (a group is part of a row
// bounded by a letter filled by spaces). The same applies to getting vertical blocks.
vector<vector<string>> Computer::getEmptyHorizontalBlocks()
{
    // The possibleMovesProv vector contains vectors of strings that represent blocks of coordinates pointing to blank tiles.
    vector<vector<string>>possibleMovesHorizProv;
    string tempStr;
    string delimiter = "#";
    int hyphenCount = 0;
    for(int row = 0; row < gameBoardMapComp->getBoardVecSize(); row++)
    {
        for(int col = 0; col < gameBoardMapComp->getBoardVecSize(); col++)
        {

            if(gameBoardMapComp->getInfoFromSquare(row, col) == "-" )
            {
                // Build a potential vector that may be used for possibleMovesHoriz
                tempStr += "["+to_string(row) + "," + to_string(col) +"]";

                // Always count hyphens, so we can ignore a full row of them.
                hyphenCount++;
            }
            else
            {
                tempStr += delimiter;
            }
        }
        //cout << "Computer: Hyphencount " << hyphenCount << endl;
        if(hyphenCount != 15)
        {
            size_t pos = 0;
            std::string token;
            // tempvec
            vector<string>tempBlock;

            while ((pos = tempStr.find(delimiter)) != std::string::npos)
            {
                token = tempStr.substr(0, pos);
                if (token.length())
                {
                    //std::cout << "dtoken"<<token << std::endl;
                    tempBlock.push_back(token);
                }
                tempStr.erase(0, pos + delimiter.length());
            }
            //std::cout <<tempStr << std::endl;
            tempBlock.push_back(tempStr);
            // Now move tempblock which is an array of seperate rows into the base array.
            possibleMovesHorizProv.push_back(tempBlock);
        }
        tempStr.clear();
        hyphenCount = 0;
    }
    return possibleMovesHorizProv;
}
//
vector<vector<string>> Computer::getEmptyVerticalBlocks()
{
    // The possibleMovesVertical vector contains vectors of strings that represent blocks of coordinates pointing to blank tiles.
    vector<vector<string>>possibleMovesVertical;
    string tempStr;
    string delimiter = "#";
    int hyphenCount = 0;

    for(int row = 0; row < gameBoardMapComp->getBoardVecSize(); row++)
    {
        for(int col = 0; col < gameBoardMapComp->getBoardVecSize(); col++)
        {
            if(gameBoardMapComp->getInfoFromSquare(col, row) == "-" )
            {
                // Build a potential vector that may be used for possibleMovesVertical.
                tempStr += "["+to_string(col) + "," + to_string(row) +"]";
                hyphenCount++;
            }
            else
            {
                tempStr += delimiter;
            }
        }
        //cout << "Computer: Hyphencount " << hyphenCount << endl;
        if(hyphenCount != 15 && hyphenCount > 0)
        {
            //std::cout <<tempStr << std::endl;
            size_t pos = 0;
            std::string token;
            // tempvec
            vector<string>tempBlock;

            while ((pos = tempStr.find(delimiter)) != std::string::npos)
            {
                token = tempStr.substr(0, pos);
                if (token.length())
                {
                    //std::cout << "dtoken"<<token << std::endl;
                    tempBlock.push_back(token);
                }
                tempStr.erase(0, pos + delimiter.length());
            }
            //std::cout <<tempStr << std::endl;
            tempBlock.push_back(tempStr);
            // Now move tempblock which is an array of seperate rows into the base array.
            possibleMovesVertical.push_back(tempBlock);
        }
        tempStr.clear();
        hyphenCount = 0;
    }
    return possibleMovesVertical;
}

// Consider changing the vector to a vector of ints, then calling allLettersSurroundingTest, and then with
// the refined list call moveSelector, so it doesnt use allLettersSurroundingTest.
bool Computer::playComputerHand(int round, string hand)
{
    // Clear previous removal string.
    removeMe="";
    //goodMovesScore = 0;
    string compHand = hand;
    if(compHand.empty())return false; // Don't try evertything below on an empty string.

    cout <<"(Computer::playComputerHand) COMPUTER: playing hand..." << endl;

    vector<vector<string>>possibleMoves;
    int orientation;

    // Pick which direction to go, even when the search is faster it would still be preferable to do this for speed.
    std::random_device rdev;
    // Get either a 0 or a 1. This function can attain a higher range of numbers, but we only need two here.
    std::uniform_int_distribution<int> dist(0,1);
    std::mt19937 mt(rdev());
    int choice = dist(mt);

    // Remember a sub array here is a string, it looks like another array because of how it was formed.
    // The following condition will make decisions on which orientation to use with the moveSelector method.
    // At the first array in possibleMovesHoriz[i] , combinations should lean towards the max Y value
    // to avoid returning too many invalid moves from checkMove |-----x|word|----|word, <like where x is here
    // if there are two or more arrays in each , gravitate to the min Y of the last array in possibleMovesHoriz[i].
    cout << "(Computer::playComputerHand) COMPUTER: Testing moves." << endl;
    int pass = 0;

    Combinations combo;
    combo.setLetters(compHand);
    combo.buildAllCombinations();

    // Choose wether to try a word by placing letters vertically or horizontally.
    if(choice == 0)
    {
        possibleMoves = getEmptyVerticalBlocks();
        orientation = VERTICAL_ORIENTATION;
    }
    else
    {
        // Choice is 1.
        possibleMoves = getEmptyHorizontalBlocks();
        orientation = HORIZONTAL_ORIENTATION;
    }

    RETRY:
    for(int i = 0; i < possibleMoves.size(); i++)
    {
        for(int j = 0; j < possibleMoves[i].size(); j++)
        {
            // turn this block into an array of coordinates made of ints not strings.
            vector<vector<int>>moveCoordinates = getCoordinatesFromString(possibleMoves[i][j]);

            if ( moveCoordinates.size() == 1 )
            {
                buildSingleMoves(moveCoordinates,compHand,round);
            }
            else
            {
                // Note j is the index of each element within the array at possibleMovesHoriz[i].
                if(j == 0)
                {
                    if(moveCoordinates[0][1] == 0 && orientation == HORIZONTAL_ORIENTATION)  // Pick out y=0
                    {
                        // Reverse the first block so its last Y coordinate is at the beginning of the block if its first Y is 0.
                        // This is because checkValidMove will order coordinates and we want to build from the word to the right.
                        if(moveCoordinates.size() > 1) reverse(moveCoordinates.begin(), moveCoordinates.end());
                        moveSelector(moveCoordinates, moveCoordinates.size(), combo, compHand, round, orientation);

                    }
                    else if(moveCoordinates[0][0] == 0 && orientation == VERTICAL_ORIENTATION)
                    {
                        // If X is 0 that's the top row, so reverse the vector, to make the builder always
                        // buid words from the highest X (highest row) towards the lowest row (x = 0 being lowest).
                        if(moveCoordinates.size() > 1) reverse(moveCoordinates.begin(), moveCoordinates.end());
                        moveSelector(moveCoordinates, moveCoordinates.size(), combo, compHand, round, orientation);
                    }
                    else
                    {
                        // build normally, we're somewhere inside the board.
                        moveSelector(moveCoordinates, moveCoordinates.size(), combo, compHand, round, orientation);
                    }
                }
                else
                {
                    moveSelector(moveCoordinates, moveCoordinates.size(), combo, compHand, round,orientation);
                }
            }
        }
    }
    // Testing a retry with a different orientation, integer pass should be 1 after trying one method (horizontal or vert).
    // Note the 'moveSelector' method has changed so that it stops when a high score of vector of n len is found.
    // So if it does find a high score it wont check with a different orientation unless it only gets a score of 0.
    pass++;
    possibleMoves.clear();
    if(pass == 1 && goodMovesScore == 0)
    {
        if(orientation == HORIZONTAL_ORIENTATION)
        {
            possibleMoves = getEmptyVerticalBlocks();
            orientation = VERTICAL_ORIENTATION;
        }
        else
        {
            // choice is 1
            possibleMoves = getEmptyHorizontalBlocks();
            orientation = HORIZONTAL_ORIENTATION;
        }
        // TODO (euser#4#03/06/19): check if there's another way to do what this bit does.
        // Now move just past where integer 'pass' is initially zero, pass should be 1 here, but after a second try
        // 'pass' will be 2 and so this whole block will be skipped over. I am using goto because I don't want to
        // rebuild combinations of letters or add extra class variables to set and reset.
        goto RETRY;
    }

    // TODO (euser#3#03/08/19): condense these for loops into smaller ones after tests.
    cout << "(Computer::playComputerHand) COMPUTER: Score: " << goodMovesScore << endl;
    cout << "(Computer::playComputerHand) COMPUTER: It took me '" << pass << "' attempts..." << endl;
    //cout <<"(Computer::playComputerHand) COMPUTER: COMBO... " <<  << endl;
    // gameBoardMapComp->updateBoard(row[0], row[1], "add");
    if(goodMovesScore > 0 && goodMoves.size() > 0 )
    {
        for(vector<string> row: goodMoves)
            gameBoardMapComp->updateBoard(row[0], row[1], "add");

        cout << "(Computer::playComputerHand) COMPUTER: setting score... " <<  endl;
        // Set score.
        {
            // Main will handle retrieving the score again.buildMove resets score. Although board will have bonused it
            gameScoreComputer->setBoardScore(goodMovesScore, COMPUTER_PLAYER_TAG);
            gameScoreComputer->updateLatestCompBonusChanges(); //Last use of computer bonus tag edits copied to the original
            goodMovesScore = 0;
        }

        // Remove the letters played from a hand (row[1] is the letter).
        cout << "(Computer::playComputerHand) COMPUTER: saving used letters... " <<  endl;
        string removeThese="";
        for(vector<string> row: goodMoves)
            removeMe += row[1];

        // Set the labels for the board buttons.
        for(vector<string> row: goodMoves)
            gameBoardMapComp->setLabelSelective(row[0], row[1]);

        // Change the colour of a button one at a time. It might be useful to do it this way when animating a placement of a tile.
        for(vector<string> row: goodMoves)
            gameBoardMapComp->changeColourComputer(row[0]);


        // Print the hand for testing.
        for(vector<string> row: goodMoves)
            cout << "(Computer::playComputerHand) COMPUTER: final hand, coord: " << row[0] << "letter: "<< row[1] << endl;

        goodMoves.clear();
        // Return true if all went well, leave goodMoves for main, so main can set labels for scores then clear it.
        return true;
    }

    return false;
}
Computer::~Computer() {}
