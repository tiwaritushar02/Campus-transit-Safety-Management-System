#ifndef NATIVE_OSM_MAP_WIDGET_HPP
#define NATIVE_OSM_MAP_WIDGET_HPP

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>
#include <QPixmap>
#include <QPointF>
#include <QTimer>
#include <QVector>

struct MapMarker {
    QString stopId;
    double lat;
    double lon;
    QString label;
    QString subtext;
    QColor color;
    QString iconText;
    bool isSelectedSource = false;
    bool isDestination = false;
};

// Inheritance
class NativeOsmMapWidget : public QWidget {
    Q_OBJECT

private:
    // Encapsulation
    double m_centerLat;
    double m_centerLon;
    int m_zoom;
    QPoint m_lastMousePos;
    QPoint m_pressPos;
    bool m_isDragging;

    QNetworkAccessManager* m_netManager;
    QHash<QString, QPixmap> m_memTileCache;
    QString m_cacheDir;

    QVector<QPointF> m_routeCoordinates;
    QVector<MapMarker> m_markers;

    QTimer* m_simTimer;
    int m_simStep;
    bool m_isSimulating;
    QPointF m_busPos;

    double lonToTileX(double lon, int z) const;
    double latToTileY(double lat, int z) const;
    double tileXToLon(double x, int z) const;
    double tileYToLat(double y, int z) const;
    QPointF geoToPixel(double lat, double lon) const;
    QPointF pixelToGeo(const QPointF& pixel) const;

    void requestTile(int z, int x, int y);

signals:
    void routeCalculated(double distanceKm, int durationMins, const QString& status);
    void busProgress(int percentage, const QString& locationText);
    void routeError(const QString& errorMsg);
    void sourceStopClicked(const QString& stopId, const QString& stopName, double lat, double lon);
    void destinationClicked();
    void invalidAreaClicked(double lat, double lon);

public:
    // Constructor
    explicit NativeOsmMapWidget(QWidget* parent = nullptr);

    // Destructor
    ~NativeOsmMapWidget() override;

    void setMapCenter(double lat, double lon, int zoom = 12);
    void clearRoute();
    void setRouteCoordinates(const QVector<QPointF>& coords);

    // Function Overloading
    void addMarker(const QString& stopId, double lat, double lon, const QString& label, const QString& subtext, const QColor& color, const QString& iconText, bool isSelectedSource = false, bool isDestination = false);

    // Function Overloading
    void addMarker(double lat, double lon, const QString& label, const QString& subtext, const QColor& color, const QString& iconText);

    void setSelectedSourceStopId(const QString& stopId);
    void clearMarkers();

    void fetchOsrmRoute(double startLat, double startLon, double destLat, double destLon);
    void startBusSimulation();
    void pauseBusSimulation();
    void resetBusSimulation();
    bool isSimulating() const { return m_isSimulating; }
    void fitRouteBounds();
    void zoomIn();
    void zoomOut();

protected:
    // Function Overriding
    // Polymorphism
    void paintEvent(QPaintEvent* event) override;

    // Function Overriding
    // Polymorphism
    void mousePressEvent(QMouseEvent* event) override;

    // Function Overriding
    // Polymorphism
    void mouseMoveEvent(QMouseEvent* event) override;

    // Function Overriding
    // Polymorphism
    void mouseReleaseEvent(QMouseEvent* event) override;

    // Function Overriding
    // Polymorphism
    void wheelEvent(QWheelEvent* event) override;

    // Function Overriding
    // Polymorphism
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onTileReply(QNetworkReply* reply);
    void onSimStep();
};

#endif // NATIVE_OSM_MAP_WIDGET_HPP
