#include "chess_logic.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>
using namespace std;

vector<vector<char>> CloneBoard(const vector<vector<char>> &board)
{
    vector<vector<char>> newBoard(board.size());

    for (size_t i = 0; i < board.size(); ++i)
    {
        newBoard[i].reserve(board[i].size()); // Reserve exact space for each row
        newBoard[i].insert(newBoard[i].end(), board[i].begin(), board[i].end());
    }

    return newBoard;
}

bool IsInBounds(int x, int y)
{
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

void PrintBoard(const vector<vector<char>> &board)
{
    // ANSI escape codes for background colors
    const string whiteBg = "\033[47m";
    const string blackBg = "\033[40m";
    const string reset = "\033[0m";

    // Print column labels
    cout << " ";
    for (char c = 'a'; c <= 'h'; ++c)
    {
        cout << " " << c;
    }
    cout << "\n";

    for (int i = 0; i < 8; ++i)
    {
        // Print row label
        cout << 8 - i << ' ';

        for (int j = 0; j < 8; ++j)
        {
            // Choose background color
            string bgColor = (j % 2 == i % 2 ? whiteBg : blackBg);

            // Print square with background color and reset afterwards
            cout << bgColor << ' ' << board[i][j] << ' ' << reset;
        }

        cout << ' ' << 8 - i; // Print row label
        cout << '\n';
    }

    // Print column labels
    cout << " ";
    for (char c = 'a'; c <= 'h'; ++c)
    {
        cout << " " << c;
    }
    cout << "\n";
}

void PrintBoard(const string &fen)
{
    vector<vector<char>> board = BoardFromFen(fen);
    PrintBoard(board);
}

string FenFromBoard(const vector<vector<char>> &board, char active_color, string castling, string en_passant, string halfmove, string fullmove)
{
    int num = 0;
    string fen = "";
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == ' ')
            {
                num++;
            }
            if (board[i][j] != ' ')
            {
                if (num > 0)
                {
                    fen.append(to_string(num));
                    num = 0;
                }
                fen.append(1, board[i][j]);
            }
        }
        if (num > 0)
        {
            fen.append(to_string(num));
            num = 0;
        }
        fen.append(1, '/');
    }

    fen.pop_back();
    return fen + ' ' + active_color + ' ' + castling + ' ' + en_passant + ' ' + halfmove + ' ' + fullmove;
}

vector<vector<char>> BoardFromFen(string fen)
{
    vector<vector<char>> board(8, vector<char>(8, ' '));
    int row = 0;
    int col = 0;

    for (char c : fen)
    {
        if (c == ' ')
        {
            break;
        }
        if (c == '/')
        {
            row++;
            col = 0;
        }
        else if (isdigit(c))
        {
            int numSpaces = c - '0';
            col += numSpaces;
        }
        else
        {
            board[row][col] = c;
            col++;
        }
    }

    return board;
}

vector<string> GetPseudovalidMoves(string fen, string color, const vector<string> &moveHistory)
{
    vector<vector<char>> chessboard = BoardFromFen(fen);

    bool is_white = false;
    vector<string> moves = {};

    if (color == "white")
    {
        is_white = true;
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chessboard[i][j] == ' ')
            {
                continue;
            }
            else if (isupper(chessboard[i][j]) && !is_white)
            {
                continue;
            }
            else if (!isupper(chessboard[i][j]) && is_white)
            {
                continue;
            }
            else
            {
                switch (toupper(chessboard[i][j]))
                {
                case 'P':
                {
                    vector<string> pawnMoves = GetPawnMoves(fen, color, i, j, moveHistory);
                    moves.insert(moves.end(), pawnMoves.begin(), pawnMoves.end());
                }
                break;
                case 'N':
                {
                    vector<string> knightMoves = GetKnightMoves(chessboard, color, i, j);
                    moves.insert(moves.end(), knightMoves.begin(), knightMoves.end());
                }
                break;
                case 'B':
                {
                    vector<string> bishopMoves = GetBishopMoves(chessboard, color, i, j);
                    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
                }
                break;
                case 'R':
                {
                    vector<string> rookMoves = GetRookMoves(chessboard, color, i, j);
                    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
                }
                break;
                case 'Q':
                {
                    vector<string> queenMoves = GetQueenMoves(chessboard, color, i, j);
                    moves.insert(moves.end(), queenMoves.begin(), queenMoves.end());
                }
                break;
                default:
                {
                    vector<string> kingMoves = GetKingMoves(chessboard, color, i, j);
                    moves.insert(moves.end(), kingMoves.begin(), kingMoves.end());
                }
                break;
                }
            }
        }
    }

    return moves;
}

