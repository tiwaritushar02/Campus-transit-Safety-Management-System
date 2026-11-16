#include "NativeOsmMapWidget.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QNetworkRequest>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor
NativeOsmMapWidget::NativeOsmMapWidget(QWidget* parent)
    : QWidget(parent),
      m_centerLat(29.2850),
      m_centerLon(79.5380),
      m_zoom(12),
      m_isDragging(false),
      m_simStep(0),
      m_isSimulating(false) {

    setMinimumSize(400, 350);
    setMouseTracking(true);

    m_cacheDir = "cache/tiles";
    QDir().mkpath(m_cacheDir);

    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, &QNetworkAccessManager::finished, this, &NativeOsmMapWidget::onTileReply);

    m_simTimer = new QTimer(this);
    connect(m_simTimer, &QTimer::timeout, this, &NativeOsmMapWidget::onSimStep);
}

// Destructor
NativeOsmMapWidget::~NativeOsmMapWidget() {
    if (m_simTimer && m_simTimer->isActive()) {
        m_simTimer->stop();
    }
}

double NativeOsmMapWidget::lonToTileX(double lon, int z) const {
    return (lon + 180.0) / 360.0 * (1 << z);
}

double NativeOsmMapWidget::latToTileY(double lat, int z) const {
    double rad = lat * M_PI / 180.0;
    return (1.0 - std::asinh(std::tan(rad)) / M_PI) / 2.0 * (1 << z);
}

double NativeOsmMapWidget::tileXToLon(double x, int z) const {
    return x / (1 << z) * 360.0 - 180.0;
}

double NativeOsmMapWidget::tileYToLat(double y, int z) const {
    double n = M_PI - 2.0 * M_PI * y / (1 << z);
    return 180.0 / M_PI * std::atan(std::sinh(n));
}

QPointF NativeOsmMapWidget::geoToPixel(double lat, double lon) const {
    double tileX = lonToTileX(lon, m_zoom);
    double tileY = latToTileY(lat, m_zoom);

    double centerTileX = lonToTileX(m_centerLon, m_zoom);
    double centerTileY = latToTileY(m_centerLat, m_zoom);

    double px = width() / 2.0 + (tileX - centerTileX) * 256.0;
    double py = height() / 2.0 + (tileY - centerTileY) * 256.0;
    return QPointF(px, py);
}

QPointF NativeOsmMapWidget::pixelToGeo(const QPointF& pixel) const {
    double centerTileX = lonToTileX(m_centerLon, m_zoom);
    double centerTileY = latToTileY(m_centerLat, m_zoom);

    double tileX = centerTileX + (pixel.x() - width() / 2.0) / 256.0;
    double tileY = centerTileY + (pixel.y() - height() / 2.0) / 256.0;

    double lon = tileXToLon(tileX, m_zoom);
    double lat = tileYToLat(tileY, m_zoom);
    return QPointF(lat, lon);
}

void NativeOsmMapWidget::setMapCenter(double lat, double lon, int zoom) {
    m_centerLat = lat;
    m_centerLon = lon;
    m_zoom = std::clamp(zoom, 10, 16);
    update();
}

void NativeOsmMapWidget::clearRoute() {
    m_routeCoordinates.clear();
    m_simStep = 0;
    m_isSimulating = false;
    if (m_simTimer->isActive()) m_simTimer->stop();
    update();
}

void NativeOsmMapWidget::setRouteCoordinates(const QVector<QPointF>& coords) {
    m_routeCoordinates = coords;
    if (!m_routeCoordinates.isEmpty()) {
        m_busPos = m_routeCoordinates.first();
    }
    update();
}

// Function Overloading
void NativeOsmMapWidget::addMarker(const QString& stopId, double lat, double lon, const QString& label, const QString& subtext, const QColor& color, const QString& iconText, bool isSelectedSource, bool isDestination) {
    m_markers.append({stopId, lat, lon, label, subtext, color, iconText, isSelectedSource, isDestination});
    update();
}

// Function Overloading
void NativeOsmMapWidget::addMarker(double lat, double lon, const QString& label, const QString& subtext, const QColor& color, const QString& iconText) {
    addMarker("", lat, lon, label, subtext, color, iconText, false, false);
}

void NativeOsmMapWidget::setSelectedSourceStopId(const QString& stopId) {
    for (auto& m : m_markers) {
        if (!m.isDestination) {
            m.isSelectedSource = (m.stopId == stopId);
            if (m.isSelectedSource) {
                m.color = QColor("#238636");
                m.iconText = "A";
            } else {
                m.color = QColor("#1F6FEB");
                m.iconText = "S";
            }
        }
    }
    update();
}

