import QtQuick

import disboard.impl

Item {
    readonly property alias controller: boardImpl.controller
    readonly property alias pgn: boardImpl.pgn

    BoardImpl {
        readonly property int boardSize: pieceSize << 3
        pieceSize: Math.min(parent.width, parent.height) >> 3

        anchors.centerIn: parent

        width: boardSize
        height: boardSize

        id: boardImpl
    }
}
