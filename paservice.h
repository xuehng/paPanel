#ifndef PASERVICE_H
#define PASERVICE_H

#include <QObject>
#include <QFile>
#include <QAudioInput>
#include <QAudioOutput>
#include <QTcpSocket>

class PaService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRecording READ isRecording WRITE setIsRecording NOTIFY isRecordingChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying WRITE setIsPlaying NOTIFY isPlayingChanged)

public:
    explicit PaService(QString addr, int port = 8099, QObject *parent = 0);

    //for recording from QAudioInput and play to QAudioOutput using
    QFile m_bufFile;   // class member.
    QIODevice *m_inputDev;

    //for QAudioInput using
    bool m_isRecording;
    QAudioInput* m_audio_input; // Class member

    //for QAudioOutput using
    bool m_isPlaying;
    QAudioOutput* m_audioOutput; // class member.


    // for TCP socket using
    QByteArray m_buffer;
    QTcpSocket *m_socket;
    QString m_addr;
    int m_port;
    bool m_connecting;
    quint16 serverPort;

    bool isRecording(){return m_isRecording;}
    void setIsRecording(bool val);
    bool isPlaying(){return m_isPlaying;}
    void setIsPlaying(bool val);

signals:
    void isRecordingChanged();
    void isPlayingChanged();
public slots:
    // for QAudioinput using
    void startRecording();
    void startRecordingToFile();
    void ReadWrite();
    void stopRecording();
    void stopRecordingToFile();

    void startPlay();
    void stopPlay();

    void handleStateChanged(QAudio::State newState);

    // for QAudiooutput using
    void handleStateChanged2(QAudio::State newState);

    //for tcp socket using
    void onServiceConnected();
    void onBytesWritten(qint64 bytes);
    void onBytesReady();
    void onSocketError(QAbstractSocket::SocketError error);
    void onDeviceDisconnected();\
    void startConn();
    void stopConn();
};

#endif // PASERVICE_H
