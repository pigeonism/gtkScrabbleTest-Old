#include <iostream>
#include <fstream>
#include <Bag.h>
#include <Board.h>
#include <Hand.h>
#include "Computer.h"
#include <Formatter.h>
#include "HighScore.h"
#include "Sounds.h"
#include "Scoring.h"
#include <gtk/gtk.h>
// To show messages: Properties>Build Targets>Type (console from gui).
// Sound lib Project>Linker settings: `pkg-config --cflags --libs glib-2.0 libcanberra`
// For 'boardButton' styling in CSS replicate 'YellowButton' passing values to prevent the quick resize.

// Wayne Warren Feb 2019
using namespace std;

// Scrabble related classes.
Hand playertestHand;                // Current player hand that grabs letters from Bag.
Hand computerTestHand;
Bag testBag;                        // Contains all available letters.
Board gameBoardMap;                 // Maintains the game board state internally and does the rule checking.
Computer computerPlayer;            // Obj empty constructor (reassigned in main with board as parameter).
Formatter format;                   // For changing label text markup.
HighScore highScores;               // Maintains the text file used for highscores.
Sounds gameSound;
Scoring gameScore;
Scoring *gameScorePtr = &gameScore;

// Values that remain the same through an entire game.
// TODO (euser#1#03/17/19): as many of these constants are used with different classes, create a base class to hold these.
const int MAX_HAND_LENGTH = 7;      // 7 is the maximum tiles a player can have.
const int MAX_PASSES = 3;           // Maximum passes allowed before a game reset.
const string PLAYER_SCORE_TITLE = "Player score: ";
const string COMPUTER_SCORE_TITLE = "Computer score: ";
const string PLAYER_TAG = "user";   // These tags should remain the same throughout the game (used in Board.h too).
const string COMPUTER_PLAYER_TAG = "computer";
int SOUND_BUTTON_PRESSED = 0;
int SOUND_PLAY_PRESSED = 1;
int SOUND_NEGATIVE = 3;
// Widgets that need to be accessed outside of seperate functions, initialized in void activate() with the others.
GtkWidget *labelPlayerScore;
GtkWidget *labelComputerScore;
GtkWidget *alphaLowerBox;
//These are hidden when the alphaLowerBox is shown.
GtkWidget *buttonGridPlayer;
GtkWidget *buttonLowerBoxPassPlay;

// Local to main, objects and variables that track game progress, scores, moves, passes etc.
vector<vector<string>> playerMoves;
int gameRound = 0;
int playerPasses = 0;
int playerScore = 0;
int computerScore = 0;
int computerPasses = 0;
bool computerGo = false;
bool fetchTiles = true;

// Helpers for computerMoveFunction().
string getRandomAlpha();
string removeBlanks(string hand);   // This is only for the computer hand.
string getLettersToRemove();
bool attemptComputerMove();
// Test for a win then show the dialog for the player.
void checkWinCondition(string whichPlayer, string hand);
void checkPassWinCondition();
bool messageDialogWin (string whichPlayer);
void deductFromScore(string hand, string whichPlayer);

// Keep track of moves for the board.
void updateMove(string pos, string letter)
{
    vector<string> oneMove{"",""};
    oneMove[0] = pos;
    oneMove[1] = letter;
    playerMoves.push_back(oneMove);
}

// For every row in <vector>Playermoves there are two elements;
// 0 - the position of the button represented as label text.
// 1 - the letter itself.
void resetMoves()
{
    if(playerMoves.size() > 0)
    {
        for(int row = 0; row < playerMoves.size(); row++)
        {
            // Put a letter back in a button with "-" as a label.
            playertestHand.currentHand += playerMoves[row].at(1);
            playertestHand.updateButtons(playerMoves[row].at(1));
            // Also reset board.
            gameBoardMap.updateBoard(playerMoves[row].at(0), playerMoves[row].at(1), "reset");
        }
    }
    gameBoardMap.revertColourChanges();

    playerMoves.clear();
    gtk_label_set_text (GTK_LABEL(playertestHand.transientLabel), playertestHand.labelTitleTrans.c_str());
    cout << "(Main:resetMoves) player hand reset.. printing buttons" << endl;
    //playertestHand.printButtons();
    //gameBoardMap.printBoard();
}

