#include "LogParser.h"

#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QRegularExpression>
#include <QDebug>

LogParser::LogParser(QObject *parent)
    : QObject(parent)
{
    // default; main.cpp will override with setLogFilePath(...)
    m_logFilePath = QStringLiteral("telemetry.log");

    auto *timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);

    connect(timer, &QTimer::timeout,
            this, &LogParser::updateFromLog);

    // check very often – we'll only read new bytes
    timer->start(10);   // 10 ms
}

QVariantList LogParser::cpuHistory() const
{
    QVariantList out;
    out.reserve(m_cpuHist.size());
    for (double v : m_cpuHist)
        out.push_back(v);
    return out;
}

QVariantList LogParser::ramHistory() const
{
    QVariantList out;
    out.reserve(m_ramHist.size());
    for (double v : m_ramHist)
        out.push_back(v);
    return out;
}

QVariantList LogParser::tempHistory() const
{
    QVariantList out;
    out.reserve(m_tempHist.size());
    for (double v : m_tempHist)
        out.push_back(v);
    return out;
}

void LogParser::pushHistory(double value,
                            QVector<double> &hist,
                            int maxSize)
{
    hist.push_back(value);
    if (hist.size() > maxSize)
        hist.pop_front();
}

void LogParser::updateFromLog()
{
    QFile file(m_logFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // qWarning() << "[LogParser] Cannot open log file" << m_logFilePath;
        return;
    }

    // On first run, start from the beginning (replay whole file once)
    // If you want to ignore old content and only follow new data,
    // set m_lastPos = file.size() the first time instead.
    if (!m_initialized) {
        m_lastPos = 0;
        m_initialized = true;
    }

    // handle truncation/rotation
    if (m_lastPos > file.size()) {
        m_lastPos = 0;
    }

    // Jump to where we stopped last time
    file.seek(m_lastPos);

    QTextStream in(&file);
    in.setCodec("UTF-8");

    static QRegularExpression reCpu (R"(CPU usage:\s*(\d+)\s*%)");
    static QRegularExpression reRam (R"(RAM usage:\s*(\d+)\s*%)");
    static QRegularExpression reTemp(R"(TEMP usage:\s*(\d+))");

    bool anyChanged = false;

    // 🔥 VALUE-BY-VALUE PROCESSING
    while (!in.atEnd()) {
        QString line = in.readLine();
        m_lastPos = file.pos();   // remember how far we got

        double oldCpu  = m_cpu;
        double oldRam  = m_ram;
        double oldTemp = m_temp;

        bool localChange = false;

        if (line.contains("TelemetryApp (CPU):")) {
            QRegularExpressionMatch m = reCpu.match(line);
            if (m.hasMatch()) {
                double v = m.captured(1).toDouble();
                if (!qFuzzyCompare(1.0 + v, 1.0 + m_cpu)) {
                    m_cpu = v;
                    pushHistory(m_cpu, m_cpuHist, m_maxHistory);
                    localChange = true;
                }
            }
        } else if (line.contains("TelemetryApp (RAM):")) {
            QRegularExpressionMatch m = reRam.match(line);
            if (m.hasMatch()) {
                double v = m.captured(1).toDouble();
                if (!qFuzzyCompare(1.0 + v, 1.0 + m_ram)) {
                    m_ram = v;
                    pushHistory(m_ram, m_ramHist, m_maxHistory);
                    localChange = true;
                }
            }
        } else if (line.contains("TelemetryApp (TEMP):")) {
            QRegularExpressionMatch m = reTemp.match(line);
            if (m.hasMatch()) {
                double v = m.captured(1).toDouble();
                if (!qFuzzyCompare(1.0 + v, 1.0 + m_temp)) {
                    m_temp = v;
                    pushHistory(m_temp, m_tempHist, m_maxHistory);
                    localChange = true;
                }
            }
        }

        if (localChange) {
            anyChanged = true;
            emit valuesChanged();
            emit historyChanged();

            // optional debug:
            // qDebug() << "[LogParser] line:" << line
            //          << "CPU" << m_cpu << "RAM" << m_ram << "TEMP" << m_temp;
        }
    }

    file.close();

    // we already emitted per-value; nothing to do here
    Q_UNUSED(anyChanged);
}