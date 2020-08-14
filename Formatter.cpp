#include "Formatter.h"

Formatter::Formatter()
{
    letters = {"A1","B3","C3","D2","E1","F4","G2","H4","I1","J8","K5","L1","M3","N1","O1","P3","Q10","R1","S1","T1","U1","V4","W4","X8","Y4","Z10"," 0"};
    // Note the space at the beginning, this is to help center the tile Letter because now it will have a subscript number next to it.
    normalFormat = " \%s<span size=\"xx-small\"> <sub>%d </sub></span>";
    // Some letters have values of ten (Q and Z), so the padding makes them wider than other tiles, so every value (%d) for these has no space after it.
    specialFormat = " \%s<span size=\"xx-small\"> <sub>%d</sub></span>";
}

// This method changes the label style.
void Formatter::changeMarkUp(GtkWidget *widget, string letter)
{
    int letterValue = 0;
    const char *format = normalFormat;
    if(letter == "Z" || letter == "Q") format = specialFormat;
    char *markup;
    const char *str = letter.c_str();
    letterValue = getLetterValue(letter);
    //gtk_label_set_use_markup(GTK_LABEL( gtk_bin_get_child( GTK_BIN(button) ) ), TRUE);
    markup = g_markup_printf_escaped (format, str, letterValue);
    gtk_label_set_markup(GTK_LABEL( gtk_bin_get_child( GTK_BIN(widget) ) ),markup);
}

// Get the associated value of a letter from letters.
int Formatter::getLetterValue(string character)
{
    int lettersSize = letters.size();
    for(int elem = 0; elem < lettersSize; elem++  )
    {
        if(character == letters.at(elem).substr(0,1))
        {
            return std::stoi( letters.at(elem).substr(1,-1) );
        }
    }
    return 0;
}

Formatter::~Formatter(){}
