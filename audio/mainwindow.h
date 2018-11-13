#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "audiorecorder.h"
#include "spectrumanalyser.h"
#include <QLineSeries>
#include <QStackedBarSeries>
#include <QBarCategoryAxis>
#include <QBarSet>
#include <QChartView>
#include <QValueAxis>
#include "spectrograph.h"
#include <math.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void chLevels(qreal, qreal);
    void spectrumChanged(FrequencySpectrum);

signals:
    void pause();
    void record();


private:
    Ui::MainWindow *ui;
    AudioRecorder *recorder;
    Spectrograph* m_spectrograph;
    int minFreq = 20;
    int maxFreq = 22000;
};

#endif // MAINWINDOW_H
