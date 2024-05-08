#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

vector<vector<char>> CloneBoard(const vector<vector<char>> &board);
bool IsInBounds(int x, int y);
void PrintBoard(const vector<vector<char>> &board);
void PrintBoard(const string &fen);
string FenFromBoard(const vector<vector<char>> &board, char active_color, string castling, string en_passant, string halfmove, string fullmove);
vector<vector<char>> BoardFromFen(string fen);
vector<string> GetPseudovalidMoves(string fen, string color, const vector<string> &moveHistory);
string encodeMove(int startRow, int startCol, int endRow, int endCol);
void decodeMove(const string &move, int &startRow, int &startCol, int &endRow, int &endCol);
vector<string> GetPawnMoves(string fen, string color, int i, int j, const vector<string> &moveHistory);
vector<string> GetKnightMoves(const vector<vector<char>> &board, string color, int i, int j);
vector<string> GetBishopMoves(const vector<vector<char>> &board, string color, int i, int j);
vector<string> GetRookMoves(const vector<vector<char>> &board, string color, int i, int j);
vector<string> GetQueenMoves(const vector<vector<char>> &board, string color, int i, int j);
vector<string> GetKingMoves(const vector<vector<char>> &board, string color, int i, int j);
vector<int> GetKingPosition(const vector<vector<char>> &board, string color);
vector<string> GetValidMoves(const vector<vector<char>> &board, string fen, string color, const vector<string> &pseudoValidMoves);
bool IsInCheck(vector<vector<char>> &board, string color, vector<int> &KingPosition);
bool isOpponent(char piece, bool is_white);
bool ApplyKingCheckMoves(vector<vector<char>> &board, const string &color, const vector<int> &KingPosition, const vector<string> &moves, char CheckPiece);
bool isValidEnPassant(const string &lastMove, int i, int j, int direction, bool is_white);
bool IsTerminal(string fen, string color, const vector<string> &MoveHistory, int depth);
string FenHalfmove(const string &fen);
int FenFullmove(const string &fen);
string FenEnPassant(const string &fen);
string FenCastling(const string &fen);
bool CutoffTest(const string &fen, int d, const string &color, const vector<string> &move_history);
float MaxValue(const string &fen, int depth, float alpha, float beta, string color, const vector<string> &MoveHistory);
float MinValue(const string &fen, int depth, float alpha, float beta, string color, const vector<string> &MoveHistory);
string MiniMax(string fen, string color, const vector<string> &MoveHistory);
string ApplyMove(string fen, string move, string color);
int GetPieceValue(char piece, int y, int x, const string &color, bool endgame);
int HEval(const string &fen, const vector<string> &moveHistory, const string &color);
int GetPawnPromotionValue(int y, const string &color);
vector<string> Split(const string &str, char delimiter);

#endif