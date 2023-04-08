import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import disboard

Window {
    height: 400
    title: qsTr("Example Project")
    visible: true
    width: 640

    DisboardControls {
        id: controls

        anchors.left: parent.left
        anchors.margins: 20
        anchors.top: parent.top
        height: 100
        width: 100
    }
    Disboard {
        id: rect

        anchors.left: controls.right
        anchors.margins: 20
        anchors.top: parent.top
        height: 100
        width: 100
    }
}
