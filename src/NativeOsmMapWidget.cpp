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

NativeOsmMapWidget::NativeOsmMapWidget(QWidget* parent)
    : QWidget(parent),
      m_centerLat(29.2850),
      m_centerLon(79.5380),
      m_zoom(12),
      m_isDragging(false),
      m_showBus(true) {

    setMinimumSize(400, 350);
    setMouseTracking(true);

    m_cacheDir = "cache/tiles";
    QDir().mkpath(m_cacheDir);

    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, &QNetworkAccessManager::finished, this, &NativeOsmMapWidget::onTileReply);
}

NativeOsmMapWidget::~NativeOsmMapWidget() {}

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
    update();
}

void NativeOsmMapWidget::setRouteCoordinates(const QVector<QPointF>& coords) {
    m_routeCoordinates = coords;
    if (!m_routeCoordinates.isEmpty()) {
        m_busPos = m_routeCoordinates.first();
    }
    update();
}

void NativeOsmMapWidget::addMarker(double lat, double lon, const QString& label, const QString& subtext, const QColor& color, const QString& iconText) {
    m_markers.append({lat, lon, label, subtext, color, iconText});
    update();
}

void NativeOsmMapWidget::clearMarkers() {
    m_markers.clear();
    update();
}

void NativeOsmMapWidget::setBusPosition(double lat, double lon) {
    m_busPos = QPointF(lat, lon);
    update();
}

void NativeOsmMapWidget::setBusVisible(bool visible) {
    m_showBus = visible;
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

    // Check disk cache first
    QString filePath = QString("%1/%2.png").arg(m_cacheDir).arg(tileKey);
    if (QFile::exists(filePath)) {
        QPixmap pm(filePath);
        if (!pm.isNull()) {
            m_memTileCache[tileKey] = pm;
            return;
        }
    }

    // Network request with required academic User-Agent
    QUrl url(QString("https://tile.openstreetmap.org/%1/%2/%3.png").arg(z).arg(x).arg(y));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, "SmartCollegeBusManagementSystem/1.0 (academic-pbl; contact: tiwaritushar565@gmail.com)");
    req.setRawHeader("Accept", "image/png,image/webp,image/*;q=0.8");

    QNetworkReply* reply = m_netManager->get(req);
    reply->setProperty("tileKey", tileKey);
}