string encodeMove(int startRow, int startCol, int endRow, int endCol)
{
    string move;
    move += 'a' + startCol;
    move += '8' - startRow;
    move += 'a' + endCol;
    move += '8' - endRow;
    return move;
}

// Function to decode a move from UCI format to coordinates
void decodeMove(const string &move, int &startRow, int &startCol, int &endRow, int &endCol)
{
    startCol = move[0] - 'a';
    startRow = '8' - move[1];
    endCol = move[2] - 'a';
    endRow = '8' - move[3];
}

vector<string> GetPawnMoves(string fen, string color, int i, int j, const vector<string> &moveHistory)
{
    vector<vector<char>> board = BoardFromFen(fen);
    bool is_white = color == "white";
    int direction = 1;
    if (is_white)
        direction = -1;
    vector<string> moves = {};

    // Move forward one
    if (IsInBounds(i + direction, j) && board[i + direction][j] == ' ')
    {
        moves.push_back(encodeMove(i, j, i + direction, j));
    }

    // Move forward 2, only on first move
    if ((is_white && i == 6) || (!is_white && i == 1))
    {
        if (board[i + direction][j] == ' ' && board[i + 2 * direction][j] == ' ' && IsInBounds(i + 2 * direction, j))
        {
            moves.push_back(encodeMove(i, j, i + 2 * direction, j));
        }
    }

    // Capturing moves (diagonal captures)
    if (IsInBounds(i + direction, j - 1) && board[i + direction][j - 1] != ' ')
    {
        if (isOpponent(board[i + direction][j - 1], is_white))
        {
            moves.push_back(encodeMove(i, j, i + direction, j - 1));
        }
    }
    if (IsInBounds(i + direction, j + 1) && board[i + direction][j + 1] != ' ')
    {
        if (isOpponent(board[i + direction][j + 1], is_white))
        {
            moves.push_back(encodeMove(i, j, i + direction, j + 1));
        }
    }

    vector<string> updated_moves = {};
    // Promotion
    for (const string &move : moves)
    {
        if ((is_white && move[3] == '8') || (!is_white && move[3] == '1'))
        {
            // Detecting a pawn that has reached the promotion rank
            updated_moves.push_back(move + 'q'); // Promote to queen
            updated_moves.push_back(move + 'r'); // Promote to rook
            updated_moves.push_back(move + 'b'); // Promote to bishop
            updated_moves.push_back(move + 'n'); // Promote to knight
        }
        else
        {
            // Add the move as is if not a promotion move
            updated_moves.push_back(move);
        }
    }

    // En passant
    string lastMove = moveHistory.size() > 1 ? moveHistory.back() : "";
    if (!lastMove.empty())
    {
        if (j > 0 && isValidEnPassant(lastMove, i, j, direction, is_white))
        {
            if (board[i + direction][j - 1] != ' ')
                moves.push_back(encodeMove(i, j, i + direction, j - 1));
        }
        if (j < 7 && isValidEnPassant(lastMove, i, j, direction, is_white))
        {
            if (board[i + direction][j + 1] != ' ')
                moves.push_back(encodeMove(i, j, i + direction, j + 1));
        }
    }

    // // Now use updated_moves instead of moves
    moves.insert(moves.end(), updated_moves.begin(), updated_moves.end());

    return moves;
}

bool isValidEnPassant(const string &lastMove, int i, int j, int direction, bool is_white)
{
    if (lastMove.length() != 4)
        return false;

    // Calculate original and final row from lastMove
    int startRow = lastMove[1] - '1';
    int endRow = lastMove[3] - '1';
    int col = lastMove[2] - 'a';

    // En passant is only possible if last move was a pawn moving two rows
    if (abs(startRow - endRow) == 2)
    {
        // Check if it's the correct column next to our pawn
        if ((col == j - 1 || col == j + 1) && (endRow == i))
        {
            return true;
        }
    }
    return false;
}