void NativeOsmMapWidget::clearMarkers() {
    m_markers.clear();
    update();
}

void NativeOsmMapWidget::zoomIn() {
    if (m_zoom < 16) {
        m_zoom++;
        update();
    }
}

void NativeOsmMapWidget::zoomOut() {
    if (m_zoom > 10) {
        m_zoom--;
        update();
    }
}

void NativeOsmMapWidget::fitRouteBounds() {
    if (m_routeCoordinates.isEmpty()) return;

    double minLat = 90.0, maxLat = -90.0;
    double minLon = 180.0, maxLon = -180.0;

    for (const auto& pt : m_routeCoordinates) {
        minLat = std::min(minLat, pt.x());
        maxLat = std::max(maxLat, pt.x());
        minLon = std::min(minLon, pt.y());
        maxLon = std::max(maxLon, pt.y());
    }

    m_centerLat = (minLat + maxLat) / 2.0;
    m_centerLon = (minLon + maxLon) / 2.0;

    double latSpan = maxLat - minLat;
    double lonSpan = maxLon - minLon;
    double maxSpan = std::max(latSpan, lonSpan);

    if (maxSpan > 0.4) m_zoom = 11;
    else if (maxSpan > 0.18) m_zoom = 12;
    else if (maxSpan > 0.08) m_zoom = 13;
    else m_zoom = 14;

    update();
}

void NativeOsmMapWidget::requestTile(int z, int x, int y) {
    QString tileKey = QString("%1_%2_%3").arg(z).arg(x).arg(y);
    if (m_memTileCache.contains(tileKey)) return;

    QString filePath = QString("%1/%2.png").arg(m_cacheDir).arg(tileKey);
    if (QFile::exists(filePath)) {
        QPixmap pm(filePath);
        if (!pm.isNull()) {
            m_memTileCache[tileKey] = pm;
            return;
        }
    }

    QUrl url(QString("https://tile.openstreetmap.org/%1/%2/%3.png").arg(z).arg(x).arg(y));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, "SmartCollegeBusManagementSystem/1.0 (academic-pbl; contact: tiwaritushar565@gmail.com)");
    req.setAttribute(QNetworkRequest::User, tileKey);

    m_netManager->get(req);
}

void NativeOsmMapWidget::onTileReply(QNetworkReply* reply) {
    QString tileKey = reply->request().attribute(QNetworkRequest::User).toString();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap pm;
        if (pm.loadFromData(data)) {
            m_memTileCache[tileKey] = pm;

            QString filePath = QString("%1/%2.png").arg(m_cacheDir).arg(tileKey);
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
            }
            update();
        }
    }
    reply->deleteLater();
}

