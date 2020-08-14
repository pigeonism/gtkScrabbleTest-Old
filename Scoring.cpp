#include "Scoring.h"
#import <math.h>
//#import <iostream>


Scoring::Scoring()
{
    //ctor
    // Letters with their score value.
    lettersAlphaValue = {"A1","B3","C3","D2","E1","F4","G2","H4","I1","J8","K5","L1","M3","N1","O1","P3","Q10","R1","S1","T1","U1","V4","W4","X8","Y4","Z10"," 0"};
    // Letter multiplier positions.
    posRedTripleWord, tripleWord = {"[0,0]","[0,7]","[0,14]","[7,0]","[7,14]","[14,0]","[14,7]","[14,14]"};
    posPinkDoubleWord, doubleWord = {"[7,7]","[1,1]", "[2,2]","[3,3]","[4,4]","[1,13]","[2,12]","[3,11]","[4,10]","[13,1]","[12,2]","[11,3]","[10,4]","[13,13]","[12,12]","[11,11]","[10,10]"};
    posLightBlueDoubleLetter, doubleLetter = {"[6,6]","[6,8]","[8,6]","[8,8]","[2,6]","[2,8]","[3,7]","[6,12]","[8,12]","[7,11]","[11,7]","[12,6]","[12,8]","[6,2]","[8,2]","[7,3]","[0,3]","[0,11]","[3,0]","[3,14]","[11,0]","[11,14]","[14,3]","[14,11]"};
    posDarkBlueTripleLetter, tripleLetter = {"[1,5]","[1,9]","[5,13]","[9,13]","[13,5]","[13,9]","[5,1]","[9,1]","[5,5]","[5,9]","[9,5]","[9,9]"};
    // User and computer scores.
    playerBoardScore = 0;
    computerBoardScore = 0;
    // Tags used to decide who to apply scoring to. These should be the same throughout the game.
    PLAYER_TAG = "user";
    COMPUTER_PLAYER_TAG = "computer";
}

void Scoring::resetBonusTilePositions()
{
    tripleWord = posRedTripleWord;
    doubleWord = posPinkDoubleWord;
    doubleLetter = posLightBlueDoubleLetter;
    tripleLetter = posDarkBlueTripleLetter;
}
// TODO (euser#1#03/16/19): Double letter not adding correctly on singles - score again with player moves bonus positions, here not in board. post scoring;
// double just add letter value to letter val, triple, just add twice etc
// Apply bonuses after initial scoring.
int Scoring::postScoreBonus(vector<vector<string>> playerMoves, int score)
{

    // If a single letter gained chain words, apply bonuses to them if the single letter is on a bonus square.
    int wordTriple = 0;
    int wordDouble = 0;
    int total = score; // initial score
    for(int i = 0; i < playerMoves.size(); i++)
    {
        // playerMoves[i][0]                 playerMoves[i][1]
        string position = playerMoves[i][0]; string letter = playerMoves[i][1];
        if(!tripleWord.empty())
        {
            for(int t = 0; t < tripleWord.size(); t++)
            {
                if(position == tripleWord[t])
                {
                    wordTriple++; // dont break incase the word crosses two
                    tripleWord.erase(tripleWord.begin() + t);
                }
            }
        }

        if(!doubleWord.empty())
        {
            for(int d = 0; d < doubleWord.size(); d++)
            {
                if(position == doubleWord[d])
                {
                    wordDouble++; // dont break incase the word crosses two
                    doubleWord.erase(doubleWord.begin() + d);
                }
            }
        }

        if(!tripleLetter.empty())
        {
            for(int t = 0; t < tripleLetter.size(); t++)
            {
                if(position == tripleLetter[t])
                {
                     int value = getLetterValue(letter);
                     // just add value twice, as this is a post score the letter is already included in the
                     // original score.
                     total += (value * 2); // initial letter value is already scored
                     tripleLetter.erase(tripleLetter.begin() + t);
                }
            }
        }

        if(!doubleLetter.empty())
        {
            for(int d = 0; d < doubleLetter.size(); d++)
            {
                if(position == doubleLetter[d])
                {
                    // just add value once, as this is a post score the letter is already included in the
                     // original score.
                    // cout << "(Scoring::postScoreBonus) adding double letter val" << endl;
                    int value = getLetterValue(letter);
                    total += value; // initial letter value is already scored
                    doubleLetter.erase(doubleLetter.begin() + d);
                }
            }
        }
    }
    // Apply bigger bonuses if any.
    if(wordTriple > 0)
    {
        total = total * pow(3,wordTriple);
    }
    if(wordDouble > 0)
    {
        total = total * pow(2,wordDouble);
    }
    return total;
}

