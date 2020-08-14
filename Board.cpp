#include <gtk/gtk.h>
#include "Board.h"
#include <Dictionary.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <Hand.h>
#include <Formatter.h>
#include "Scoring.h"

using namespace std;

Board::Board(){}
Board::Board(Scoring* gameScoreB)
{
    gameScoreBoard = gameScoreB;

    dict.buildRefinedDictionaryList();
    alphas = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    // Letters with their score value.
    lettersAlphaValue = {"A1","B3","C3","D2","E1","F4","G2","H4","I1","J8","K5","L1","M3","N1","O1","P3","Q10","R1","S1","T1","U1","V4","W4","X8","Y4","Z10"," 0"};
    // Letter multiplier positions.
    bonusPositionsRedTripleWord = {"[0,0]","[0,7]","[0,14]","[7,0]","[7,14]","[14,0]","[14,7]","[14,14]"};
    bonusPositionsPinkDoubleWord = {"[7,7]","[1,1]", "[2,2]","[3,3]","[4,4]","[1,13]","[2,12]","[3,11]","[4,10]","[13,1]","[12,2]","[11,3]","[10,4]","[13,13]","[12,12]","[11,11]","[10,10]"};
    bonusPositionsLightBlueDoubleLetter = {"[6,6]","[6,8]","[8,6]","[8,8]","[2,6]","[2,8]","[3,7]","[6,12]","[8,12]","[7,11]","[11,7]","[12,6]","[12,8]","[6,2]","[8,2]","[7,3]","[0,3]","[0,11]","[3,0]","[3,14]","[11,0]","[11,14]","[14,3]","[14,11]"};
    bonusPositionsDarkBlueTripleLetter = {"[1,5]","[1,9]","[5,13]","[9,13]","[13,5]","[13,9]","[5,1]","[9,1]","[5,5]","[5,9]","[9,5]","[9,9]"};
    // The default string returned if no letter is found in a playerMove vector at the position given (for Board::getLetterFromPlayerVec).
    EMPTY_SQUARE = "no-letter";
    // Tags used to decide who to apply scoring to. These should be the same throughout the game.
    PLAYER_TAG = "user";
    COMPUTER_PLAYER_TAG = "computer";
}

// Build the internal board for use later. This keeps track of all letters played.
void Board::buildBoard()
{
    vector<vector<string>> boardVector(15, vector<string>(15));
    for(int row = 0; row < boardVector.size(); row++)
    {
        for(int col = 0; col < boardVector.size(); col++)
        {
            boardVector[row][col] = "-";
        }
    }
    this->boardVec = boardVector;
    this->boardTest  = boardVector;
}

// Make references for the CSS file to see for when it is needed to colour board buttons. These are added to
// "boardButton" to create a full reference to the button using gtk_widget_set_name().
string Board::setColourTag(string pos)
{
    for(int i = 0; i < bonusPositionsRedTripleWord.size(); i++)
    {
        if(pos == bonusPositionsRedTripleWord[i]) return "Red";
    }
    for(int i = 0; i < bonusPositionsPinkDoubleWord.size(); i++)
    {
        if(pos == bonusPositionsPinkDoubleWord[i]) return "Pink";
    }
    for(int i = 0; i < bonusPositionsLightBlueDoubleLetter.size(); i++)
    {
        if(pos == bonusPositionsLightBlueDoubleLetter[i]) return "LightBlue";
    }
    for(int i = 0; i < bonusPositionsDarkBlueTripleLetter.size(); i++)
    {
        if(pos == bonusPositionsDarkBlueTripleLetter[i]) return "DarkBlue";
    }
}

// Set a single button colour to yellow on a board button click.
void Board::changeColour(GtkButton* button)
{
    string oldName = gtk_widget_get_name(GTK_WIDGET(button));
    string newName = "boardButtonYellow";
    string position = gtk_widget_get_tooltip_text (GTK_WIDGET(button));
    gtk_widget_set_name(GTK_WIDGET(button), newName.c_str());
    vector<string>change;
    change.push_back(newName);
    change.push_back(oldName);
    change.push_back(position);
    this->colourChanges.push_back(change);
    // During a game there could be any number of changes to the button, so keep them per move.
}

// A computer only plays a valid move, so there is no need to store information on colour changes. So we simply change the colour.
void Board::changeColourComputer(string position)
{
    string newName = "boardButtonYellow";
    vector<int>singleButtonVector = getPositionFromStr(position);
    int posX = singleButtonVector[0];
    int posY = singleButtonVector[1];
    gtk_widget_set_name(GTK_WIDGET(this->buttonsBoard[posX][posY]), newName.c_str());
}

void Board::revertColourChanges()
{
    // Undo all of what Board::changeColour did.
    // The vector elements: change[0] newName given; change[1] old name to set back; change[2] position to get a button to change.
    if(!this->colourChanges.empty())
    {
        for(int i = 0; i < this->colourChanges.size(); i++)
        {
            // Collect each part.
            string newName;
            string oldName;
            string position;
            for(int j = 0; j < this->colourChanges[i].size(); j++)      // Size is always 3
            {
                if(j == 0)
                {
                    newName = this->colourChanges[i][j];
                }
                if(j == 1)
                {
                    oldName = this->colourChanges[i][j];
                }
                if(j == 2)
                {
                    position = this->colourChanges[i][j];
                }
            }
            // Use collected parts here.
            vector<int>singleButtonVector = getPositionFromStr(position);
            int posX = singleButtonVector[0];
            int posY = singleButtonVector[1];
            gtk_widget_set_name(GTK_WIDGET(this->buttonsBoard[posX][posY]), oldName.c_str());
            colourChanges[i].pop_back();
        }
        // Make sure to clear 'colourChanges' when done. As this is meant to be a per move method.
        colourChanges.clear();
    }
}

void Board::resetAllColours()
{
    for(int row = 0; row < this->buttonsBoard.size(); row++)
    {
        for(int col = 0; col < this->buttonsBoard.size(); col++)
        {
            string indexes = "["+to_string(row) + "," + to_string(col) +"]";
            string name = "boardButton" + setColourTag(indexes);
            // When changing name CSS is updated internally.
            gtk_widget_set_name(GTK_WIDGET(this->buttonsBoard[row][col]), name.c_str());
        }
    }
}

