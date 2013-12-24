#ifndef RTPLOT_H
#define RTPLOT_H

#include "qcustomplot.h"
#include <QDockWidget>

#include "qcustomplot.h"
#include <QObject>
#include <QElapsedTimer>

class Waveform : public QObject
{
    Q_OBJECT
public:
    explicit Waveform(QObject *parent = 0);
    int id() { return m_id; }

    QCPGraph *graph;
private:
    int m_id;
    static int m_nextID;
};

class RTPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit RTPlot(int id, QWidget *parent = 0);
    Waveform* addWaveform(const QString &name = QString());
    Waveform* waveform(int id);
    void setTimeWindow(int sec);
    int timeWindow();

signals:

public slots:
    void start();
    void stop();
    void addData(double data, int id);
    void addData(double data, Waveform *wf);

private slots:


private:
    void _setup();
    void _clearAllWaveforms();
    double _elapsedSeconds();
    QColor _pickWaveformColor();
    void mousePressEvent(QMouseEvent *event);

    int m_id;
    double m_timeWindow_sec;
    int m_nextColorIdx;
    QVector<QColor> m_defaultColors;
    QElapsedTimer m_clock;
    QMap<int, Waveform*> m_waveforms;
    bool m_replotAfterAdding;
};

class WaveformMapper
{
public:
     RTPlot *plot;
     int waveformID;
};


class RTPlotDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit RTPlotDock(RTPlot *plot, QWidget *parent= 0);
    void setTitle(const QString &title);
    RTPlot *plot();
private:
    RTPlot *m_plot;

};

#endif // RTPLOT_H
