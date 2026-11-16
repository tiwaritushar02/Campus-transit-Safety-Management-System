#include "BusTrackingService.hpp"
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDir>
#include <QDateTime>

BusTrackingService::BusTrackingService()
    : m_gpsDatPath("data/simulated_gps.dat"),
      m_historyCsvPath("data/tracking_history.csv"),
      m_arrivalCsvPath("data/arrival_records.csv") {
    ensureDataDirectory();

    // Default Fleet
    m_fleet.append({"BUS101", "UK04-PA-1011", "Ramesh Chandra Joshi", "+91-9876543210", 42, "IDLE"});
    m_fleet.append({"BUS102", "UK04-PA-2022", "Kundan Singh Bisht", "+91-9876543211", 45, "IDLE"});
    m_fleet.append({"BUS103", "UK04-PA-3033", "Mohan Lal Pandey", "+91-9876543212", 40, "IDLE"});
}

void BusTrackingService::ensureDataDirectory() const {
    QDir dir("data");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

FleetBusInfo BusTrackingService::getBusInfo(const QString& busId) const {
    for (const auto& b : m_fleet) {
        if (b.busId == busId) return b;
    }
    return {"BUS101", "UK04-PA-1011", "Ramesh Chandra Joshi", "+91-9876543210", 42, "IDLE"};
}

void BusTrackingService::updateBusStatus(const QString& busId, const QString& status) {
    for (auto& b : m_fleet) {
        if (b.busId == busId) {
            b.currentStatus = status;
            break;
        }
    }
}

void BusTrackingService::appendGpsRecord(const QString& busId, const GPSLocation& location) {
    QFile file(m_gpsDatPath);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        // Write binary/dat fields: busId, lat, lon, alt, speed, timestamp
        out << busId;
        out << location.getLatitude();
        out << location.getLongitude();
        out << location.getAltitude();
        out << location.getSpeed();
        out << location.getTimestamp();
        out << location.isSimulated();
        file.close();
    }
}

void BusTrackingService::logTrackingEvent(const QString& busId, const QString& event, const QString& location, const QString& status) {
    ensureDataDirectory();
    QFile file(m_historyCsvPath);
    bool writeHeader = !file.exists();
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        if (writeHeader) {
            out << "timestamp,bus_id,event,location,status\n";
        }
        QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        out << ts << "," << busId << ",\"" << event << "\",\"" << location << "\"," << status << "\n";
        file.close();
    }
}

void BusTrackingService::recordStopArrival(const QString& busId, const QString& stopName, const QString& arrivalTime) {
    ensureDataDirectory();
    QFile file(m_arrivalCsvPath);
    bool writeHeader = !file.exists();
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        if (writeHeader) {
            out << "timestamp,bus_id,stop_name,arrival_time,mode\n";
        }
        QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        out << ts << "," << busId << ",\"" << stopName << "\"," << arrivalTime << ",SIMULATED\n";
        file.close();
    }
}

QVector<TrackingLogEntry> BusTrackingService::readTrackingHistory(int maxEntries) const {
    QVector<TrackingLogEntry> history;
    QFile file(m_historyCsvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return history;
    }

    QTextStream in(&file);
    QString header = in.readLine(); // skip header
    while (!in.atEnd() && history.size() < maxEntries) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(',');
        if (parts.size() >= 5) {
            TrackingLogEntry entry;
            entry.timestamp = parts[0];
            entry.busId = parts[1];
            entry.event = parts[2].remove('"');
            entry.location = parts[3].remove('"');
            entry.status = parts[4];
            history.prepend(entry); // newest first
        }
    }
    file.close();
    return history;
}

QVector<QStringList> BusTrackingService::getScheduleTable(const QString& busId) const {
    Q_UNUSED(busId);
    QVector<QStringList> schedule;
    schedule.append({"ST101", "Haldwani Bus Terminal", "07:30 AM", "07:30 AM", "COMPLETED"});
    schedule.append({"ST102", "Kathgodam Railway Station", "07:48 AM", "07:49 AM", "DEPARTED"});
    schedule.append({"ST103", "Ranibagh Junction", "08:02 AM", "08:05 AM", "IN TRANSIT"});
    schedule.append({"ST104", "Bhujiaghat Mountain Pass", "08:22 AM", "08:25 AM", "SCHEDULED"});
    schedule.append({"ST105", "Bhimtal Lake Mall Road", "08:40 AM", "08:42 AM", "SCHEDULED"});
    schedule.append({"ST106", "Graphic Era Hill University", "08:55 AM", "08:58 AM", "DESTINATION"});
    return schedule;
}
