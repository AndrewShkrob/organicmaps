#include <QTextEdit>

#include <QApplication>
#include <QComboBox>
#include <QEventLoop>
#include <QFormLayout>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSpinBox>
#include <QWidget>

#include "om/tiles/configs.hpp"
#include "om/tiles/tile_request_builder.hpp"
#include "om/tiles/tile_server.hpp"

QImage loadImageFromUrl(const QUrl & imageUrl)
{
  QNetworkAccessManager manager;
  QNetworkRequest request(imageUrl);
  request.setRawHeader("User-Agent", "MyApp/1.0 (myemail@example.com)");
  QEventLoop loop;

  // Initiate the network request
  QNetworkReply * reply = manager.get(request);

  // Connect the reply's finished signal to the event loop's quit slot
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

  // Wait until the network request is finished
  loop.exec();

  // Check for errors
  if (reply->error() != QNetworkReply::NoError)
  {
    qDebug() << "Error loading image from URL:" << reply->errorString();
    delete reply;
    return QImage();
  }

  // Read the data and load it into a QImage
  QByteArray imageData = reply->readAll();
  QImage image;
  if (!image.loadFromData(imageData))
  {
    qDebug() << "Failed to load image from data";
  }

  delete reply;
  return image;
}

class TileConfigWidget : public QFormLayout
{
public:
  explicit TileConfigWidget(QWidget * parent = nullptr) : QFormLayout(parent)
  {
    AddTileServerConfigRow();
    AddZoomSpinBox();
    AddXSpinBox();
    AddYSpinBox();
    AddTileRequestRow();
    AddTileRow();
    OnTileServerConfigChanged(0);
  }

signals:
  void TileServerChanged(om::tiles::ServerConfig const &) {};
  void TileRequest(om::tiles::TileKey const &) {};

private slots:
  void OnTileServerConfigChanged(int)
  {
    auto const & config = GetCurrentConfig();
    m_zoomSpinBox->setRange(config.m_minZoomLevel, config.m_maxZoomLevel);
    m_requestBuilder = std::make_unique<om::tiles::TileRequestBuilder>(config.m_url);
    UpdateTileRequest();
    emit TileServerChanged(config);
  }

  void OnZoomChanged(int)
  {
    uint16_t const maxTileNumber = GetMaxTileNumberForZoomLevel(m_zoomSpinBox->value());
    m_xSpinBox->setRange(0, maxTileNumber);
    m_ySpinBox->setRange(0, maxTileNumber);
    UpdateTileRequest();
    emit TileRequest(GetTileKey());
  }

  void OnCoordChanged(int)
  {
    UpdateTileRequest();
    emit TileRequest(GetTileKey());
  }

private:
  void UpdateTileRequest()
  {
    m_tileRequest->setText(QString::fromStdString(m_requestBuilder->BuildRequestUrl(GetTileKey())));

    QPixmap pixmap = QPixmap::fromImage(loadImageFromUrl(QUrl(m_tileRequest->toPlainText())));

    m_tile->setPixmap(pixmap);
    m_tile->resize(pixmap.size());
  }

  om::tiles::ServerConfig GetCurrentConfig()
  {
    return m_tileServerConfig->currentData().value<om::tiles::ServerConfig>();
  }

  om::tiles::TileKey GetTileKey()
  {
    return {
        .x = static_cast<uint32_t>(m_xSpinBox->value()),
        .y = static_cast<uint32_t>(m_ySpinBox->value()),
        .zoomLevel = static_cast<uint8_t>(m_zoomSpinBox->value()),
    };
  }

  static uint16_t GetMaxTileNumberForZoomLevel(uint8_t zoomLevel) { return (1 << zoomLevel) - 1; }

  void AddTileServerConfigRow()
  {
    m_tileServerConfig = new QComboBox();
    for (auto const & [name, config] : om::tiles::config::GetTileServers())
    {
      m_tileServerConfig->addItem(QString::fromStdString(name), QVariant::fromValue(config));
    }
    connect(m_tileServerConfig, &QComboBox::currentIndexChanged, this, &TileConfigWidget::OnTileServerConfigChanged);
    addRow("Tile server:", m_tileServerConfig);
  }

  void AddZoomSpinBox()
  {
    auto const & config = GetCurrentConfig();

    m_zoomSpinBox = new QSpinBox();
    m_zoomSpinBox->setRange(config.m_minZoomLevel, config.m_maxZoomLevel);
    connect(m_zoomSpinBox, &QSpinBox::valueChanged, this, &TileConfigWidget::OnZoomChanged);
    addRow("Zoom:", m_zoomSpinBox);
  }

  void AddXSpinBox()
  {
    uint16_t const maxTileNumber = GetMaxTileNumberForZoomLevel(m_zoomSpinBox->value());

    m_xSpinBox = new QSpinBox();
    m_xSpinBox->setRange(0, maxTileNumber);
    connect(m_xSpinBox, &QSpinBox::valueChanged, this, &TileConfigWidget::OnCoordChanged);
    addRow("X:", m_xSpinBox);
  }

  void AddYSpinBox()
  {
    uint16_t const maxTileNumber = GetMaxTileNumberForZoomLevel(m_zoomSpinBox->value());

    m_ySpinBox = new QSpinBox();
    m_ySpinBox->setRange(0, maxTileNumber);
    connect(m_ySpinBox, &QSpinBox::valueChanged, this, &TileConfigWidget::OnCoordChanged);
    addRow("Y:", m_ySpinBox);
  }

  void AddTileRequestRow()
  {
    m_tileRequest = new QTextEdit();
    m_tileRequest->setReadOnly(true);
    addRow("Tile request:", m_tileRequest);
  }

  void AddTileRow()
  {
    m_tile = new QLabel();
    m_tile->setScaledContents(true);
    addRow("Tile:", m_tile);
  }

  QComboBox * m_tileServerConfig = nullptr;
  QSpinBox * m_xSpinBox = nullptr;
  QSpinBox * m_ySpinBox = nullptr;
  QSpinBox * m_zoomSpinBox = nullptr;
  QTextEdit * m_tileRequest = nullptr;
  QLabel * m_tile = nullptr;
  std::unique_ptr<om::tiles::TileRequestBuilder> m_requestBuilder = nullptr;
};

int main(int argc, char * argv[])
{
  om::tiles::TileServer server(om::tiles::config::kOSM);

  QApplication app(argc, argv);
  QWidget window;
  window.resize(320, 240);
  window.show();
  window.setWindowTitle("Tile Viewer");

  [[maybe_unused]] auto * tileConfig = new TileConfigWidget(&window);

  return QApplication::exec();
}
