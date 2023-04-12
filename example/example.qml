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

        Component {
            id: tableViewDelegate

            ItemDelegate {
                readonly property var model2: model

                visible: model.node != null
                horizontalPadding: 16

                contentItem: RowLayout {
                    Text {
                        Layout.alignment: Qt.AlignVCenter

                        text: model.node != null ? model.display : ""
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        visible: model2.variations != null
                        orientation: ListView.Horizontal
                        clip: true

                        model: model2.variations

                        delegate: variationListViewDelegate
                    }
                }

                onClicked: board.controller.curNode = model.node
            }
        }

        Component {
            id: variationListViewDelegate

            Button {
                anchors.verticalCenter: parent.verticalCenter

                horizontalPadding: 8

                background.implicitWidth: 0

                text: modelData.display
                highlighted: true
                font.pointSize: 8
            }
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

            delegate: tableViewDelegate
        }
    }
}