// Create a vector of button widgets, setting label, tool tip (which contains the position) and css reference.
void Board::buildButtons()
{
    string blank = " ";
    vector<vector<GtkWidget*>> board_buttons(boardVec.size(), vector<GtkWidget*>(boardVec.size()));

    for(int row = 0; row < board_buttons.size(); row++)
    {
        for(int col = 0; col < board_buttons.size(); col++)
        {
            string indexes = "["+to_string(row) + "," + to_string(col) +"]";
            GtkWidget *buttonSquare = gtk_button_new_with_label(blank.c_str() );
            // Use tooltips instead of labels to communicate with stored vector of game state.
            //gtk_container_add (GTK_CONTAINER (button), label);
            gtk_widget_set_tooltip_text (GTK_WIDGET(buttonSquare), indexes.c_str() );
            // For CSS.
            string name = "boardButton" + setColourTag(indexes);
            gtk_widget_set_name(buttonSquare, name.c_str());
            // Finally add it now that all the needed properties are set.
            board_buttons[row][col] = buttonSquare;
        }
    }
    this->buttonsBoard = board_buttons;
}

void Board::resetButtonLabels()
{
    // Change the button labels to a blank space so the label text can't be seen.
    string blank = " ";
    for(int row = 0; row < buttonsBoard.size(); row++)
    {
        for(int col = 0; col < buttonsBoard.size(); col++)
        {
            gtk_button_set_label (GTK_BUTTON(buttonsBoard[row][col]), blank.c_str());
        }
    }
}

void Board::printBoard()
{
    for (unsigned row = 0; row < boardVec.size(); row++)
    {
        for (string elem : boardVec[row])
            cout << elem << ",";
        cout << endl;
    }
}

// Unique test; this is for checking groups of squares. It checks if the vector of that group has distributed x or y coordinates.
bool Board::testIfUniquelySpread(vector<int>arrayOfInts)
{
    bool testPassed = true;
    for (int i = 0; i < arrayOfInts.size() -1; i++)
    {
        // [4,5,6,9]
        // Does the array element at i, add one equal the next array elements value? if so return false.
        // We don;t want grouped indexes.
        if(arrayOfInts[i]+1 == arrayOfInts[i +1])
        {
            testPassed = false;
            break;
        }
    }
    string message;
    /*if(testPassed)
    {
        cout << "(Board::testIfUniquelySpread): passed" << endl;
    }
    else
    {
        cout << "(Board::testIfUniquelySpread): failed" << endl;
    }*/
    return testPassed;
}

// The helper method to get a position's integer vector from a string version of that vector.
vector<int> Board::getPositionFromStr(string position)
{
    //cout << "positon: " << position << endl;
    // position [9,15]
    // Remove brackets.
    position = position.erase(position.find("["), 1);
    position = position.erase(position.find("]"), 1);

    // position 9,15
    // Use the correct substring indexes to get integers.
    int commaPlace = position.find(",");
    string x = position.substr(0,commaPlace);
    string y = position.substr(commaPlace+1, -1);
    int posX = std::atoi(   x.c_str()     );
    int posY = std::atoi(    y.c_str()   );

    //cout << "x: [" << posX << "]" << endl;
    //cout << "y: [" << posY << "]" << endl;
    vector<int>coordinate(2);
    coordinate[0] = posX;
    coordinate[1] = posY;
    return coordinate;
}

// The helper method to get what's at the square at the moment from the original board multi dimensional vector.
string Board::getInfoFromSquare(int xIn, int yIn)
{
    int x = xIn;
    int y = yIn;
    if(x >= 0 && x <= 14 && y >= 0 && y <= 14)
    {
        return this->boardVec[x][y];
    }
    else
    {
        return "-";
    }
}

// A helper method that gets a letter from a player hand vector.
string Board::getLetterFromPlayerVec(vector<vector<string>> playerMoves,int x, int y)
{
    for(unsigned int row = 0; row < playerMoves.size(); row++)
    {
        vector<int>coordinates = getPositionFromStr(playerMoves[row][0]);
        if(coordinates[0] == x && coordinates[1] == y)
        {
            return playerMoves[row].at(1);
        }
    }
    return EMPTY_SQUARE;
}

// This method updates the test board with a letter at specified position.
void Board::updateBoard(string position, string letter, string method)
{
    // Check the test board vector before applying a result; stored boardVec will eventually contain computer moves and past player moves.
    vector<int>coordinates = getPositionFromStr(position);
    int posX = coordinates[0];
    int posY = coordinates[1];

    if (method == "add")
    {
        this->boardTest[posX][posY] = letter;
    }
    else if (method == "reset")
    {
        //string coord = "[" + to_string(posX) + "," + to_string(posY) + "]";
        this->boardTest[posX][posY] = "-";
        gtk_button_set_label(GTK_BUTTON(this->buttonsBoard[posX][posY]), " ");
    }
}

// Used by computer players to set labels. Also this method will now change label markup as well.
void Board::setLabelSelective(string position, string letter)
{
    vector<int>coordinates = getPositionFromStr(position);
    int posX = coordinates[0];
    int posY = coordinates[1];
    gtk_button_set_label(GTK_BUTTON(this->buttonsBoard[posX][posY]), letter.c_str());

    format.changeMarkUp(this->buttonsBoard[posX][posY], letter);
}


// When tests on user letters are passed, the test board with its correct moves is assigned to boardVec.
void Board::mergeBoards()
{
    this->boardVec = boardTest;
}

// A helper method to return missing numbers in a list of integers, the new vector returned will be bigger filled with those.
vector<int> Board::getMissingElements(vector<int>arry)
{
    vector<int>testArray = arry;
    // {1,2,5,6,9}
    // The array is already sorted, the first element should be the lowest N.
    int minN = *min_element(testArray.begin(), testArray.end());
    int maxN = *max_element(testArray.begin(), testArray.end());

    //cout << "(Board::getMissingElements) max n in getmissing element method: " << maxN << endl;

    vector<int>newArray;
    // {1,2,3,4.... 9} ALL integers from min of vector to max of vector.
    for(int i = minN; i <= maxN; i++)
    {
        newArray.push_back(i);
    }
    // Now the vector should be a series of counting numbers up to and including the orginal vector's maximum value.
    return newArray;
}

