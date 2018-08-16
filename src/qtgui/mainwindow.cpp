#include <QtWidgets>
#include "mainwindow.h"
#include <QAbstractTableModel>
#include <vector>
#include <string>

#include <rc_genicam_api/system.h>
#include <rc_genicam_api/interface.h>
#include <rc_genicam_api/device.h>
#include <rc_genicam_api/stream.h>
#include <rc_genicam_api/config.h>
#include <rc_genicam_api/exception.h>
#include <rc_genicam_api/pixel_formats.h>
#include <rc_genicam_api/image.h>

#include "imageviewer/image-viewer.h"
#include "tcontainer.h"

const QString system_keys[] =
  { "ID", "Ventdor", "Model", "Vendor version",
    "TL type", "Pathname", "GenTL version"};
const QString interf_keys[] =
  { "ID", "Display Name", "TL type:"};
const QString device_keys[] =
  { "ID", "Vendor", "Model", "TL type", "Display name", "Aceess status",
    "Serial number", "Version", "TS Frequency"};

struct imageRecvThreadParam {
  std::thread th;
  bool stop;
  bool pause;
  std::shared_ptr<rcg::Stream> stream;
  pal::ImageViewer *imageViewer;
};
struct imageRecvThreadParam imageParam;

void imageRecvThreadFunc(struct imageRecvThreadParam *pImageParam)
{
  while (!pImageParam->stop) {
    const rcg::Buffer *buffer = pImageParam->stream->grab(1000);
    if (buffer) {
      if (!buffer->getIsIncomplete() && buffer->getImagePresent()) {
        size_t width = buffer->getWidth();
        size_t height = buffer->getHeight();
        const unsigned char *p=static_cast<const unsigned char *>(buffer->getBase())+buffer->getImageOffset();
        size_t px = buffer->getXPadding();
        uint64_t format = buffer->getPixelFormat();
        static QImage image;
        size_t pstep;

        switch (format) {
        case Mono8:
          if (image.isNull() || image.width() != width || image.height() != height
              || image.format() != QImage::Format_Grayscale8)
            image = QImage(width, height, QImage::Format_Grayscale8);
          for (size_t k=0; k<height; k++) {
              memcpy(image.scanLine(k), p, width);
              p += width + px;
          }
          break;
        case YCbCr411_8: // convert and store as color image
          pstep = (width>>2) * 6 + px;
          if (image.isNull() || image.width() != width || image.height() != height
              || image.format() != QImage::Format_RGB888)
            image = QImage(width, height, QImage::Format_RGB888);
          for (size_t k=0; k<height; k++) {
            uchar *data = image.scanLine(k);
              for (size_t i=0; i<width; i+=4) {
                  uint8_t rgb[12];
                  rcg::convYCbCr411toQuadRGB(rgb, p, static_cast<int>(i));
                  for (int j=0; j<12; j++)
                    *data++ = static_cast<uchar>(rgb[j]);
              }
              p += pstep;
          }
          break;
        default:
          break;
        }
        pImageParam->imageViewer->setImage(image);
      } else if (buffer->getIsIncomplete()) {
          std::cerr << "storeBuffer(): Received incomplete buffer" << std::endl;
      } else if (!buffer->getImagePresent()) {
          std::cerr << "storeBuffer(): Received buffer without image" << std::endl;
      }
    }
  }
}

RectLabel::RectLabel()
{
  QTimer *timer = new QTimer(this);
  QObject::connect(timer, &QTimer::timeout, [=] {
     char tip[256];
     QPoint pt = parentWidget()->mapToParent(pos());
     QGraphicsView *imageView = (QGraphicsView*)parentWidget()->parentWidget();
     QRect rc(pt.x(), pt.y(), size().width(), size().height());
     QPolygonF polyf = imageView->mapToScene(rc);
     QRectF rcf = polyf.boundingRect();
     sprintf(tip, "(%d, %d, %d, %d)", (int)rcf.x(), (int)rcf.y(),
             (int)rcf.width(), (int)rcf.height());
     setText(tip);
  });
  timer->start(500);
}

MainWindow::MainWindow()
{
  imageViewer = new pal::ImageViewer("", this);
  setCentralWidget(imageViewer);

  QLabel *lab1 = new RectLabel;
  lab1->setStyleSheet("border: 1px solid red");
  lab1->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  TContainer *con1 = new TContainer(imageViewer->getView(), QPoint(10, 10), lab1);

  createDeviceWindow();
  createActions();
}

