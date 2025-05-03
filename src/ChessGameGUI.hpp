#ifndef CHESSGAMEGUI_HPP
#define CHESSGAMEGUI_HPP

#include <string.h>

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/image.h>

#include "GameUser.hpp"
#include "WaitingPanel.hpp"
#include "games/chess.hpp"
#include "AbstractGameGUI.hpp"

uint8_t SQR_SIZE = 40;

struct ChessPiece {
    // lower case letter is black piece, and upper case is white
    char piece; // p (black pawn), P (white pawn), r (black rook), R (white rook)
    int row, col;
    wxBitmap bitmap;  // "♔", "♞", etc.

    wxString get_path() {
        wxString path;
        path = "./resources/chess/";
        switch (piece) {
            case 'p': path += "bp.png"; break;
            case 'P': path += "wp.png"; break;
            case 'r': path += "br.png"; break;
            case 'R': path += "wr.png"; break;
            case 'n': path += "bn.png"; break;
            case 'N': path += "wn.png"; break;
            case 'b': path += "bb.png"; break;
            case 'B': path += "wb.png"; break;
            case 'q': path += "bq.png"; break;
            case 'Q': path += "wq.png"; break;
            case 'k': path += "bk.png"; break;
            case 'K': path += "wk.png"; break;
        }
        return path;
    }
    ChessPiece(int r, int c, char p) : row(r), col(c), piece(p){
        // creates a bitmap from the path with the correct size
        wxImage img(get_path(), wxBITMAP_TYPE_PNG);
        if (img.IsOk()) {
            img.Rescale(SQR_SIZE, SQR_SIZE, wxIMAGE_QUALITY_HIGH);
            bitmap = wxBitmap(img);
        } else {
            wxLogError("Failed to load image: %s", get_path().c_str());
        }
    }
};

class ChessGameGUI : public AbstractGamePanel 
{

private:
    std::vector<ChessPiece> pieces;
    ChessPiece* draggedPiece = nullptr;
    wxPoint dragOffset;

    chess::Square from_sqr;
    chess::Square to_sqr;

    chess::Board* board;

    void setupGame() override{
        board = new chess::Board();
        my_msg_ = new GameMessage();
        my_msg_->game_id(GAME_ID);
        opp_msg_ = new GameMessage();
    }

    const chess::PieceGenType charToPieceGen(char c) {
        switch (tolower(c)) {
            case 'p': return chess::PieceGenType::PAWN;
            case 'n': return chess::PieceGenType::KNIGHT;
            case 'b': return chess::PieceGenType::BISHOP;
            case 'r': return chess::PieceGenType::ROOK;
            case 'q': return chess::PieceGenType::QUEEN;
            case 'k': return chess::PieceGenType::KING;
        }
        return chess::PieceGenType::PAWN;
    }

public:
    const uint8_t GAME_ID = 4;
    ChessGameGUI(wxFrame* parent, WaitingPanel* waitingPanel, std::function<void(int)> setScreen, GameUser* game_user)
    :AbstractGamePanel(parent, waitingPanel, setScreen, game_user)
    {
        Bind(wxEVT_PAINT, &ChessGameGUI::OnPaint, this);
        Bind(wxEVT_LEFT_DOWN, &ChessGameGUI::OnMouseDown, this);
        Bind(wxEVT_MOTION, &ChessGameGUI::OnMouseMove, this);
        Bind(wxEVT_LEFT_UP, &ChessGameGUI::OnMouseUp, this);
        setupGame();
    }
    ~ChessGameGUI() {};

