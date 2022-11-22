import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import Components 1.0 as Z
import Theme 1.0
Z.CGridScroll{
    id:root
    property alias running: sendControl.checked
    property var command: [0,0,0,0,40,2]
    property var commandStep: [1,20,20,20,10,1]
    property var crazyModel:[
        [qsTr("ID[↑/↓]"), [0,0,16], [["+","="],["-","_"]], false, ],
        [qsTr("Vx[W/S]"), [-511,0,511], [["w"],["s"]], true, ],
        [qsTr("Vy[A/D]"), [-511,0,511], [["a"],["d"]], true, ],
        [qsTr("Vw[←/→]"), [-1023,0,1023], [["left"],["right"]], true, ],
        [qsTr("Shoot (E)\n[Ctrl+↑/↓]"), [0,0,127], [["ctrl+up"],["ctrl+down"]], true, ],
        [qsTr("Spin (Q)\n[Shift+↑/↓]"), [0,0,3], [["shift+up"],["shift+down"]], true, ],
    ]
    property bool shootSwitch : false
    property bool spinSwitch : false

    Item{
        Shortcut{
            sequence: "ctrl+c"
            onActivated: sendControl.checked = !sendControl.checked
        }
        Shortcut{
            sequence: "q"
            onActivated: root.spinSwitch = !root.spinSwitch
        }
        Shortcut{
            sequence: "e"
            onActivated: root.shootSwitch = !root.shootSwitch
        }

        Shortcut{
            sequence: "space"
            onActivated: {
                for(var i=1;i<4;i++){
                    command[i] = crazyModel[i][1][1]
                    spinboxes.itemAt(i).value = crazyModel[i][1][1]
                }
                root.spinSwitch = false
                root.shootSwitch = false
            }
        }
    }
    Z.CGroupBox{
        container.columns: 3
        Repeater{
            id:texts
            model:crazyModel
            Z.CText{
                Layout.column:0
                font.pixelSize: 13
                Layout.row:index
                text: modelData[0]
            }
        }
        Repeater{
            id:lights
            model:crazyModel
            Z.CLight{
                visible: modelData[3]
                running: spinboxes.itemAt(index).value !== modelData[1][1]
                Layout.column:1
                Layout.row:index
            }
        }
        Repeater{
            id:spinboxes
            model:crazyModel
            Z.CSpinBox{
                property int curIndex : index
                Layout.column:2
                Layout.row:index
                Layout.fillWidth: true;
                from:modelData[1][0];
                to:modelData[1][2];
                value:command[curIndex];
                btn.stepSize:commandStep[curIndex];
                Shortcut{
                    sequences: modelData[2][0]
                    onActivated: {
                        command[curIndex] += commandStep[curIndex]
                        value = command[curIndex]
                        command[curIndex] = value
                    }
                }
                Shortcut{
                    sequences: modelData[2][1]
                    onActivated: {
                        command[curIndex] -= commandStep[curIndex]
                        value = command[curIndex]
                        command[curIndex] = value
                    }
                }
            }
        }
        Component.onCompleted: {
            lights.itemAt(4).running = Qt.binding(function(){return root.shootSwitch})
            lights.itemAt(5).running = Qt.binding(function(){return root.spinSwitch})
        }
    }
    Z.CButton{
        id:sendControl
        _s:"main2"
        text: (checked ? qsTr("STOP") : qsTr("SEND")) + " (Ctrl+c)"
        checkable: true
        Z.CLight{
            size:8
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            running: root.running
        }
    }
}