void resetGame()
{
    computerGo = false;
    gtk_label_set_text (GTK_LABEL(playertestHand.transientLabel), playertestHand.labelTitleTrans.c_str());
    // Possibly update highscore before doing this.
    Computer::removeMe="";
    fetchTiles = true;
    gameRound = 0;
    playerPasses = 0;
    computerPasses = 0;
    // Re assign pointer to the newly created obj.
    gameScore = Scoring();
    gameScorePtr = nullptr;
    gameScorePtr = &gameScore;
    //gameScorePtr->resetBonusTilePositions(); // So bonuses will be valid again.
    playerScore = 0;
    computerScore = 0;
    //gameScorePtr->resetScore(PLAYER_TAG);
    //gameScorePtr->resetScore(COMPUTER_PLAYER_TAG);
    gtk_label_set_text (GTK_LABEL(labelPlayerScore), PLAYER_SCORE_TITLE.c_str());
    gtk_label_set_text (GTK_LABEL(labelComputerScore), COMPUTER_SCORE_TITLE.c_str());
    gtk_widget_hide(GTK_WIDGET(alphaLowerBox));

    // Reset player moves.
    playerMoves.clear();
    // Rebuild board.
    gameBoardMap.buildBoard();          // Sets the text strings of "-" in the board vector (not buttonsVector).
    gameBoardMap.resetButtonLabels();   // Sets labels of buttons to (" ").
    gameBoardMap.resetAllColours();     // Reset colours.

    // By building a new bag, the old bag is gone.
    testBag.buildBag();
    // Reset the player's hand.
    playertestHand.currentHand ="";
    string newLettersFromBag = testBag.getLetters(MAX_HAND_LENGTH);
    playertestHand.setHand(newLettersFromBag);
    //
    cout << "(Main:resetGame) game reset, building new player buttons..." << endl;
    playertestHand.relabelAllButtons(newLettersFromBag, true); // A reset of true means new letters are applied.
    // Set a new computer hand.
    computerTestHand.setHand(testBag.getLetters(MAX_HAND_LENGTH));
    computerTestHand.currentHand = removeBlanks(computerTestHand.currentHand);
}

void buttonAlphaReplace(GtkButton *widget, gpointer data)
{
    gameSound.playSound(SOUND_BUTTON_PRESSED);
    // Swap a space character with the new letter chosen from the alphabet tile window.
    cout << "(Main:buttonAlphaReplace): a button called me.(letter)" << endl;
    playertestHand.transientLETTER = gtk_button_get_label (widget);
    cout << "(Main:buttonAlphaReplace): New transient letter: " << "'"<< playertestHand.transientLETTER<<"'" << endl;
    playertestHand.currentHand = playertestHand.currentHand.erase(playertestHand.currentHand.find(" "), 1);
    playertestHand.currentHand += playertestHand.transientLETTER;
    playertestHand.replaceButtonLabel(" ", playertestHand.transientLETTER);
    cout << "(Main:buttonAlphaReplace) playertestHand.currentHand:" << playertestHand.currentHand << endl;
    playertestHand.transientSELECTED = false;
    gtk_widget_show(buttonGridPlayer);
    gtk_widget_show(buttonLowerBoxPassPlay);
    gtk_widget_hide(GTK_WIDGET(alphaLowerBox));
}

// Get the label text of a button so the 'buttonBoard' can use it and also 'blank' the tile used.
bool singleLetterButtonFunction (GtkButton *widget, gpointer data)
{
    gameSound.playSound(SOUND_BUTTON_PRESSED);
    // Hide the button pressed (*widget) and place its text label on the board.
    playertestHand.transientLETTER = gtk_button_get_label (widget);
    cout << "(Main:singleLetterButtonFunction) transient letter: '" << playertestHand.transientLETTER << "' "<< endl;
    // Change the transientLetter if it is blank.
    if(playertestHand.transientLETTER == " ")
    {
        gtk_widget_hide(buttonGridPlayer);
        gtk_widget_hide(buttonLowerBoxPassPlay);
        gtk_widget_show(GTK_WIDGET(alphaLowerBox));
        return true;
    }

    string title = playertestHand.labelTitleTrans + playertestHand.transientLETTER;
    gtk_label_set_text (GTK_LABEL(playertestHand.transientLabel), title.c_str());

    // This hides the button clicked and then sets its label to a hypehn.
    if(playertestHand.transientLETTER != "-" && playertestHand.transientSELECTED == false)
    {
        playertestHand.transientSELECTED = true;
        gtk_widget_hide ( GTK_WIDGET(widget));
        // Set the widget with a hyphen as a label so that letter is not used again elsewhere.
        gtk_button_set_label(widget, "-");
        playertestHand.currentHand = playertestHand.removeLetter(playertestHand.transientLETTER, playertestHand.currentHand);
    }
    //gtk_widget_set_sensitive (widget, false);
    cout << "(Main:singleLetterButtonFunction) Pressed a letter from player hand...clicked: " << endl;
    return true;
}

// When a button of the board is clicked the transient letter is placed on it by
// changing the label of the board's button to it.
static void buttonBoard (GtkButton *widget, gpointer data)
{
    gameSound.playSound(SOUND_BUTTON_PRESSED);
    cout << "(Main:buttonBoard) Pressed a label cell of board..." << endl;

    // Get position before the button on the board's label is changed.
    // string position = gtk_button_get_label (widget);
    string position = gtk_widget_get_tooltip_text (GTK_WIDGET(widget));

    // A test for checking if the player is pressing same board tile.
    string currentLabel = "";
    if ( gtk_button_get_label (widget ) ) currentLabel = gtk_button_get_label (widget ) ;
    cout << "(Main:buttonBoard) currentLabel: '" << currentLabel << "' "<< endl;

    if(playertestHand.transientSELECTED && position.length() > 1 && playertestHand.transientLETTER != "-" && currentLabel== " ")
    {
        gtk_button_set_label (widget, playertestHand.transientLETTER.c_str());
        // Set the selected back to false, so next time a transient letter can be selected again.
        playertestHand.transientSELECTED = false;
        // Keep track of moves in main, and only test when play is clicked.
        updateMove(position, playertestHand.transientLETTER);
        //gtk_button_set_always_show_image (widget,TRUE);
        gameBoardMap.changeColour(widget);
        // Changing the appearence of the text so it fits with the appearence of all other letter buttons.
        format.changeMarkUp(GTK_WIDGET(widget), playertestHand.transientLETTER);
    }
    else
    {
        cout << "(Main:buttonBoard) double placing or invalid placement. letter: '" << playertestHand.transientLETTER << "' " << endl;
    }
}

