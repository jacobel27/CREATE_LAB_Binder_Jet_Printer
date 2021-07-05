import QtQuick 2.15
import QtQuick.Controls 2.15
import untitled 1.0

Rectangle {
    width: Constants.width
    height: Constants.height

    color: Constants.backgroundColor

    Text {
        text: qsTr("Hello BinderJet")
        anchors.verticalCenterOffset: -318
        anchors.horizontalCenterOffset: -497
        anchors.centerIn: parent
        font.family: Constants.font.family
    }

    RoundButton {
        id: positiveXButton1
        x: 226
        y: 171
        text: "^"
    }

    RoundButton {
        id: roundButton
        x: 294
        y: 248
        text: ">"
    }

    RoundButton {
        id: roundButton1
        x: 226
        y: 324
        text: "V"
    }

    RoundButton {
        id: roundButton2
        x: 147
        y: 248
        text: "<"
    }

    Text {
        id: text1
        x: 288
        y: 288
        text: qsTr("Positive Y")
        font.pixelSize: 12
    }

    Text {
        id: text2
        x: 219
        y: 206
        text: qsTr("Positive X")
        font.pixelSize: 12
    }
}
