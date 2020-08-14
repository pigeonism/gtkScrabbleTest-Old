#include "Hand.h"
#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <iostream>
#include <Formatter.h>

using namespace std;

Hand::Hand()
{
    transientSELECTED = false;
    labelTitleTrans = "Letter in play: ";
    transientLETTER = "-";
}

void Hand::setHand(string letters)
{
    this->currentHand = letters;
    cout << "(Hand::setHand) hand class: " << this->currentHand << endl;
}

string Hand::removeLetter(string letter, string hand)
{
    std::string::size_type i = hand.find(letter);

    if (i != std::string::npos)
        hand.erase(i, letter.length());
    return hand;
}

void Hand::updateButtons(string letter)
{
    // This will put the letter back in the label of any button  with a "-" label,
    for(int x = 0; x < buttons.size(); x++)
    {
        string buttonLabel = gtk_button_get_label( GTK_BUTTON(buttons[x]) );
        if( buttonLabel == "-")
        {
            gtk_button_set_label(GTK_BUTTON(buttons[x]), letter.c_str());
            format.changeMarkUp(buttons[x], letter);
            gtk_widget_show ( buttons[x]);
            // Do this PER LETTER not to all.
            break;
        }
    }
}
// Similar to upDateButtons except it will replace an existing letter with a specified letter, this is mainly
// for replacing spaces.
bool Hand::replaceButtonLabel(string letter, string replacement)
{
    for(int x = 0; x < buttons.size(); x++)
    {
        string buttonLabel = gtk_button_get_label( GTK_BUTTON(buttons[x]) );
        cout << "(Hand::replaceButtonLabel) button label '" << x << "' " << "'"<<buttonLabel<<"'" << endl;
        if( buttonLabel == letter)
        {
            gtk_button_set_label(GTK_BUTTON(buttons[x]), replacement.c_str());
            format.changeMarkUp(buttons[x], replacement);
            // Do this PER LETTER not to all.
            return true;
        }
    }
    return false;
}

void Hand::buildHandButtons()
{
    // Getting the letter value and the letter string from the hand to make a label.
    transientLabel = gtk_label_new(labelTitleTrans.c_str());

    for(int i = 0; i < 7; i++)
    {
        GtkWidget *button = gtk_button_new_with_label(currentHand.substr(i,1).c_str() );
        format.changeMarkUp(button, currentHand.substr(i,1));
        buttons.push_back(button);
    }
}


void Hand::relabelAllButtons(string letters, bool completeReset)
{
    // Remember (singleLetterButtonFunction) in 'main' changes a button label to ("-") and hides it when clicked.
    int lettersLen = letters.size();

    if (completeReset == false)
    {
        int hyphenFound = 0;
        if(!letters.empty())
        {
            for(int x = 0; x < buttons.size(); x++)
            {
                if(hyphenFound == lettersLen) break;    // Prevent out of range errors.

                string buttonLabel = gtk_button_get_label( GTK_BUTTON(buttons[x]) );
                //cout << "(relabelAllButtons) button label '" << x << "' " << buttonLabel << endl;
                if( buttonLabel == "-")
                {
                    string letter = letters.substr(hyphenFound,1);
                    gtk_button_set_label(GTK_BUTTON(buttons[x]), letter.c_str());
                    format.changeMarkUp(buttons[x], letter);
                    gtk_widget_show ( buttons[x]);
                    hyphenFound++;
                }
            }
        }
    }
    // A complete reset re-assigns all tiles regardless of their label.
    // The same loop as above is used, but seperated to help me remember the logic. when a game is started
    // the hand is always 7 and so is the button array in size.
    if (completeReset == true)
    {
        for(int x = 0; x < buttons.size(); x++)
        {
            string buttonLabel = letters.substr(x,1);
            gtk_button_set_label(GTK_BUTTON(buttons[x]), buttonLabel.c_str());
            format.changeMarkUp(buttons[x], buttonLabel);
            gtk_widget_show ( buttons[x]);
        }
    }
}

void Hand::printButtons()
{
    for(int x = 0; x < buttons.size(); x++)
    {
        string buttonLabel = gtk_button_get_label( GTK_BUTTON(buttons[x]) );
        cout << "(Hand::printButtons) button label '" << x << "' " << "'"<<buttonLabel<<"'" << endl;
    }
}

Hand::~Hand() {}