// Play the computer hand only when the thread function 'attemptComputerMove()' finds 'computerGo' to be true (which calls this function).
static void computerMoveFunction ()
{
    if(computerGo == true)
    {
        bool winConditionTested = false;
        //checkWinCondition(PLAYER_TAG, playertestHand.currentHand);
        // The first check to make. Trying to fetch tiles from an empty bag will cause errors later, so avoid that here.
        if(testBag.isBagEmpty())fetchTiles = false;

        if(computerPlayer.playComputerHand(gameRound, computerTestHand.currentHand) == true)
        {
            // Update the score and the score label.
            computerScore = gameScorePtr->getBoardScore(COMPUTER_PLAYER_TAG);
            string compScoreUpdated = COMPUTER_SCORE_TITLE + to_string(computerScore);
            gtk_label_set_text (GTK_LABEL(labelComputerScore), compScoreUpdated.c_str());
            gameRound++;
            gameBoardMap.mergeBoards();         // Merge boards again but with a computer played move in it.
            gameBoardMap.printBoard();
            computerGo = false;

            computerPasses = 0;                 // Played a good move, so reset passes.
            // Remove played letters from the computer's hand.
            string removeThese = Computer::removeMe;
            if(!removeThese.empty())
            {
                for(int c = 0; c < removeThese.size(); c++)
                {
                    string letter(1, removeThese[c]);
                    computerTestHand.currentHand = computerTestHand.removeLetter(letter, computerTestHand.currentHand );
                }
            }

            if(testBag.isBagEmpty() )
            {
                cout <<"(Main:computerMoveFunction) COMPUTER: bag empty not fetching any tiles." << endl;
                fetchTiles = false;
            }

            if(fetchTiles == true)
            {
                // Update the hand with new letters.
                int lenCurrentHand = computerTestHand.currentHand.length();
                cout << "(Main:computerMoveFunction) COMPUTER:  hand len: " << lenCurrentHand << endl;
                int replaceMentLen = MAX_HAND_LENGTH - lenCurrentHand;
                int availableTilesFromBag = testBag.getBagSize();

                if(replaceMentLen < availableTilesFromBag )
                {
                    cout << "(Main:computerMoveFunction) COMPUTER:getting this many tiles: " << replaceMentLen << endl;
                    string newLettersFromBag = testBag.getLetters(replaceMentLen);
                    computerTestHand.currentHand += newLettersFromBag;
                    computerTestHand.currentHand = removeBlanks(computerTestHand.currentHand);
                    checkWinCondition(COMPUTER_PLAYER_TAG, computerTestHand.currentHand);
                    winConditionTested  = true;
                }
                else if(replaceMentLen == availableTilesFromBag)
                {
                    cout << "(Main:computerMoveFunction) COMPUTER:getting last tile: " << replaceMentLen << endl;
                    string newLettersFromBag = testBag.getLetters(replaceMentLen);
                    computerTestHand.currentHand += newLettersFromBag;
                    computerTestHand.currentHand = removeBlanks(computerTestHand.currentHand);
                    checkWinCondition(COMPUTER_PLAYER_TAG, computerTestHand.currentHand);
                    fetchTiles = false;
                    winConditionTested = true;
                }
                else
                {
                    cout << "(Main:computerMoveFunction) COMPUTER: not enough in bag, so adding (" <<  availableTilesFromBag << ") left from bag to hand."<<  endl;
                    string newLettersFromBag = testBag.getLetters( availableTilesFromBag );
                    fetchTiles = false;
                    if(newLettersFromBag.size() > 0)
                    {
                        computerTestHand.currentHand += newLettersFromBag;
                        computerTestHand.currentHand = removeBlanks(computerTestHand.currentHand);
                        cout << "(Main:computerMoveFunction) COMPUTER: bag is empty now. bag size: " <<testBag.getBagSize()  << endl;
                    }
                    checkWinCondition(COMPUTER_PLAYER_TAG, computerTestHand.currentHand);
                    winConditionTested = true;
                }
            }

            // The game also ends when there are no tiles left in hand and no tiles in bag.
            if(!winConditionTested )checkWinCondition(COMPUTER_PLAYER_TAG, computerTestHand.currentHand);
        }
        else
        {
            checkWinCondition(COMPUTER_PLAYER_TAG, computerTestHand.currentHand);

            if(computerPasses == MAX_PASSES )
            {
                computerGo = false;
                checkPassWinCondition();
            }
            else
            {
                cout << "(Main:computerMoveFunction) COMPUTER passes..." << endl;
                gameSound.playSound(SOUND_NEGATIVE);
                computerGo = false;
                computerPasses++;
            }
        }
    }
    gameSound.playSound(SOUND_PLAY_PRESSED);
}

