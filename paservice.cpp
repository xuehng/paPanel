#include "paservice.h"
#include <QAudioFormat>
#include <QAudio>
#include <QDebug>
#include <QTimer>

PaService::PaService(QString addr, int port, QObject *parent) : QObject(parent)
{
    m_bufFile.setFileName("tmp/test.raw");
    m_inputDev = NULL;
    setIsRecording(false);
    setIsPlaying(false);
    //instantiate a Qaudioinput
    {
        m_bufFile.setFileName("tmp/test.raw");


        QAudioFormat format;
        // Set up the desired format, for example:

        //format.setSampleSize(16);
        //format.setSampleRate(44100);
        //format.setChannelCount(2);
        //format.setSampleType(QAudioFormat::SignedInt);
        //format.setByteOrder(QAudioFormat::LittleEndian);
        //format.setCodec("audio/pcm");

        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(8);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);

        qWarning() << "device0:" << QAudioDeviceInfo::availableDevices(QAudio::AudioInput).at(0).deviceName();
        qWarning() << "device default:" << QAudioDeviceInfo::defaultInputDevice().deviceName();
        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
        if (!info.isFormatSupported(format)) {
            qWarning() << "Default format not supported, trying to use the nearest.";
            format = info.nearestFormat(format);
        }

        m_audio_input = new QAudioInput(format, this);
        connect(m_audio_input, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));

    }

    //instantiate a QAudioOutput

    {
        QAudioFormat format;
        // Set up the format, eg.
        format.setSampleRate(8000);
        format.setChannelCount(1);
        format.setSampleSize(8);
        format.setCodec("audio/pcm");
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::UnSignedInt);

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) {
            qWarning() << "Raw audio format not supported by backend, cannot play audio.";
            return;
        }

        m_audioOutput = new QAudioOutput(format, this);
        connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged2(QAudio::State)));

    }


    // instantiate a QTcpSocket
    {
        m_connecting = false;

        m_addr = addr;
        m_port = port;
        m_socket = new QTcpSocket(this);
        connect(m_socket, SIGNAL(connected()), this, SLOT(onServiceConnected()));
        connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,SLOT(onSocketError(QAbstractSocket::SocketError)));
        connect(m_socket, &QTcpSocket::bytesWritten, this, &PaService::onBytesWritten);
        connect(m_socket, SIGNAL(readyRead()), this, SLOT(onBytesReady()));
        connect(m_socket, &QTcpSocket::disconnected, this, &PaService::onDeviceDisconnected);

        if ( m_addr.isEmpty() )
        {
            qWarning() << "no pa event address";
            return;
        }

    }
}


void PaService::startRecording()
{
    setIsRecording(true);
    //m_bufFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
    //m_audio_input->start(m_socket);
    m_inputDev = m_audio_input->start();
    connect( m_inputDev, SIGNAL(readyRead()), this, SLOT(ReadWrite()) );

    //QTimer::singleShot( 20000, this, SLOT(stopRecording()) );
}

void PaService::startRecordingToFile()
{
    setIsRecording(true);
    m_bufFile.open( QIODevice::WriteOnly | QIODevice::Truncate );
    m_audio_input->start(&m_bufFile);
    //m_inputDev = m_audio_input->start();
    //connect( m_inputDev, SIGNAL(readyRead()), this, SLOT(ReadWrite()) );

    //QTimer::singleShot( 20000, this, SLOT(stopRecording()) );
}

void PaService::ReadWrite(){
    QByteArray data = m_inputDev->readAll();
    m_socket->write( data, data.length() );
}

void PaService::stopRecording()
{    
    m_audio_input->stop();
    //m_bufFile.close();
    //delete m_audio_input;
    setIsRecording(false);
}

void PaService::stopRecordingToFile()
{
    m_audio_input->stop();
    m_bufFile.close();
    //delete m_audio_input;
    setIsRecording(false);
}

void PaService::setIsRecording(bool val){

    if(m_isRecording != val){
        m_isRecording = val;
        emit isRecordingChanged();
    }
}

