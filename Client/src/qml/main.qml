import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import com.kdab.dockwidgets 1.0 as KDDW
import Components 1.0 as Z
import ZSS 1.0 as ZSS
import Theme 1.0

ApplicationWindow {
    id:root
    property bool needSaveLayout:true
    width: ZSS.Interaction.getParam("GUI/windowWidth",1400)
    height: ZSS.Interaction.getParam("GUI/windowHeight",700)
    visible: true
    title: qsTr("@rocosCli") + (needSaveLayout?"*":"")
    background: Rectangle{
        color: T.__c["main"][T.i_main]
    }
    property bool extraControlVisiable: true
    KDDW.MainWindowLayout{
        uniqueName: "TinyCli"
        anchors.fill: parent
        KDDW.DockWidget{
            id:field
            uniqueName:"main"
            ZSS.Field{
                anchors.fill: parent
                onWidthChanged: {
                    resize(width,height);
                }
                onHeightChanged: {
                    resize(width,height);
                }
                Component.onCompleted: {
                    resize(width,height);
                }
            }
        }
        KDDW.DockWidget{
            id:controlBoard
            uniqueName:"controlBoard"
            ControlBoard{}
        }
        KDDW.DockWidget{
            id:extra1
            uniqueName:"refereeBox"
            RefereeBox{}
        }
        KDDW.DockWidget{
            id:extra2
            uniqueName:"crazyBoard"
            CrazyBoard{}
        }
        KDDW.DockWidget{
            id:extra3
            uniqueName:"setting(ctrl+r)"
            Settings{}
        }
        Component.onCompleted: {
            this.addDockWidget(field,KDDW.KDDockWidgets.Location_OnBottom);
            this.addDockWidget(controlBoard,KDDW.KDDockWidgets.Location_OnRight,field,Qt.size(300,100));
            this.addDockWidget(extra1,KDDW.KDDockWidgets.Location_OnBottom,controlBoard,Qt.size(300,460));
            extra1.addDockWidgetAsTab(extra2);
            extra1.addDockWidgetAsTab(extra3);
            layoutSaver.restoreFromFile("layout.json");
        }
    }
    KDDW.LayoutSaver {
        id: layoutSaver
    }
    function saveLayout(){
        layoutSaver.saveToFile("layout.json")
        needSaveLayout = false
    }
    Shortcut{
        sequence:"Ctrl+s"
        onActivated: saveLayout()
    }
}
