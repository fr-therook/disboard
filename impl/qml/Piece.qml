import QtQuick

Square {
    required property var piece
    required property int sourceSize
    property bool animationEnabled: false

    readonly property int transitionDuration: 150

    z: 10

    id: piece_canvas

    BasePiece {
        anchors.fill: parent

        id: basePiece

        piece: piece_canvas.piece // required properties does not work with aliases
        sourceSize: piece_canvas.sourceSize
    }

    Behavior on x {
        enabled: animationEnabled

        PropertyAnimation {
            duration: transitionDuration
        }
    }
    Behavior on y {
        enabled: animationEnabled

        PropertyAnimation {
            duration: transitionDuration
        }
    }

    MouseArea {
        anchors.fill: parent

        cursorShape: Qt.DragMoveCursor

        enabled: false
    }
}
