#ifndef HIGHSCORE_H
#define HIGHSCORE_H
#include <string>
#include <vector>

using namespace std;

class HighScore
{
    public:

        HighScore();
        virtual ~HighScore();

        void addHighScore(int score, const string whichPlayer);
        vector<string> getHighScores();
        void sortHighScores();

    protected:

    private:

        int maxLines;
};

#endif // HIGHSCORE_H
