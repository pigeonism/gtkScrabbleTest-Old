#include "Bag.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

Bag::Bag()
{
    // Letters with their frequency are stored in 'bagBase' (this determines how many of each letter will appear in a bag).
    bagBase = {"E12","A9","I9","O8","N6","R6","T6","L4","S4","U4","D4","G3","B2","C2","M2","P2","F2","H2","V2","W2","Y2","K1","J1","X1","Q1","Z1"," 2"};
}

// Make a 'bag' vector of strings filled with the right amount of letters, using 'bagBase' to fill 'bag'.
void Bag::buildBag()
{
    bag.clear();    // This is done here incase 'buildBag' is called from a reset game method.
    // std::string stuff(4, '.'), b(2, 'c');
    for(string elem: bagBase)
    {
        // Make a string using the std::string constructor method  string(n, char), n being 1st character of the 'bagBase' element.
        int n = std::stoi(elem.substr(1,-1));
        char c = elem.at(0);
        string letterBase(n, c);

        // cout << ":" <<letterBase << ":"<< endl;
        // To stop repeated letters all being one row in the vector split them so the vector is all seperate letters.
        for(char character: letterBase )
        {
            string tempLetter(1, character);
            //cout << ":" <<tempLetter << ":";
            bag.push_back(tempLetter);
        }
        //cout << endl;
    }
    for(string l: this->bag)
        cout << l << ":";
    cout << endl << endl;
    // Shuffle bag to prevent seeing an ordered series of letters in a player hand.
    this->shuffleBag(this->bag);
    //cout << this->bag.size() << endl;
}

// Retrieve a certain amount of letters from the bag. Also shrink the bag so a player is not pulling infinite letters.
string Bag::getLetters(int amount)
{
    // It might be better to make bag a string
    string hand = "";
    if(this->bag.empty()) return hand;
    if(amount == 1 && amount == this->bag.size())
    {
        string t = this->bag.front();
        this->bag.pop_back();
        hand+=t;
        return hand;
    }

    if(amount > 0 && amount <= this->bag.size() )
    {
        vector<string>temp;
        for(int i = 0; i < amount; i ++)
        {
            string t = this->bag.back();
            temp.push_back(t);
            this->bag.pop_back();
        }

        for(int x = 0; x < temp.size(); x++)
        {
            hand+= temp[x];
        }
        temp.clear();
        return hand;

    }
    else
    {
        return hand;
    }
}

// Return a random integer in a specified range. This is used once in main, so keep it here for now.
int Bag::getRandomInteger(int begin, int end)
{
    if (begin >= end)
    {
        return 0;
    }
    else
    {
        srand(time(0));
        int range = end - begin;
        return begin + rand() % range;
    }
}

void Bag::shuffleBag(vector<string>&vecBag)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rn(seed);
    std::shuffle(std::begin(vecBag), std::end(vecBag), rn);
}

void Bag::printBag()
{
    cout << "bag: " << endl;
    for(string elem: this->bag)
        cout << elem;
    //cout << endl;
}

bool Bag::isBagEmpty()
{

    if(this->bag.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}
int Bag::getBagSize()
{
    return this->bag.size();
}

Bag::~Bag() {}