void NativeOsmMapWidget::fetchOsrmRoute(double startLat, double startLon, double destLat, double destLon) {
    QString urlStr = QString("https://router.project-osrm.org/route/v1/driving/%1,%2;%3,%4?overview=full&geometries=geojson")
                         .arg(startLon, 0, 'f', 6)
                         .arg(startLat, 0, 'f', 6)
                         .arg(destLon, 0, 'f', 6)
                         .arg(destLat, 0, 'f', 6);

    QUrl url(urlStr);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, "SmartCollegeBusManagementSystem/1.0 (academic-pbl; contact: tiwaritushar565@gmail.com)");

    QNetworkReply* reply = m_netManager->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            if (root["code"].toString() == "Ok") {
                QJsonArray routes = root["routes"].toArray();
                if (!routes.isEmpty()) {
                    QJsonObject r = routes[0].toObject();
                    double distMeters = r["distance"].toDouble();
                    double durationSecs = r["duration"].toDouble();

                    double distKm = distMeters / 1000.0;
                    int durMins = static_cast<int>(std::round(durationSecs / 60.0));

                    QJsonObject geom = r["geometry"].toObject();
                    QJsonArray coords = geom["coordinates"].toArray();

                    QVector<QPointF> pathPoints;
                    for (int i = 0; i < coords.size(); ++i) {
                        QJsonArray pt = coords[i].toArray();
                        pathPoints.append(QPointF(pt[1].toDouble(), pt[0].toDouble()));
                    }

                    setRouteCoordinates(pathPoints);
                    fitRouteBounds();
                    emit routeCalculated(distKm, durMins, "ACTIVE ROUTE LOADED");
                }
            } else {
                emit routeError("OSRM API returned no route: " + root["message"].toString());
            }
        } else {
            emit routeError("Network error contacting routing engine: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void NativeOsmMapWidget::startBusSimulation() {
    if (m_routeCoordinates.isEmpty()) return;
    m_isSimulating = true;
    m_simTimer->start(60);
}

void NativeOsmMapWidget::pauseBusSimulation() {
    m_isSimulating = false;
    m_simTimer->stop();
    update();
}

void NativeOsmMapWidget::resetBusSimulation() {
    m_isSimulating = false;
    m_simTimer->stop();
    m_simStep = 0;
    if (!m_routeCoordinates.isEmpty()) {
        m_busPos = m_routeCoordinates.first();
    }
    update();
}

void NativeOsmMapWidget::onSimStep() {
    if (m_simStep < m_routeCoordinates.size()) {
        m_busPos = m_routeCoordinates[m_simStep];
        int pct = static_cast<int>(std::round((m_simStep * 100.0) / m_routeCoordinates.size()));
        emit busProgress(pct, QString("Traversing Highway NH109 [%1%] • Speed: 42 km/h").arg(pct));
        m_simStep++;
        update();
    } else {
        m_simTimer->stop();
        m_isSimulating = false;
        m_busPos = m_routeCoordinates.last();
        emit busProgress(100, "Arrived at Graphic Era Hill University, Bhimtal Campus!");
        update();
    }
}

// Function Overriding
// Polymorphism
void NativeOsmMapWidget::paintEvent(QPaintEvent* event) {
    (void)event;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 1. Render Map Slippy Tiles
    double centerTileX = lonToTileX(m_centerLon, m_zoom);
    double centerTileY = latToTileY(m_centerLat, m_zoom);

    int startTileX = static_cast<int>(std::floor(centerTileX - (width() / 2.0) / 256.0));
    int endTileX = static_cast<int>(std::ceil(centerTileX + (width() / 2.0) / 256.0));
    int startTileY = static_cast<int>(std::floor(centerTileY - (height() / 2.0) / 256.0));
    int endTileY = static_cast<int>(std::ceil(centerTileY + (height() / 2.0) / 256.0));

    p.fillRect(rect(), QColor("#0D1117"));

    for (int tx = startTileX; tx <= endTileX; ++tx) {
        for (int ty = startTileY; ty <= endTileY; ++ty) {
            double px = width() / 2.0 + (tx - centerTileX) * 256.0;
            double py = height() / 2.0 + (ty - centerTileY) * 256.0;
            QRectF tileRect(px, py, 256, 256);

            QString tileKey = QString("%1_%2_%3").arg(m_zoom).arg(tx).arg(ty);
            if (m_memTileCache.contains(tileKey)) {
                p.drawPixmap(tileRect.topLeft(), m_memTileCache[tileKey]);
            } else {
                requestTile(m_zoom, tx, ty);
                p.setBrush(QColor("#161B22"));
                p.setPen(QPen(QColor("#21262D"), 1));
                p.drawRect(tileRect);
            }
        }
    }

    // High-Contrast Subtle Dark Overlay
    p.fillRect(rect(), QColor(13, 17, 23, 75));

    // 2. Draw Real Road Route Geometry
    if (m_routeCoordinates.size() >= 2) {
        QPainterPath routePath;
        QPointF firstPt = geoToPixel(m_routeCoordinates[0].x(), m_routeCoordinates[0].y());
        routePath.moveTo(firstPt);

        for (int i = 1; i < m_routeCoordinates.size(); ++i) {
            QPointF pt = geoToPixel(m_routeCoordinates[i].x(), m_routeCoordinates[i].y());
            routePath.lineTo(pt);
        }

        // Route Shadow / Glow
        QPen glowPen(QColor(9, 105, 218, 90), 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(glowPen);
        p.drawPath(routePath);

        // Core Route Line
        QPen roadPen(QColor("#388BFD"), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(roadPen);
        p.drawPath(routePath);
    }

    // 3. Draw All Registered Stop Markers
    for (const auto& m : m_markers) {
        QPointF pt = geoToPixel(m.lat, m.lon);

        if (m.isSelectedSource) {
            // Selected Source: Neon Green Pulsing Rings
            p.setBrush(QColor(63, 185, 80, 50));
            p.setPen(QPen(QColor("#3FB950"), 2));
            p.drawEllipse(pt, 24, 24);

            p.setBrush(QColor("#238636"));
            p.setPen(QPen(Qt::white, 2.5));
            p.drawEllipse(pt, 15, 15);

            p.setPen(Qt::white);
            QFont f = p.font();
            f.setBold(true);
            f.setPixelSize(12);
            p.setFont(f);
            p.drawText(QRectF(pt.x() - 15, pt.y() - 15, 30, 30), Qt::AlignCenter, "A");

            // Selected Source Tag
            QRectF badgeRect(pt.x() - 65, pt.y() - 44, 130, 24);
            p.setBrush(QColor(22, 27, 34, 240));
            p.setPen(QPen(QColor("#3FB950"), 1.5));
            p.drawRoundedRect(badgeRect, 4, 4);

            p.setPen(QColor("#3FB950"));
            QFont bf = p.font();
            bf.setPixelSize(11);
            bf.setBold(true);
            p.setFont(bf);
            p.drawText(badgeRect, Qt::AlignCenter, "SELECTED SOURCE");

            // Label Below
            QRectF labelRect(pt.x() + 18, pt.y() - 10, 145, 22);
            p.setBrush(QColor(22, 27, 34, 230));
            p.setPen(QPen(QColor("#30363D"), 1));
            p.drawRoundedRect(labelRect, 4, 4);
            p.setPen(QColor("#E6EDF3"));
            QFont lf = p.font();
            lf.setPixelSize(10);
            lf.setBold(true);
            p.setFont(lf);
            p.drawText(labelRect.adjusted(6, 0, -4, 0), Qt::AlignLeft | Qt::AlignVCenter, m.label);

        } else if (m.isDestination) {
            // Destination: Crimson Red Beacon
            p.setBrush(QColor(248, 81, 73, 50));
            p.setPen(QPen(QColor("#F85149"), 2));
            p.drawEllipse(pt, 24, 24);

            p.setBrush(QColor("#DA3633"));
            p.setPen(QPen(Qt::white, 2.5));
            p.drawEllipse(pt, 15, 15);

            p.setPen(Qt::white);
            QFont f = p.font();
            f.setBold(true);
            f.setPixelSize(9);
            p.setFont(f);
            p.drawText(QRectF(pt.x() - 15, pt.y() - 15, 30, 30), Qt::AlignCenter, "GEHU");

            // Fixed Destination Tag
            QRectF badgeRect(pt.x() - 75, pt.y() - 44, 150, 24);
            p.setBrush(QColor(22, 27, 34, 240));
            p.setPen(QPen(QColor("#F85149"), 1.5));
            p.drawRoundedRect(badgeRect, 4, 4);

            p.setPen(QColor("#F85149"));
            QFont bf = p.font();
            bf.setPixelSize(11);
            bf.setBold(true);
            p.setFont(bf);
            p.drawText(badgeRect, Qt::AlignCenter, "FIXED DESTINATION");

            // Label Below
            QRectF labelRect(pt.x() + 18, pt.y() - 10, 160, 22);
            p.setBrush(QColor(22, 27, 34, 230));
            p.setPen(QPen(QColor("#30363D"), 1));
            p.drawRoundedRect(labelRect, 4, 4);
            p.setPen(QColor("#E6EDF3"));
            QFont lf = p.font();
            lf.setPixelSize(10);
            lf.setBold(true);
            p.setFont(lf);
            p.drawText(labelRect.adjusted(6, 0, -4, 0), Qt::AlignLeft | Qt::AlignVCenter, m.label);

        } else {
            // Intermediate Regular Stop
            p.setBrush(QColor(0, 0, 0, 70));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QPointF(pt.x() + 2, pt.y() + 2), 13, 13);

            p.setBrush(QColor("#1F6FEB"));
            p.setPen(QPen(Qt::white, 2));
            p.drawEllipse(pt, 12, 12);

            p.setPen(Qt::white);
            QFont f = p.font();
            f.setBold(true);
            f.setPixelSize(10);
            p.setFont(f);
            p.drawText(QRectF(pt.x() - 12, pt.y() - 12, 24, 24), Qt::AlignCenter, "S");

            QRectF labelRect(pt.x() + 16, pt.y() - 10, 125, 20);
            p.setBrush(QColor(22, 27, 34, 220));
            p.setPen(QPen(QColor("#30363D"), 1));
            p.drawRoundedRect(labelRect, 4, 4);

            p.setPen(QColor("#E6EDF3"));
            QFont lf = p.font();
            lf.setPixelSize(10);
            lf.setBold(true);
            p.setFont(lf);
            p.drawText(labelRect.adjusted(5, 0, -4, 0), Qt::AlignLeft | Qt::AlignVCenter, m.label);
        }
    }

    // 4. Draw Animated Simulated Bus
    if (!m_routeCoordinates.isEmpty() && m_busPos.x() != 0.0) {
        QPointF bpt = geoToPixel(m_busPos.x(), m_busPos.y());

        p.setBrush(QColor(210, 153, 34, 70));
        p.setPen(Qt::NoPen);
        p.drawEllipse(bpt, 22, 22);

        p.setBrush(QColor("#D29922"));
        p.setPen(QPen(Qt::white, 2.5));
        p.drawEllipse(bpt, 15, 15);

        p.setPen(Qt::white);
        QFont bf = p.font();
        bf.setBold(true);
        bf.setPixelSize(9);
        p.setFont(bf);
        p.drawText(QRectF(bpt.x() - 15, bpt.y() - 15, 30, 30), Qt::AlignCenter, "BUS");
    }

    // 5. On-Screen Zoom Controls
    QRect zoomInRect(width() - 44, 16, 28, 28);
    p.setBrush(QColor(22, 27, 34, 235));
    p.setPen(QPen(QColor("#30363D"), 1.5));
    p.drawRoundedRect(zoomInRect, 5, 5);
    p.setPen(Qt::white);
    QFont zf = p.font();
    zf.setBold(true);
    zf.setPixelSize(16);
    p.setFont(zf);
    p.drawText(zoomInRect, Qt::AlignCenter, "+");

    QRect zoomOutRect(width() - 44, 48, 28, 28);
    p.setBrush(QColor(22, 27, 34, 235));
    p.setPen(QPen(QColor("#30363D"), 1.5));
    p.drawRoundedRect(zoomOutRect, 5, 5);
    p.setPen(Qt::white);
    p.drawText(zoomOutRect, Qt::AlignCenter, "-");

    // Attribution
    QRect attrRect(width() - 310, height() - 22, 305, 18);
    p.setBrush(QColor(22, 27, 34, 210));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(attrRect, 3, 3);
    p.setPen(QColor("#8B949E"));
    QFont af = p.font();
    af.setPixelSize(9);
    p.setFont(af);
    p.drawText(attrRect.adjusted(6, 0, -4, 0), Qt::AlignVCenter, "© OpenStreetMap contributors | OSRM Driving Road Network");
}

// Function Overriding
// Polymorphism
void NativeOsmMapWidget::mousePressEvent(QMouseEvent* event) {
    QRect zoomInRect(width() - 44, 16, 28, 28);
    QRect zoomOutRect(width() - 44, 48, 28, 28);

    if (zoomInRect.contains(event->pos())) {
        zoomIn();
        return;
    }
    if (zoomOutRect.contains(event->pos())) {
        zoomOut();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        m_pressPos = event->pos();
        m_lastMousePos = event->pos();
    }
}

// Function Overriding
// Polymorphism
void NativeOsmMapWidget::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        if (!m_isDragging && (event->pos() - m_pressPos).manhattanLength() > 6) {
            m_isDragging = true;
            setCursor(Qt::ClosedHandCursor);
        }

        if (m_isDragging) {
            QPoint delta = event->pos() - m_lastMousePos;
            m_lastMousePos = event->pos();

            double centerTileX = lonToTileX(m_centerLon, m_zoom) - delta.x() / 256.0;
            double centerTileY = latToTileY(m_centerLat, m_zoom) - delta.y() / 256.0;

            m_centerLon = tileXToLon(centerTileX, m_zoom);
            m_centerLat = tileYToLat(centerTileY, m_zoom);

            update();
        }
    }
}

// Function Overriding
// Polymorphism
void NativeOsmMapWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_isDragging) {
            m_isDragging = false;
            setCursor(Qt::ArrowCursor);
            return;
        }

        // Direct Map Marker Hit-Testing (Method B)
        bool markerHit = false;
        for (const auto& marker : m_markers) {
            QPointF screenPos = geoToPixel(marker.lat, marker.lon);
            double dist = std::hypot(screenPos.x() - event->pos().x(), screenPos.y() - event->pos().y());
            if (dist <= 26.0) {
                markerHit = true;
                if (marker.isDestination) {
                    emit destinationClicked();
                } else {
                    emit sourceStopClicked(marker.stopId, marker.label, marker.lat, marker.lon);
                }
                break;
            }
        }

        if (!markerHit) {
            QPointF geo = pixelToGeo(event->position());
            emit invalidAreaClicked(geo.x(), geo.y());
        }
    }
}

// Function Overriding
// Polymorphism
void NativeOsmMapWidget::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else if (event->angleDelta().y() < 0) {
        zoomOut();
    }
}

// Function Overriding
// Polymorphism
void NativeOsmMapWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    update();
}
