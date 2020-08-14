#ifndef HAND_H
#define HAND_H
#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <Formatter.h>

using namespace std;

class Hand
{
    public:

        Hand();
        virtual ~Hand();

        string currentHand;
        string transientLETTER;
        GtkWidget* transientLabel;
        string labelTitleTrans;
        bool transientSELECTED;
        vector<GtkWidget*>buttons;
        void setHand(string letters);
        string removeLetter(string l, string hand);
        void buildHandButtons();
        void updateButtons(string l);
        void relabelAllButtons(string letters, bool completeReset);
        bool replaceButtonLabel(string letter, string replacement);
        void printButtons();
        Formatter format;

    protected:

    private:
};

#endif // HAND_H
