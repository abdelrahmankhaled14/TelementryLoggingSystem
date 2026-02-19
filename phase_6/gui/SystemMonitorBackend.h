#pragma once

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QString>

class SystemMonitorBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(int cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)

public:
    explicit SystemMonitorBackend(QObject *parent = nullptr);

    int cpuUsage() const { return m_cpuUsage; }

signals:
    void cpuUsageChanged();
    void cpuSample(int value);   // For QML history graph
    void logLoaded(bool ok);

public slots:
    void start();
    void pause();
    void reset();

private slots:
    void playNext();             // Called periodically, step to next value

private:
    void loadLogOnce();          // Read log1.txt and fill m_values

    int m_cpuUsage = 0;
    QTimer m_timer;

    QVector<int> m_values;       // Parsed CPU values from log
    int m_index = 0;
    bool m_loaded = false;
    QString m_logPath;
};
