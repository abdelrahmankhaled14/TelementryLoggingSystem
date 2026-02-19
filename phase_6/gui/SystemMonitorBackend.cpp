#include "SystemMonitorBackend.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

SystemMonitorBackend::SystemMonitorBackend(QObject *parent)
    : QObject(parent)
{
    // From gui/build → ../../build/log1.txt
    m_logPath = QCoreApplication::applicationDirPath()
                + "/../../build/log1.txt";

    // 200 ms between samples (adjust if you want slower/faster playback)
    m_timer.setInterval(200);

    connect(&m_timer, &QTimer::timeout,
            this, &SystemMonitorBackend::playNext);
}

void SystemMonitorBackend::loadLogOnce() {
    if (m_loaded)
        return;

    m_values.clear();

    QFile file(m_logPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[GUI] Failed to open log file:" << m_logPath;
        emit logLoaded(false);
        return;
    }

    QTextStream in(&file);

    // Matches: "CPU usage: 74%"
    QRegularExpression re(QStringLiteral("CPU usage:\\s*(\\d+)%"));
    if (!re.isValid()) {
        qWarning() << "[GUI] Regex invalid:" << re.errorString();
        // We could early-return here, but let's just not match anything.
    }

    while (!in.atEnd()) {
        const QString line = in.readLine();

        if (!re.isValid())
            continue;

        QRegularExpressionMatch m = re.match(line);
        if (m.hasMatch()) {
            bool ok = false;
            int v = m.captured(1).toInt(&ok);
            if (ok) {
                if (v < 0)   v = 0;
                if (v > 100) v = 100;
                m_values.append(v);
                // qDebug() << "[GUI] Parsed CPU value:" << v;
            }
        }
    }

    file.close();

    if (m_values.isEmpty()) {
        qWarning() << "[GUI] No CPU values found in log file:" << m_logPath;
        emit logLoaded(false);
        return;
    }

    qDebug() << "[GUI] Loaded" << m_values.size() << "CPU samples from log.";
    m_loaded = true;
    m_index = 0;
    emit logLoaded(true);
}

void SystemMonitorBackend::start() {
    loadLogOnce();
    if (!m_loaded)
        return;

    if (!m_timer.isActive())
        m_timer.start();
}

void SystemMonitorBackend::pause() {
    m_timer.stop();
}

void SystemMonitorBackend::reset() {
    pause();
    m_index = 0;

    if (!m_values.isEmpty()) {
        m_cpuUsage = m_values.first();
        emit cpuUsageChanged();
        emit cpuSample(m_cpuUsage);
    }
}

void SystemMonitorBackend::playNext() {
    if (!m_loaded || m_values.isEmpty())
        return;

    int v = m_values[m_index];

    if (v != m_cpuUsage) {
        m_cpuUsage = v;
        emit cpuUsageChanged();
    }
    emit cpuSample(v);

    m_index++;
    if (m_index >= m_values.size()) {
        m_index = 0; // loop playback
    }
}
