#include "Sounds.h"
#include <stdlib.h> // getdir and getenv
#include <unistd.h> //usleep
#include <canberra.h>
#include "glib.h"
#include <iostream>
#include <string>

using namespace std;

Sounds::Sounds()
{
    tempSoundsFolder = "/sounds/";
    currentDirectory = get_current_dir_name();

    buttonPressedFile = currentDirectory + tempSoundsFolder + "button-pressed.ogg";
    playPressedFile = currentDirectory + tempSoundsFolder + "window-slide.ogg";
    negativeActionFile = currentDirectory + tempSoundsFolder + "dialog-error.ogg";

    SOUND_BUTTON_PRESSED = 0; // same consts in main
    SOUND_PLAY_PRESSED = 1;
    SOUND_NEGATIVE = 3;
    //printf(g);
    initStarted = false;
    soundEnabled = true;
    init();
}

void Sounds::init()
{
    retSound = ca_context_create(&context);
    // Context stays valid til used again with another change_props call
    retSound = ca_context_change_props(context,CA_PROP_APPLICATION_NAME, "ScrabbleGtk",
                                      CA_PROP_APPLICATION_ID, "org.gtk.ScrabbleGtkTest",
                                      CA_PROP_WINDOW_X11_SCREEN, getenv("DISPLAY"),
                                      NULL);

    retSound = ca_context_open(context);
    initStarted = true;
}

// TODO (euser#4#03/15/19): Use sounds based on what was pressed or the action, such as passing or pressing play etc.
void Sounds::playSound(int action)
{
    //std::cout << currentDirectory << std::endl;
    if(initStarted)
    {
        if(soundEnabled  )
        {
            //
            string fileName;
            bool useSoundFile = false;

            if(action == SOUND_BUTTON_PRESSED)
            {
                fileName = buttonPressedFile; // if file not found set useSoundFile false
                useSoundFile = true;
            }
            else if(action == SOUND_PLAY_PRESSED)
            {
                fileName = playPressedFile; // if file not found set useSoundFile false
                useSoundFile = true;
            }
            else if(action == SOUND_NEGATIVE)
            {
                fileName = negativeActionFile; // if file not found set useSoundFile false
                useSoundFile = true;
            }
            else
            {
                useSoundFile = false;
            }

            if(useSoundFile == true) // if ca_context_playing () is 0?, 0 is if no sound is playing
            {
                //CA_PROP_MEDIA_FILENAME, "/usr/share/sounds/freedesktop/stereo/onboard-key-feedback.oga"
                // check file exists, then;
                retSound = ca_context_play(context, 2, CA_PROP_MEDIA_FILENAME, fileName.c_str(),CA_PROP_MEDIA_NAME, "Button",CA_PROP_MEDIA_LANGUAGE, "en_EN",NULL);
                //usleep(500000); // 0.5 sec sleep
                //ret = ca_context_destroy(context);
            }
        }
    }
}

Sounds::~Sounds()
{
    retSound = ca_context_destroy(context);
}