int Scoring::postScoreBonusComp(vector<vector<string>> playerMoves, int score)
{
   // This resets the Comp vectors to the previous state of the copies, left by player.
    tripleWordComp = tripleWord;
    doubleWordComp = doubleWord;
    doubleLetterComp = doubleLetter;
    tripleLetterComp = tripleLetter;

    // If a single letter gained chain words, apply bonuses to them if the single letter is on a bonus square.
    int wordTriple = 0;
    int wordDouble = 0;
    int total = score; // initial score
    for(int i = 0; i < playerMoves.size(); i++)
    {
        // playerMoves[i][0]                 playerMoves[i][1]
        string position = playerMoves[i][0]; string letter = playerMoves[i][1];
        if(!tripleWordComp.empty())
        {
            for(int t = 0; t < tripleWordComp.size(); t++)
            {
                if(position == tripleWordComp[t])
                {
                    wordTriple++; // dont break incase the word crosses two
                    tripleWordComp.erase(tripleWordComp.begin() + t);
                }
            }
        }

        if(!doubleWordComp.empty())
        {
            for(int d = 0; d < doubleWordComp.size(); d++)
            {
                if(position == doubleWordComp[d])
                {
                    wordDouble++; // dont break incase the word crosses two
                    doubleWordComp.erase(doubleWordComp.begin() + d);
                }
            }
        }

        if(!tripleLetterComp.empty())
        {
            for(int t = 0; t < tripleLetterComp.size(); t++)
            {
                if(position == tripleLetterComp[t])
                {
                     int value = getLetterValue(letter);
                     // just add value twice, as this is a post score the letter is already included in the
                     // original score.
                     total += (value * 2); // initial letter value is already scored
                     tripleLetterComp.erase(tripleLetterComp.begin() + t);
                }
            }
        }

        if(!doubleLetterComp.empty())
        {
            for(int d = 0; d < doubleLetterComp.size(); d++)
            {
                if(position == doubleLetterComp[d])
                {
                    // just add value once, as this is a post score the letter is already included in the
                     // original score.
                     //cout << "(Scoring::postScoreBonusComp) adding double letter val" << endl;
                    int value = getLetterValue(letter);
                    total += value; // initial letter value is already scored
                    doubleLetterComp.erase(doubleLetterComp.begin() + d);
                }
            }
        }
    }
    // Apply bigger bonuses if any.
    if(wordTriple > 0)
    {
        total = total * pow(3,wordTriple);
    }
    if(wordDouble > 0)
    {
        total = total * pow(2,wordDouble);
    }
    return total;
}
// Get the associated value of a letter from letters.
int Scoring::getLetterValue(string character)
{
    for(int elem = 0; elem < lettersAlphaValue.size(); elem++  )
    {
        if(character == lettersAlphaValue.at(elem).substr(0,1))
        {
            return std::stoi( lettersAlphaValue.at(elem).substr(1,-1) );
        }
    }
    return 0;
}

// Score chainwords. Chainwords are words made surrounding a player partial word or a player's fullword.
int Scoring::scoreChainWords(vector<string>chainWordsIn)
{
    int score = 0;
    for(int i = 0; i < chainWordsIn.size(); i++)
    {
        for(int j = 0; j < chainWordsIn[i].size(); j++)  // the word from row size
        {
            string tempLetter(1, chainWordsIn[i].at(j));
            //cout << "(Board::scoreChainWords) scoring letter: '" << tempLetter << "' ";
            score += getLetterValue(tempLetter);
        }
        //cout << endl;
    }
    return score;
}
// Keep track of scores by incrementing a value.
void Scoring::setBoardScore(int number, string whichPlayer)
{
    if(whichPlayer == PLAYER_TAG)
    {
        playerBoardScore += number;
    }
    if(whichPlayer == COMPUTER_PLAYER_TAG)
    {
        computerBoardScore += number;
    }
}
// Modify Score
// Keep track of scores by re-assigning value.
void Scoring::modifyBoardScore(int number, string whichPlayer)
{
    if(whichPlayer == PLAYER_TAG)
    {
        playerBoardScore = number;
    }
    if(whichPlayer == COMPUTER_PLAYER_TAG)
    {
        computerBoardScore = number;
    }
}
// Reset score.
void Scoring::resetScore(string whichPlayer)
{
    if(whichPlayer == PLAYER_TAG)
    {
        playerBoardScore = 0;
    }
    if(whichPlayer == COMPUTER_PLAYER_TAG)
    {
        computerBoardScore = 0;
    }
}

// Retrieve score when needed.
int Scoring::getBoardScore(string whichPlayer)
{
    if(whichPlayer == PLAYER_TAG)
    {
        return playerBoardScore;
    }
    if(whichPlayer == COMPUTER_PLAYER_TAG)
    {
        return computerBoardScore;
    }
}

// Computer player only method, uses the vectors last changed to assign back to these copies;
// tripleWord doubleWord doubleLetter tripleLetter
void Scoring::updateLatestCompBonusChanges()
{
    tripleWord = tripleWordComp;
    doubleWord = doubleWordComp;
    doubleLetter = doubleLetterComp;
    tripleLetter = tripleLetterComp;

}
Scoring::~Scoring()
{
    //dtor
}