vector<string> GetKnightMoves(const vector<vector<char>> &board, string color, int i, int j)
{
    vector<string> moves = {};
    bool is_white = color == "white";

    // Over one up two
    if (IsInBounds(i - 2, j - 1) && ((isupper(board[i - 2][j - 1]) && !is_white) || (!isupper(board[i - 2][j - 1]) && is_white) || board[i - 2][j - 1] == ' '))
    {
        moves.push_back(encodeMove(i, j, i - 2, j - 1));
    }
    if (IsInBounds(i - 2, j + 1) && ((isupper(board[i - 2][j + 1]) && !is_white) || (!isupper(board[i - 2][j + 1]) && is_white)))
    {
        moves.push_back(encodeMove(i, j, i - 2, j + 1));
    }

    // Over one down 2
    if (IsInBounds(i + 2, j - 1) && ((isupper(board[i + 2][j - 1]) && !is_white) || (!isupper(board[i + 2][j - 1]) && is_white) || board[i + 2][j - 1] == ' '))
    {
        moves.push_back(encodeMove(i, j, i + 2, j - 1));
    }
    if (IsInBounds(i + 2, j + 1) && ((isupper(board[i + 2][j + 1]) && !is_white) || (!isupper(board[i + 2][j + 1]) && is_white) || board[i + 2][j + 1] == ' '))
    {
        moves.push_back(encodeMove(i, j, i + 2, j + 1));
    }

    // Over two down one
    if (IsInBounds(i + 1, j - 2) && ((isupper(board[i + 1][j - 2]) && !is_white) || (!isupper(board[i + 1][j - 2]) && is_white) || board[i + 1][j - 2] == ' '))
    {
        moves.push_back(encodeMove(i, j, i + 1, j - 2));
    }
    if (IsInBounds(i + 1, j + 2) && ((isupper(board[i + 1][j + 2]) && !is_white) || (!isupper(board[i + 1][j + 2]) && is_white) || board[i + 1][j + 2] == ' '))
    {
        moves.push_back(encodeMove(i, j, i + 1, j + 2));
    }

    // Over two up one
    if (IsInBounds(i - 1, j - 2) && ((isupper(board[i - 1][j - 2]) && !is_white) || (!isupper(board[i - 1][j - 2]) && is_white) || board[i - 1][j - 2] == ' '))
    {
        moves.push_back(encodeMove(i, j, i - 1, j - 2));
    }
    if (IsInBounds(i - 1, j + 2) && ((isupper(board[i - 1][j + 2]) && !is_white) || (!isupper(board[i - 1][j + 2]) && is_white) || board[i - 1][j + 2] == ' '))
    {
        moves.push_back(encodeMove(i, j, i - 1, j + 2));
    }

    return moves;
}

vector<string> GetBishopMoves(const vector<vector<char>> &board, string color, int i, int j)
{
    vector<string> moves;
    bool is_white = color == "white";
    vector<pair<int, int>> directions = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

    for (const auto &direction : directions)
    {
        for (int k = 1; k < 8; ++k)
        {
            int new_i = i + k * direction.first;
            int new_j = j + k * direction.second;

            if (!IsInBounds(new_i, new_j))
            {
                continue;
            }

            char piece = board[new_i][new_j];

            if (piece == ' ')
            {
                moves.push_back(encodeMove(i, j, new_i, new_j));
            }
            else if ((islower(piece) && is_white) || (isupper(piece) && !is_white))
            {
                moves.push_back(encodeMove(i, j, new_i, new_j));
                break;
            }
            else
            {
                break;
            }
        }
    }

    return moves;
}

bool isOpponent(char piece, bool is_white)
{
    return is_white ? islower(piece) : isupper(piece);
}

vector<string> GetRookMoves(const vector<vector<char>> &board, string color, int i, int j)
{
    vector<string> moves;
    bool is_white = color == "white";
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; // Down, Up, Right, Left

    for (int d = 0; d < 4; ++d)
    {
        int temp_i = i;
        int temp_j = j;
        while (true)
        {
            temp_i += directions[d][0];
            temp_j += directions[d][1];

            if (!IsInBounds(temp_i, temp_j))
                break;
            char piece = board[temp_i][temp_j];
            if (piece != ' ' && !isOpponent(piece, is_white))
                break; // Stop if it's a friendly piece or non-empty square

            moves.push_back(encodeMove(i, j, temp_i, temp_j));
            if (piece != ' ' && isOpponent(piece, is_white))
                break; // Stop after capturing an opponent's piece
        }
    }

    return moves;
}

