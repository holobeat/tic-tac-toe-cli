/*

Minimal implementation of Tic-Tac-Toe for CLI. State of the game is
printed to console after each move.

Copyright (c) 2020 Gustav Elsner, MIT License

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define EMPTY_BOARD "........."
#define PLAYER1 'O'
#define PLAYER2 'X'

enum game_states
{
    pending_move,
    game_over,
    terminated
} game_state;

enum input_results
{
    none,
    correct,
    invalid_move,
    quit
} input_result;

typedef struct Model
{
    enum game_states game_state;
    enum input_results input_result;
    char board[9];
    int active_player;
} Model;

// display the current state of the game board
void render(char *board)
{
    printf("\n1 2 3 | %c %c %c\n", board[0], board[1], board[2]);
    printf("4 5 6 | %c %c %c\n", board[3], board[4], board[5]);
    printf("7 8 9 | %c %c %c\n", board[6], board[7], board[8]);
}

// returns 1 if all player's pieces are in
bool all_pieces_in(char *board, char player)
{
    int count = 0;
    for (int i = 0; i < 9; i++)
    {
        if (board[i] == player)
            count++;
        if (count == 3)
            return true;
    }
    return false;
}

// evaluate user input and return input_result
enum input_results eval_input(Model *model, char *choice)
{
    int choice_num, from_index, to_index;
    // check for quit command
    if (strcasecmp(choice, "q") == 0)
        return quit;

    choice_num = atoi(choice);
    // input must be numeric
    if (choice_num == 0)
        return invalid_move;

    if (all_pieces_in(model->board, model->active_player))
    {
        // if all pieces of the player are in, we expect the player
        // to move from one position to another
        if (strlen(choice) == 1)
            return invalid_move;
        from_index = (int)(choice_num / 10 - 1);
        to_index = (int)(choice_num % 10 - 1);
        // >>from<< position must match the correct player
        if (model->active_player != model->board[from_index])
            return invalid_move;
        // target position must be empty
        if (model->board[to_index] != '.')
            return invalid_move;
    }
    else
    {
        // we are just adding the pieces in
        if (strlen(choice) == 2)
            return invalid_move;
        to_index = choice_num - 1;
        // target position must be empty
        if (model->board[to_index] != '.')
            return invalid_move;
    }
    return correct;
}

// updates board with player's move
void move(Model *model, char *choice)
{
    int choice_num, from_index, to_index;
    choice_num = atoi(choice);
    if (all_pieces_in(model->board, model->active_player))
    {
        from_index = (int)(choice_num / 10 - 1);
        to_index = (int)(choice_num % 10 - 1);
        model->board[from_index] = '.';
        model->board[to_index] = model->active_player;
    }
    else
    {
        to_index = choice_num - 1;
        model->board[to_index] = model->active_player;
    }
}

// return true if the given player won
bool did_player_win(Model *model)
{
    // winning patterns
    static const int wp[8][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9},
        {1, 4, 7},
        {2, 5, 8},
        {3, 6, 9},
        {1, 5, 9},
        {3, 5, 7}};

    for (int i = 0; i < 8; i++)
    {
        if (model->board[wp[i][0] - 1] == model->active_player &&
            model->board[wp[i][1] - 1] == model->active_player &&
            model->board[wp[i][2] - 1] == model->active_player)
        {
            return true;
        }
    }
    return false;
}

// flush the input
void flush_stdin()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void render_intro()
{
    printf(
        "\nCLI Tic-Tac-Toe, 2020 (c) Gustav Elsner\n\n"
        "The numbers on the left side correspond to the position on the board.\n"
        "The 2 players are identified by characters 'O' and 'X'. The '.' is empty\n"
        "position on the board. This variation of the game allows player to have\n"
        "the maximum of 3 pieces. To place a mark, the player enters the appropriate\n"
        "number for the position. To move the mark, the player enters two numbers.\n"
        "Example: entering 38 will move the player from position 3 to position 8.\n"
        "To quit the game, enter 'q'.\n");
}

void main()
{
    Model model = {
        .active_player = PLAYER1,
        .game_state = pending_move};
    strcpy_s(model.board, 9, EMPTY_BOARD);
    char choice[3];

    render_intro();
    render(model.board);

    while (model.game_state == pending_move)
    {
        printf("Player '%c' move: ", model.active_player);
        scanf("%2s", choice);
        switch (eval_input(&model, choice))
        {
        case correct:
            move(&model, choice);
            render(model.board);
            if (did_player_win(&model))
            {
                printf("Player '%c' wins!\n", model.active_player);
                model.game_state = game_over;
            }
            // switch the player
            model.active_player = model.active_player == PLAYER1 ? PLAYER2 : PLAYER1;
            break;
        case invalid_move:
            printf("Invalid move!\n");
            flush_stdin();
            break;
        case quit:
            printf("Quitting...Bye!\n");
            model.game_state = terminated; // not really used
            break;
        default:
            printf("Not implemented\n");
        }
    }
}
