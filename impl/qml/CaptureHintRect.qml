import QtQuick

Square {
    Rectangle {
        anchors.fill: parent

        radius: (width / 2) - 1
        border.width: width / 8
        border.color: "#000000"
        color: "transparent"
        opacity: 0.2
    }
}
