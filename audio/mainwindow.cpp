#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"


/*
 To-do:
    normalise on current volume
    remove zeros from spectrum
    increase resolution
    move chatter to thread
    mb store buffer file to ram
    wtf with constant bitrate encoding?
    pass pointers instead of whole spectrum
    convert spectrograph to openGL
    map spectrumChanged directly to spectrograph
    try integration with weights instead of searching maximum amplitude in order to prevent white colour
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    recorder = new AudioRecorder();
    QObject::connect(this, &MainWindow::record, recorder, &AudioRecorder::toggleRecord);
    QObject::connect(this, &MainWindow::pause, recorder, &AudioRecorder::togglePause);
    QObject::connect(recorder, &AudioRecorder::chLevels, this, &MainWindow::chLevels);
    QObject::connect(recorder, &AudioRecorder::spectrumChangedSignal, this, &MainWindow::spectrumChanged);
    record();

    m_spectrograph = new Spectrograph(this);
    ui->splitter->addWidget(m_spectrograph);
    int SpectrumNumBands = 256;
    m_spectrograph->setParams(SpectrumNumBands, minFreq, maxFreq);
}

MainWindow::~MainWindow()
{
    pause();
    delete recorder;
    delete ui;
}

void MainWindow::chLevels(qreal l, qreal r){
    ui->ch1->setValue(l*100);
    ui->ch2->setValue(r*100);
}

void MainWindow::spectrumChanged(FrequencySpectrum data){
    m_spectrograph->spectrumChanged(data);
}
