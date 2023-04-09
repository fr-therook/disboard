import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Pane {
    required property int pieceSize
    required property int file
    required property bool side // 0: bottom, 1: top
    required property var pieces

    readonly property var revPieces: pieces != null ? pieces.slice().reverse() : null

    signal selected(var piece)

    width: pieceSize
    height: pieceSize * 4
    x: pieceSize * file
    y: side ? 0 : pieceSize * 4
    z: 20
    Material.elevation: 6

    id: promotionWindow

    padding: 0

    contentItem: Column {
        spacing: 0

        Repeater {
            model: promotionWindow.side ? pieces : revPieces

            ItemDelegate {
                width: parent.width
                height: parent.width

                padding: 4

                contentItem: BasePiece {
                    piece: modelData
                    sourceSize: width
                }

                onClicked: promotionWindow.selected(modelData)
            }
        }
    }
}