vector<string> GetQueenMoves(const vector<vector<char>> &board, string color, int i, int j)
{
    static std::map<std::tuple<int, int, string>, vector<string>> cache;
    auto key = std::make_tuple(i, j, color);

    // Check if the moves for this position are already calculated
    auto it = cache.find(key);
    if (it != cache.end())
    {
        return it->second;
    }

    vector<string> rookMoves = GetRookMoves(board, color, i, j);
    vector<string> bishopMoves = GetBishopMoves(board, color, i, j);

    vector<string> moves;
    moves.reserve(rookMoves.size() + bishopMoves.size());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());

    // Cache the result before returning
    cache[key] = moves;
    return moves;
}

vector<string> GetKingMoves(const vector<vector<char>> &board, string color, int i, int j)
{
    vector<string> moves = {};
    bool is_white = color == "white";

    // Check one square in all directions
    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
            {
                continue; // Skip the current position
            }
            int newRow = i + dx;
            int newCol = j + dy;
            if (!IsInBounds(newRow, newCol))
                continue;
            if (board[newRow][newCol] == ' ')
            {
                moves.push_back(encodeMove(i, j, newRow, newCol));
                continue;
            }
            if (IsInBounds(newRow, newCol) && isOpponent(board[newRow][newCol], is_white))
            {
                moves.push_back(encodeMove(i, j, newRow, newCol));
            }
        }
    }
    return moves;
}

vector<int> GetKingPosition(const vector<vector<char>> &board, string color)
{
    char king = 'k';
    if (color == "white")
    {
        king = 'K';
    }
    vector<int> pos = {};

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] == king)
            {
                pos.push_back(i);
                pos.push_back(j);
                break;
            }
        }
    }

    return pos;
}

vector<string> GetValidMoves(const vector<vector<char>> &board, const string fen, const string color, const vector<string> &pseudoValidMoves)
{
    vector<string> validMoves;
    vector<vector<char>> tempBoard = CloneBoard(board); // Clone the board once before the loop

    for (const string &move : pseudoValidMoves)
    {
        if (move[0] == '_' || move[0] == '`')
        {
            continue; // Skip invalid or special-case moves
        }

        // Get the start and end coordinates of the move
        int startRow, startCol, endRow, endCol;
        decodeMove(move, startRow, startCol, endRow, endCol);

        // Temporarily make the move on the tempBoard
        char piece = tempBoard[startRow][startCol];
        char capturedPiece = tempBoard[endRow][endCol];
        tempBoard[startRow][startCol] = ' ';
        tempBoard[endRow][endCol] = piece;

        // Determine the king's position after the move
        vector<int> pos = GetKingPosition(tempBoard, color);

        // Check if the move results in a check against the moving player
        if (!IsInCheck(tempBoard, color, pos))
        {
            validMoves.push_back(move);
        }

        // Revert the move
        tempBoard[startRow][startCol] = piece;
        tempBoard[endRow][endCol] = capturedPiece;
    }

    return validMoves;
}

bool IsInCheck(vector<vector<char>> &board, string color, vector<int> &KingPosition)
{
    if (KingPosition.empty())
        return true;

    string opponent_color = (color == "black") ? "white" : "black";
    bool is_white = color == "white";
    // char opponent_pawn = (opponent_color == "white") ? 'P' : 'p';
    char opponent_knight = (opponent_color == "white") ? 'N' : 'n';
    char opponent_king = (opponent_color == "white") ? 'K' : 'k';
    // char opponent_bishop = (opponent_color == "white") ? 'B' : 'b';

    vector<pair<int, int>> directions_knight = {{-2, -1}, {-1, -2}, {1, -2}, {2, -1}, {2, 1}, {1, 2}, {-1, 2}, {-2, 1}};
    vector<pair<int, int>> directions_king = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    vector<pair<int, int>> directions_straight = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    vector<pair<int, int>> directions_diagonal = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
    int i = KingPosition[0];
    int j = KingPosition[1];

    int direction = is_white ? -1 : 1;

    if (IsInBounds(i + direction, j - 1))
    {
        if (isOpponent(board[i + direction][j - 1], is_white))
        {
            if (toupper(board[i + direction][j - 1]) == 'P')
            {
                return true;
            }
        }
    }
    if (IsInBounds(i + direction, j + 1))
    {
        if (isOpponent(board[i + direction][j + 1], is_white))
        {
            if (toupper(board[i + direction][j + 1]) == 'P')
            {
                return true;
            }
        }
    }

    for (const auto &offset : directions_knight)
    {
        int dx = offset.first;
        int dy = offset.second;
        if (IsInBounds(i + dx, j + dy) && board[i + dx][j + dy] == opponent_knight)
        {
            return true;
        }
    }

    for (const auto &offset : directions_king)
    {
        int dx = offset.first;
        int dy = offset.second;
        if (IsInBounds(i + dx, j + dy) && board[i + dx][j + dy] == opponent_king)
        {
            return true;
        }
    }

    for (const auto &offset : directions_diagonal)
    {
        int dx = offset.first;
        int dy = offset.second;
        for (int k = 1; k < 8; k++)
        {
            int new_i = i + k * dx;
            int new_j = j + k * dy;
            if (!IsInBounds(new_i, new_j))
            {
                break;
            }
            char piece = board[new_i][new_j];
            if (piece == ' ')
            {
                continue;
            }
            if (is_white)
            {
                if (piece == 'b' || piece == 'q')
                    return true;
            }
            else
            {
                if (piece == 'B' || piece == 'Q')
                    return true;
            }
            break;
        }
    }

    for (const auto &offset : directions_straight)
    {
        int dx = offset.first;
        int dy = offset.second;
        for (int k = 1; k < 8; k++)
        {
            int new_i = i + k * dx;
            int new_j = j + k * dy;
            if (!IsInBounds(new_i, new_j))
            {
                break; // Exit if out of bounds
            }

            char piece = board[new_i][new_j];
            if (piece == ' ')
            {
                continue; // Skip empty spaces
            }

            // Check if the piece is a rook or queen
            if (is_white)
            {
                if (piece == 'r' || piece == 'q')
                    return true;
            }
            else
            {
                if (piece == 'R' || piece == 'Q')
                    return true;
            }

            break; // Stop checking further in this direction after hitting a non-empty square
        }
    }

    return false;
}

