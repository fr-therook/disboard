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

        TableView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            id: tableView

            model: MoveListModel {
                controller: board.controller
            }

            rowHeightProvider: function(row) {
                return 48;
            }
            columnWidthProvider: function(col) {
                return width / 2;
            }

            delegate: ItemDelegate {
                visible: model.node != null
                horizontalPadding: 16

                contentItem: RowLayout {
                    Text {
                        Layout.fillWidth: true

                        text: model.node != null ? model.display : ""
                    }

                    Text {
                        visible: model.variations != null
                        text: model.variations != null ? "..." : ""
                    }
                }

                onClicked: board.controller.curNode = model.node
            }
        }
    }
}