// Here 'gameBoardMap' is used to the check validity of a player move.
static void buttonPlayHandFunction (GtkWidget *widget, gpointer data)
{
    gameSound.playSound(SOUND_PLAY_PRESSED);

    computerGo = false;
    bool winConditionTested = false;
    cout << "(Main:buttonPlayHandFunction) Checking Player Hand..." << endl;
    // Update shadow board with mock position and letter forming an unordered hand.
    // This is is then tested in the next condition using 'gameBoardMap.checkValidMove' which orders the letters and checks the positions of them.
    if(!playerMoves.empty())
    {
        for(vector<string> row: playerMoves)
            gameBoardMap.updateBoard(row.at(0), row.at(1), "add");
    }

    // The last parameter of 'checkValidMove' is used to distinguish which player called it.
    if ( gameBoardMap.checkValidMove(playerMoves, gameRound, PLAYER_TAG)  == false )
    {
        cout << "(Main:buttonPlayHandFunction) hand does not cross center or other rule not passed." << endl;
        gameSound.playSound(SOUND_NEGATIVE);
        resetMoves();
    }
    else
    {
        // If the tests above have been passed (we have a valid move) update 'boardVec' vector in gameBoardMap with the 'boardTest' vector.
        gtk_label_set_text (GTK_LABEL(playertestHand.transientLabel), playertestHand.labelTitleTrans.c_str());
        cout << "(Main:buttonPlayHandFunction) main.cpp tests passed" << endl;
        gameRound++;
        playerMoves.clear();
        gameBoardMap.mergeBoards();
        gameBoardMap.printBoard();
        Computer::removeMe="";
        // If a good move is played, also reset the player passes so the player has a chance to pass again in later rounds.
        playerPasses = 0;

        // Clear the colour changes vector. It is no longer needed now we have played a correct move, so there is no reason to undo this.
        gameBoardMap.colourChanges.clear();

        // Only get more letters if there are enough left and only after a good move is played.
        int lenCurrentHand = playertestHand.currentHand.length();

        cout << "(Main:buttonPlayHandFunction): handLength: " << lenCurrentHand << endl;

        if(testBag.isBagEmpty() )
        {
            cout <<"(Main:buttonPlayHandFunction) bag empty not fetching any tiles." << endl;
            fetchTiles = false;
        }

        if(fetchTiles == true)
        {
            int replaceMentLen = MAX_HAND_LENGTH - lenCurrentHand;
            int availableTilesFromBag = testBag.getBagSize();

            if(replaceMentLen < availableTilesFromBag)
            {
                cout << "(Main:buttonPlayHandFunction): getting this many tiles: " << replaceMentLen << endl;
                string newLettersFromBag = testBag.getLetters(replaceMentLen);
                playertestHand.currentHand += newLettersFromBag;
                cout << "(Main:buttonPlayHandFunction): NEW hand: " << playertestHand.currentHand << endl;
                if(availableTilesFromBag > 0 ) playertestHand.relabelAllButtons(newLettersFromBag, false);
                checkWinCondition(PLAYER_TAG, playertestHand.currentHand );
                winConditionTested = true;

            }
            else if(replaceMentLen == availableTilesFromBag)
            {
                cout << "(Main:buttonPlayHandFunction) getting last tile: " << replaceMentLen << endl;
                string newLettersFromBag = testBag.getLetters(replaceMentLen);
                playertestHand.currentHand += newLettersFromBag;
                playertestHand.currentHand = removeBlanks(playertestHand.currentHand);
                playertestHand.relabelAllButtons(newLettersFromBag, false);
                checkWinCondition(PLAYER_TAG, playertestHand.currentHand);
                fetchTiles = false;
                winConditionTested = true;
            }
            else
            {
                cout << "(Main:buttonPlayHandFunction): not enough in bag, so adding (" <<  availableTilesFromBag << ") left from bag to hand."<<  endl;
                string newLettersFromBag = testBag.getLetters( availableTilesFromBag );
                fetchTiles = false;
                if( !newLettersFromBag.empty())
                {
                    playertestHand.currentHand += newLettersFromBag;
                    playertestHand.relabelAllButtons(newLettersFromBag, false); // False means only change hidden tiles, rather than all of them.
                    cout << "(Main:buttonPlayHandFunction): bag is empty now. bag size:" << testBag.getBagSize() <<  endl;
                }
                checkWinCondition(PLAYER_TAG, playertestHand.currentHand );
                winConditionTested = true;
            }
        }
        //cout << "(Main:buttonPlayHandFunction) bag size: " << testBag.getBagSize() << endl;

        // Update the player score and score label.
        playerScore = gameScorePtr->getBoardScore(PLAYER_TAG);
        string scoreUpdated = PLAYER_SCORE_TITLE + to_string(playerScore);
        gtk_label_set_text (GTK_LABEL(labelPlayerScore), scoreUpdated.c_str());
        computerGo = true;
        // The game also ends when there are no tiles in the bag and when the player hand is empty.
        if(!winConditionTested) checkWinCondition(PLAYER_TAG, playertestHand.currentHand );
        // After the the tests on player hand it is time for the computer to try a move on the board.
        //computerGo = true;
    }
    // Start thread for the computer to play, calling attemptComputerMove. Some time is added for player button redraw after fetching tiles.
    cout << "(Main:buttonPlayHandFunction): Player score: "  << playerScore << endl;
    if(computerGo) gdk_threads_add_timeout (1000, GSourceFunc(attemptComputerMove), NULL);
}

