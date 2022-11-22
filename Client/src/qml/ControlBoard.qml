import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Universal 2.12
import Components 1.0 as Z
import ZSS 1.0 as ZSS
import Theme 1.0
Z.CGridScroll{
    anchors.fill: parent
    Item{
        Timer{
            id:updateTimer;
            property var jobs : []
            interval:600;
            running:true;
            repeat:true;
            onTriggered: {
                ZSS.Interaction.updateInterfaces();
                for(var i=0;i<jobs.length;i++){
                    jobs[i].update()
                }
            }
        }
    }
    Z.CGroupBox {
        Z.CGroupBox{
            container.columns: 2
            bg.border.width: 0
            padding: 0
            Z.CText{
                text:"Network"
            }
            Z.CComboBox{
                model:ZSS.Interaction.getInterfaces()
                Layout.fillWidth: true
                function update(){
                    model = ZSS.Interaction.getInterfaces();
                    if(btn.currentIndex >= 0)
                        ZSS.Interaction.setInterfaceIndex(btn.currentIndex);
                }
                Component.onCompleted: {
                    updateTimer.jobs.push(this)
                }
                btn.onActivated:ZSS.Interaction.setInterfaceIndex(btn.currentIndex);
            }
        }
        Z.CGroupBox {
            container.columns: 3
            bg.border.width: 0
            padding: 0
            Z.CSwitch{
                id:ifReal
                display: AbstractButton.TextUnderIcon
                text:[qsTr("Sim"),qsTr("Real")]
                icon:["/resource/icon/brain.png","/resource/icon/field.png"]
            }
            Z.CSwitch{
                id:ifYellow
                display: AbstractButton.TextUnderIcon
                style:["info","warning"];
                text:[qsTr("T-Blue"),qsTr("T-Yellow")]
                icon:["/resource/icon/shirt.png","/resource/icon/shirt.png"]
            }
            Z.CSwitch{
                id:ifRight
                display: AbstractButton.TextUnderIcon
                text:[qsTr("ToRight"),qsTr("ToLeft")]
                icon:["/resource/icon/right-arrow.png","/resource/icon/left-arrow.png"]
            }
        }
        Z.CButton {
            checkable: true
            source: checked ? "/resource/icon/connect.png" : "/resource/icon/disconnect.png"
            text: qsTr("Connect")
            btn.onClicked:{
                ZSS.Interaction.setVision(checked, ifReal.checked);
            }
        }
//        Rectangle{
//            Layout.fillWidth: true
//            color:"transparent"
//            width:parent.contentWidth
//            height:childrenRect.height
//            RowLayout{
//                width:parent.width
//                height:childrenRect.height
//                Z.CButton{
//                    id:mainCore
//                    checkable:true
//                    _s:ifYellow.checked ? "warning" : "info"
//                    fillWidth: false
//                    Layout.preferredWidth: 300
//                    Layout.fillWidth: true
//                    btn.icon.source:checked ? "/resource/icon/stop.png" : "/resource/icon/start.png";
//                    btn.onClicked: {
//                        ZSS.Interaction.changeCoreSetting(ifYellow.checked,ifRight.checked)
//                        ZSS.Interaction.controlCore(true,checked)
//                    }
//                }
//                Z.CButton{
//                    id:oppCore
//                    checkable:true
//                    visible:!ifReal.checked
//                    _s:ifYellow.checked ? "info" : "warning"
//                    fillWidth: false
//                    Layout.preferredWidth: 300
//                    Layout.fillWidth: true
//                    btn.icon.source:checked ? "/resource/icon/stop.png" : "/resource/icon/start.png";
//                    btn.onClicked: {
//                        ZSS.Interaction.changeCoreSetting(!ifYellow.checked,!ifRight.checked)
//                        ZSS.Interaction.controlCore(false,checked)
//                    }
//                }
//            }
//        }
    }
}
