import QtQuick 2.12
import Components 1.0 as Z
import ZSS 1.0 as ZSS
Z.CGridScroll {
    Item{
        ZSS.Referee { id : refereeBox; }
    }
    id : root;
    property bool useFSM : false;
    readonly property var style : ["main2","info","warning"];
    property int state : ZSS.GameState.HALTED;
    property bool haltGame : false;
    property bool stopGame : true;
    property bool forceStart : false;
    property bool normalStart : false;
    property bool teamControl : false;
    property int next_command : ZSS.RefBoxCommand.HALT;
    function getButtonsCommand(index){
        switch(index){
        case ZSS.RefBoxCommand.HALT:
            state = ZSS.GameState.HALTED;break;
        case ZSS.RefBoxCommand.STOP_GAME:
        case ZSS.RefBoxCommand.BALL_PLACEMENT_YELLOW:
        case ZSS.RefBoxCommand.BALL_PLACEMENT_BLUE:
            state = ZSS.GameState.STOPPED;break;
        case ZSS.RefBoxCommand.FORCE_START:
        case ZSS.RefBoxCommand.NORMAL_START:
        case ZSS.RefBoxCommand.DIRECT_KICK_YELLOW:
        case ZSS.RefBoxCommand.INDIRECT_KICK_YELLOW:
        case ZSS.RefBoxCommand.DIRECT_KICK_BLUE:
        case ZSS.RefBoxCommand.INDIRECT_KICK_BLUE:
            state = ZSS.GameState.RUNNING;break;
        case ZSS.RefBoxCommand.TIMEOUT_YELLOW:
            state = ZSS.GameState.TIMEOUT_YELLOW;break;
        case ZSS.RefBoxCommand.TIMEOUT_BLUE:
            state = ZSS.GameState.TIMEOUT_BLUE;break;
        case ZSS.RefBoxCommand.KICKOFF_YELLOW:
        case ZSS.RefBoxCommand.KICKOFF_BLUE:
            state = ZSS.GameState.PREPARE_KICKOFF;break;
        case ZSS.RefBoxCommand.PENALTY_YELLOW:
        case ZSS.RefBoxCommand.PENALTY_BLUE:
            state = ZSS.GameState.PREPARE_PENALTY;break;
        default:
            console.log("RefBox Command ERROR!!!!!!");
            return;
        }
        controlButtons(state);
        refereeBox.changeCommand(index);
    }
    function controlButtons(state){
        function unify(onOff){ haltGame = stopGame = forceStart = normalStart = teamControl = onOff; }
        switch(state){
        case ZSS.GameState.HALTED:
            unify(false);
            stopGame = true;
            break;
        case ZSS.GameState.PREPARE_KICKOFF:
        case ZSS.GameState.PREPARE_PENALTY:
            unify(false);
            normalStart = true;
            break;
        case ZSS.GameState.RUNNING:
        case ZSS.GameState.TIMEOUT_BLUE:
        case ZSS.GameState.TIMEOUT_YELLOW:
            unify(false);
            haltGame = stopGame = true;
            break;
        case ZSS.GameState.STOPPED:
            unify(true);
            stopGame = normalStart = false;
            break;
        }
    }
    Z.CGroupBox{
        title:qsTr("Referee Setting");
        property bool visionGetter : false;
        Z.CButton{
            id:refBoxSwitch;
            _s:root.style[0]
            text:(refBoxSwitch.refereeSwitch ? qsTr("Stop") : qsTr("Send"));
            width:parent.itemWidth;
            property bool refereeSwitch: false;
            btn.onClicked: changeState();
            function changeState(){
                refBoxSwitch.refereeSwitch = !refBoxSwitch.refereeSwitch;
                refereeBox.changeSendMod(refBoxSwitch.refereeSwitch);
            }
        }
    }
    Z.CGroupBox{
        enabled : refBoxSwitch.refereeSwitch;
        title:qsTr("Control Command");
        container.columns:2;
        Z.CButton{
            property int index : ZSS.RefBoxCommand.HALT;
            _s:root.style[0]
            text:qsTr("Halt");
            btn.onClicked:getButtonsCommand(index);
            enabled: useFSM ? haltGame : true;
        }
        Z.CButton{
            property int index : ZSS.RefBoxCommand.STOP_GAME;
            _s:root.style[0]
            text:qsTr("Stop Game");
            btn.onClicked:getButtonsCommand(index);
            enabled: useFSM ? stopGame : true;
        }
        Z.CButton{
            property int index : ZSS.RefBoxCommand.FORCE_START;
            _s:root.style[0]
            text:qsTr("Force Start");
            btn.onClicked:getButtonsCommand(index);
            enabled: useFSM ? forceStart : true;
        }
        Z.CButton{
            property int index : ZSS.RefBoxCommand.NORMAL_START;
            _s:root.style[0]
            text:qsTr("Normal Start");
            btn.onClicked:getButtonsCommand(index);
            enabled: useFSM ? normalStart : true;
        }
    }
    Z.CGroupBox{
        title:qsTr("Yellow Team Control");
        enabled: refBoxSwitch.refereeSwitch && (useFSM ? teamControl : true);
        container.columns:2;
        Repeater{
            model:[
                {cmd_type:2,cmd_num:ZSS.RefBoxCommand.KICKOFF_YELLOW,cmd_str:"Kick off"},
                {cmd_type:2,cmd_num:ZSS.RefBoxCommand.PENALTY_YELLOW,cmd_str:"Penalty"},
                {cmd_type:2,cmd_num:ZSS.RefBoxCommand.DIRECT_KICK_YELLOW,cmd_str:"Direct Kick"},
                {cmd_type:2,cmd_num:ZSS.RefBoxCommand.INDIRECT_KICK_YELLOW,cmd_str:"Indirect Kick"},
                {cmd_type:2,cmd_num:ZSS.RefBoxCommand.TIMEOUT_YELLOW,cmd_str:"Timeout"},
                {cmd_type:2,cmd_num:ZSS.RefBoxCommand.BALL_PLACEMENT_YELLOW,cmd_str:"Ball Placement"},
            ]
            Z.CButton{
                property int index : modelData.cmd_num;
                _s:root.style[modelData.cmd_type];
                text:qsTr(modelData.cmd_str);
                btn.onClicked:getButtonsCommand(index);
            }
        }
    }
    Z.CGroupBox{
        title:qsTr("Blue Team Control");
        enabled: refBoxSwitch.refereeSwitch && (useFSM ? teamControl : true);
        container.columns:2;
        Repeater{
            model:[
                {cmd_type:1,cmd_num:ZSS.RefBoxCommand.KICKOFF_BLUE,cmd_str:"Kick off"},
                {cmd_type:1,cmd_num:ZSS.RefBoxCommand.PENALTY_BLUE,cmd_str:"Penalty"},
                {cmd_type:1,cmd_num:ZSS.RefBoxCommand.DIRECT_KICK_BLUE,cmd_str:"Direct Kick"},
                {cmd_type:1,cmd_num:ZSS.RefBoxCommand.INDIRECT_KICK_BLUE,cmd_str:"Indirect Kick"},
                {cmd_type:1,cmd_num:ZSS.RefBoxCommand.TIMEOUT_BLUE,cmd_str:"Timeout"},
                {cmd_type:1,cmd_num:ZSS.RefBoxCommand.BALL_PLACEMENT_BLUE,cmd_str:"Ball Placement"},
            ]
            Z.CButton{
                property int index : modelData.cmd_num;
                _s:root.style[modelData.cmd_type];
                text:qsTr(modelData.cmd_str);
                btn.onClicked:getButtonsCommand(index);
            }
        }
    }
    Z.CGroupBox{
        title:qsTr("Next Command");
        container.columns:2;
        enabled: refBoxSwitch.refereeSwitch;
        Repeater{
            model: [
                {cmd_type:0,cmd_str:"Stop Game",cmd_num:ZSS.RefBoxCommand.STOP_GAME},
                {cmd_type:0,cmd_str:"Force Start",cmd_num:ZSS.RefBoxCommand.FORCE_START},
                {cmd_type:2,cmd_str:"Direct Yellow",cmd_num:ZSS.RefBoxCommand.DIRECT_KICK_YELLOW},
                {cmd_type:1,cmd_str:"Direct Blue",cmd_num:ZSS.RefBoxCommand.DIRECT_KICK_BLUE},
                {cmd_type:2,cmd_str:"Indirect Yellow",cmd_num:ZSS.RefBoxCommand.INDIRECT_KICK_YELLOW},
                {cmd_type:1,cmd_str:"Indirect Blue",cmd_num:ZSS.RefBoxCommand.INDIRECT_KICK_BLUE},
            ]
            Z.CButton{
                property int index : modelData.cmd_num;
                _s:root.style[modelData.cmd_type];
                text:qsTr(modelData.cmd_str);
                btn.onClicked: {
                    root.next_command = index;
                    refereeBox.setNextCommand(index);
                }
            }
        }
    }
}
