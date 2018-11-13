#include "audiorecorder.h"

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);

template <class T>
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

AudioRecorder::AudioRecorder()
    :m_spectrumAnalyser()
{
    qRegisterMetaType<FrequencySpectrum>("FrequencySpectrum");
    qRegisterMetaType<WindowFunction>("WindowFunction");
    audioRecorder = new QAudioRecorder();
    probe = new QAudioProbe;
    QObject::connect(probe, &QAudioProbe::audioBufferProbed, this, &AudioRecorder::processBuffer);
    probe->setSource(audioRecorder);

    qDebug() << "inputs:";
    foreach (const QString &device, audioRecorder->audioInputs()) {
        //ui->audioDeviceBox->addItem(device, QVariant(device));
        qDebug() << "   " << device;
        inDevice = device;
    }

    qDebug() << "codecs:";
    foreach (const QString &codecName, audioRecorder->supportedAudioCodecs()) {
        //ui->audioCodecBox->addItem(codecName, QVariant(codecName));
        qDebug() << "   " << codecName;
        codec = codecName;
    }

    qDebug() << "containers:";
    foreach (const QString &containerName, audioRecorder->supportedContainers()) {
        //ui->containerBox->addItem(containerName, QVariant(containerName));
        qDebug() << "   " << containerName;
        container = containerName;
    }

    qDebug() << "sample rate:";
    foreach (int sampleRate, audioRecorder->supportedAudioSampleRates()) {
        //ui->sampleRateBox->addItem(QString::number(sampleRate), QVariant(sampleRate));
        qDebug() << "   " << sampleRate;
        sRate = sampleRate;
    }

    QObject::connect(audioRecorder, &QAudioRecorder::durationChanged, this, &AudioRecorder::updateProgress);
    QObject::connect(audioRecorder, &QMediaRecorder::statusChanged, this, &AudioRecorder::updateStatus);
    QObject::connect(audioRecorder, &QMediaRecorder::stateChanged, this, &AudioRecorder::onStateChanged);
    //QObject::connect(audioRecorder, &QMediaRecorder::error, this, &AudioRecorder::displayErrorMessage);
    QObject::connect(&m_spectrumAnalyser, &SpectrumAnalyser::spectrumChanged, this, &AudioRecorder::spectrumChangedSignal);
}

AudioRecorder::~AudioRecorder()
{
    delete audioRecorder;
    delete probe;
}

void AudioRecorder::updateProgress(qint64 duration)
{
    if (audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;
    qDebug() << "updateProgress " << tr("Recorded %1 sec").arg(duration / 1000);
    audioRecorder->stop();
    audioRecorder->record();
}

void AudioRecorder::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status) {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(audioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
        statusMessage = tr("Stopped");
    default:
        break;
    }

    if (audioRecorder->error() == QMediaRecorder::NoError)
        qDebug() << "updateStatus: " << statusMessage;
}

void AudioRecorder::onStateChanged(QMediaRecorder::State state)
{
    switch (state) {
    case QMediaRecorder::RecordingState:
        qDebug() << "recording state";
        //ui->recordButton->setText(tr("Stop"));
        //ui->pauseButton->setText(tr("Pause"));
        break;
    case QMediaRecorder::PausedState:
        qDebug() << "paused state";
        //ui->recordButton->setText(tr("Stop"));
        //ui->pauseButton->setText(tr("Resume"));
        break;
    case QMediaRecorder::StoppedState:
        qDebug() << "stopped state";
        //ui->recordButton->setText(tr("Record"));
        //ui->pauseButton->setText(tr("Pause"));
        break;
    }

    //ui->pauseButton->setEnabled(audioRecorder->state() != QMediaRecorder::StoppedState);
    qDebug() << "pauseEnabled " << (audioRecorder->state() != QMediaRecorder::StoppedState);
}


void AudioRecorder::toggleRecord()
{
    if (audioRecorder->state() == QMediaRecorder::StoppedState) {
        audioRecorder->setAudioInput(inDevice);

        QAudioEncoderSettings settings;
        /*
        settings.setCodec(boxValue(ui->audioCodecBox).toString());
        settings.setSampleRate(boxValue(ui->sampleRateBox).toInt());
        settings.setBitRate(boxValue(ui->bitrateBox).toInt());
        settings.setChannelCount(boxValue(ui->channelsBox).toInt());
        settings.setQuality(QMultimedia::EncodingQuality(ui->qualitySlider->value()));
        settings.setEncodingMode(ui->constantQualityRadioButton->isChecked() ?
                                 QMultimedia::ConstantQualityEncoding :
                                 QMultimedia::ConstantBitRateEncoding);

        QString container = boxValue(ui->containerBox).toString();
        */
        settings.setCodec(codec);
        settings.setSampleRate(sRate);
        settings.setBitRate(bRate);
        settings.setChannelCount(chCount);
        settings.setQuality(QMultimedia::VeryHighQuality);
        settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
        //settings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
        audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
        audioRecorder->setOutputLocation(QUrl::fromLocalFile("test"));
        audioRecorder->record();
    }
    else {
        audioRecorder->stop();
    }
}

void AudioRecorder::togglePause()
{
    if (audioRecorder->state() != QMediaRecorder::PausedState)
        audioRecorder->pause();
    else
        audioRecorder->record();
}

void AudioRecorder::displayErrorMessage()
{
    qDebug() << "displayError " << audioRecorder->errorString();
}

void AudioRecorder::clearAudioLevels()
{
    for (int i = 0; i < audioLevels.size(); ++i)
        audioLevels.replace(i, 0);
}

// This function returns the maximum possible sample value for a given audio format
qreal getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

// returns the audio level for each channel
QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}


template <class T>
QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}

void AudioRecorder::processBuffer(const QAudioBuffer& buffer)
{
    if (audioLevels.count() != buffer.format().channelCount()) {
        audioLevels.clear();
        for (int i = 0; i < buffer.format().channelCount(); ++i) {
            audioLevels.append(0);
        }
    }

    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i){
        audioLevels.replace(i, levels.at(i));
    }
    emit chLevels(audioLevels.first(), audioLevels.last());
    stackBuffer(buffer);

}

void AudioRecorder::stackBuffer(const QAudioBuffer &buffer){
    byteArr.append(buffer.constData<char>(), buffer.byteCount());
    int count = pow(2, FFTLengthPowerOfTwo) * (buffer.format().sampleSize() * buffer.format().channelCount() / 8);
    while(byteArr.size() >= count){
        m_spectrumAnalyser.calculate(byteArr.left(count), buffer.format());
        byteArr.remove(0, count);
    }

}