// Helper methods to get adjacent letters for checkValidMove() less repeat code than before.
string Board::getWordsFromLeft(int x, int y)
{
    string leftFromSingle;
    int y1 = y;
    while(getInfoFromSquare(x, --y1) != "-")
    {
        leftFromSingle += getInfoFromSquare(x, y1);
        //cout << "(Board::getWordsFromLeft) getWordsFromLeft: " << leftFromSingle << endl;
    }
    return leftFromSingle;
}

string Board::getWordsFromRight(int x, int y)
{
    string rightFromSingle;
    // CHECK RIGHT
    int y2 = y;
    while(getInfoFromSquare(x, ++y2) != "-")
    {
        rightFromSingle += getInfoFromSquare(x, y2);
        //cout <<"(getWordsFromRight) getWordsFromRight: " <<rightFromSingle <<endl;
    }
    return rightFromSingle;
}

string Board::getWordsFromUp(int x, int y)
{
    string upFromSingle;
    // CHECK UP
    int x1 = x;
    while(getInfoFromSquare(--x1, y) != "-")
    {
        upFromSingle += getInfoFromSquare(x1, y);
        //cout <<"(Board::getWordsFromUp) getWordsFromUp: " << upFromSingle<<endl;
    }
    return upFromSingle;
}

string Board::getWordsFromDown(int x, int y)
{
    // CHECK DOWN
    string downFromSingle;
    int x1 = x;
    while(getInfoFromSquare(++x1, y) != "-")
    {
        downFromSingle += getInfoFromSquare(x1, y);
        //cout << "(Board::getWordsFromDown) getWordsFromDown: "<<downFromSingle <<endl;
    }
    return downFromSingle;
}

int Board::getBoardVecSize()
{
    return this->boardVec.size();
}

vector<string> Board::getRow(int n)
{
    return this->boardVec[n];
}

// This is for when checkMove finds a word that intersects another, the element where this happens is stored and tested.
bool Board::matchCrossWordLine(vector<int>crossword, int elem)
{
    for(int i = 0; i < crossword.size(); i++)
    {
        if(crossword[i] == elem)
        {
            return true;
        }
    }
    return false;
}

