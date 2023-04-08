import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
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
