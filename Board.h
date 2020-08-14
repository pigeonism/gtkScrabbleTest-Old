#ifndef BOARD_H
#define BOARD_H
#include <Dictionary.h>
#include <vector>
#include <string>
#include <gtk/gtk.h>
#include <vector>
#include <Formatter.h>
#include "Scoring.h"

using namespace std;

class Board
{
    public:

        Board();
        Board(Scoring *gameScoreB);
        virtual ~Board();

        string setColourTag(string pos);
        void buildBoard();
        void printBoard();
        string alphas;
        void updateBoard(string positionXY, string letter, string method);
        bool centerSquareCheck();
        void buildButtons();
        vector<vector<GtkWidget*>>buttonsBoard;
        void mergeBoards();
        bool checkValidMove(vector<vector<string>> playerMoves, int round, string whichPlayer);
        vector<int>getPositionFromStr(string pos);
        string getInfoFromSquare(int x, int y);
        string getLetterFromPlayerVec(vector<vector<string>> playerMoves, int x, int y);
        bool testIfUniquelySpread(vector<int>arry);
        vector<int>getMissingElements(vector<int>arry);
        void resetButtonLabels();
        string getWordsFromLeft(int x, int y);
        string getWordsFromRight(int x, int y);
        string getWordsFromUp(int x, int y);
        string getWordsFromDown(int x, int y);

        int getBoardVecSize();
        vector<string> getRow(int i);
        bool matchCrossWordLine(vector<int>elementsCrossed, int elementTest);
        void setLabelSelective(string position, string letter);
        void changeColour(GtkButton* buttonWidget);
        void revertColourChanges();
        void resetAllColours();
        void changeColourComputer(string pos);
        vector<vector<string>>colourChanges;

        Formatter format;
        Scoring *gameScoreBoard;
    protected:

    private:

        vector<string>lettersAlphaValue;
        string EMPTY_SQUARE;
        Dictionary dict;
        vector<vector<string>> boardTest;
        vector<vector<string>> boardVec;

        string PLAYER_TAG;
        string COMPUTER_PLAYER_TAG;

        vector<string>bonusPositionsRedTripleWord;
        vector<string>bonusPositionsPinkDoubleWord;
        vector<string>bonusPositionsLightBlueDoubleLetter;
        vector<string>bonusPositionsDarkBlueTripleLetter;
};

#endif // BOARD_H