// Here the player can pass the current round if no words can be formed. The attemptComputerMove() function tied to
// a thread will see 'computerGo' as true and start the function that plays its move.
void buttonPassRoundFunction(GtkButton *widget, gpointer data)
{
    cout << "(Main:buttonPassRoundFunction) player passed round.." << endl;
    gameSound.playSound(SOUND_NEGATIVE);
    playerPasses++;

    if(playerPasses == MAX_PASSES )
    {
        computerGo = false;
        checkPassWinCondition();
    }
    if(gameRound > 0)
    {
        computerGo = true;
        computerMoveFunction ();
    }
}

// The helper functions below are used for changing the computer hand by removing spaces.
string getRandomAlpha()
{
    string compAlphas = gameBoardMap.alphas;
    int randInt = testBag.getRandomInteger(0, compAlphas.size());
    string randAlpha = compAlphas.substr(randInt,1);
    return randAlpha;
}

string removeBlanks(string hand)
{
    string letters = hand;
    int spaceCount = 0;

    for(int i = 0; i < letters.size(); i++)
    {
        string c = letters.substr(i,1);
        if (c == " ")
        {
            letters.erase(letters.find(c), 1);
            spaceCount++;
        }
    }
    if ( spaceCount > 0 )
    {
        for(int i = 0; i < spaceCount; i++)
        {
            letters += getRandomAlpha();
        }
    }
    return letters;
}

// This is tied to gdk_threads_add_timeout() and currently checks every second to see if the computer can play.
bool attemptComputerMove()
{
    cout << "(main:attemptComputerMove) in thread for computer move " << endl;
    if(computerGo == true)
    {
        computerMoveFunction();
    }
    return false;
}

bool messageDialogDraw ()
{
    computerGo = false;
    GtkWidget *dialog;
    string winMessage = "Game over.\n\nPlayers: " + PLAYER_TAG + " and " + COMPUTER_PLAYER_TAG + " finish the game with a draw.";
    winMessage += "\nScoring " + to_string( gameScorePtr->getBoardScore(PLAYER_TAG) ) + " points!";
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL, winMessage.c_str() );
    gtk_widget_set_name(dialog, "dialogMain");
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);

    resetGame();
    return true;
}

// An end game test used when both players have used up their passes.
void checkPassWinCondition()
{
    computerGo = false;
    cout << "(Main:checkPassWinCondition) Checking for final winner..." << endl;
    // Deduct from score also updates the score in main to the newly reduced score.
    deductFromScore(computerTestHand.currentHand, COMPUTER_PLAYER_TAG);
    deductFromScore(playertestHand.currentHand, PLAYER_TAG);

    if(playerScore > computerScore)
    {
        highScores.addHighScore(playerScore, PLAYER_TAG);
        messageDialogWin (PLAYER_TAG);
    }
    else if (computerScore > playerScore)
    {
        highScores.addHighScore(computerScore, COMPUTER_PLAYER_TAG);
        messageDialogWin (COMPUTER_PLAYER_TAG);
    }
    else
    {
        highScores.addHighScore(computerScore, COMPUTER_PLAYER_TAG);
        highScores.addHighScore(playerScore, PLAYER_TAG);
        // call a draw
        messageDialogDraw();
    }
}
// Check for a win at different stages of the game.
void checkWinCondition(string whichPlayer, string hand)
{
    cout << "(Main:checkWinCondition) Checking win condition for ..." << whichPlayer << endl;
    cout << "(Main:checkWinCondition) Bag size: " << testBag.getBagSize() << endl;
    // The game also ends when there are no tiles in the bag and when the player hand is empty.
    if(whichPlayer == PLAYER_TAG)
    {
        if(testBag.isBagEmpty() && hand.empty())
        {
            computerGo = false;
            cout << "(Main:checkWinCondition) Player played remaining tiles from hand..." << endl << "Player wins!" << endl;
            deductFromScore(computerTestHand.currentHand, COMPUTER_PLAYER_TAG); // Remaining tile value of computers hand removed from computer score.
            highScores.addHighScore(playerScore, PLAYER_TAG);
            messageDialogWin(whichPlayer);
        }
    }
    if(whichPlayer == COMPUTER_PLAYER_TAG)
    {
        if(testBag.isBagEmpty() && hand.empty())
        {
            computerGo = false;
            cout << "(Main:checkWinCondition) Computer played remaining tiles from hand..." << endl << "Computer wins!" << endl;
            deductFromScore(playertestHand.currentHand, PLAYER_TAG);
            highScores.addHighScore(computerScore, COMPUTER_PLAYER_TAG);
            messageDialogWin(whichPlayer);
        }
    }
}