bool ApplyKingCheckMoves(vector<vector<char>> &board, const string &color, const vector<int> &KingPosition, const vector<string> &moves, char CheckPiece)
{
    for (const string &move : moves)
    {
        int startRow, startCol, endRow, endCol;
        decodeMove(move, startRow, startCol, endRow, endCol);
        char piece = board[startRow][startCol];

        // Only proceed if the move is directly towards the target piece's location
        if (board[endRow][endCol] == CheckPiece)
        {
            board[startRow][startCol] = ' ';
            board[endRow][endCol] = piece;

            bool result = board[endRow][endCol] == CheckPiece;

            board[startRow][startCol] = piece;
            board[endRow][endCol] = board[endRow][endCol];

            if (result)
            {
                return true;
            }
        }
    }
    return false;
}

bool IsTerminal(string fen, string color, const vector<string> &MoveHistory, int depth)
{
    // If the current player has no valid moves, it's a terminal state
    vector<string> pseudoValidMoves = GetPseudovalidMoves(fen, color, MoveHistory);
    const vector<string> Valid = GetValidMoves(BoardFromFen(fen), fen, color, pseudoValidMoves);
    if (Valid.empty())
    {
        return true;
    }

    if (MoveHistory.size() < 4)
    {
        return false;
    }

    // Compare proposed moves to recent history
    if (MoveHistory.size() >= 4 && MoveHistory[MoveHistory.size() - 2].substr(2, 2) == MoveHistory[MoveHistory.size() - 4].substr(0, 2) && MoveHistory[MoveHistory.size() - 2].substr(0, 2) == MoveHistory[MoveHistory.size() - 4].substr(2, 2))
    {
        return true;
    }

    // If the game has been going on for 50 moves without a capture or pawn movement, it's a draw
    if (stoi(FenHalfmove(fen)) >= 98)
    {
        return true;
    }

    // Check if the last two moves are the same as the two moves before that
    if (MoveHistory[MoveHistory.size() - 1] == MoveHistory[MoveHistory.size() - 3] && MoveHistory[MoveHistory.size() - 2] == MoveHistory[MoveHistory.size() - 4])
    {
        return true;
    }

    if (MoveHistory.size() >= 8)
    {
        vector<string> last_8_moves(MoveHistory.end() - 8, MoveHistory.end());
        if (last_8_moves[0] == last_8_moves[4] && last_8_moves[1] == last_8_moves[5] && last_8_moves[2] == last_8_moves[6] && last_8_moves[3] == last_8_moves[7])
        {
            return true;
        }
    }

    return false;
}

string FenHalfmove(const string &fen)
{
    size_t spacePos = fen.find_last_of(' ');
    size_t spacePos2 = fen.find_last_of(' ', spacePos - 1);
    return fen.substr(spacePos2 + 1, spacePos - spacePos2 - 1);
}

