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

enum game_states
{
    pending_move,
    game_over,
    terminated
} game_state;

enum input_results
{
    correct,
    invalid_move,
    quit
} input_result;

// initialize new board
char *init_board()
{
    char *board = (char *)calloc(9, sizeof(char));
    for (int i = 0; i < 9; i++)
    {
        board[i] = '.';
    }
    return board;
}

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
        {
            return true;
        }
    }
    return false;
}

// evaluate user input and return input_result
enum input_results eval_input(char *board, char *choice, char player)
{
    int choice_num, from_index, to_index;
    // check for quit command
    if (strcasecmp(choice, "q") == 0)
        return quit;

    choice_num = atoi(choice);
    // input must be numeric
    if (choice_num == 0)
        return invalid_move;

    if (all_pieces_in(board, player))
    {
        // if all pieces of the player are in, we expect the player
        // to move from one position to another
        if (strlen(choice) == 1)
            return invalid_move;
        from_index = (int)(choice_num / 10 - 1);
        to_index = (int)(choice_num % 10 - 1);
        // >>from<< position must match the correct player
        if (player != board[from_index])
            return invalid_move;
        // target position must be empty
        if (board[to_index] != '.')
            return invalid_move;
    }
    else
    {
        // we are just adding the pieces in
        if (strlen(choice) == 2)
            return invalid_move;
        to_index = choice_num - 1;
        // target position must be empty
        if (board[to_index] != '.')
            return invalid_move;
    }
    return correct;
}

// updates board with player's move
void move(char *board, char *choice, char player)
{
    int choice_num, from_index, to_index;
    choice_num = atoi(choice);
    if (all_pieces_in(board, player))
    {
        from_index = (int)(choice_num / 10 - 1);
        to_index = (int)(choice_num % 10 - 1);
        board[from_index] = '.';
        board[to_index] = player;
    }
    else
    {
        to_index = choice_num - 1;
        board[to_index] = player;
    }
}

// return true if the given player won
bool did_player_win(char *board, char player)
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
        if (board[wp[i][0] - 1] == player &&
            board[wp[i][1] - 1] == player &&
            board[wp[i][2] - 1] == player)
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
    char *board = init_board();
    char choice[3];
    char active_player = 'O';

    render_intro();

    render(board);
    game_state = pending_move;
    while (game_state == pending_move)
    {
        printf("Player '%c' move: ", active_player);
        scanf("%2s", choice);
        switch (eval_input(board, choice, active_player))
        {
        case correct:
            move(board, choice, active_player);
            render(board);
            if (did_player_win(board, active_player))
            {
                printf("Player '%c' wins!\n", active_player);
                game_state = game_over;
            }
            // switch the player
            active_player = active_player == 'O' ? 'X' : 'O';
            break;
        case invalid_move:
            printf("Invalid move!\n");
            flush_stdin();
            break;
        case quit:
            printf("Quitting...Bye!\n");
            game_state = terminated;
            break;
        default:
            printf("Not implemented\n");
        }
    }

    free(board);
}