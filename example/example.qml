import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import disboard
import disboard.impl.controller

Window {
    height: 600
    title: qsTr("Example Project")
    visible: true
    width: 900

    RowLayout {
        anchors.fill: parent

        Board {
            Layout.preferredWidth: height
            Layout.fillHeight: true

            id: board
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: MoveListModel {
                controller: board.controller
            }

            delegate: Rectangle {
                width: parent.width
                height: 32

                color: index % 2 ? "#ebebeb" : "#fafafa"

                RowLayout {
                    anchors.fill: parent

                    Control {
                        Layout.preferredWidth: parent.width / 2
                        Layout.fillHeight: true

                        visible: model.whiteMove != null
                        padding: 4

                        contentItem: Text {
                            text: model.whiteMove != null ? model.whiteMove : ""
                        }
                    }

                    Control {
                        Layout.preferredWidth: parent.width / 2
                        Layout.fillHeight: true

                        visible: model.blackMove  != null
                        padding: 4

                        contentItem: Text {
                            text: model.blackMove != null ? model.blackMove : ""
                        }
                    }
                }
            }
        }
    }
}
