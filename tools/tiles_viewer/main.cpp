#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMutex>
#include <QNetworkReply>
#include <QSpinBox>
#include <QThread>
#include <QWidget>

#include <atomic>
#include <memory>

#include "base/logging.hpp"
#include "tiles/configs.hpp"
#include "tiles/tile_request_builder.hpp"
#include "tiles/tile_server.hpp"

static constexpr int kGridSize = 3;

static QPixmap MakeSolidPixmap(QSize size, QColor color)
{
  if (!size.isValid() || size.isEmpty())
    size = QSize(256, 256);
  QPixmap px(size);
  px.fill(color);
  return px;
}

class TileConfigWidget : public QFormLayout
{
  Q_OBJECT

public:
  explicit TileConfigWidget(QWidget * parent = nullptr) : QFormLayout(parent)
  {
    setFieldGrowthPolicy(ExpandingFieldsGrow);
    AddTileServerConfigRow();
    AddZoomSpinBox();
    AddXSpinBox();
    AddYSpinBox();
    AddTileGridRow();
    OnTileServerConfigChanged(0);
  }

signals:
  void TileServerChanged(om::tiles::ServerConfig const &);

private slots:
  void OnTileServerConfigChanged(int)
  {
    auto const & config = GetCurrentConfig();
    m_zoomSpinBox->setRange(config.minZoom, config.maxZoom);
    m_lastZoom = m_zoomSpinBox->value();
    m_requestBuilder = std::make_unique<om::tiles::TileRequestBuilder>(config.url);
    m_tileServer =
        std::make_unique<om::tiles::TileServer>(config, [this](auto const & tile) { OnTileDownloaded(tile); });
    UpdateCoordRanges();
    StartDownloads();
    emit TileServerChanged(config);
  }

  void OnZoomChanged(int newZoom)
  {
    // Scale current coords to the new zoom level so the viewed area stays roughly the same.
    // Old tile covers 1/2^oldZoom of the world; map to equivalent tile at newZoom.
    int const oldZoom = m_lastZoom;
    m_lastZoom = newZoom;

    if (oldZoom != newZoom)
    {
      auto scaleCoord = [](int coord, int fromZoom, int toZoom) -> int
      {
        if (toZoom > fromZoom)
          return coord << (toZoom - fromZoom);
        else
          return coord >> (fromZoom - toZoom);
      };

      // Block signals so changing the value doesn't trigger another OnZoomChanged / StartDownloads.
      QSignalBlocker bx(m_xSpinBox);
      QSignalBlocker by(m_ySpinBox);

      int const newX = scaleCoord(m_xSpinBox->value(), oldZoom, newZoom);
      int const newY = scaleCoord(m_ySpinBox->value(), oldZoom, newZoom);

      // Update ranges first, then clamp the scaled value into the new range.
      UpdateCoordRanges();
      m_xSpinBox->setValue(std::clamp(newX, m_xSpinBox->minimum(), m_xSpinBox->maximum()));
      m_ySpinBox->setValue(std::clamp(newY, m_ySpinBox->minimum(), m_ySpinBox->maximum()));
    }
    else
    {
      UpdateCoordRanges();
    }

    StartDownloads();
  }

  void OnCoordChanged(int) { StartDownloads(); }

  void OnTileDownloaded(om::tiles::Tile const & tile)
  {
    LOG(LINFO, ("Tile downloaded:", tile.key.x, tile.key.y, static_cast<int>(tile.key.zoomLevel),
                "data size:", tile.data.size()));
    if (tile.key.zoomLevel != m_zoomSpinBox->value())
    {
      LOG(LDEBUG, ("Tile zoom level doesn't match current zoom level, ignoring."));
      return;
    }

    int32_t const row = tile.key.y - m_ySpinBox->value();
    int32_t const col = tile.key.x - m_xSpinBox->value();
    if (row < 0 || col < 0 || row >= kGridSize || col >= kGridSize)
    {
      LOG(LDEBUG, ("Tile coords are out of current view bounds, ignoring."));
      return;
    }

    QLabel * label = m_tileLabels[row][col];
    if (tile.data.empty())
    {
      label->setPixmap(MakeSolidPixmap(m_tileSize, Qt::red));
    }
    else
    {
      int const resolution = GetCurrentConfig().resolution;
      QImage image(tile.data.data(), resolution, resolution, resolution * 4, QImage::Format_RGBA8888);
      QPixmap pixmap = QPixmap::fromImage(image);
      m_tileSize = pixmap.size();
      label->setPixmap(pixmap);
      label->setFixedSize(pixmap.size());
    }
  }

private:
  void StartDownloads()
  {
    uint8_t const zoom = static_cast<uint8_t>(m_zoomSpinBox->value());
    uint32_t const originX = static_cast<uint32_t>(m_xSpinBox->value());
    uint32_t const originY = static_cast<uint32_t>(m_ySpinBox->value());
    uint16_t const maxTileNumber = GetMaxTileNumberForZoomLevel(zoom);

    int const validCols = ClampedGridSize(originX, maxTileNumber);
    int const validRows = ClampedGridSize(originY, maxTileNumber);

    for (int row = 0; row < kGridSize; ++row)
    {
      for (int col = 0; col < kGridSize; ++col)
      {
        if (row < validRows && col < validCols)
        {
          m_tileServer->RequestTile({
              .x = originX + static_cast<uint32_t>(col),
              .y = originY + static_cast<uint32_t>(row),
              .zoomLevel = zoom,
          });
        }
        else
        {
          m_tileLabels[row][col]->setPixmap(MakeSolidPixmap(m_tileSize, Qt::black));
        }
      }
    }
  }

