import QtQuick

import disboard

Window {
    height: 600
    title: qsTr("Example Project")
    visible: true
    width: 640

    Board {
        id: board

        anchors.fill: parent
    }
}
