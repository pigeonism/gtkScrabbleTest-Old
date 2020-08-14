#ifndef BAG_H
#define BAG_H
#include <vector>
#include <string>

using namespace std;

class Bag
{
    public:

        Bag();
        virtual ~Bag();
        void buildBag();
        string getLetters(int amount);
        int getRandomInteger(int begin, int end);
        void printBag();
        int getBagSize();
        bool isBagEmpty();

    protected:

    private:

        void shuffleBag(vector<string>&vecBag);
        vector<string>bagBase;
        vector<string>bag;
};

#endif // BAG_H