  // Updates x/y spinbox ranges so origin + kGridSize - 1 never exceeds maxTileNumber.
  void UpdateCoordRanges() const
  {
    uint32_t const maxTileNumber = GetMaxTileNumberForZoomLevel(m_zoomSpinBox->value());
    int const maxOrigin = std::max(0, static_cast<int>(maxTileNumber) - kGridSize + 1);
    m_xSpinBox->setRange(0, maxOrigin);
    m_ySpinBox->setRange(0, maxOrigin);
  }

  // Returns how many tiles fit from origin to the tile boundary along one axis.
  int ClampedGridSize(uint32_t const originCoord, uint32_t const maxTileNumber) const
  {
    int const available = static_cast<int>(maxTileNumber) - static_cast<int>(originCoord) + 1;
    return std::min(kGridSize, available);
  }

  om::tiles::ServerConfig GetCurrentConfig() const
  {
    return m_tileServerConfig->currentData().value<om::tiles::ServerConfig>();
  }

  static uint32_t GetMaxTileNumberForZoomLevel(uint8_t const zoomLevel) { return (1 << zoomLevel) - 1; }

  void AddTileServerConfigRow()
  {
    m_tileServerConfig = new QComboBox();
    for (auto const & [id, config] : om::tiles::config::GetTileServers())
      m_tileServerConfig->addItem(QString::fromStdString(config.name), QVariant::fromValue(config));
    connect(m_tileServerConfig, &QComboBox::currentIndexChanged, this, &TileConfigWidget::OnTileServerConfigChanged);
    addRow("Tile server:", m_tileServerConfig);
  }

  void AddZoomSpinBox()
  {
    auto const & config = GetCurrentConfig();
    m_zoomSpinBox = new QSpinBox();
    m_zoomSpinBox->setRange(config.minZoom, config.maxZoom);
    connect(m_zoomSpinBox, &QSpinBox::valueChanged, this, &TileConfigWidget::OnZoomChanged);
    addRow("Zoom:", m_zoomSpinBox);
  }

  void AddXSpinBox()
  {
    uint32_t const maxTileNumber = GetMaxTileNumberForZoomLevel(m_zoomSpinBox->value());
    m_xSpinBox = new QSpinBox();
    m_xSpinBox->setRange(0, maxTileNumber);
    connect(m_xSpinBox, &QSpinBox::valueChanged, this, &TileConfigWidget::OnCoordChanged);
    addRow("X (top-left):", m_xSpinBox);
  }

  void AddYSpinBox()
  {
    uint32_t const maxTileNumber = GetMaxTileNumberForZoomLevel(m_zoomSpinBox->value());
    m_ySpinBox = new QSpinBox();
    m_ySpinBox->setRange(0, maxTileNumber);
    connect(m_ySpinBox, &QSpinBox::valueChanged, this, &TileConfigWidget::OnCoordChanged);
    addRow("Y (top-left):", m_ySpinBox);
  }

  void AddTileGridRow()
  {
    auto * gridContainer = new QWidget();
    m_tileGridLayout = new QGridLayout(gridContainer);
    m_tileGridLayout->setContentsMargins(0, 0, 0, 0);
    m_tileGridLayout->setSpacing(2);

    for (int row = 0; row < kGridSize; ++row)
    {
      for (int col = 0; col < kGridSize; ++col)
      {
        auto * label = new QLabel();
        label->setAlignment(Qt::AlignCenter);
        label->setFixedSize(m_tileSize);
        label->setPixmap(MakeSolidPixmap(m_tileSize, Qt::black));
        m_tileLabels[row][col] = label;
        m_tileGridLayout->addWidget(label, row, col);
      }
    }

    addRow("Tiles:", gridContainer);
  }

  QComboBox * m_tileServerConfig = nullptr;
  QSpinBox * m_xSpinBox = nullptr;
  QSpinBox * m_ySpinBox = nullptr;
  QSpinBox * m_zoomSpinBox = nullptr;
  QGridLayout * m_tileGridLayout = nullptr;
  QLabel * m_tileLabels[kGridSize][kGridSize] = {};
  QSize m_tileSize{256, 256};
  int m_lastZoom{0};
  std::unique_ptr<om::tiles::TileRequestBuilder> m_requestBuilder;

  std::unique_ptr<om::tiles::TileServer> m_tileServer;
};

#include "main.moc"

int main(int argc, char * argv[])
{
  QApplication app(argc, argv);
  QWidget window;
  window.resize(900, 700);
  window.show();
  window.setWindowTitle("Tile Viewer");

  [[maybe_unused]] auto * tileConfig = new TileConfigWidget(&window);

  return QApplication::exec();
}