bool messageDialogWin (string whichPlayer)
{
    computerGo = false;
    GtkWidget *dialog;
    string whichScore = to_string( gameScorePtr->getBoardScore(whichPlayer) );
    string winMessage = "Game over.\n\nPlayer: " + whichPlayer + " wins with a final score of " + whichScore + "!";
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL, winMessage.c_str() );
    gtk_widget_set_name(dialog, "dialogMain");
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    resetGame();
    return true;
}

// Deducts the value of a player's hand on every pass on "user" or "computer".
void deductFromScore(string hand, string whichPlayer)
{
    //score += getLetterValue(-letter);
    cout << "(Main:deductFromScore) Deducting point value of hand from score." << endl;
    if(!hand.empty() && gameScorePtr->getBoardScore(whichPlayer) > 0 )
    {
        for(int i = 0; i < hand.size(); i++)
        {
            int value = gameScorePtr->getLetterValue(hand.substr(i,1));
            gameScorePtr->setBoardScore( -value, whichPlayer); // Add a signed integer.
        }
    }
    if(whichPlayer == PLAYER_TAG) playerScore = gameScorePtr->getBoardScore(whichPlayer);
    if(whichPlayer == COMPUTER_PLAYER_TAG) computerScore = gameScorePtr->getBoardScore(whichPlayer);
}

// View previous High Scores
bool viewHighScores()
{
    // In view highscores check size of returned string vector first.
    // Use HighScore::sortHighScores() then HighScore::getHighScores so the dialog is not filled with a lot of lines.
    highScores.sortHighScores();
    vector<string> highScoresFromFile = highScores.getHighScores();
    string messageForDialog = "\nHigh Scores\n\n";
    if(!highScoresFromFile.empty())
    {
        int num = 0;
        for(string scoreRow: highScoresFromFile)
            messageForDialog += to_string(++num) + ":  " + scoreRow + "\n";
    }
    else
    {
        messageForDialog += "No High Scores to view.";
    }
    GtkWidget *dialogScore;
    dialogScore = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL, messageForDialog.c_str() );
    gtk_widget_set_name( dialogScore, "dialogMain");
    gtk_dialog_run (GTK_DIALOG (dialogScore));
    gtk_widget_destroy (dialogScore);
    return true;
}