// Utility function to split strings based on delimiter
vector<string> Split(const string &str, char delimiter)
{
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to extract the full move number from a FEN string
int FenFullmove(const string &fen)
{
    vector<string> parts = Split(fen, ' ');
    if (parts.size() >= 6)
    {
        return stoi(parts[5]); // Fullmove number is the 6th part
    }
    return 0; // Return 0 or throw an exception if the FEN is invalid
}

// Function to extract the en passant square from a FEN string
string FenEnPassant(const string &fen)
{
    vector<string> parts = Split(fen, ' ');
    if (parts.size() >= 4)
    {
        return parts[3]; // En passant field is the 4th part
    }
    return "-"; // Return '-' indicating no en passant possible
}

// Function to extract castling rights from a FEN string
string FenCastling(const string &fen)
{
    vector<string> parts = Split(fen, ' ');
    if (parts.size() >= 3)
    {
        return parts[2]; // Castling rights are the 3rd part
    }
    return "-"; // Return '-' indicating no castling rights
}

bool CutoffTest(const string &fen, int d, const string &color, const vector<string> &move_history)
{
    // Checks if the search should be cut off at depth d.
    return IsTerminal(fen, color, move_history, d) || d == 0;
}

float MaxValue(const string &fen, int depth, float alpha, float beta, string color, const vector<string> &MoveHistory)
{
    // Check for depth of zero or if it's a terminal node
    if (CutoffTest(fen, depth, color, MoveHistory))
        return HEval(fen, MoveHistory, color);

    float v = -INFINITY;

    for (const string &action : GetValidMoves(BoardFromFen(fen), fen, color, GetPseudovalidMoves(fen, color, MoveHistory)))
    {
        v = max(v, MinValue(ApplyMove(fen, action, color), depth - 1, alpha, beta, color, MoveHistory));
        if (v >= beta)
            return v;
        alpha = max(alpha, v);
    }

    return v;
}

float MinValue(const string &fen, int depth, float alpha, float beta, string color, const vector<string> &MoveHistory)
{
    // Check for depth of zero or if it's a terminal node
    if (CutoffTest(fen, depth, color, MoveHistory))
        return HEval(fen, MoveHistory, color);

    float v = INFINITY;

    for (const string &action : GetValidMoves(BoardFromFen(fen), fen, color, GetPseudovalidMoves(fen, color, MoveHistory)))
    {
        // V should be the minimum between the current value and the max value
        v = min(v, MaxValue(ApplyMove(fen, action, color), depth - 1, alpha, beta, color, MoveHistory));
        if (v >= beta)
            return v;
        alpha = max(alpha, v);
    }

    return v;
}

string MiniMax(string fen, string color, const vector<string> &MoveHistory)
{
    int d = 0;
    string besta = "";
    float best_max = -INFINITY;
    float best_min = INFINITY;

    float v = -INFINITY;

    while (d < 3)
    {
        for (string &move : GetValidMoves(BoardFromFen(fen), fen, color, GetPseudovalidMoves(fen, color, MoveHistory)))
        {
            if (MoveHistory.size() >= 4 && MoveHistory[MoveHistory.size() - 2].substr(2, 2) == MoveHistory[MoveHistory.size() - 4].substr(0, 2) && MoveHistory[MoveHistory.size() - 2].substr(0, 2) == MoveHistory[MoveHistory.size() - 4].substr(2, 2))
                if (v != -INFINITY)
                    continue;
            if (color == "white")
            {
                v = MaxValue(ApplyMove(fen, move, color), d, -INFINITY, INFINITY, color, MoveHistory);
                if (v >= best_max)
                {
                    best_max = v;
                    besta = move;
                }
            }
            else
            {
                v = MinValue(ApplyMove(fen, move, color), d, -INFINITY, INFINITY, color, MoveHistory);
                if (v <= best_min)
                {
                    best_min = v;
                    besta = move;
                }
            }
        }
        d++;
    }
    return besta;
}

string ApplyMove(string fen, string move, string color)
{
    vector<vector<char>> board = BoardFromFen(fen);
    int startRow, startCol, endRow, endCol;
    decodeMove(move, startRow, startCol, endRow, endCol);
    char piece_moved = board[startRow][startCol];
    board[startRow][startCol] = ' ';
    board[endRow][endCol] = piece_moved;

    string fen_halfmove = FenHalfmove(fen);
    int fen_fullmove = FenFullmove(fen) + 1;
    string fen_enpassant = FenEnPassant(fen);

    if (tolower(piece_moved) == 'p' && abs(startCol - endCol) == 2)
    {
        fen_enpassant = encodeMove(startRow, startCol + 1, startRow, startCol + 1);
    }
    else
    {
        fen_enpassant = "-";
    }

    if (tolower(piece_moved) == 'p' || tolower(piece_moved) == 'r' || tolower(piece_moved) == 'k')
    {
        fen_halfmove = "0";
    }

    string fen_castling = FenCastling(fen);

    if (tolower(piece_moved) == 'k')
    {
        if (color == "white")
        {
            size_t posK = fen_castling.find('K');
            if (posK != std::string::npos)
            {
                fen_castling.replace(posK, 1, "");
            }
            size_t posQ = fen_castling.find('Q');
            if (posQ != std::string::npos)
            {
                fen_castling.replace(posQ, 1, "");
            }
        }
        else
        {
            size_t posK = fen_castling.find('k');
            if (posK != std::string::npos)
            {
                fen_castling.replace(posK, 1, "");
            }
            size_t posQ = fen_castling.find('q');
            if (posQ != std::string::npos)
            {
                fen_castling.replace(posQ, 1, "");
            }
        }
    }
    else if (tolower(piece_moved) == 'r')
    {
        if (startRow == 0)
        {
            if (color == "white")
            {
                size_t posQ = fen_castling.find('Q');
                if (posQ != std::string::npos)
                {
                    fen_castling.replace(posQ, 1, "");
                }
            }
            else
            {
                size_t posQ = fen_castling.find('q');
                if (posQ != std::string::npos)
                {
                    fen_castling.replace(posQ, 1, "");
                }
            }
        }
        else if (startRow == 7)
        {
            if (color == "white")
            {
                size_t posK = fen_castling.find('K');
                if (posK != std::string::npos)
                {
                    fen_castling.replace(posK, 1, "");
                }
            }
            else
            {
                size_t posK = fen_castling.find('k');
                if (posK != std::string::npos)
                {
                    fen_castling.replace(posK, 1, "");
                }
            }
        }
    }

    char active_color;

    if (color == "white")
    {
        active_color = 'w';
    }
    else
    {
        active_color = 'b';
    }

    return FenFromBoard(board, active_color, fen_castling, fen_enpassant, fen_halfmove, to_string(fen_fullmove));
}

static const vector<vector<int>> pawnEvalWhite = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {5, 5, 10, 25, 25, 10, 5, 5},
    {0, 0, 0, 20, 20, 0, 0, 0},
    {5, -5, -10, 0, 0, -10, -5, 5},
    {5, 10, 10, -20, -20, 10, 10, 5},
    {0, 0, 0, 0, 0, 0, 0, 0}};

