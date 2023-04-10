import QtQuick

Item {
    property var square: null
    required property int size

    readonly property int file: square != null ? square.file : 0
    readonly property int rank: square != null ? square.rank : 0

    visible: square != null

    width: size
    height: size
    x: file * size
    y: (7 - rank) * size
}
