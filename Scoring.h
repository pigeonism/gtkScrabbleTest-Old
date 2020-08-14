#ifndef SCORING_H
#define SCORING_H
#include <string>
#include <vector>

using namespace std;

//Shared by Computer.h, Board.h and main
class Scoring
{
    public:
        Scoring();
        virtual ~Scoring();

        int getLetterValue(string character);
        int scoreChainWords(vector<string>chainWordsIn);

        void setBoardScore(int n, string who);
        void modifyBoardScore(int number, string whichPlayer);
        int getBoardScore(string who);
        void resetScore(string who);

        int postScoreBonus(vector<vector<string>> playerMoves, int score);
        int postScoreBonusComp(vector<vector<string>> playerMoves, int score);

        void resetBonusTilePositions();

        void updateLatestCompBonusChanges();

    protected:

    private:

        int playerBoardScore;
        int computerBoardScore;
        string PLAYER_TAG;
        string COMPUTER_PLAYER_TAG;
        vector<string>lettersAlphaValue;

        vector<string>posRedTripleWord, tripleWord;
        vector<string>posPinkDoubleWord, doubleWord;
        vector<string>posLightBlueDoubleLetter, doubleLetter;
        vector<string>posDarkBlueTripleLetter, tripleLetter;

        vector<string>tripleWordComp;
        vector<string>doubleWordComp;
        vector<string>doubleLetterComp;
        vector<string>tripleLetterComp;

};

#endif // SCORING_H
