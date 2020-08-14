#include "HighScore.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

HighScore::HighScore()
{
    // Maximum lines to hold
    maxLines = 10;
}

void HighScore::addHighScore(int score, const string whichPlayer)
{
    // Get the time and date.
    auto sysClock = std::chrono::system_clock::now();
    std::time_t endTime = std::chrono::system_clock::to_time_t(sysClock);
    //std::ctime(&endTime);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&endTime), "%Y-%m-%d %X");   // iomanip put_time
    string timey = ss.str();

    // Open a text file for writing.
    string fileName = "highscores/scores.txt";
    std::ofstream out(fileName, ios::app);
    if (out.good())
    {
        out << timey << " " << whichPlayer << ": " <<  to_string(score) << "\n";
        out.close();
    }
}

vector<string> HighScore::getHighScores()
{
    string fileName = "highscores/scores.txt";
    std::ifstream in(fileName);
    vector<string> highScores;
    if (in.good())
    {
        string lineInFile;
        // Read until end of file. Use 'getline' otherwise 'in' gets space delimited characters or words from a file.
        while (std::getline(in, lineInFile) )
        {
            highScores.push_back(lineInFile);
        }
        in.close();
    }
    return highScores;
}

void HighScore::sortHighScores()
{
    vector<string> listScores = getHighScores();
    if(!listScores.empty())
    {
        map <int, string> scoresVecMap;
        vector<int>sortScores;
        for(int i = 0; i < listScores.size(); i++)
        {
            int semi = listScores[i].rfind(":") +2; // skip space and colon by adding 2
            int val = std::stoi(  listScores[i].substr(semi, -1)  );
            string dateName = listScores[i].substr(0, semi); // include space and colon adding 2
            //cout << " '" << dateName<<"' " << endl;
            scoresVecMap[val] = dateName;
            sortScores.push_back(val);
        }
        // Use the 'sortScores' integer vector element as s key for the scoresVecMap. Here the sort is reversed so highest is first.
        std::sort(std::begin(sortScores), std::end(sortScores),std::greater<int>());
        // NewScores will have no duplicates and will be sorted from highest first.
        vector<string> newScores;
        for(int score: sortScores)
            newScores.push_back( scoresVecMap[score] + to_string(score) );

        // Reduce high scores vector if over ten elements (all ignored elements are lower scores).
        if(newScores.size() > 10)
        {
            vector<string> tempScores;
            for(int i = 0; i < maxLines; i++)
            {
                tempScores.push_back(newScores[i]);
            }
            newScores.clear();
            newScores = tempScores;
        }
        // Open scores text file for writing leaving out  ios::app to overwrite it.
        string fileName = "highscores/scores.txt";
        std::ofstream out(fileName);
        if (out.good())
        {
            for(string scoreUpdated: newScores)
                out << scoreUpdated << "\n";
            out.close();
        }
    }
}

HighScore::~HighScore(){}
