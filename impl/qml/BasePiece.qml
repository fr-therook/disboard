import QtQuick

Item {
    required property var piece
    required property int sourceSize

    QtObject {
        readonly property string pieceUri: piece ? ("qrc:/chess/pieces/" + piece.pieceStr + ".svg") : ""

        id: inner
    }

    Image {
        anchors.fill: parent

        sourceSize.width: parent.sourceSize
        sourceSize.height: parent.sourceSize

        source: inner.pieceUri

        fillMode: Image.Pad
        smooth: false
        cache: true
        asynchronous: false
    }
}
