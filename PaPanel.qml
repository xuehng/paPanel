import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Item {
    RowLayout {
        anchors.centerIn: parent

        Button {
            id: btn1
            text: "录音"
            checkable: !pa.isPlaying
            onCheckedChanged: {
                if(btn1.checked){
                    console.log("checked");
                    pa.startRecording();
                    console.log(pa.isRecording);
                }else{
                    console.log("unchecked");
                    pa.stopRecording();
                    console.log(pa.isRecording);
                }
            }
        }

        Button {
            id: btn11
            text: "录音到文件"
            checkable: !pa.isPlaying
            onCheckedChanged: {
                if(btn11.checked){
                    console.log("checked");
                    pa.startRecordingToFile();
                    console.log(pa.isRecording);
                }else{
                    console.log("unchecked");
                    pa.stopRecordingToFile();
                    console.log(pa.isRecording);
                }
            }
        }


        Button {
            id: btn2
            text: "试听文件"
            checkable: pa.isRecording ? false : true
            onCheckedChanged: {
                if(btn2.checked){
                    console.log("checked");
                    pa.startPlay();
                    console.log(pa.isPlaying);
                }else{
                    console.log("unchecked");
                    pa.stopPlay();
                    console.log(pa.isPlaying);
                }
            }
        }

        Button {
            id: btn3
            text: "连接"
            checkable: true
            onCheckedChanged: {
                if(btn3.checked){
                    console.log("checked");
                    pa.startConn();
                }else{
                    console.log("unchecked");
                    pa.stopConn();
                }
            }
        }
    }
}
