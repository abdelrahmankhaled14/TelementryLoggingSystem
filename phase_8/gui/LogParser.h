#pragma once

#include <QObject>
#include <QVector>
#include <QVariant>
#include <QString>

class LogParser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double cpu  READ cpu  NOTIFY valuesChanged)
    Q_PROPERTY(double ram  READ ram  NOTIFY valuesChanged)
    Q_PROPERTY(double temp READ temp NOTIFY valuesChanged)

public:
    explicit LogParser(QObject *parent = nullptr);

    double cpu()  const { return m_cpu; }
    double ram()  const { return m_ram; }
    double temp() const { return m_temp; }

    // main.cpp uses this
    void setLogFilePath(const QString &path) { m_logFilePath = path; }

    // history for the graph
    Q_INVOKABLE QVariantList cpuHistory() const;
    Q_INVOKABLE QVariantList ramHistory() const;
    Q_INVOKABLE QVariantList tempHistory() const;

public slots:
    void updateFromLog();

signals:
    void valuesChanged();    // CPU/RAM/TEMP updated
    void historyChanged();   // history arrays updated

private:
    void pushHistory(double value,
                     QVector<double> &hist,
                     int maxSize);

private:
    QString m_logFilePath;

    double m_cpu  = 0.0;
    double m_ram  = 0.0;
    double m_temp = 0.0;

    QVector<double> m_cpuHist;
    QVector<double> m_ramHist;
    QVector<double> m_tempHist;

    int m_maxHistory = 500;

    // tailing state
    qint64 m_lastPos   = 0;   // where we stopped reading last time
    bool   m_initialized = false;
};