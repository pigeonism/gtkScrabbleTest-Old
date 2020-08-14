#ifndef SOUNDS_H
#define SOUNDS_H
#include <canberra.h>

// Currently using the working directory of the project. Most of the time sounds will be in /usr/share/sounds/.
// So move them later.
#include <string>

using namespace std;

class Sounds
{
    public:
        Sounds();
        virtual ~Sounds();
        void playSound(int action);

    protected:

        ca_context *context;
        //ca_proplist *p;
        int retSound;
        void init();

    private:

        bool initStarted;
        bool soundEnabled;
        string currentDirectory;
        string tempSoundsFolder;

        string buttonPressedFile;
        string playPressedFile;
        string negativeActionFile;
        //actions same consts in main
        int SOUND_BUTTON_PRESSED;
        int SOUND_PLAY_PRESSED;
        int SOUND_NEGATIVE;
};

#endif // SOUNDS_H