void MainWindow::addFeatureNode(QTreeWidgetItem *parent, GenApi::INode *node)
{
  if (node != 0) {
    switch (node->GetPrincipalInterfaceType()) {
      case GenApi::intfIValue:
        break;

      case GenApi::intfIBase:
        break;

      case GenApi::intfIInteger:
        {
          GenApi::IInteger *p=dynamic_cast<GenApi::IInteger *>(node);

          if (GenApi::IsReadable(p))
            addFeatureInt(parent, node);

          break;
        }
      case GenApi::intfIBoolean:
        {
          GenApi::IBoolean *p=dynamic_cast<GenApi::IBoolean *>(node);

          if (GenApi::IsReadable(p))
            addFeatureBool(parent, node);

          break;
        }
      case GenApi::intfICommand:
        {
          if (node->GetAccessMode() == GenApi::RW)
            addFeatureCmd(parent, node);
          break;
        }
      case GenApi::intfIFloat:
        {
          GenApi::IFloat *p=dynamic_cast<GenApi::IFloat *>(node);

          if (GenApi::IsReadable(p))
            addFeatureFloat(parent, node);
          break;
        }
      case GenApi::intfIString:
        {
          GenApi::IString *p=dynamic_cast<GenApi::IString *>(node);

          if (GenApi::IsReadable(p))
            addFeatureString(parent, node);

          break;
        }

      case GenApi::intfIRegister:
          break;

      case GenApi::intfICategory:
        {
          if (node->GetName() != "Root")
            parent = addFeatureCategory(node);

          GenApi::ICategory *root=dynamic_cast<GenApi::ICategory *>(node);

          if (root != 0) {
            GenApi::FeatureList_t feature;
            root->GetFeatures(feature);
            for (size_t i=0; i<feature.size(); i++)
              addFeatureNode(parent, feature[i]->GetNode());
          }
        }
        break;

      case GenApi::intfIEnumeration:
        {
          GenApi::IEnumeration *p=dynamic_cast<GenApi::IEnumeration *>(node);

          if (GenApi::IsReadable(p))
            addFeatureEnum(parent, node);

          break;
        }

      case GenApi::intfIEnumEntry:
        break;

      case GenApi::intfIPort:
        break;
    }
  }
}

void MainWindow::onDeviceItemClicked(QTreeWidgetItem *item, int col)
{
  std::cout << item->text(0).toStdString() << std::endl;
  const QVariant &data = item->data(0, Qt::UserRole);
  deviceInfoView->clear();

  if (data.isNull())
    return;

  const QHash<QString, QVariant> &hash = data.toHash();
  const QString &type = hash["type"].toString();
  QString *keys = NULL;
  int len = 0;
  if ( type == "system") {
    keys = (QString *)&system_keys;
    len = sizeof(system_keys) / sizeof(system_keys[0]);
  } else if (type == "interface") {
    keys = (QString *)&interf_keys;
    len = sizeof(interf_keys) / sizeof(interf_keys[0]);
  } else if (type == "device") {
    keys = (QString *)&device_keys;
    len = sizeof(device_keys) / sizeof(device_keys[0]);
    curDeviceID = hash["ID"].toString().toStdString();
  }

  for (int i = 0; i < len; i++)
    deviceInfoView->addItem(keys[i] + ": " + hash[keys[i]].toString());
}

