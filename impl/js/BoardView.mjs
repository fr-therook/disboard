export class Piece {
    constructor(componentConstructor) {
        this.componentConstructor = componentConstructor;
        this.pieceVec = Array(64).fill(null);
    }

    connect(boardCon) {
        const this_out = this;
        boardCon.placePiece.connect(function (piece, square) {
            this_out.place(
                piece, square
            );
        });
        boardCon.movePiece.connect(function (src, dest) {
            this_out.move(src, dest);
        });
        boardCon.removePiece.connect(function (square) {
            this_out.remove(square);
        });
        boardCon.resetBoard.connect(function (squares, pieces) {
            this_out.reset(squares, pieces)
        })
    }

    remove(square) {
        const piece = this.pieceVec[square.index];
        if (piece == null) return;
        this.pieceVec[square.index] = null;
        piece.destroy();
    }

    place(piece, square) {
        this.remove(square);

        const new_piece = this.componentConstructor(piece, square);
        this.pieceVec[square.index] = new_piece;
    }

    move(src, dest) {
        const srcPiece = this.pieceVec[src.index];
        if (srcPiece == null) return;

        this.remove(dest);

        srcPiece.animationEnabled = true;
        srcPiece.square = dest;
        srcPiece.animationEnabled = false;

        this.pieceVec[src.index] = null;
        this.pieceVec[dest.index] = srcPiece;
    }

    reset(squares, pieces) {
        var initial = [];
        var i = 0;
        while (i < squares.length) {
            initial.push([squares[i], pieces[i]]);
            i += 1;
        }

        const prevPieceVec = this.pieceVec;
        this.pieceVec = Array(64).fill(null);

        for (const val of initial) {
            const square = val[0];
            const piece = val[1];

            const idx = square.index;

            if (prevPieceVec[idx] != null) {
                if (prevPieceVec[idx].piece == piece) {
                    // Move piece to new array
                    this.pieceVec[idx] = prevPieceVec[idx];
                    prevPieceVec[idx] = null;
                    continue;
                }
            }

            const new_piece = this.componentConstructor(piece, square);
            this.pieceVec[idx] = new_piece;
        }

        for (const piece of prevPieceVec) {
            if (piece != null) {
                piece.destroy();
            }
        }
    }
}