// The player hand testing method. Note the computer move does not need to fill missing elements which is standard here, so that part is skipped for the computer.
bool Board::checkValidMove(vector<vector<string>> playerMoves, int round, string whichPlayer)
{
    // AFTER ROUND 0 WORDNEW IS ALWAYS SOME COMBINATION OF BOARDLETTERS AND ITSELF.
    // Check if the middle square is used in first hand played and if the hand meets the criteria of being over 2 in length on the first move.
    if(round == 0)
    {
        if(!centerSquareCheck())
        {
            cout << "(Board::checkValidMove) first hand must include a letter covering center square" << endl;
            return false;
        }
        if(playerMoves.size() < 2)
        {
            cout << "(Board::checkValidMove) not enough tiles played, play 2 or more for first move" << endl;
            return false;
        }
    }
    // If play moves size is bigger than 1 do the orientation tests. (Check Single Letters seperately).
    if(playerMoves.size() > 1)
    {

        vector<string>chainWords;
        // A utility class for storing a potential legitimate move.
        class Word
        {
        public:
            // These variables now store the values used for testing.
            vector<int>xcoords;
            vector<int>ycoords;
            vector<string>letters;
            string orientation;
        };
        Word testWord;

        // Checking orientation; the array is 2 or more in size to get to this branch, so the next element from 0 can be tested safely.
        for(unsigned int row = 0; row < playerMoves.size(); row++)
        {
            vector<int>coordinates = getPositionFromStr(playerMoves[row][0]);
            //xcoords.push_back(coordinates[0]);
            //ycoords.push_back(coordinates[1]);
            testWord.xcoords.push_back(coordinates[0]);
            testWord.ycoords.push_back(coordinates[1]);
            //testWord.letters.push_back(playerMoves[row][1]);
        }
        // Checking for gaps by incrementing, then if testing if the next cardinal number is missing in a sorted array of integers.
        // Looks at 'boardTest' for the missing coordinate, if there is none, return false.
        // Letts are put in order.
        // PlayerMove will adpot missing tiles from the board if they are not blank ("-").
        // But after that playerMove still stores unordered coordinates, so 'Word testWord' is its ordered counterpart.
        if(testWord.xcoords[0] == testWord.xcoords[1])
        {
            // HORIZONTAL, sort by Y.
            testWord.orientation = "HORIZONTAL";
            std::sort(std::begin(testWord.ycoords), std::end(testWord.ycoords));

            if (whichPlayer == PLAYER_TAG ) testWord.ycoords= getMissingElements(testWord.ycoords);

            std::sort(std::begin(testWord.ycoords), std::end(testWord.ycoords));
            // Also add the missing X created by filling missing Y's
            // The Y coordinate vector should be bigger.
            int difference = (testWord.ycoords.size() - testWord.xcoords.size() );
            // if they are not the same
            if(difference != 0)
            {
                for(int n = 0; n < difference; n++)
                {
                    // Update xcoord n times X's are the same, so using first element of xcoords is ok.
                    testWord.xcoords.push_back(testWord.xcoords[0]);
                }
            }
        }
        else
        {
            // VERTICAL sorty by X.
            testWord.orientation = "VERTICAL";
            std::sort(std::begin(testWord.xcoords), std::end(testWord.xcoords));

            if (whichPlayer == PLAYER_TAG) testWord.xcoords= getMissingElements(testWord.xcoords);

            std::sort(std::begin(testWord.xcoords), std::end(testWord.xcoords));
            // X vector should be bigger, if no additions were made the difference should be 0 anyway.
            int difference = (testWord.xcoords.size() - testWord.ycoords.size() );
            // If these vectors are not the same in size.
            if(difference != 0)
            {
                for(int n = 0; n < difference; n++)
                {
                    // Update ycoord n times, here Y's are the same, so using the first element of ycoords is ok.
                    testWord.ycoords.push_back(testWord.ycoords[0]);
                }
            }
        }
        // ##### AFTER SORTING (The heavy way ) make a word from sorted indexes. ############################################
        // Now the object WORD contains everything in order.
        string wordNew;
        bool comboWordMade = false;
        // This only fills the gaps in-between if there are any, not squares before or after.
        for(int i = 0; i < testWord.xcoords.size(); i++)
        {
            string letter = getLetterFromPlayerVec(playerMoves, testWord.xcoords[i], testWord.ycoords[i] );
            //cout << "letter in  boardcheck loop: " << letter << endl;
            if(letter == EMPTY_SQUARE)
            {

                letter = getInfoFromSquare(testWord.xcoords[i], testWord.ycoords[i]);

                if (letter != "-")
                {

                    wordNew += letter;
                    testWord.letters.push_back(letter);
                    // Making sure the original 'playerMoves' is now updated with the board letter becasuse it is referenced again later.
                    vector<string> oneMove{"",""};
                    oneMove[0] = "["+to_string(testWord.xcoords[i])+","+to_string(testWord.ycoords[i])+"]";
                    oneMove[1] = letter;
                    playerMoves.push_back(oneMove);
                    comboWordMade = true; // If all the other tests below fail at least a combination word was made with an intersecting board letter.
                }
                if (letter == "-")
                {
                    //cout << "(Board::checkValidMove) Post sorting test, in loop to fill gaps, empty square found(-): "<< endl;
                    return false;
                }
            }
            else
            {
                // Else there is a letter from 'playerMoves' with the given coordinates.
                wordNew += letter;
                testWord.letters.push_back(letter);
            }
        }
        //cout << "(Board::checkValidMove) wordNew created from ordered letters: ";
        //cout << wordNew<<endl;

        // Now to check surrounding squares based on orientation, because the coordinate boundary rules and tests are slightly different for each orientation.
        if(testWord.orientation == "HORIZONTAL" )
        {
            //cout << "(Board::checkValidMove) in horizontal test branch: ";
            // horizontal, so work with y
            int maxY = *max_element(testWord.ycoords.begin(), testWord.ycoords.end());
            int minY = *min_element(testWord.ycoords.begin(), testWord.ycoords.end());
            int regularX = testWord.xcoords[0];
            // The following tests make sure to look in the bounds of a row (horizontal).
            string leftWord="";
            string rightWord="";
            // Also not this is different from single letter tests, because min and max cant be the same, this is a vector of 2 or more elements.
            if(minY > 0 && maxY < 14)
            {
                // Get possible words from both the left and the right of each position.
                leftWord = getWordsFromLeft(regularX, minY);
                rightWord = getWordsFromRight(regularX, maxY);
            }
            if(minY == 0)
            {
                // Count from the max of our vector to the right, not over ourselves.
                rightWord = getWordsFromRight(regularX, maxY);
            }
            if(maxY == 14)
            {
                // Count from the min of our vector to the left, not over ourselves.
                leftWord = getWordsFromLeft(regularX, minY);
            }
            // Edit the left word (if found) because the 'getWordsFromLeft' method returns a word in reverse. No need to reverse a string of length 1.
            if(leftWord.length() > 1)
            {
                //cout << "(Board::checkValidMove) in horizontal test branch: reversing leftWord: " << leftWord;
                reverse(leftWord.begin(), leftWord.end());
                //cout << " to: " << leftWord << endl;
            }
            // Check a combination on its own either with no words to left or to the right. The concatenation should be alright if adding an empty string.
            if(leftWord.length() > 0 || rightWord.length() > 0)
            {
                string wordNewCombined = leftWord+wordNew+rightWord;

                if (  dict.dictionaryCheckWord( wordNewCombined ))
                {
                    //cout << "(Board::checkValidMove) in horizontal test branch: player word: "+ wordNewCombined+" is a word " << endl;
                    wordNew = wordNewCombined;
                    comboWordMade = true;
                }
                else
                {
                    comboWordMade = false;
                    return false;
                }
            }
            // Check row below & above in one block, and if the indexes are grouped one next to the other, this should fail.
            bool rowTestsPassed = false;
            // Pass all the Y coordinates of X-1 (row), an array of integers containing only Y coordinates.
            int regularXAbove = testWord.xcoords[0] - 1;
            int regularXBelow = testWord.xcoords[0] + 1;
            vector<int>yElementsAbove;
            vector<int>yElementsBelow;
            vector<int>crossWordY;
            if(regularXAbove >= -1 && regularXBelow <= 15)
            {
                for(int i = 0; i < testWord.xcoords.size(); i++)
                {
                    // If any square in the above row AND below row has a letter, store its Y coordinate for testing.
                    if(regularXAbove != -1)
                    {
                        if(getInfoFromSquare(regularXAbove, testWord.ycoords[i]) != "-" )
                        {
                            yElementsAbove.push_back(testWord.ycoords[i]);
                        }
                    }
                    if(regularXBelow != 15)
                    {
                        if(getInfoFromSquare(regularXBelow, testWord.ycoords[i]) != "-" )
                        {
                            yElementsBelow.push_back(testWord.ycoords[i]);
                        }
                    }
                }
                if(yElementsAbove.size() > 0)
                {
                    if (testIfUniquelySpread(yElementsAbove) == true )
                    {
                        rowTestsPassed = true;
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                        for (int i = 0; i < yElementsAbove.size(); i++)
                        {
                            //cout << "(Board::checkValidMove) in horizontal test branch PROVISIONAL CHAIN  uniq ABOVE X: " <<testWord.xcoords[0] << " Y: " << yElementsAbove[i] << endl;
                            string letter = getLetterFromPlayerVec(playerMoves, testWord.xcoords[0], yElementsAbove[i] );
                            string upWord = getWordsFromUp(testWord.xcoords[0], yElementsAbove[i]);
                            // Sometimes a word intersects, so the row below is unique as well, if not, nothing is added.
                            string belowWord = getWordsFromDown(testWord.xcoords[0], yElementsAbove[i]);
                            reverse(upWord.begin(), upWord.end());
                            string word = upWord + letter + belowWord;
                            //cout << "(Board::checkValidMove) in horizontal test branch PROVISIONAL CHAIN  uniq ABOVE Test word: " << word << endl;
                            if(dict.dictionaryCheckWord( word ) && dict.dictionaryCheckWord( wordNew ))
                            {
                                chainWords.push_back(word);
                                rowTestsPassed = true;
                                comboWordMade = true;
                                if(belowWord.size() > 0 ) crossWordY.push_back(yElementsAbove[i]);
                            }
                            else
                            {
                                //cout << "(Board::checkValidMove) in horizontal test branch PROVISIONAL CHAIN uniq ABOVE (no word found)" << endl;
                                rowTestsPassed = false;
                                return false;
                            }
                        }
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                    }
                    else
                    {
                        //cout << "(Board::checkValidMove) in horizontal test branch uniq ABOVE: row above failed" << endl;
                    }
                }
                if(yElementsBelow.size() > 0)
                {
                    if( testIfUniquelySpread(yElementsBelow) == true )
                    {
                        rowTestsPassed = true;
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                        for (int i = 0; i < yElementsBelow.size(); i++)
                        {
                            // Sometimes in the case above, a word is made from above and below, so ignore it with continue
                            // to save making the condition return false, because a second look here will only find one part of the combo.
                            if(crossWordY.size() > 0)
                            {
                                if(matchCrossWordLine(crossWordY, yElementsBelow[i]) == true)
                                {
                                    // Don't invalidate the row, skip pass tests that were already done in the 'yElementsAbove' condition.
                                    continue;
                                }
                            }

                            //cout << "(Board::checkValidMove) in horizontal test branch PROVISIONAL CHAIN  uniq BELOW X: " <<testWord.xcoords[0] << " Y: " << yElementsBelow[i] << endl;
                            string letter = getLetterFromPlayerVec(playerMoves, testWord.xcoords[0], yElementsBelow[i] );
                            string downWord = getWordsFromDown(testWord.xcoords[0], yElementsBelow[i]);
                            string word =  letter + downWord;
                            //cout << "(Board::checkValidMove) in horizontal test branch PROVISIONAL CHAIN  uniq BELOW Test word: " << word << endl;
                            if(dict.dictionaryCheckWord( word ) && dict.dictionaryCheckWord( wordNew ))
                            {
                                chainWords.push_back(word);
                                rowTestsPassed = true;
                                comboWordMade = true;
                            }
                            else
                            {
                                //cout << "(Board::checkValidMove) in horizontal test branch PROVISIONAL CHAIN uniq BELOW (no word found)" << endl;
                                rowTestsPassed = false;
                                return false;
                            }
                        }
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                    }
                    else
                    {
                        //cout << "(Board::checkValidMove) in horizontal test branch uniq BELOW: row below failed" << endl;
                    }
                }
                // If the rows are empty (above and below), this will count as a row test pass.
                if(yElementsBelow.empty() && yElementsAbove.empty() )
                {
                    rowTestsPassed = true;
                }
            }
            // If the row below or above contain no groupings and made some words or a single word were made.
            if (rowTestsPassed  && comboWordMade)
            {
                cout << endl <<"(Board::checkValidMove) Horizontal test branch end(rowTestsPassed): " << endl;
                cout << "(Board::checkValidMove.getLetterValue) Horizontal. Scoring letters (round 0):\n '";
                int score = 0;
                for(int i = 0; i < wordNew.size(); i++)  // More code but need to add a bonus check from player moves.
                {
                    string letter(1, wordNew[i]);
                    cout << letter ;
                    score += gameScoreBoard->getLetterValue(letter);
                }
                cout << "' "  << endl;
                //gameScoreBoard->setBoardScore(score, whichPlayer); // Finally set the score calculated above, user and computer both have scores maintained in this class.
                // BONUS TEST - modifying re-assigns a value, setting increments. So we just reset it to the modified score
                int modifiedScore;
                if(whichPlayer == PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonus(playerMoves, score);
                if(whichPlayer == COMPUTER_PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonusComp(playerMoves, score);
                gameScoreBoard->setBoardScore(modifiedScore, whichPlayer);

                // add chain words after bonuses.
                // SCORE WORDS. (using chainWords) this condition is true so chainwords will have at least one word.
                if(chainWords.size() > 0)
                {
                    int scoreC = 0;
                    scoreC += gameScoreBoard->scoreChainWords(chainWords);
                    gameScoreBoard->setBoardScore(scoreC, whichPlayer);
                }

                return true;

            }
            else
            {
                if (round == 0 && dict.dictionaryCheckWord( wordNew ))
                {
                    // SCORE WORD. Show words to consol then return true (this is round 0 no chainwords).
                    cout << endl <<"(Board::checkValidMove) Horizontal test branch end: Round 0: single word: " << wordNew << endl;
                    cout << "(Board::checkValidMove.getLetterValue) Horizontal. Scoring letters (round 0):\n '";
                    int score = 0;
                    for(int i = 0; i < wordNew.size(); i++)
                    {
                        string letter(1, wordNew[i]);
                        cout << letter  ;
                        score += gameScoreBoard->getLetterValue(letter);
                    }
                    cout << "' "<< endl;
                    //gameScoreBoard->setBoardScore(score, whichPlayer);
                    // BONUS TEST - modifying re-assigns a value, setting increments. So we just reset it to the modified score
                    int modifiedScore;
                    if(whichPlayer == PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonus(playerMoves, score);
                    if(whichPlayer == COMPUTER_PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonusComp(playerMoves, score);
                    gameScoreBoard->setBoardScore(modifiedScore, whichPlayer);

                    return true;
                }
                else
                {
                    cout << "(Board::checkValidMove) in horizontal test branch end: no words found"<< endl;
                    return false;
                }
            }
        }
        else if (testWord.orientation == "VERTICAL")
        {
            // TODO (euser#4#02/26/19): (repeat code), Only slight difference from horizontal test, move to a single function.
            // Y is the coordinate on the right i.e [1,7] the 7, and is always the same for vertical words.
            // X is the one that varies here,that is what should be tested.
            //cout << "(Board::checkValidMove) in Vertical test branch..." << endl;
            int maxX = *max_element(testWord.xcoords.begin(), testWord.xcoords.end());
            int minX = *min_element(testWord.xcoords.begin(), testWord.xcoords.end());
            int regularY = testWord.ycoords[0];
            //cout << "in Vertical test branch...REGULAR Y:" << regularY << endl;
            //cout << "in Vertical test branch...MIN X:" << minX << endl;
            //cout << "in Vertical test branch...MAX X:" << maxX << endl;
            string upWord="";
            string downWord="";
            // These are different from single letter tests, because min and max cant be the same, (this is a vector of 2 or more).
            if(minX > 0 && maxX < 14)
            {
                // Get words from above and below in one block.
                upWord = getWordsFromUp(minX, regularY);
                downWord = getWordsFromDown(maxX, regularY);
            }
            if(minX == 0)
            {
                // Get letters from below the max of our letters coordinate.
                downWord = getWordsFromDown(maxX, regularY);
            }
            if(maxX == 14)
            {
                // Get letters from above min of our letter coordinate, as there's no need to count from max backwards.
                // (because this would include our own letters or return empty).
                upWord = getWordsFromUp(minX, regularY);
            }
            // Edit the upper word if found, as 'getWordsFromUp' returns a word in reverse.
            if(upWord.length() > 1)
            {
                //cout << "(Board::checkValidMove) in Vertical test branch: reversing upWord: "<< upWord;
                reverse(upWord.begin(), upWord.end());
                //cout << " to: " << upWord << endl;
            }
            // Check a column combination on its own.
            if(upWord.length() > 0 || downWord.length() > 0)
            {
                // Check the possible word combined with words coming from above or from below. Both are used because adding an empty string will not affect the word.
                string wordNewUpCombined = upWord+wordNew+downWord;
                //cout << "(Board::checkValidMove) in Vertical test branch...combo word: " << wordNewUpCombined << endl;

                if (  dict.dictionaryCheckWord( wordNewUpCombined ))
                {
                    //cout << "(Board::checkValidMove) in Vertical test branch: word vert: "+ wordNewUpCombined+" is a word " << endl;
                    wordNew = wordNewUpCombined;
                    comboWordMade = true;
                }
                else
                {
                    comboWordMade = false;
                    return false;
                }
            }
            // Check the column's left and right squares in one block, and if indexes are grouped one next to the other, fail.
            bool colTestsPassed = false;
            int regularYLeft = testWord.ycoords[0] - 1;
            int regularYRight = testWord.ycoords[0] + 1;
            vector<int>xElementsLeft;
            vector<int>xElementsRight;
            vector<int>crossWordX;
            if(regularYLeft >= -1 && regularYRight <= 15)
            {
                for(int i = 0; i < testWord.ycoords.size(); i++)
                {
                    // If any square in the left column AND right column has a letter, store its X coordinate for testing.
                    if(regularYLeft != -1)
                    {
                        if(getInfoFromSquare( testWord.xcoords[i], regularYLeft) != "-" )
                        {
                            xElementsLeft.push_back(testWord.xcoords[i]);
                        }
                    }
                    if(regularYRight != 15)
                    {
                        if(getInfoFromSquare(testWord.xcoords[i], regularYRight) != "-" )
                        {
                            xElementsRight.push_back(testWord.xcoords[i]);
                        }
                    }
                }
                if(xElementsLeft.size() > 0)
                {
                    if (testIfUniquelySpread(xElementsLeft) == true )
                    {
                        colTestsPassed = true; // Provisionally true unless shown otherwise.
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                        for (int i = 0; i < xElementsLeft.size(); i++)
                        {
                            //cout << "(Board::checkValidMove) in vertical test branch PROVISIONAL CHAIN  uniq LEFT X: " <<xElementsLeft[i] << " Y: " << testWord.ycoords[0] << endl;
                            string letter = getLetterFromPlayerVec(playerMoves, xElementsLeft[i], testWord.ycoords[0] );
                            string leftWord = getWordsFromLeft(xElementsLeft[i], testWord.ycoords[0]);
                            // Sometimes left and right are unique, if not nothing is added to the letter.
                            string rightWord = getWordsFromRight(xElementsLeft[i], testWord.ycoords[0]);

                            reverse(leftWord.begin(), leftWord.end());
                            string word = leftWord + letter + rightWord;
                            //cout << "(Board::checkValidMove) in vertical test branch PROVISIONAL CHAIN  uniq LEFT Test word: " << word << endl;
                            if(dict.dictionaryCheckWord( word ) && dict.dictionaryCheckWord( wordNew ))
                            {
                                chainWords.push_back(word);
                                colTestsPassed = true;
                                comboWordMade = true;
                                if(rightWord.size()> 0) crossWordX.push_back(xElementsLeft[i]);

                            }
                            else
                            {
                                //cout << "(Board::checkValidMove) in vertical test branch PROVISIONAL CHAIN uniq LEFT (no word found)" << endl;
                                colTestsPassed = false;
                                return false;
                            }
                        }
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                    }
                    else
                    {
                        //cout << "(Board::checkValidMove) in Vertical test branch uniq LEFT:...column left failed" << endl;
                    }
                }
                if(xElementsRight.size() > 0)
                {
                    if( testIfUniquelySpread(xElementsRight) == true )
                    {
                        colTestsPassed = true; // Provisionally true unless shown otherwise.
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                        for (int i = 0; i < xElementsRight.size(); i++)
                        {
                            // Sometimes in the case above, a word is made from left and right, so ignore it with continue
                            // to save making the condition return false.
                            if(crossWordX.size() > 0)
                            {
                                if(matchCrossWordLine(crossWordX, xElementsRight[i]) == true)
                                {
                                    // Don't invalidate the row, just skip past future tests, because they have already been tested above.
                                    continue;
                                }
                            }
                            //cout << "(Board::checkValidMove) in vertical test branch PROVISIONAL CHAIN  uniq RIGHT X: " <<xElementsRight[i] << " Y: " << testWord.ycoords[0] << endl;
                            string letter = getLetterFromPlayerVec(playerMoves, xElementsRight[i], testWord.ycoords[0] );
                            string rightWord = getWordsFromRight(xElementsRight[i], testWord.ycoords[0]);
                            string word = letter+ rightWord;
                            //cout << "(Board::checkValidMove) in vertical test branch PROVISIONAL CHAIN  uniq RIGHT Test word: " << word << endl;
                            if(dict.dictionaryCheckWord( word ) && dict.dictionaryCheckWord( wordNew ))
                            {
                                chainWords.push_back(word);
                                colTestsPassed = true;
                                comboWordMade = true;
                            }
                            else
                            {
                                //cout << "(Board::checkValidMove) in vertical test branch PROVISIONAL CHAIN uniq RIGHT (no word found)" << endl;
                                colTestsPassed = false;
                                return false;
                            }
                        }
                        // ###################################### PROVISIONAL CHAIN WORD TEST
                    }
                    else
                    {
                        //cout << "(Board::checkValidMove) in Vertical test branch uniq RIGHT:...column right failed" << endl;
                    }
                }

                // When both vectors are empty, this also counts as a column test pass.
                if(xElementsLeft.empty()  && xElementsRight.empty())
                {
                    colTestsPassed = true;
                }
            }
            // SCORE WORDS
            if (colTestsPassed && comboWordMade)
            {
                cout << endl <<"(Board::checkValidMove) Vertical test branch end(colTestsPassed): " << endl;
                cout << "(Board::checkValidMove.getLetterValue) Vertical. Scoring letters (round 0):\n '";
                int score = 0;
                for(int i = 0; i < wordNew.size(); i++)
                {
                    string letter(1, wordNew[i]);
                    cout << letter  ;
                    score += gameScoreBoard->getLetterValue(letter);
                }
                cout << "' "<< endl;
                //gameScoreBoard->setBoardScore(score, whichPlayer); // Store the score of either the user or computer.
                // BONUS TEST - modifying re-assigns a value, setting increments. So we just reset it to the modified score
                int modifiedScore;
                if(whichPlayer == PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonus(playerMoves, score);
                if(whichPlayer == COMPUTER_PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonusComp(playerMoves, score);
                gameScoreBoard->setBoardScore(modifiedScore, whichPlayer);

                // add chain words after bonuses.
                // SCORE WORDS. (using chainWords) this condition is true so chainwords will have at least one word.
                if(chainWords.size() > 0)
                {
                    int scoreC = 0;
                    scoreC += gameScoreBoard->scoreChainWords(chainWords);
                    gameScoreBoard->setBoardScore(scoreC, whichPlayer);
                }
                return true;

            }
            else
            {
                if(round == 0 && dict.dictionaryCheckWord( wordNew ))
                {
                    // SCORE WORD.
                    cout << endl<<"(Board::checkValidMove) Vertical test branch end: Round 0: single word: " << wordNew << endl;
                    cout << "(Board::checkValidMove.getLetterValue) Vertical. Scoring letters (round 0):\n '";
                    int score = 0;
                    for(int i = 0; i < wordNew.size(); i++)
                    {
                        string letter(1, wordNew[i]);
                        cout << letter ;
                        score += gameScoreBoard->getLetterValue(letter);
                    }
                    cout << "' " << endl;
                    //gameScoreBoard->setBoardScore(score, whichPlayer);
                    // BONUS TEST - modifying re-assigns a value, setting increments. So we just reset it to the modified score
                    int modifiedScore;
                    if(whichPlayer == PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonus(playerMoves, score);
                    if(whichPlayer == COMPUTER_PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonusComp(playerMoves, score);
                    gameScoreBoard->setBoardScore(modifiedScore, whichPlayer);
                    return true;
                }
                else
                {
                    cout << "(Board::checkValidMove) in Vertical test branch end: no words found" <<endl;
                    return false;
                }
            }
        }
        else
        {
            cout << "(Board::checkValidMove) no orientation set, perhaps it's a single letter" << endl;
            return false;
        }
        // Check single letter.
    }
    else if (playerMoves.size() == 1)
    {
        //cout << "(Board::checkValidMove) in Single letter test branch: " << endl;
        // If all these are blank... return false, this means the player placed tile with surrounded with blanks or placed it diagonally.
        string leftFromSingle = "";
        string rightFromSingle = "";
        string upFromSingle = "";
        string downFromSingle = "";

        vector<int>singleCoordinate = getPositionFromStr(playerMoves[0][0]);
        int xSingleX = singleCoordinate[0];
        int ySingleY = singleCoordinate[1];
        //cout << "(Board::checkValidMove) Checking single letter, coords x: " << singleCoordinate[0] << " y:"  << singleCoordinate[1] << endl;
        //cout << "(Board::checkValidMove) Checking single letter: " <<  playerMoves[0][1]<< endl;

        // Check corner cases first, then edge cases.
        // ## Corners ##
        if(xSingleX == 0 && ySingleY == 0)
        {
            // Top left, check down and right.
            // CHECK DOWN
            downFromSingle = getWordsFromDown(xSingleX, ySingleY);
            // CHECK RIGHT
            rightFromSingle = getWordsFromRight(xSingleX, ySingleY);
        }
        else if (xSingleX == 14 && ySingleY == 0)
        {
            // Bottom left, check up and right.
            // CHECK UP
            upFromSingle = getWordsFromUp(xSingleX, ySingleY);
            // CHECK RIGHT
            rightFromSingle = getWordsFromRight(xSingleX, ySingleY);
        }
        else if (xSingleX == 0 && ySingleY == 14)
        {
            // Top right, check left and down.
            // CHECK LEFT
            leftFromSingle = getWordsFromLeft(xSingleX, ySingleY);
            // CHECK DOWN
            downFromSingle = getWordsFromDown(xSingleX, ySingleY);
        }
        else if (xSingleX == 14 && ySingleY == 14)
        {
            // Bottom right, check left and up.
            // CHECK LEFT
            leftFromSingle = getWordsFromLeft(xSingleX, ySingleY);
            // CHECK UP
            upFromSingle = getWordsFromUp(xSingleX, ySingleY);
        }
        // ## Edges ###
        else if(xSingleX == 0 && ySingleY < 14)
        {
            // Check left, right and down only (cant go up any more).
            // CHECK LEFT
            leftFromSingle = getWordsFromLeft(xSingleX, ySingleY);
            // CHECK RIGHT
            rightFromSingle = getWordsFromRight(xSingleX, ySingleY);
            // CHECK DOWN
            downFromSingle = getWordsFromDown(xSingleX, ySingleY);
        }
        else if (ySingleY == 0 && xSingleX < 14)
        {
            // Check up, down and right (cant go left, this is the left column).
            // CHECK UP
            upFromSingle = getWordsFromUp(xSingleX, ySingleY);
            // CHECK RIGHT
            rightFromSingle = getWordsFromRight(xSingleX, ySingleY);
            // CHECK DOWN
            downFromSingle = getWordsFromDown(xSingleX, ySingleY);
        }
        else if (xSingleX == 14 && ySingleY < 14)
        {
            // At bottom row, check up, left and right.
            // CHECK LEFT
            leftFromSingle = getWordsFromLeft(xSingleX, ySingleY);
            // CHECK RIGHT
            rightFromSingle = getWordsFromRight(xSingleX, ySingleY);
            // CHECK UP
            upFromSingle = getWordsFromUp(xSingleX, ySingleY);
        }
        else if(ySingleY == 14 && xSingleX < 14)
        {
            // At right column, check up, down and left.
            // CHECK UP
            upFromSingle = getWordsFromUp(xSingleX, ySingleY);
            // CHECK DOWN
            downFromSingle = getWordsFromDown(xSingleX, ySingleY);
            // CHECK LEFT
            leftFromSingle = getWordsFromLeft(xSingleX, ySingleY);
        }
        // For when a single letter is placed inside the board (not at edges or corner) - check all directions safely.
        else if(xSingleX > 0 && ySingleY <14)
        {
            // CHECK UP
            upFromSingle = getWordsFromUp(xSingleX, ySingleY);
            // CHECK DOWN
            downFromSingle = getWordsFromDown(xSingleX, ySingleY);
            // CHECK LEFT
            leftFromSingle = getWordsFromLeft(xSingleX, ySingleY);
            // CHECK RIGHT
            rightFromSingle = getWordsFromRight(xSingleX, ySingleY);
        }
        else
        {
            return false;
            //cout << "(Board::checkValidMove) invalid single coord" << endl;
        }
        //cout << "(Board::checkValidMove) upsingle: " << upFromSingle<<  endl;
        //cout << "(Board::checkValidMove) downsingle: " << downFromSingle<<  endl;
        //cout << "(Board::checkValidMove) leftsingle: " << leftFromSingle<<  endl;
        //cout << "(Board::checkValidMove) rightsingle: " << rightFromSingle<<  endl;
        // Now test if no words were formed.
        if(upFromSingle.length() == 0 && downFromSingle.length() == 0 && leftFromSingle.length() == 0 && rightFromSingle.length() == 0 )
        {
            //cout << "(Board::checkValidMove) no words formed with single letter (invalid move)" << endl;
            return false;
        }
        // Check grouping if the right and bottom > 0, check bottom right to prevent filling the grid.
        // If their length is greater than 0, a string was formed from tests above.
        // TODO (euser#1#03/12/19): sometimes words are made forming word squares and a tile bunching rule will dismiss it, change.
        if(downFromSingle.length() > 0 && rightFromSingle.length() > 0)
        {
            // Check game board for occupied letter adding one to coord of x and y for the tile below to the right.
            if(getInfoFromSquare(xSingleX+1, ySingleY+1) != "-")
            {
                //cout << "(Board::checkValidMove) tile bunching detected";
                return false;
            }
        }
        if(downFromSingle.length() > 0 && leftFromSingle.length() > 0)
        {
            // Check game board for occupied letter adding one to coord of x and -y for the tile below to the left.
            if(getInfoFromSquare(xSingleX+1, ySingleY-1) != "-")
            {
                //cout << "(Board::checkValidMove) tile bunching detected";
                return false;
            }
        }
        if(upFromSingle.length() > 0 && leftFromSingle.length() > 0)
        {
            // Check game board for occupied letter removing one from coord of x and -y for the tile above to the left.
            if(getInfoFromSquare(xSingleX-1, ySingleY-1) != "-")
            {
                //cout << "(Board::checkValidMove) tile bunching detected";
                return false;
            }
        }
        if(upFromSingle.length() > 0 && rightFromSingle.length() > 0)
        {
            // Check game board for occupied letter removing one from coord of x and +y for the tile above to the right.
            if(getInfoFromSquare(xSingleX-1, ySingleY+1) != "-")
            {
                //cout << "(Board::checkValidMove) tile bunching detected";
                return false;
            }
        }
        // End of bunching test, now check if any words were made - playerMoves[0][1] is a single letter played.
        // Reverse strings found from methods that return reversed strings (getWordsFromUp & getWordsFromLeft).
        reverse(upFromSingle.begin(), upFromSingle.end());
        reverse(leftFromSingle.begin(), leftFromSingle.end());

        string comboVert = upFromSingle + playerMoves[0][1] + downFromSingle;
        string comboHoriz = leftFromSingle + playerMoves[0][1] + rightFromSingle;
        bool dictionaryPassed = false;
        // Score the words and return true, the combos should be of a length greater than 1, as they include player letter.
        // Store words found in chain words.
        vector<string>chainwords;
        // 'vertChecked' if this is true and dictionaryPassed==false, make sure that setting remains
        if(comboHoriz.length() > 1 && comboVert.length() > 1)
        {
            if(dict.dictionaryCheckWord(comboVert) && dict.dictionaryCheckWord(comboHoriz))
            {
                chainwords.push_back(comboVert);
                chainwords.push_back(comboHoriz);
                dictionaryPassed = true;
            }
            else
            {
                dictionaryPassed = false;
            }
            // Check seperate comboword below, only one condition will be met now.
        }
        else
        {
            if(comboVert.length() > 1)
            {
                if (  dict.dictionaryCheckWord(comboVert) )
                {
                    //cout << "(Board::checkValidMove) player word single v: "+ comboVert+" is a word " << endl;
                    chainwords.push_back(comboVert);
                    dictionaryPassed = true; // Provisionally.
                }
                else
                {
                    dictionaryPassed = false;
                }
            }
            if(comboHoriz.length() > 1)
            {
                if (  dict.dictionaryCheckWord(comboHoriz) )
                {
                    //cout << "(Board::checkValidMove) player word single h: "+ comboHoriz+" is a word " << endl;
                    chainwords.push_back(comboHoriz);
                    dictionaryPassed = true; // Provisionally.
                }
                else
                {
                    dictionaryPassed = false;
                }
            }
        }

        if(dictionaryPassed == false)
        {
            return false;
        }
        else
        {
            // SCORE WORDS. (using chainWords) this condition is true, so chainwords will have at least one word.
            // Chain words here include the single letter.
            int score = gameScoreBoard->scoreChainWords(chainwords);
            //gameScoreBoard->setBoardScore(score, whichPlayer); // Set the score for the user or the computer player.
            // BONUS TEST - modifying re-assigns a value, setting increments. So we just reset it to the modified score
            int modifiedScore;
            if(whichPlayer == PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonus(playerMoves, score );
            if(whichPlayer == COMPUTER_PLAYER_TAG) modifiedScore = gameScoreBoard->postScoreBonusComp(playerMoves, score );
            gameScoreBoard->setBoardScore(modifiedScore, whichPlayer);
            return true;
        }

        return false; // Default

    }
    else
    {
        cout << "(Board::checkValidMove) Player moves is empty, returning false" << endl;
        return false;
    }

    return false;
}

bool Board::centerSquareCheck()
{
    // The center square is found at position [7,7].
    if (this->boardTest[7][7] == "-")
    {
        return false;
    }
    else
    {
        return true;
    }
}

Board::~Board() {}