static vector<vector<int>> pawnEvalBlack(pawnEvalWhite.rbegin(), pawnEvalWhite.rend());

static const vector<vector<int>> knightEval = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20, 0, 0, 0, 0, -20, -40},
    {-30, 0, 10, 15, 15, 10, 0, -30},
    {-30, 5, 15, 20, 20, 15, 5, -30},
    {-30, 0, 15, 20, 20, 15, 0, -30},
    {-30, 5, 10, 15, 15, 10, 5, -30},
    {-40, -20, 0, 5, 5, 0, -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50}};

static const vector<vector<int>> bishopEvalWhite = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 5, 10, 10, 5, 0, -10},
    {-10, 5, 5, 10, 10, 5, 5, -10},
    {-10, 0, 10, 10, 10, 10, 0, -10},
    {-10, 10, 10, 10, 10, 10, 10, -10},
    {-10, 5, 0, 0, 0, 0, 5, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}};

static vector<vector<int>> bishopEvalBlack(bishopEvalWhite.rbegin(), bishopEvalWhite.rend());

static const vector<vector<int>> rookEvalWhite = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {5, 10, 10, 10, 10, 10, 10, 5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {0, 0, 0, 5, 5, 0, 0, 0}};

static vector<vector<int>> rookEvalBlack(rookEvalWhite.rbegin(), rookEvalWhite.rend());

static const vector<vector<int>> evalQueen = {
    {-20, -10, -10, -5, -5, -10, -10, -20},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 5, 5, 5, 5, 0, -10},
    {-5, 0, 5, 5, 5, 5, 0, -5},
    {0, 0, 5, 5, 5, 5, 0, -5},
    {-10, 5, 5, 5, 5, 5, 0, -10},
    {-10, 0, 5, 0, 0, 0, 0, -10},
    {-20, -10, -10, -5, -5, -10, -10, -20}};

