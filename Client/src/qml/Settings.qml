import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Components 1.0 as Z
import ZSS 1.0 as ZSS
import Theme 1.0
Rectangle{
    id:root
    property string _s:"main2"
    property string _s2:"main"
    readonly property int rowHeight:22
    color: T.__c[_s][T.i_main]
    ZSS.ParamModel{
        id:paramModel;
    }
    Shortcut{
        sequence: "ctrl+r"
        onActivated: {
            paramModel.reload();
        }
    }
    TreeView{
        id:paramTree
        anchors.fill: parent
        anchors.margins: 0
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        backgroundVisible: false
        TableViewColumn {
            title: "Name"
            role: "settingName"
            width:parent.width*0.5;
            delegate:normalTextDelegate;
        }
        TableViewColumn {
            title: "Type"
            role: "settingType"
            width:parent.width*0.2;
            delegate:normalTextDelegate;
        }
        TableViewColumn {
            title: "Value"
            role: "settingValue"
            width:parent.width*0.3;
            delegate:stringDelegate;
        }
        model:paramModel;
        style: TreeViewStyle {
            frame: Rectangle {color:"transparent";border { width:1;color: T.background_lighter}}
            backgroundColor: T.__c[_s][T.i_main]
            alternateBackgroundColor:T.__c[_s2][T.i_main]
            textColor: Qt.darker(T.__c[_s][T.i_main_font],1.2);
            headerDelegate: Rectangle {
                height: root.rowHeight
                width: headerItem.implicitWidth
                color: T.__c[_s][T.i_main]
                Text {
                    id: headerItem
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: styleData.value
                    elide: Text.ElideRight
                    color: T.__c[_s][T.i_main_font];
                    font.pixelSize: 14;
                }
                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 1
                    anchors.topMargin: 1
                    width: 1
                    color: Qt.lighter(T.__c[_s][T.i_main],1.4)
                }
                Rectangle {
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 1
                    anchors.rightMargin: 1
                    height:1
                    color: Qt.lighter(T.__c[_s][T.i_main],1.4)
                }
            }
            rowDelegate:Rectangle{
                width:parent.width;
                height:root.rowHeight;
                property color selectedColor: T.__c[_s][T.i_active_font]
                color: !styleData.alternate ? alternateBackgroundColor : backgroundColor
            }
        }
    }
    Component {
        id: stringDelegate
        Loader{
            property var styleData : parent.styleData;
            property string type : paramModel.getType(styleData.index);
            sourceComponent: styleData.hasChildren ? normalTextDelegate : (paramModel.getType(styleData.index) == "Bool" ? boolDelegate : notBoolDelegate)
        }
    }
    Component{
        id:notBoolDelegate
        Rectangle{
            color: input.activeFocus  ? Qt.darker(T.__c[_s][T.i_hover],2) : "transparent";
            height:root.rowHeight;
            TextInput {
                id:input;
                width:parent.width
                anchors.verticalCenter: parent.verticalCenter
                text: styleData.value
                color:activeFocus?Qt.lighter(styleData.textColor,1.5):styleData.textColor;
                horizontalAlignment: styleData.textAlignment
                font.pixelSize: T.normalSize;
                leftPadding: 12;
                verticalAlignment: Text.AlignVCenter
                onEditingFinished:{
                    if(styleData.index.valid && paramModel.setData(styleData.index,text))
                        focus = false;
                }
            }
        }
    }
    Component{
        id:boolDelegate;
        Rectangle{
            color: "transparent";
            height:root.rowHeight;
            width:parent.width;
            CheckBox{
                id:input;
                height:root.rowHeight;
                x:12;
                checked: styleData.value === "true"
                onClicked: paramModel.setData(styleData.index,checked ? "true" : "false")
            }
        }
    }
    Component{
        id: normalTextDelegate
        Text {
            width:parent.width
            height:parent.height
            leftPadding:12;
            verticalAlignment:Text.AlignVCenter
            color: styleData.textColor
            elide: styleData.elideMode
            text: styleData.value
            font.pixelSize: T.normalSize
        }
    }
}
