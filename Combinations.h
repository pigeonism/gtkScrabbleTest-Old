#ifndef COMBINATIONS_H
#define COMBINATIONS_H
#include <string>
#include <vector>

using namespace std;

class Combinations
{
    public:

        Combinations();
        virtual ~Combinations();

        string lettersBase;

        vector<string> pairsVector;
        vector<string> threeVector;
        vector<string> fourVector;
        vector<string> fiveVector;
        vector<string> sixVector;
        vector<string> sevenVector;

        void setLetters(string letters);

        void combinationsPair(int index);
        void combinationsThree(string paired);
        void combinationsFour(string three);
        void combinationsFive(string fours);
        void combinationsSix(string fives);
        void combinationsSeven(string sixes);

        void buildAllCombinations();

    protected:

    private:

};

#endif // COMBINATIONS_H
