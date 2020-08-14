#ifndef COMPUTER_H
#define COMPUTER_H
#include <Bag.h>
#include <Board.h>
#include <Combinations.h>
#include "Scoring.h"

class Computer
{
    public:

        Computer();
        Computer( Board* board, Scoring *gameScoreC);
        virtual ~Computer();

        bool minDepth;

        inline static string removeMe;
        Board *gameBoardMapComp;

        bool playComputerHand(int round, string hand);
        void printBlockHorizontal(string block);
        vector<vector<int>> getCoordinatesFromString(string coord);
        void buildMove(vector<vector<int>>coordinates, vector<string> comboVector, string hand, int round);
        void buildSingleMoves(vector<vector<int>>coordinates,string hand, int round);
        bool moveSelector(vector<vector<int>>coordinates,int sizeOfarry,Combinations combo, string hand, int round, int orientation);
        bool allLettersSurroundingTest(vector<vector<int>>coordinates, int whichTest);

        vector<vector<string>> getEmptyHorizontalBlocks();
        vector<vector<string>> getEmptyVerticalBlocks();

        Scoring *gameScoreComputer;

    protected:

        vector<vector<string>>goodMoves;
        int goodMovesScore;

    private:
        int VERTICAL_ORIENTATION;
        int HORIZONTAL_ORIENTATION;
        string COMPUTER_PLAYER_TAG;
};

#endif // COMPUTER_H
