#ifndef FORMATTER_H
#define FORMATTER_H
#include <gtk/gtk.h>
#include <string>
#include <vector>

using namespace std;

class Formatter
{
    public:
        Formatter();
        virtual ~Formatter();

        void changeMarkUp(GtkWidget *widget, string letter);
        void changeToolTipMarkUpInvis(GtkWidget *widget, string indexString);
        int getLetterValue(string character);

    protected:

    private:

        vector<string>letters;
        const char* normalFormat;
        const char* specialFormat;
};

#endif // FORMATTER_H