void MainWindow::updateDeviceTree()
{
  deviceTree->clear();

  std::vector<std::shared_ptr<rcg::System> > system=rcg::System::getSystems();
  QHash<QString, QVariant> hash;
  for (size_t i=0; i<system.size(); i++) {
    try {
      system[i]->open();
    } catch (rcg::GenTLException &e) {
      std::cout << e.what();
      continue;
    }

    QTreeWidgetItem *sysitem = new QTreeWidgetItem(deviceTree);
    sysitem->setText(0, system[i]->getID().c_str());

    hash.clear();
    hash["type"] = "system";
    int n = 0;
    hash[system_keys[n++]] = system[i]->getID().c_str();
    hash[system_keys[n++]] = system[i]->getVendor().c_str();
    hash[system_keys[n++]] = system[i]->getModel().c_str();
    hash[system_keys[n++]] = system[i]->getVersion().c_str();
    hash[system_keys[n++]] = system[i]->getTLType().c_str();
    hash[system_keys[n++]] = system[i]->getPathname().c_str();
    hash[system_keys[n++]] = system[i]->getMajorVersion();
    sysitem->setData(0, Qt::UserRole, QVariant(hash));

    std::vector<std::shared_ptr<rcg::Interface> > interf=system[i]->getInterfaces();
    for (size_t k=0; k<interf.size(); k++) {
      try {
        interf[k]->open();
      } catch (rcg::GenTLException &e) {
        std::cout << e.what();
        continue;
      }

      QTreeWidgetItem *interfitem = new QTreeWidgetItem(sysitem);
      interfitem->setText(0, interf[k]->getDisplayName().c_str());

      hash.clear();
      hash["type"] = "interface";
      int n = 0;
      hash[interf_keys[n++]] = interf[k]->getID().c_str();
      hash[interf_keys[n++]] = interf[k]->getDisplayName().c_str();
      hash[interf_keys[n++]] = interf[k]->getTLType().c_str();
      interfitem->setData(0, Qt::UserRole, QVariant(hash));

      std::vector<std::shared_ptr<rcg::Device> > device=interf[k]->getDevices();
      for (size_t j=0; j<device.size(); j++) {
        QTreeWidgetItem *devitem = new QTreeWidgetItem(interfitem);
        devitem->setText(0, device[j]->getDisplayName().c_str());

        if (curDeviceID.empty())
          setCurDevice(device[j]->getID());

        hash.clear();
        hash["type"] = "device";
        int n = 0;
        hash[device_keys[n++]] = device[j]->getID().c_str();
        hash[device_keys[n++]] = device[j]->getVendor().c_str();
        hash[device_keys[n++]] = device[j]->getModel().c_str();
        hash[device_keys[n++]] = device[j]->getTLType().c_str();
        hash[device_keys[n++]] = device[j]->getDisplayName().c_str();
        hash[device_keys[n++]] = device[j]->getAccessStatus().c_str();
        hash[device_keys[n++]] = device[j]->getSerialNumber().c_str();
        hash[device_keys[n++]] = device[j]->getVersion().c_str();
        hash[device_keys[n++]] = (qulonglong)device[j]->getTimestampFrequency();
        devitem->setData(0, Qt::UserRole, QVariant(hash));
      }
      interf[k]->close();
    }
    system[i]->close();
  }
}

void MainWindow::setCurDevice(std::string const &dev)
{
  curDeviceID = dev;

  if (curActiveDeviceID.empty()) {
    if (!curDeviceID.empty())
      connAct->setEnabled(true);
    else
      connAct->setEnabled(false);
  }
}

void MainWindow::setCurActiveDevice(std::string const &dev)
{
  curActiveDeviceID = dev;

  if (!curActiveDeviceID.empty()) {
    connAct->setEnabled(false);
    disconnAct->setEnabled(true);
  } else {
    connAct->setEnabled(true);
    disconnAct->setEnabled(false);
  }
}

MainWindow::~MainWindow() {
}

void MainWindow::createActions()
{
  toolbar = new QToolBar();
  QList<QByteArray> formats = QImageReader::supportedImageFormats();
  QStringList list;
  for (auto &fmt : formats)
    list.append("*." + QString(fmt));
  auto filter = "Images (" + list.join(" ") + ")";

  toolbar->addAction(QIcon("data/icons/open_file.png"), "update",[=] {
  QString path = QFileDialog::getOpenFileName(nullptr, "Pick an image file",nullptr, filter);
        if (path.isEmpty())
            return;
        imageViewer->setImage(QImage(path));
  });

  updateAct = toolbar->addAction(QIcon("data/icons/update.png"), "update",
                                 this,  &MainWindow::updateDeviceTree);
  connAct = toolbar->addAction(QIcon("data/icons/connect.png"), "connect",
                               this, &MainWindow::openDevice);
  connAct->setEnabled(false);
  disconnAct = toolbar->addAction(QIcon("data/icons/disconnect.png"), "disconnect",
                                  this, &MainWindow::closeDevice);
  disconnAct->setEnabled(false);
  toolbar->addAction(QIcon("data/icons/device_attribute.png"), "device attribute",
                    [=] { deviceDock->setVisible(!deviceDock->isVisible()); });

  imageStartAct = toolbar->addAction(QIcon("data/icons/start_acquisition.png"),
                                     "start acquisition", [=] {
       if (imageParam.pause) {
         imageParam.pause = false;
         imageStartAct->setEnabled(false);
         imageStopAct->setEnabled(true);
         imagePauseAct->setEnabled(true);
         return;
       }

       std::shared_ptr<rcg::Device> dev=rcg::getDevice(curActiveDeviceID.c_str());
       if (dev) {
         std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();
         if (stream.size() > 0) {
             // opening first stream
           try {
             stream[0]->open();
             stream[0]->startStreaming();

             imageParam.stop = false;
             imageParam.pause = false;
             imageParam.stream = stream[0];
             imageParam.imageViewer = this->imageViewer;
             imageParam.th = std::thread(imageRecvThreadFunc, &imageParam);

             imageStartAct->setEnabled(false);
             imageStopAct->setEnabled(true);
             imagePauseAct->setEnabled(true);
           } catch (rcg::GenTLException &e) {
             std::cout << e.what() << std::endl;
           }
         }
       }
  });

  imagePauseAct = toolbar->addAction(QIcon("data/icons/pause.png"), "pause", [=] {
       imageParam.pause = true;
       imageStartAct->setEnabled(true);
       imagePauseAct->setEnabled(false);
       imageStopAct->setEnabled(false);
  });

  imageStopAct = toolbar->addAction(QIcon("data/icons/stop_acquisition.png"),
                                    "stop acquisition", [=] {
       std::shared_ptr<rcg::Device> dev=rcg::getDevice(curActiveDeviceID.c_str());
       if (dev) {
         std::vector<std::shared_ptr<rcg::Stream> > stream=dev->getStreams();
         if (stream.size() > 0) {
           imageParam.stop = true;
           imageParam.th.join();
           stream[0]->stopStreaming();
           stream[0]->close();
         }
       }
       imageStartAct->setEnabled(true);
       imageStopAct->setEnabled(false);
       imagePauseAct->setEnabled(false);
  });

  addToolBar(toolbar);
}