    void OnPaint(wxPaintEvent& event) {
        wxPaintDC dc(this);
        dc.SetBackground(*wxBLACK_BRUSH); // Set background color
        dc.Clear();

        pieces.clear();
        // Draw the chessboard (8x8)
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                wxColour color = (row + col) % 2 == 0 ? wxColor(119,149,85,255) : wxColor(236,236,209,255);
                dc.SetBrush(wxBrush(color));
                dc.DrawRectangle(col * SQR_SIZE, row * SQR_SIZE, SQR_SIZE, SQR_SIZE);

                // check for piece and place it
                chess::Piece piece = board->at(chess::Square(((7 - row) * 8) + col));
                if (piece != chess::Piece::NONE){
                    pieces.emplace_back(row, col, std::string(piece)[0]);
                }
            }
        }

        // Draw pieces
        for (const auto& piece : pieces) {
            if (&piece == draggedPiece) continue;
            wxPoint pos(piece.col * SQR_SIZE, piece.row * SQR_SIZE);
            dc.DrawBitmap(piece.bitmap, pos, true);
        }

        // Draw dragged piece following mouse
        if (draggedPiece) {
            wxPoint mousePos = ScreenToClient(wxGetMousePosition()) - dragOffset;
            dc.DrawBitmap(draggedPiece->bitmap, mousePos, true);
        }
    }

    void OnMouseDown(wxMouseEvent& event) {
        if (!is_opp_active_ || !interactionEnabled) return;

        int col = event.GetX() / SQR_SIZE;
        int row = event.GetY() / SQR_SIZE;

        // finds the piece and sets it to be draggable
        for (auto& piece : pieces) {
            if (piece.row == row && piece.col == col) {
                draggedPiece = &piece;
                dragOffset = event.GetPosition() - wxPoint(col * SQR_SIZE + 10, row * SQR_SIZE + 5);

                // converts A8 -> H1 to A1 -> H8 coords
                from_sqr = chess::Square(((7 - row) * 8) + col);

                break;
            }
        }
    }

    void OnMouseMove(wxMouseEvent& event) {
        if (draggedPiece && event.Dragging() && event.LeftIsDown()) {
            Refresh();  // Redraw with the dragged piece
        }
    }

    void OnMouseUp(wxMouseEvent& event) {
        if (draggedPiece) {
            int col = event.GetX() / SQR_SIZE;
            int row = event.GetY() / SQR_SIZE;
            // piece is dropped
            to_sqr = chess::Square(((7 - row) * 8) + col);

            //check if move is legal
            chess::PieceGenType p = charToPieceGen(draggedPiece->piece);
            chess::Movelist moves;
            chess::movegen::legalmoves(moves, *board, p);

            chess::Move move = chess::Move::make<chess::Move::NORMAL>(from_sqr, to_sqr);
        
            bool legal = false;
            // if the move is legal, set the piece position and make the move
            if (std::find(moves.begin(), moves.end(), move) != moves.end()) legal = true;
            // check if move is an enpassant
            if (!legal && p == chess::PieceGenType::PAWN) {
                move = chess::Move::make<chess::Move::ENPASSANT>(from_sqr, to_sqr);
                if (std::find(moves.begin(), moves.end(), move) != moves.end()) legal = true;

                // check for promotion
                if (!legal){
                    // forces queen promotion (TODO: add dialog for user to pick)
                    move = chess::Move::make<chess::Move::PROMOTION>(from_sqr, to_sqr, chess::PieceType::QUEEN);
                    if (std::find(moves.begin(), moves.end(), move) != moves.end()) legal = true;
                }
            }
            // check if move is a castling
            if (!legal && p == chess::PieceGenType::KING) {
                move = chess::Move::make<chess::Move::CASTLING>(from_sqr, to_sqr);
                if (std::find(moves.begin(), moves.end(), move) != moves.end()) legal = true;
            }

            if (legal) {
                draggedPiece->row = row;
                draggedPiece->col = col;
                board->makeMove(move);

                // send dds message
                my_msg_->chess_from(from_sqr.index());
                my_msg_->chess_to(to_sqr.index());
                game_user_->sendGameMessage(my_msg_);

                waitingMoveEnter();
            }

            draggedPiece = nullptr;
            from_sqr = chess::Square::NO_SQ;
            to_sqr = chess::Square::NO_SQ;
            Refresh();  // Final redraw
        }
    }


    bool determineWinner() override{
        std::pair<chess::GameResultReason, chess::GameResult> result = board->isGameOver();
        if(chess::GameResultReason::NONE != result.first){
            chess::GameResult winner = result.second;
            
            if (winner == chess::GameResult::WIN) wxMessageBox("You Win!!");
            if (winner == chess::GameResult::LOSE) wxMessageBox("You Lose :(");
            if (winner == chess::GameResult::DRAW) wxMessageBox("it is a draw!");
    
            setupGame();
            updateDisplay();
    
            return true;
        }
        return false;
    }

    void updateDisplay() override{
        if (!is_opp_active_) return;

        if(game_user_->messageAvailable()){
            opp_msg_ = game_user_->readGameMessage();

            // if opp left the game here!!!
            if (opp_msg_->message() == "screen selection" && opp_msg_->game_id() != GAME_ID){
                std::cout << "opp left the game" << std::endl;
                setFrameStatusText("opp left the game");
                setupGame();
                setOppActive(false);
                return;
            }

            // TODO: check if move is legal, and type of move
            chess::Move m = chess::Move::make<chess::Move::NORMAL>(opp_msg_->chess_from(), opp_msg_->chess_to());
            board->makeMove(m);
        }
    
        Refresh();
    
        determineWinner();
    }
    std::string getFrameStatusText() override{
        if (game_user_->turn_){
            return game_user_->first_ ? "it is your turn (you are X)" : "it is your turn (you are O)";
        }
        else return "it is opp turn";
    }
};

#endif