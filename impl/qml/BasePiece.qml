import QtQuick

Item {
    required property var piece
    required property int sourceSize

    QtObject {
        readonly property string uriName: piece ? piece.pieceStr : "bN"

        readonly property string pieceUri: "qrc:/chess/pieces/" + uriName + ".svg"

        id: inner
    }

    Image {
        anchors.fill: parent

        sourceSize.width: parent.sourceSize
        sourceSize.height: parent.sourceSize

        source: inner.pieceUri

        fillMode: Image.Pad
        smooth: false
        cache: false
        asynchronous: true
    }
}