void MainWindow::createDeviceWindow()
{
  deviceDock = new QDockWidget(tr("Device Infomation"), this);
  deviceDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  QTabWidget *tab = new QTabWidget(deviceDock);
  deviceDock->setWidget(tab);
  QWidget *pageDevice = new QWidget();
  tab->addTab(pageDevice, "Device");

  QVBoxLayout *deviceLayout = new QVBoxLayout();
  deviceTree = new QTreeWidget;
  deviceLayout->addWidget(deviceTree);
  QObject::connect(deviceTree, &QTreeWidget::itemClicked,
                   this, &MainWindow::onDeviceItemClicked);

  deviceInfoView = new QListWidget();
  deviceLayout->addWidget(deviceInfoView);
  pageDevice->setLayout(deviceLayout);

  featureTree = new QTreeWidget;
  featureTree->setColumnCount(2);
  featureTree->setSelectionMode(QAbstractItemView::NoSelection);
  featureTree->setFocusPolicy(Qt::NoFocus);
  tab->addTab(featureTree, "Feature");
  addDockWidget(Qt::RightDockWidgetArea, deviceDock);
}

void MainWindow::openDevice()
{
  try {
    std::shared_ptr<rcg::Device> dev=rcg::getDevice(curDeviceID.c_str());
    if (dev) {
      dev->open(rcg::Device::CONTROL);
      setCurActiveDevice(curDeviceID);

      std::shared_ptr<GenApi::CNodeMapRef> nodemap = dev->getRemoteNodeMap();
      addFeatureNode(NULL, nodemap->_GetNode("Root"));
    }
  } catch (rcg::GenTLException &e) {
    std::cout << e.what();
  }
}

void MainWindow::closeDevice()
{
  try {
    std::shared_ptr<rcg::Device> dev=rcg::getDevice(curActiveDeviceID.c_str());
    if (dev) {
      dev->close();
      setCurActiveDevice("");
      featureTree->clear();
    }
  } catch (rcg::GenTLException &e) {
    std::cout << e.what();
  }
}

QTreeWidgetItem *MainWindow::addFeatureCategory(GenApi::INode *node)
{
  QTreeWidgetItem *topitem = new QTreeWidgetItem(featureTree);
  topitem->setText(0, node->GetName().c_str());
  return topitem;
}