void NativeOsmMapWidget::onTileReply(QNetworkReply* reply) {
    QString tileKey = reply->property("tileKey").toString();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap pm;
        if (pm.loadFromData(data)) {
            m_memTileCache[tileKey] = pm;
            QString filePath = QString("%1/%2.png").arg(m_cacheDir).arg(tileKey);
            QFile f(filePath);
            if (f.open(QIODevice::WriteOnly)) {
                f.write(data);
                f.close();
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
                    emit routeCalculated(distKm, durMins, "OPTIMAL PATH GENERATED");
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

void NativeOsmMapWidget::paintEvent(QPaintEvent* event) {
    (void)event;
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Natural OSM canvas background
    p.fillRect(rect(), QColor("#E5E3DF"));

    // 1. Draw OpenStreetMap Slippy Tiles
    double centerTileX = lonToTileX(m_centerLon, m_zoom);
    double centerTileY = latToTileY(m_centerLat, m_zoom);

    int startTileX = static_cast<int>(std::floor(centerTileX - (width() / 2.0) / 256.0));
    int endTileX = static_cast<int>(std::ceil(centerTileX + (width() / 2.0) / 256.0));
    int startTileY = static_cast<int>(std::floor(centerTileY - (height() / 2.0) / 256.0));
    int endTileY = static_cast<int>(std::ceil(centerTileY + (height() / 2.0) / 256.0));

    for (int x = startTileX; x <= endTileX; ++x) {
        for (int y = startTileY; y <= endTileY; ++y) {
            double px = width() / 2.0 + (x - centerTileX) * 256.0;
            double py = height() / 2.0 + (y - centerTileY) * 256.0;

            QString tileKey = QString("%1_%2_%3").arg(m_zoom).arg(x).arg(y);
            if (m_memTileCache.contains(tileKey)) {
                p.drawPixmap(static_cast<int>(px), static_cast<int>(py), m_memTileCache[tileKey]);
            } else {
                requestTile(m_zoom, x, y);
                p.setPen(QPen(QColor("#D0CECA"), 1));
                p.drawRect(static_cast<int>(px), static_cast<int>(py), 256, 256);
            }
        }
    }

    // 2. Draw Real Road Route Polyline (Green road corridor)
    if (m_routeCoordinates.size() > 1) {
        QPainterPath routePath;
        QPointF firstPt = geoToPixel(m_routeCoordinates[0].x(), m_routeCoordinates[0].y());
        routePath.moveTo(firstPt);

        for (int i = 1; i < m_routeCoordinates.size(); ++i) {
            QPointF pt = geoToPixel(m_routeCoordinates[i].x(), m_routeCoordinates[i].y());
            routePath.lineTo(pt);
        }

        // Route Shadow
        QPen shadowPen(QColor(0, 0, 0, 90), 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(shadowPen);
        p.drawPath(routePath);

        // Vibrant Green Route Line
        QPen roadPen(QColor("#238636"), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(roadPen);
        p.drawPath(routePath);
    }

    // 3. Draw Checkpoint & Destination Markers
    for (const auto& m : m_markers) {
        QPointF pt = geoToPixel(m.lat, m.lon);

        // Marker Shadow
        p.setBrush(QColor(0, 0, 0, 80));
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(pt.x() + 2, pt.y() + 2), 14, 14);

        // Marker Pin Body
        p.setBrush(m.color);
        p.setPen(QPen(Qt::white, 2));
        p.drawEllipse(pt, 13, 13);

        // Icon text inside marker
        p.setPen(Qt::white);
        QFont f = p.font();
        f.setBold(true);
        f.setPixelSize(11);
        p.setFont(f);
        p.drawText(QRectF(pt.x() - 13, pt.y() - 13, 26, 26), Qt::AlignCenter, m.iconText);

        // Label Tag
        p.setBrush(QColor(22, 27, 34, 220));
        p.setPen(QPen(QColor("#30363D"), 1));
        QRectF labelRect(pt.x() + 16, pt.y() - 12, 145, 24);
        p.drawRoundedRect(labelRect, 4, 4);

        p.setPen(QColor("#E6EDF3"));
        QFont lf = p.font();
        lf.setPixelSize(10);
        lf.setBold(true);
        p.setFont(lf);
        p.drawText(labelRect.adjusted(6, 4, -4, -4), Qt::AlignLeft | Qt::AlignVCenter, m.label);
    }

    // 4. Draw Animated Bus Marker
    if (m_showBus && m_busPos.x() != 0.0) {
        QPointF bpt = geoToPixel(m_busPos.x(), m_busPos.y());

        // Bus Pulse Circle
        p.setBrush(QColor(46, 160, 67, 100));
        p.setPen(Qt::NoPen);
        p.drawEllipse(bpt, 22, 22);

        // Bus Body
        p.setBrush(QColor("#238636"));
        p.setPen(QPen(Qt::white, 2.5));
        p.drawEllipse(bpt, 16, 16);

        // Bus Icon
        p.setPen(Qt::white);
        QFont bf = p.font();
        bf.setPixelSize(14);
        p.setFont(bf);
        p.drawText(QRectF(bpt.x() - 16, bpt.y() - 16, 32, 32), Qt::AlignCenter, "🚌");
    }

    // 5. On-screen Controls (Zoom Buttons & Attribution)
    // Zoom In Button (+)
    QRect zoomInRect(width() - 44, 16, 28, 28);
    p.setBrush(QColor(22, 27, 34, 230));
    p.setPen(QPen(QColor("#30363D"), 1.5));
    p.drawRoundedRect(zoomInRect, 5, 5);
    p.setPen(Qt::white);
    QFont zf = p.font();
    zf.setBold(true);
    zf.setPixelSize(16);
    p.setFont(zf);
    p.drawText(zoomInRect, Qt::AlignCenter, "+");

    // Zoom Out Button (-)
    QRect zoomOutRect(width() - 44, 48, 28, 28);
    p.setBrush(QColor(22, 27, 34, 230));
    p.setPen(QPen(QColor("#30363D"), 1.5));
    p.drawRoundedRect(zoomOutRect, 5, 5);
    p.setPen(Qt::white);
    p.drawText(zoomOutRect, Qt::AlignCenter, "-");

    // Fit Route Button (⛶)
    QRect fitRect(width() - 44, 80, 28, 28);
    p.setBrush(QColor(22, 27, 34, 230));
    p.setPen(QPen(QColor("#30363D"), 1.5));
    p.drawRoundedRect(fitRect, 5, 5);
    p.setPen(QColor("#58A6FF"));
    p.drawText(fitRect, Qt::AlignCenter, "⛶");

    // DEMO MODE Transparency Banner (Watermark Pill)
    QRect demoPill(14, 14, 390, 28);
    p.setBrush(QColor(13, 17, 23, 225));
    p.setPen(QPen(QColor("#D29922"), 1.5));
    p.drawRoundedRect(demoPill, 6, 6);
    p.setPen(QColor("#D29922"));
    QFont df = p.font();
    df.setBold(true);
    df.setPixelSize(11);
    p.setFont(df);
    p.drawText(demoPill.adjusted(10, 0, -10, 0), Qt::AlignVCenter, "⚠️ DEMO MODE: SIMULATED BUS TELEMETRY (NO PHYSICAL GPS)");

    // Attribution
    QRect attrRect(width() - 290, height() - 22, 285, 18);
    p.setBrush(QColor(22, 27, 34, 210));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(attrRect, 3, 3);
    p.setPen(QColor("#8B949E"));
    QFont af = p.font();
    af.setPixelSize(9);
    p.setFont(af);
    p.drawText(attrRect.adjusted(6, 0, -4, 0), Qt::AlignVCenter, "© OpenStreetMap contributors | OSRM Live Engine");
}

void NativeOsmMapWidget::mousePressEvent(QMouseEvent* event) {
    QRect zoomInRect(width() - 44, 16, 28, 28);
    QRect zoomOutRect(width() - 44, 48, 28, 28);
    QRect fitRect(width() - 44, 80, 28, 28);

    if (zoomInRect.contains(event->pos())) {
        zoomIn();
        return;
    }
    if (zoomOutRect.contains(event->pos())) {
        zoomOut();
        return;
    }
    if (fitRect.contains(event->pos())) {
        fitRouteBounds();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void NativeOsmMapWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        double centerTileX = lonToTileX(m_centerLon, m_zoom);
        double centerTileY = latToTileY(m_centerLat, m_zoom);

        double newTileX = centerTileX - delta.x() / 256.0;
        double newTileY = centerTileY - delta.y() / 256.0;

        m_centerLon = tileXToLon(newTileX, m_zoom);
        m_centerLat = tileYToLat(newTileY, m_zoom);

        update();
    }
}

void NativeOsmMapWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
}

void NativeOsmMapWidget::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else if (event->angleDelta().y() < 0) {
        zoomOut();
    }
    event->accept();
}

void NativeOsmMapWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    update();
}
