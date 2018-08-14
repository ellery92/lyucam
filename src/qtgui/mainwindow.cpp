#include <QtWidgets>
#include "mainwindow.h"
// #include "devicelistmodel.h"
#include <QAbstractTableModel>
#include <vector>
#include <string>

#include <rc_genicam_api/system.h>
#include <rc_genicam_api/interface.h>
#include <rc_genicam_api/device.h>
#include <rc_genicam_api/stream.h>
#include <rc_genicam_api/config.h>
#include <rc_genicam_api/exception.h>

const QString system_keys[] =
  { "ID", "Ventdor", "Model", "Vendor version",
    "TL type", "Pathname", "GenTL version"};
const QString interf_keys[] =
  { "ID", "Display Name", "TL type:"};
const QString device_keys[] =
  { "ID", "Vendor", "Model", "TL type", "Display name", "Aceess status",
    "Serial number", "Version", "TS Frequency"};

struct device_status {
  std::string device_id;
  std::shared_ptr<rcg::Device> dev;
  bool bopen;
  std::shared_ptr<GenApi::CNodeMapRef> nodemap;
  std::vector<std::shared_ptr<rcg::Stream> > stream;
  std::vector<bool> bstreaming;
};

QHash<std::string, device_status> devHash;

void ImageView::paintEvent(QPaintEvent *evt)
{
  QPainter painter(this);
  painter.drawImage(QPoint(0, 0), image);
}

void image_recv(uint8_t *buf, int w, int h, int len, void *user_data)
{
  MainWindow *mwnd = (MainWindow*)user_data;
  QImage &image = mwnd->imageView->getImage();
  for (int i = 0; i < h; i++)
    memcpy(image.scanLine(i), buf + i * w * 3, w * 3);

  emit mwnd->frameFinished();
}

MainWindow::MainWindow() {
  QWidget *mainContentWidget = new QWidget();
  setCentralWidget(mainContentWidget);
  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainContentWidget->setLayout(mainLayout);

  QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);

  QWidget *deviceContentWidget = new QWidget();
  QVBoxLayout *deviceLayout = new QVBoxLayout();
  sp.setHorizontalStretch(1.5);
  deviceContentWidget->setSizePolicy(sp);
  mainLayout->addWidget(deviceContentWidget);
  deviceContentWidget->setLayout(deviceLayout);

  deviceTree = new QTreeWidget;
  // featureTree->header()->hide();
  // sp.setVerticalStretch(5);
  // deviceTree->setSizePolicy(sp);
  deviceLayout->addWidget(deviceTree);

  QHash<QString, QVariant> hash;

  QTreeWidgetItem *topitem, *item, *item2;
  topitem = new QTreeWidgetItem(deviceTree);
  topitem->setText(0, "System");
  hash["type"] = "system";
  for (int i = 0; i < sizeof(system_keys) / sizeof(system_keys[0]); i++)
    hash[system_keys[i]] = "This is a system";
  topitem->setData(0, Qt::UserRole, QVariant(hash));

  item = new QTreeWidgetItem(topitem);
  item->setText(0, "Interface");
  hash.clear();
  hash["type"] = "interface";
  for (int i = 0; i < sizeof(interf_keys) / sizeof(interf_keys[0]); i++)
    hash[interf_keys[i]] = "This is an Interface";
  item->setData(0, Qt::UserRole, QVariant(hash));

  item2 = new QTreeWidgetItem(item);
  item2->setText(0, "device");
  hash.clear();
  hash["type"] = "device";
  for (int i = 0; i < sizeof(device_keys) / sizeof(device_keys[0]); i++)
    hash[device_keys[i]] = "This is a device";
  item2->setData(0, Qt::UserRole, QVariant(hash));

  QTreeWidgetItem *header = deviceTree->headerItem();
  header->setText(0, "Device Tree");

  deviceInfoView = new QListWidget();
  deviceLayout->addWidget(deviceInfoView);

  imageView = new ImageView;
  sp.setHorizontalStretch(5);
  imageView->setSizePolicy(sp);
  imageView->setAutoFillBackground(true);
  imageView->setBackgroundRole(QPalette::Dark);
  imageView->setScaledContents(true);
  mainLayout->addWidget(imageView);

  featureTree = new QTreeWidget;
  // featureTree->header()->hide();
  featureTree->setColumnCount(2);
  sp.setHorizontalStretch(3);
  featureTree->setSizePolicy(sp);
  mainLayout->addWidget(featureTree);
  featureTree->setSelectionMode(QAbstractItemView::NoSelection);
  featureTree->setFocusPolicy(Qt::NoFocus);

  QTreeWidgetItem *category = new QTreeWidgetItem(featureTree);
  category->setText(0, "Category");
  item = new QTreeWidgetItem(category);
  item->setText(0, "Exposure");
  QWidget *widget = new QWidget();
  QHBoxLayout *lout = new QHBoxLayout();
  widget->setLayout(lout);

  QSlider *slider = new QSlider(Qt::Horizontal);
  QSpinBox *spinBox = new QSpinBox();

  lout->addWidget(slider);
  lout->addWidget(spinBox);
  featureTree->setItemWidget(item, 1, widget);

  item = new QTreeWidgetItem(category);
  item->setText(0, "DeviceName");
  lout = new QHBoxLayout();

  QComboBox *comboBox = new QComboBox;
  comboBox->addItem("Item1");
  comboBox->addItem("Item2");
  comboBox->addItem("Item3");
  comboBox->setCurrentText("Item1");

  QPushButton *btn = new QPushButton;
  btn->setText("Exec");

  lout->addWidget(btn);
  widget = new QWidget();
  widget->setLayout(lout);
  featureTree->setItemWidget(item, 1, widget);

  createActions();

  QObject::connect(this, SIGNAL(frameFinished()),
                   imageView, SLOT(update()));

  QObject::connect(deviceTree, &QTreeWidget::itemClicked,
                   this, &MainWindow::onDeviceItemClicked);
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
    QMenu *deviceMenu = menuBar()->addMenu(tr("Device"));

    deviceMenu->addAction(tr("Open"), this, &MainWindow::openDevice);
    deviceMenu->addAction(tr("Close"), this, &MainWindow::closeDevice);

    QToolBar *toolbar = new QToolBar();
    updateAct = toolbar->addAction(QIcon("data/icons/update.png"), "update",
                                   this,  &MainWindow::updateDeviceTree);
    connAct = toolbar->addAction(QIcon("data/icons/connect.png"), "connect",
                                 this, &MainWindow::openDevice);
    connAct->setEnabled(false);
    disconnAct = toolbar->addAction(QIcon("data/icons/disconnect.png"), "disconnect",
                                    this, &MainWindow::closeDevice);
    disconnAct->setEnabled(false);
    addToolBar(toolbar);
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
