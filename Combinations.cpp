#include "Combinations.h"
#include <iostream>
#include <vector>
#include <algorithm>
// for scrabble
//
// TODO (euser#5#03/03/19): reduce code repetition

using namespace std;

Combinations::Combinations() {}

void Combinations::combinationsPair(int index)
{
    string lettersIn = lettersBase;
    //std::cout << "---------------\n";
    string single = lettersIn.substr(index,1);
    // So it's not included in combo. If one letter, the loop won't start.
    lettersIn.erase(lettersIn.begin() + index );

    for(int i = 0; i < lettersIn.size(); i ++)
    {
        string tempPair = single + lettersIn[i];
        pairsVector.push_back(tempPair);

        //cout << "{" << tempPair[0] << "," << tempPair[1] << "}" << endl;
    }
    //std::cout << "---------------\n";
}

void Combinations::combinationsThree(string paired)
{
    string lettersIn = lettersBase;

    string l1 = paired.substr(0,1);
    string l2 = paired.substr(1,1);

    lettersIn.erase(lettersIn.find(l1), 1);
    lettersIn.erase(lettersIn.find(l2), 1);

    for(int i = 0; i < lettersIn.size(); i++)
    {
        string tempThree = paired + lettersIn[i];
        threeVector.push_back(tempThree);
        //cout << tempThree << endl;
    }
}

void Combinations::combinationsFour(string three)
{
    string lettersIn = lettersBase;
    string l1 = three.substr(0,1);
    string l2 = three.substr(1,1);
    string l3 = three.substr(2,1);
    lettersIn.erase(lettersIn.find(l1), 1);
    lettersIn.erase(lettersIn.find(l2), 1);
    lettersIn.erase(lettersIn.find(l3), 1);
    for(int i = 0; i < lettersIn.size(); i++)
    {
        string tempFour = three + lettersIn[i];
        fourVector.push_back(tempFour);
        //cout << tempFour << endl;
    }
}

void Combinations::combinationsFive(string fours)
{
    string lettersIn = lettersBase;
    string l1 = fours.substr(0,1);
    string l2 = fours.substr(1,1);
    string l3 = fours.substr(2,1);
    string l4 = fours.substr(3,1);
    lettersIn.erase(lettersIn.find(l1), 1);
    lettersIn.erase(lettersIn.find(l2), 1);
    lettersIn.erase(lettersIn.find(l3), 1);
    lettersIn.erase(lettersIn.find(l4), 1);
    for(int i = 0; i < lettersIn.size(); i++)
    {
        string tempFive = fours + lettersIn[i];
        fiveVector.push_back(tempFive);
        //cout << tempFive << endl;
    }
}

void Combinations::combinationsSix(string fives)
{
    string lettersIn = lettersBase;
    string l1 = fives.substr(0,1);
    string l2 = fives.substr(1,1);
    string l3 = fives.substr(2,1);
    string l4 = fives.substr(3,1);
    string l5 = fives.substr(4,1);
    lettersIn.erase(lettersIn.find(l1), 1);
    lettersIn.erase(lettersIn.find(l2), 1);
    lettersIn.erase(lettersIn.find(l3), 1);
    lettersIn.erase(lettersIn.find(l4), 1);
    lettersIn.erase(lettersIn.find(l5), 1);
    for(int i = 0; i < lettersIn.size(); i++)
    {
        string tempSix = fives + lettersIn[i];
        sixVector.push_back(tempSix);
        //cout << tempSix<< endl;
    }
}

void Combinations::combinationsSeven(string sixes)
{
    string lettersIn = lettersBase;
    string l1 = sixes.substr(0,1);
    string l2 = sixes.substr(1,1);
    string l3 = sixes.substr(2,1);
    string l4 = sixes.substr(3,1);
    string l5 = sixes.substr(4,1);
    string l6 = sixes.substr(5,1);
    lettersIn.erase(lettersIn.find(l1), 1);
    lettersIn.erase(lettersIn.find(l2), 1);
    lettersIn.erase(lettersIn.find(l3), 1);
    lettersIn.erase(lettersIn.find(l4), 1);
    lettersIn.erase(lettersIn.find(l5), 1);
    lettersIn.erase(lettersIn.find(l6), 1);
    for(int i = 0; i < lettersIn.size(); i++)
    {
        string tempSeven = sixes + lettersIn[i];
        sevenVector.push_back(tempSeven);
        //cout << tempSeven<< endl;
    }
}

// generate all combinations
void Combinations::buildAllCombinations()
{
    // check letterBase Length first
    string letters =  lettersBase;

    if( !letters.empty())
    {
        // If letters is of length 1, the vector will be of one element,
        for(int i = 0; i < letters.size(); i++)
        {
            combinationsPair( i) ;
        }
        for(string pairStr: pairsVector)
            combinationsThree(pairStr);

        for(string threeStr: threeVector)
            combinationsFour(threeStr);

        for(string fourStr: fourVector)
            combinationsFive(fourStr);

        for(string fiveStr: fiveVector)
            combinationsSix(fiveStr);

        for(string sixStr: sixVector)
            combinationsSeven(sixStr);
    }
}

void Combinations::setLetters(string letters)
{
    lettersBase = letters;
}

Combinations::~Combinations()
{}
