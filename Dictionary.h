#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <vector>
#include <string>
#include <map>

using namespace std;

class Dictionary
{
    public:

        Dictionary();
        virtual ~Dictionary();
        void buildRefinedDictionaryList();
        bool dictionaryCheckWord(string word);
        bool dictionaryCheckList(vector<string> dictionaryList, string word);
        vector<string> getAlphaWordList(char keyLetter);
        void printDict();

    protected:

    private:

        map <char, vector<string>> alphaWordVector;
        string strDictAlphaIndex;
};

#endif // DICTIONARY_H
