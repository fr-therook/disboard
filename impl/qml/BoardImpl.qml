import QtQuick

import disboard.impl.controller

import "../js/BoardView.mjs" as BoardView

Item {
    required property int pieceSize

    property alias pgn: boardCon.pgn

    id: board

    QtObject {
        readonly property var component: Qt.createComponent("Piece.qml")

        function componentConstructor(piece, square) {
            return component.createObject(board, {
                "piece": piece,
                "square": square,
                "size": Qt.binding(function () {
                    return board.pieceSize
                }),
                "sourceSize": Qt.binding(function () {
                    return board.pieceSize
                })
            });
        }

        readonly property var inner: new BoardView.Piece(componentConstructor)

        id: pieceView
    }

    QtObject {
        readonly property var square: boardCon.promotionSq

        readonly property var component: Qt.createComponent("PromotionWindow.qml")
        property var createdObject: null

        function componentConstructor(newSq) {
            const isWhite = newSq.rank >= 4;
            return component.createObject(board, {
                "file": newSq.file,
                "pieces": boardCon.promotionPieces,
                "side": isWhite,
                "pieceSize": Qt.binding(function () {
                    return board.pieceSize
                }),
            });
        }

        id: promotionWindow

        function open(newSquare) {
            close();
            createdObject = componentConstructor(newSquare);
            createdObject.selected.connect(function (piece) {
                boardCon.promote(piece);
            });
        }

        function close() {
            if (createdObject == null) {
                return;
            }
            createdObject.destroy();
            createdObject = null;
        }

        onSquareChanged: {
            if (square == null) close();
            else open(square);
        }
    }

    Component.onCompleted: {
        pieceView.inner.connect(boardCon);
        boardCon.resyncBoard();
    }

    Controller {
        id: boardCon

        pieceSize: board.pieceSize
        dragPos: dragArea.dragPos
    }

    DragArea {
        id: dragArea
        anchors.fill: parent

        onClicked: function (x, y) {
            boardCon.coordClicked(x, y);
        }
        onDragEnded: function (srcX, srcY, destX, destY) {
            boardCon.coordDragEnded(srcX, srcY, destX, destY);
        }
        onDragStarted: function (srcX, srcY, destX, destY) {
            boardCon.coordDragStarted(srcX, srcY, destX, destY);
        }
        onScrolled: function (delta) {
            if (delta > 0) {
                boardCon.nextMove();
            } else {
                boardCon.prevMove();
            }
        }
    }

    Item {
        anchors.fill: parent

        id: hintCanvas

        Repeater {
            model: boardCon.hintSq

            HintRect {
                required property var modelData

                square: modelData
                size: board.pieceSize
            }
        }

        Repeater {
            model: boardCon.captureSq

            CaptureHintRect {
                required property var modelData

                square: modelData
                size: board.pieceSize
            }
        }
    }

    Item {
        anchors.fill: parent

        HighlightRect {
            square: boardCon.highlightedSq
            size: board.pieceSize
        }
        HighlightRect {
            square: boardCon.lastSrcSq
            size: board.pieceSize
        }
        HighlightRect {
            square: boardCon.lastDestSq
            size: board.pieceSize
        }
    }

    Item {
        anchors.fill: parent

        z: 11

        PhantomPiece {
            id: phantomPiece

            z: 1

            visible: boardCon.phantom != null
            piece: boardCon.phantom

            centerX: boardCon.dragPos.x
            centerY: boardCon.dragPos.y

            size: board.pieceSize
            sourceSize: board.pieceSize
        }

        HoverRect {
            id: hoverRect

            z: 0

            visible: boardCon.phantom != null
            square: boardCon.dragSq

            size: board.pieceSize
        }
    }

    BoardBackground {
        anchors.fill: parent

        z: -1

        pieceSize: board.pieceSize
    }
}