void PaService::startPlay(){
    setIsPlaying(true);
    m_bufFile.open(QIODevice::ReadOnly);
    m_audioOutput->start(&m_bufFile);
}

void PaService::stopPlay(){
    m_audioOutput->stop();
    m_bufFile.close();
    //delete m_audioOutput;
    setIsPlaying(false);
}

void PaService::setIsPlaying(bool val){

    if(m_isPlaying != val){
        m_isPlaying = val;
        emit isRecordingChanged();
    }
}

void PaService::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::StoppedState:
            if (m_audio_input->error() != QAudio::NoError) {
                // Error handling
                qWarning() << "StoppedState";
            } else {
                // Finished recording
            }
            break;

        case QAudio::ActiveState:
            // Started recording - read from IO device
            qWarning() << "ActiveState  --  Started recording - read from IO device";
            break;

        default:
            // ... other cases as appropriate
            qWarning() << "... other cases as appropriate";
            break;
    }
}

void PaService::handleStateChanged2(QAudio::State newState)
{
    switch (newState) {
        case QAudio::IdleState:
            // Finished playing (no more data)
            qWarning() << "handleStateChanged2 IdleState";
            //stopPlay();
            break;

        case QAudio::StoppedState:
            qWarning() << "handleStateChanged2 StoppedState";
            // Stopped for other reasons
            if (m_audioOutput->error() != QAudio::NoError) {
                // Error handling
            }
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}



// for QTcpSocket using
void PaService::onServiceConnected()
{
    m_connecting = false;
    startRecording();

}

void PaService::onBytesWritten(qint64 bytes)
{
}

void PaService::onBytesReady()
{
    QByteArray data = m_socket->readAll();
    m_buffer.append(data);

    //while ( 1 ) {
        //if ( !parseBuffer() )
           // break;
    //}
}

void PaService::onSocketError(QAbstractSocket::SocketError error)
{
    qWarning() << "_____________tcp socket error";
   // emit paEventServiceOffline();

    //if ( !m_connecting ) {
        //m_connecting = true;
        //qWarning() << "socket error, reconnecting";
       // QTimer::singleShot(10000, this, SLOT(onTimeupAndReconnect()));
    //}
}

void PaService::onDeviceDisconnected()
{
    qWarning() << "_____tcp_disconnected";
    //emit paEventServiceOffline();

    //if ( !m_connecting ) {
        //m_connecting = true;
        //qWarning() << "pa event service disconnected, reconnecting";
        //QTimer::singleShot(10000, this, SLOT(onTimeupAndReconnect()));
    //}
}

void PaService::startConn(){
    m_connecting = true;
    m_socket->connectToHost(m_addr, m_port);
}

void PaService::stopConn(){
    m_connecting = false;
    m_socket->disconnectFromHost();
}


//void PAEventClient::onTimeupAndReconnect()
//{
    //m_connecting = false;
    //m_socket->connectToHost(m_addr, m_port);
//}

/*
bool PAEventClient::parseBuffer()
{
    int begin = m_buffer.indexOf('{');
    if ( begin == -1 ) {
        return false;
    }

    int end = m_buffer.indexOf('}');
    if ( end == -1 ) {
        return false;
    }

    QByteArray message = m_buffer.mid(begin, end - begin + 1);

    int left = m_buffer.size() - end - 1;
    if ( left >= 0 )
        m_buffer = m_buffer.right(left);
    else {
        qWarning() << "no data in buffer";
        m_buffer.clear();
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(message, &error);
    if ( error.error != QJsonParseError::NoError )
    {
        qWarning() << "parse json failed," << error.errorString();
        return true;
    }

    if ( !doc.isObject() )
    {
        return true;
    }

    QVariantMap result = doc.toVariant().toMap();
    handleMessage(result);

    return true;
}


void paClient::handleMessage(const QVariantMap& map)
{
    QString type = map.value("type").toString();

    if ( type == "pa_start" || type == "pa_end")
    {
        QString addr = map.value("addr").toString();
        emit paEventSignal(type,addr);
        return;
    }
}
*/
