#include "LogParser.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QTimer>
#include <QMap>
#include <QDebug>

LogParser::LogParser(QObject *parent)
    : QObject(parent)
{
    // default: log file in current working directory
    m_logFilePath = QStringLiteral("telemetry.log");

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LogParser::updateFromLog);

    // Faster refresh for more "alive" gauges
    timer->start(200); // 200 ms instead of 1000 ms
}

QVariantList LogParser::cpuHistory() const
{
    QVariantList list;
    list.reserve(m_cpuHistory.size());
    for (double v : m_cpuHistory)
        list.push_back(v);
    return list;
}

QVariantList LogParser::ramHistory() const
{
    QVariantList list;
    list.reserve(m_ramHistory.size());
    for (double v : m_ramHistory)
        list.push_back(v);
    return list;
}

QVariantList LogParser::tempHistory() const
{
    QVariantList list;
    list.reserve(m_tempHistory.size());
    for (double v : m_tempHistory)
        list.push_back(v);
    return list;
}

void LogParser::setLogFilePath(const QString &path)
{
    m_logFilePath = path;
    // reset state so we re-read everything
    m_lastTimestamp = QDateTime();
    m_cpuHistory.clear();
    m_ramHistory.clear();
    m_tempHistory.clear();
}

void LogParser::updateFromLog()
{
    parseLogFile();
}

void LogParser::parseLogFile()
{
    QFile file(m_logFilePath);
    if (!file.exists()) {
        // no file yet, nothing to do
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open log file" << m_logFilePath;
        return;
    }

    QTextStream in(&file);

    // Example line:
    // [2026-02-22 13:54:37] [INFO] TelemetryApp (CPU): CPU usage: 10%
    // [2026-02-22 13:54:37] [INFO] TelemetryApp (RAM): RAM usage: 61%
    // [2026-02-22 13:54:37] [INFO] TelemetryApp (TEMP): TEMP usage: 58°C

    QRegularExpression re(
        R"(\[(?<ts>[\d\-: ]+)\]\s+\[INFO\]\s+TelemetryApp\s+\((?<kind>CPU|RAM|TEMP)\):.*?(?<value>\d+))"
    );

    struct Sample {
        double cpu = -1.0;
        double ram = -1.0;
        double temp = -1.0;
        QDateTime dt;
    };

    QMap<QString, Sample> samples; // key: timestamp string

    while (!in.atEnd()) {
        const QString line = in.readLine();
        QRegularExpressionMatch m = re.match(line);
        if (!m.hasMatch())
            continue;

        const QString tsStr = m.captured("ts");
        const QString kind  = m.captured("kind");
        const QString valStr = m.captured("value");

        bool ok = false;
        double val = valStr.toDouble(&ok);
        if (!ok)
            continue;

        Sample sample = samples.value(tsStr);
        if (!sample.dt.isValid()) {
            sample.dt = QDateTime::fromString(tsStr, "yyyy-MM-dd HH:mm:ss");
        }

        if (kind == QLatin1String("CPU"))
            sample.cpu = val;
        else if (kind == QLatin1String("RAM"))
            sample.ram = val;
        else if (kind == QLatin1String("TEMP"))
            sample.temp = val;

        samples.insert(tsStr, sample);
    }

    file.close();

    bool updated = false;
    bool historyUpdated = false;

    // Process samples in chronological order
    const auto keys = samples.keys();
    for (const QString &tsStr : keys) {
        const Sample &s = samples.value(tsStr);
        if (!s.dt.isValid())
            continue;

        // require full triple (CPU, RAM, TEMP)
        if (s.cpu < 0 || s.ram < 0 || s.temp < 0)
            continue;

        // skip ones we've already processed
        if (m_lastTimestamp.isValid() && s.dt <= m_lastTimestamp)
            continue;

        m_lastTimestamp = s.dt;

        m_cpu = s.cpu;
        m_ram = s.ram;
        m_temp = s.temp;
        updated = true;

        m_cpuHistory.push_back(m_cpu);
        m_ramHistory.push_back(m_ram);
        m_tempHistory.push_back(m_temp);
        historyUpdated = true;
    }

    // Limit history size a bit so it doesn't grow forever
    const int maxHistory = 500;
    auto trim = [maxHistory](QVector<double> &v) {
        if (v.size() > maxHistory) {
            int toRemove = v.size() - maxHistory;
            v.erase(v.begin(), v.begin() + toRemove);
        }
    };
    if (historyUpdated) {
        trim(m_cpuHistory);
        trim(m_ramHistory);
        trim(m_tempHistory);
    }

    if (updated)
        emit dataChanged();
    if (historyUpdated)
        emit historyChanged();
}