static const vector<vector<int>> kingEvalWhite = {
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-20, -30, -30, -40, -40, -30, -30, -20},
    {-10, -20, -20, -20, -20, -20, -20, -10},
    {20, 20, 0, 0, 0, 0, 20, 20},
    {20, 30, 10, 0, 0, 10, 30, 20}};

static vector<vector<int>> kingEvalBlack(kingEvalWhite.rbegin(), kingEvalWhite.rend());

static const vector<vector<int>> kingEvalWhiteEndgame = {
    {-50, -40, -30, -20, -20, -30, -40, -50},
    {-30, -20, -10, 0, 0, -10, -20, -30},
    {-30, -10, 20, 30, 30, 20, -10, -30},
    {-30, -10, 30, 40, 40, 30, -10, -30},
    {-30, -10, 30, 40, 40, 30, -10, -30},
    {-30, -10, 20, 30, 30, 20, -10, -30},
    {-30, -30, 0, 0, 0, 0, -30, -30},
    {-50, -30, -30, -30, -30, -30, -30, -50}};

static vector<vector<int>> kingEvalBlackEndgame(kingEvalWhiteEndgame.rbegin(), kingEvalWhiteEndgame.rend());

int GetPieceValue(char piece, int y, int x, const string &color, bool endgame)
{
    switch (piece)
    {
    case 'P':
        return color == "white" ? pawnEvalWhite[y][x] : pawnEvalBlack[y][x];
    case 'R':
        return color == "white" ? rookEvalWhite[y][x] : rookEvalBlack[y][x];
    case 'N':
        return knightEval[y][x];
    case 'B':
        return color == "white" ? bishopEvalWhite[y][x] : bishopEvalBlack[y][x];
    case 'Q':
        return evalQueen[y][x];
    case 'K':
        return endgame ? (color == "white" ? kingEvalWhiteEndgame[y][x] : kingEvalBlackEndgame[y][x])
                       : (color == "white" ? kingEvalWhite[y][x] : kingEvalBlack[y][x]);
    default:
        return 0;
    }
}

int HEval(const string &fen, const vector<string> &moveHistory, const string &color)
{
    // Assume create_board_from_fen is defined elsewhere to create a board from a FEN string
    vector<vector<char>> tempBoard = BoardFromFen(fen);
    int score = 0;

    map<char, int> pieceWeights = {
        {'P', 10},
        {'N', 30},
        {'B', 30},
        {'R', 50},
        {'Q', 90},
        {'K', 900}};

    int numPieces = 0;

    // Count pieces
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            char piece = tempBoard[y][x];
            if (piece != ' ')
            {
                numPieces++;
            }
        }
    }

    // Calculate score
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            char piece = tempBoard[y][x];
            if (piece != ' ')
            {
                bool isEndGame = numPieces < 10;
                if (isupper(piece))
                {
                    score += pieceWeights[piece] + GetPieceValue(piece, y, x, color, isEndGame);
                }
                else
                {
                    score -= pieceWeights[toupper(piece)] - GetPieceValue(toupper(piece), y, x, color, isEndGame);
                }
            }
        }
    }

    // Check for draw conditions
    if (moveHistory.size() >= 4 && moveHistory[moveHistory.size() - 2].substr(2, 2) == moveHistory[moveHistory.size() - 4].substr(0, 2) && moveHistory[moveHistory.size() - 2].substr(0, 2) == moveHistory[moveHistory.size() - 4].substr(2, 2))
    {
        score += (color == "white" ? -1000 : 1000);
    }

    int halfMoveClock = stoi(FenHalfmove(fen));
    if (halfMoveClock > 25)
    {
        score += (color == "white" ? -10 * halfMoveClock : 10 * halfMoveClock);
    }

    // Prioritize pawn promotion in endgame
    if (numPieces < 10)
    {
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                char piece = tempBoard[y][x];
                if (piece == 'P' || piece == 'p')
                {
                    score += (piece == 'P' ? GetPawnPromotionValue(y, "white") : -GetPawnPromotionValue(y, "black"));
                }
            }
        }
    }

    return score;
}

int GetPawnPromotionValue(int y, const string &color)
{
    if (color == "white")
    {
        return (7 - y) * 50; // Increase multiplier for more urgency
    }
    else
    {
        return y * 50;
    }
}