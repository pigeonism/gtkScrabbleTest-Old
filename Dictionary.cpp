#include "Dictionary.h"
#include <fstream>
#include <iostream>
#include <map>

Dictionary::Dictionary()
{
    // This string represents all the keys for the map 'alphaWordVector', they key will be a char of a letter at each position.
    strDictAlphaIndex = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

// File names here are comprised of a single letter, each a letter of the alphabet containing all the words beginning with that letter.
void Dictionary::buildRefinedDictionaryList()
{
    for(int i = 0; i < strDictAlphaIndex.size(); i++)
    {
        vector<string>dList;
        string fileName = "dictionary/letters/"+strDictAlphaIndex.substr(i,1)+".txt";
        std::ifstream in(fileName);

        if (in.good())
        {
            dList.clear();
            string value;
            // Read until end of file
            while (in >> value)
            {
                dList.push_back(value);
            }
            in.close();
        }
        alphaWordVector[strDictAlphaIndex.at(i)] = dList;
    }
}

bool Dictionary::dictionaryCheckList(vector<string> dictionaryList, string word)
{
    for(string elem: dictionaryList)
    {
        if(elem == word)
        {
            return true;
        }
    }
    return false;
}

bool Dictionary::dictionaryCheckWord(string word)
{
    //cout << endl;
    char keyLetter = word.at(0);
    //cout << "(Dictionary::dictionaryCheckWord) keyletter: " << keyLetter << endl;
    for(int i = 0; i < strDictAlphaIndex.size(); i++ )
    {
        if(keyLetter == strDictAlphaIndex.at(i))
        {
            //cout << "(Dictionary::dictionaryCheckWord) key match: " << keyLetter << endl;
            if( dictionaryCheckList(alphaWordVector[keyLetter], word) == true )
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

vector<string> Dictionary::getAlphaWordList(char keyLetter)
{
    bool indexCorrect = false;
    vector<string> wordList;

    for(char alpha: strDictAlphaIndex) if(alpha == keyLetter) indexCorrect = true;

    if (indexCorrect == true)
    {
        wordList = alphaWordVector[keyLetter];
        return wordList;
    }

}

void Dictionary::printDict()
{
    for(char letter: strDictAlphaIndex)
        for(string word: alphaWordVector[letter])
            cout << word << endl;
    cout << endl;
}

Dictionary::~Dictionary() {}
