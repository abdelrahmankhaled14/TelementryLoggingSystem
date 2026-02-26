#ifndef LOGPARSER_H
#define LOGPARSER_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QVariantList>

class LogParser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double cpu READ cpu NOTIFY dataChanged)
    Q_PROPERTY(double ram READ ram NOTIFY dataChanged)
    Q_PROPERTY(double temp READ temp NOTIFY dataChanged)

public:
    explicit LogParser(QObject *parent = nullptr);

    double cpu() const { return m_cpu; }
    double ram() const { return m_ram; }
    double temp() const { return m_temp; }

    Q_INVOKABLE QVariantList cpuHistory() const;
    Q_INVOKABLE QVariantList ramHistory() const;
    Q_INVOKABLE QVariantList tempHistory() const;

    Q_INVOKABLE void setLogFilePath(const QString &path);

signals:
    void dataChanged();
    void historyChanged();

private slots:
    void updateFromLog();

private:
    void parseLogFile();

    QString m_logFilePath;

    double m_cpu = 0.0;
    double m_ram = 0.0;
    double m_temp = 0.0;

    QVector<double> m_cpuHistory;
    QVector<double> m_ramHistory;
    QVector<double> m_tempHistory;

    QDateTime m_lastTimestamp; // last timestamp we processed
};

#endif // LOGPARSER_H