QTreeWidgetItem* MainWindow::addFeatureInt(QTreeWidgetItem *parent, GenApi::INode *node)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  item->setText(0, node->GetName().c_str());
  QWidget *widget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  widget->setLayout(layout);

  GenApi::IInteger *p=dynamic_cast<GenApi::IInteger *>(node);

  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setMinimum(p->GetMin());
  slider->setMaximum(p->GetMax());
  slider->setValue(p->GetValue());
  slider->setSingleStep(p->GetInc());

  QSpinBox *spinBox = new QSpinBox();
  spinBox->setMinimum(p->GetMin());
  spinBox->setMaximum(p->GetMax());
  spinBox->setValue(p->GetValue());
  spinBox->setSingleStep(p->GetInc());

  QObject::connect(slider, &QSlider::valueChanged,
                   [=](int v) { spinBox->setValue(v); p->SetValue(v); });
  QObject::connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                   [=](int v) { slider->setValue(v); p->SetValue(v); });

  if (!GenApi::IsWritable(p)) {
    slider->setEnabled(false);
    spinBox->setEnabled(false);
  }
  layout->addWidget(slider);
  layout->addWidget(spinBox);
  featureTree->setItemWidget(item, 1, widget);

  return item;
}

QTreeWidgetItem *MainWindow::addFeatureFloat(QTreeWidgetItem *parent, GenApi::INode *node)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  item->setText(0, node->GetName().c_str());
  QWidget *widget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  widget->setLayout(layout);

  GenApi::IFloat *p=dynamic_cast<GenApi::IFloat *>(node);

  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setMinimum(p->GetMin());
  slider->setMaximum(p->GetMax());
  slider->setValue(p->GetValue());
  slider->setSingleStep(p->GetInc());

  QDoubleSpinBox *spinBox = new QDoubleSpinBox;
  spinBox->setMinimum(p->GetMin());
  spinBox->setMaximum(p->GetMax());
  spinBox->setValue(p->GetValue());
  spinBox->setSingleStep(p->GetInc());

  QObject::connect(slider, &QSlider::valueChanged,
                   [=](int v) { spinBox->setValue(v); p->SetValue(v); });
  QObject::connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                   [=](double v) { slider->setValue(v); p->SetValue(v); });

  if (!GenApi::IsWritable(p)) {
    slider->setEnabled(false);
    spinBox->setEnabled(false);
  }
  layout->addWidget(slider);
  layout->addWidget(spinBox);
  featureTree->setItemWidget(item, 1, widget);
  return item;
}

QTreeWidgetItem *MainWindow::addFeatureBool(QTreeWidgetItem *parent, GenApi::INode *node)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  item->setText(0, node->GetName().c_str());
  QWidget *widget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  widget->setLayout(layout);
  QCheckBox *checkBox = new QCheckBox;

  GenApi::IBoolean *p=dynamic_cast<GenApi::IBoolean *>(node);
  checkBox->setCheckState(p->GetValue() ? Qt::Checked : Qt::Unchecked);

  if (!GenApi::IsWritable(p))
    checkBox->setEnabled(false);

  layout->addWidget(checkBox);
  featureTree->setItemWidget(item, 1, widget);
  return item;
}

QTreeWidgetItem *MainWindow::addFeatureEnum(QTreeWidgetItem *parent, GenApi::INode *node)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  item->setText(0, node->GetName().c_str());
  QWidget *widget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  widget->setLayout(layout);
  QComboBox *comboBox = new QComboBox();

  GenApi::IEnumeration *p=dynamic_cast<GenApi::IEnumeration *>(node);

  GenApi::StringList_t list;
  p->GetSymbolics(list);
  for (size_t i=0; i<list.size(); i++)
    comboBox->addItem(list[i].c_str());

  if (p->GetCurrentEntry() != 0)
    comboBox->setCurrentText(p->GetCurrentEntry()->GetSymbolic().c_str());

  if (!GenApi::IsWritable(p))
    comboBox->setEnabled(false);

  layout->addWidget(comboBox);
  featureTree->setItemWidget(item, 1, widget);
  return item;
}

QTreeWidgetItem *MainWindow::addFeatureCmd(QTreeWidgetItem *parent, GenApi::INode *node)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  item->setText(0, node->GetName().c_str());
  QWidget *widget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  widget->setLayout(layout);
  QPushButton *btn = new QPushButton("Execute");
  layout->addWidget(btn);
  featureTree->setItemWidget(item, 1, widget);
  return item;
}

QTreeWidgetItem *MainWindow::addFeatureString(QTreeWidgetItem *parent, GenApi::INode *node)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  item->setText(0, node->GetName().c_str());

  QWidget *widget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  widget->setLayout(layout);

  GenApi::IString *p=dynamic_cast<GenApi::IString *>(node);

  QLineEdit *lineEdit = new QLineEdit();
  lineEdit->setText(p->GetValue().c_str());
  layout->addWidget(lineEdit);
  // if (!GenApi::IsWritable(p))
    lineEdit->setReadOnly(true);

  featureTree->setItemWidget(item, 1, widget);
  return item;
}
