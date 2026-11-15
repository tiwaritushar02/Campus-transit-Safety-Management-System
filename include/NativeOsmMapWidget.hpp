#ifndef NATIVE_OSM_MAP_WIDGET_HPP
#define NATIVE_OSM_MAP_WIDGET_HPP

/**
 * ============================================================================
 * CLASS: NativeOsmMapWidget
 * OOP CONCEPTS:
 * - GUI Component Encapsulation: Native OpenStreetMap Slippy-tile engine embedded
 *   directly in Qt Widgets using QPainter and QNetworkAccessManager.
 * - Zero Browser Dependency: No Chrome/Firefox, no window.open, no QDesktopServices.
 * - Local Tile Cache: Solves OSM 403 Forbidden via local disk caching and standard User-Agent.
 * - Demo Mode Watermark: Explicitly indicates simulated vehicle motion.
 * ============================================================================
 */

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>
#include <QPixmap>
#include <QPointF>
#include <QTimer>
#include <QVector>

struct MapMarker {
    double lat;
    double lon;
    QString label;
    QString subtext;
    QColor color;
    QString iconText;
};

class NativeOsmMapWidget : public QWidget {
    Q_OBJECT

private:
    double m_centerLat;
    double m_centerLon;
    int m_zoom; // 10 to 16
    QPoint m_lastMousePos;
    bool m_isDragging;

    QNetworkAccessManager* m_netManager;
    QHash<QString, QPixmap> m_memTileCache;
    QString m_cacheDir;

    QVector<QPointF> m_routeCoordinates; // [lat, lon]
    QVector<MapMarker> m_markers;

    // Bus Position (Simulated)
    QPointF m_busPos;
    bool m_showBus;

    // Projections
    double lonToTileX(double lon, int z) const;
    double latToTileY(double lat, int z) const;
    double tileXToLon(double x, int z) const;
    double tileYToLat(double y, int z) const;
    QPointF geoToPixel(double lat, double lon) const;
    QPointF pixelToGeo(const QPointF& pixel) const;

    void requestTile(int z, int x, int y);

signals:
    void routeCalculated(double distanceKm, int durationMins, const QString& status);
    void routeError(const QString& errorMsg);

public:
    explicit NativeOsmMapWidget(QWidget* parent = nullptr);
    ~NativeOsmMapWidget() override;

    void setMapCenter(double lat, double lon, int zoom = 12);
    void clearRoute();
    void setRouteCoordinates(const QVector<QPointF>& coords);
    void addMarker(double lat, double lon, const QString& label, const QString& subtext, const QColor& color, const QString& iconText);
    void clearMarkers();

    void setBusPosition(double lat, double lon);
    void setBusVisible(bool visible);

    void fetchOsrmRoute(double startLat, double startLon, double destLat, double destLon);
    void fitRouteBounds();
    void zoomIn();
    void zoomOut();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onTileReply(QNetworkReply* reply);
};

#endif // NATIVE_OSM_MAP_WIDGET_HPP