void viewLegend()
{
    GtkWidget *dialog, *labelRed, *labelPink,*labelDarkBlue,*labelLightBlue, *content_area;
    char *message = "\nMultipliers";
    char *messageRed = "Triple Word";
    char *messagePink = "Double Word";
    char *messageDarkBlue = "Triple Letter";
    char *messageLightBlue = "Double Letter";
    // Create the widgets
    dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL, message );
    gtk_widget_set_name(dialog, "dialogMain");

    content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_widget_set_hexpand(content_area, FALSE);
    gtk_widget_set_vexpand (content_area,FALSE);

    labelRed = gtk_label_new (messageRed);
    labelPink = gtk_label_new (messagePink);
    labelDarkBlue = gtk_label_new (messageDarkBlue);
    labelLightBlue = gtk_label_new (messageLightBlue);
    gtk_widget_set_name(labelRed, "dialogLegendLabelRed");
    gtk_widget_set_name(labelPink, "dialogLegendLabelPink");
    gtk_widget_set_name(labelDarkBlue, "dialogLegendLabelDarkBlue");
    gtk_widget_set_name(labelLightBlue, "dialogLegendLabelLightBlue");

    g_signal_connect_swapped (dialog,"response",G_CALLBACK (gtk_widget_destroy),dialog);

    gtk_container_add (GTK_CONTAINER (content_area), labelRed);
    gtk_container_add (GTK_CONTAINER (content_area), labelPink);
    gtk_container_add (GTK_CONTAINER (content_area), labelDarkBlue);
    gtk_container_add (GTK_CONTAINER (content_area), labelLightBlue);
    gtk_widget_show_all (dialog);
}
// Build all widgets for the main window here.
static void activate (GtkApplication* app, gpointer user_data)
{
    // -------------------- Use gtk_widget_set_name() for referencing widgets in the .css file.
    GtkWidget *boxRoot;
    GtkWidget *window;
    GtkWidget *buttonGridBoxPlayer;
    GtkWidget *labelTransientLetter;
    GtkWidget *buttonlGridBoard;
    GtkWidget *buttonPlayHand;
    GtkWidget *boardGreenTileImg;
    GtkWidget *buttonPass;
    GtkWidget *alphaButton;
    // File Menu related widgets.
    GtkWidget *menuVBox, *menuMain, *menuFileItemHeader, *subMenuFileOptions, *itemFileNewGame, *itemFileQuit;
    GtkWidget *menuViewItemHeader, *subMenuViewOptions, *itemViewHighScore, *itemViewLegend;

    // CSS
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "gameStyle/test.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);

    // MAIN WINDOW
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Scrabble");
    //gtk_window_set_default_size (GTK_WINDOW (window), 680, 680);
    gtk_widget_set_size_request(GTK_WIDGET(window),680, 680);
    gtk_widget_set_name(window, "mainWIndow"); // for css

    // GENERAL CONTAINER
    boxRoot = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
    gtk_widget_set_hexpand(boxRoot, FALSE);
    gtk_widget_set_vexpand (boxRoot,FALSE);

    // BOARD OF BUTTONS - The indexes represent the indexes that will be used for class 'Board'.
    buttonlGridBoard = gtk_grid_new ();
    gtk_widget_set_name(buttonlGridBoard, "gameBoardGrid");
    gtk_grid_set_row_spacing (GTK_GRID(buttonlGridBoard), 4);
    gtk_grid_set_column_spacing (GTK_GRID(buttonlGridBoard), 4);
    gtk_grid_set_row_homogeneous(GTK_GRID(buttonlGridBoard), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(buttonlGridBoard), TRUE);
    gameBoardMap.buildButtons();
    for(int row = 0; row < gameBoardMap.buttonsBoard.size(); row++)
    {
        for(int col= 0; col < gameBoardMap.buttonsBoard.size(); col++)
        {
            //( x, y, col span,row span)
            gtk_grid_attach (GTK_GRID (buttonlGridBoard), gameBoardMap.buttonsBoard[row][col], col, row, 1, 1);
            g_signal_connect (gameBoardMap.buttonsBoard[row][col], "clicked", G_CALLBACK (buttonBoard), NULL);
        }
    }

    // PLAYER LETTER BUTTONS
    playertestHand.buildHandButtons();
    buttonGridPlayer =  gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID(buttonGridPlayer), 4);
    gtk_grid_set_column_spacing (GTK_GRID(buttonGridPlayer), 4);
    gtk_widget_set_hexpand(buttonGridPlayer, FALSE);
    gtk_widget_set_vexpand (buttonGridPlayer,FALSE);
    for(int i = 0; i < playertestHand.buttons.size(); i++)
    {
        gtk_widget_set_name(playertestHand.buttons[i], "playerButton"); //#ButtonYellow + letter val
        gtk_grid_attach(GTK_GRID(buttonGridPlayer), playertestHand.buttons[i],i,0,1,1);
        g_signal_connect (playertestHand.buttons[i], "clicked", G_CALLBACK (singleLetterButtonFunction), NULL);
    }
    buttonGridBoxPlayer = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_name(buttonGridBoxPlayer, "playerbuttonGridBoxPlayer");
    gtk_widget_set_hexpand(buttonGridBoxPlayer, FALSE);
    gtk_widget_set_vexpand (buttonGridBoxPlayer,FALSE);
    gtk_button_box_set_layout ( GTK_BUTTON_BOX(buttonGridBoxPlayer), GTK_BUTTONBOX_CENTER );
    gtk_box_pack_start(GTK_BOX(buttonGridBoxPlayer), buttonGridPlayer,FALSE,FALSE,3);

    // LABELS
    labelComputerScore = gtk_label_new(COMPUTER_SCORE_TITLE.c_str());
    gtk_widget_set_name(labelComputerScore, "infoLabelScore");
    labelPlayerScore = gtk_label_new(PLAYER_SCORE_TITLE.c_str());
    gtk_widget_set_name(labelPlayerScore, "infoLabelScore");
    labelTransientLetter = playertestHand.transientLabel;
    gtk_widget_set_name(labelTransientLetter, "infoLabelScore");

    // PLAY BUTTON
    buttonPlayHand = gtk_button_new_with_label("Play");
    g_signal_connect (buttonPlayHand, "clicked", G_CALLBACK (buttonPlayHandFunction), NULL);
    gtk_widget_set_name(buttonPlayHand, "playHandButton");

    // BUTTON PASS
    buttonPass = gtk_button_new_with_label("Pass");
    g_signal_connect (buttonPass, "clicked", G_CALLBACK (buttonPassRoundFunction), NULL);
    gtk_widget_set_name(buttonPass, "passButton");

    // CONTAINER FOR LOWER BUTTONS (PLAY AND PASS AND UNDO) AND SPINNER
    buttonLowerBoxPassPlay = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_name(buttonLowerBoxPassPlay, "boxPassPlay");
    gtk_button_box_set_layout ( GTK_BUTTON_BOX(buttonLowerBoxPassPlay), GTK_BUTTONBOX_CENTER );
    gtk_box_set_spacing(GTK_BOX(buttonLowerBoxPassPlay), 10);
    gtk_container_add (GTK_CONTAINER (buttonLowerBoxPassPlay), buttonPlayHand);
    gtk_container_add (GTK_CONTAINER (buttonLowerBoxPassPlay), buttonPass);

    // ALPHABET REPLACEMENT TILES
    alphaLowerBox =  gtk_grid_new ();
    gtk_widget_set_name(alphaLowerBox, "hiddenAlphaTileBox");
    gtk_grid_set_row_spacing (GTK_GRID(alphaLowerBox), 7);
    gtk_grid_set_column_spacing (GTK_GRID(alphaLowerBox), 7);
    gtk_grid_set_row_homogeneous(GTK_GRID(alphaLowerBox), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(alphaLowerBox), TRUE);
    int indexAlpha = 0;
    for(int row = 0; row < 2; row++)
    {
        for(int col = 0; col < 13; col++)
        {
            string buttonText = gameBoardMap.alphas.substr(indexAlpha,1);
            alphaButton = gtk_button_new_with_label(buttonText.c_str());
            // Start changing markup. Here only the changeMarkup method is needed.
            format.changeMarkUp(alphaButton, buttonText);
            // End changing markup,
            gtk_widget_set_name(alphaButton, "hiddenAlphaTile"); // for css
            gtk_widget_set_hexpand(alphaButton, FALSE);
            gtk_widget_set_vexpand (alphaButton,FALSE);
            gtk_grid_attach (GTK_GRID (alphaLowerBox), alphaButton, col, row, 1, 1);
            g_signal_connect (alphaButton, "clicked", G_CALLBACK (buttonAlphaReplace), NULL);
            indexAlpha++;
        }
    }

    // GAME MENU -
    menuMain = gtk_menu_bar_new ();
    gtk_widget_set_name(menuMain, "menu");
    // File Menu
    menuFileItemHeader = gtk_menu_item_new_with_mnemonic ("_File");
    subMenuFileOptions = gtk_menu_new ();
    itemFileNewGame = gtk_menu_item_new_with_label ("New Game");
    itemFileQuit = gtk_menu_item_new_with_label ("Quit");
    gtk_menu_shell_append (GTK_MENU_SHELL (subMenuFileOptions), itemFileNewGame);
    gtk_menu_shell_append (GTK_MENU_SHELL (subMenuFileOptions), itemFileQuit);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuFileItemHeader), subMenuFileOptions);
    gtk_menu_shell_append (GTK_MENU_SHELL (menuMain), menuFileItemHeader);
    // View Menu
    menuViewItemHeader = gtk_menu_item_new_with_mnemonic ("_View");
    subMenuViewOptions = gtk_menu_new ();
    itemViewHighScore = gtk_menu_item_new_with_label ("High Scores");
    itemViewLegend = gtk_menu_item_new_with_label ("Legend");
    gtk_menu_shell_append (GTK_MENU_SHELL (subMenuViewOptions), itemViewHighScore);
    gtk_menu_shell_append (GTK_MENU_SHELL (subMenuViewOptions), itemViewLegend);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuViewItemHeader), subMenuViewOptions);
    gtk_menu_shell_append (GTK_MENU_SHELL (menuMain), menuViewItemHeader);

    menuVBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_box_pack_start (GTK_BOX (menuVBox), menuMain, FALSE, TRUE, 0);

    g_signal_connect_swapped (itemFileQuit, "activate", G_CALLBACK (gtk_widget_destroy), window);
    g_signal_connect (itemFileNewGame, "activate", G_CALLBACK (resetGame), NULL);
    g_signal_connect (itemViewHighScore, "activate", G_CALLBACK (viewHighScores), NULL);
    g_signal_connect (itemViewLegend, "activate", G_CALLBACK (viewLegend), NULL);

    // PACK WIDGETS
    gtk_box_pack_start(GTK_BOX(boxRoot), menuVBox,FALSE,FALSE,1);
    gtk_box_pack_start(GTK_BOX(boxRoot), labelComputerScore,FALSE,FALSE,1);
    gtk_box_pack_start(GTK_BOX(boxRoot), buttonlGridBoard,TRUE,TRUE,1);
    gtk_box_pack_start(GTK_BOX(boxRoot), labelPlayerScore,FALSE,FALSE,1);
    gtk_box_pack_start(GTK_BOX(boxRoot), labelTransientLetter,FALSE,FALSE,1);
    gtk_box_pack_start(GTK_BOX(boxRoot), buttonGridBoxPlayer,FALSE,FALSE,1);
    gtk_box_pack_start(GTK_BOX(boxRoot), buttonLowerBoxPassPlay,FALSE,FALSE,1);
    gtk_box_pack_start(GTK_BOX(boxRoot), alphaLowerBox,FALSE,FALSE,1);
    gtk_container_add (GTK_CONTAINER (window), boxRoot);
    gtk_widget_show_all (window);

    // Things to set just after adding widgets to the window (such as hiding the replacement tiles box).
    gtk_widget_hide (GTK_WIDGET(alphaLowerBox));
    //gtk_widget_set_can_default (buttonPlayHand, TRUE); // for pressing enter, statement below is needed too
    //gtk_widget_grab_default (buttonPlayHand);
}

int main(int argc, char **argv)
{
    // ############################################################ SCRABBLE ######################
    cout << "(Main): Hello testing scrabble" << endl;
    // Set up the internal board that saves the game state, and also make board button widgets
    gameBoardMap = Board(&gameScore);
    gameBoardMap.buildBoard();
    testBag.buildBag();
    playertestHand.setHand(testBag.getLetters(MAX_HAND_LENGTH));

    // Set up computer player, which needs the same board.
    computerPlayer = Computer( &gameBoardMap, &gameScore);
    computerTestHand.setHand(testBag.getLetters(MAX_HAND_LENGTH));
    computerTestHand.currentHand = removeBlanks(computerTestHand.currentHand);

    // ############################################################ GTK ##########################
    GtkApplication *app;
    int status;
    app = gtk_application_new ("org.gtk.ScrabbleGtkTest", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return 0;
}
