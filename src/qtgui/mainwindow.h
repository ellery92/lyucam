/*  -*- mode: c++ -*- */
#pragma once

#include <QMainWindow>
class QTableView;
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
class QListWidget;
#include <QLabel>
#include <map>
#include <rc_genicam_api/config.h>

// class ImageView;

class ImageView : public QLabel
{
  Q_OBJECT

  public:
  ImageView(QWidget * parent = 0) : QLabel(parent) {
  }
  ~ImageView(){}

  virtual void paintEvent(QPaintEvent *evt);
  QImage &getImage() { return image; }

private:
  QImage image;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();
  void createActions();

signals:
  void frameFinished();

private slots:
  void openDevice();
  void closeDevice();
  void onDeviceItemClicked(QTreeWidgetItem *item, int col);

protected:
  void updateDeviceTree();
  void connectDevice();

  void addFeatureNode(QTreeWidgetItem *parent, GenApi::INode *node);
  QTreeWidgetItem *addFeatureCategory(GenApi::INode *node);
  QTreeWidgetItem *addFeatureInt(QTreeWidgetItem *parent, GenApi::INode *node);
  QTreeWidgetItem *addFeatureString(QTreeWidgetItem *parent, GenApi::INode *node);
  QTreeWidgetItem *addFeatureFloat(QTreeWidgetItem *parent, GenApi::INode *node);
  QTreeWidgetItem *addFeatureBool(QTreeWidgetItem *parent, GenApi::INode *node);
  QTreeWidgetItem *addFeatureEnum(QTreeWidgetItem *parent, GenApi::INode *node);
  QTreeWidgetItem *addFeatureCmd(QTreeWidgetItem *parent, GenApi::INode *node);

  void setCurDevice(std::string const &dev);
  void setCurActiveDevice(std::string const &dev);

public:
  QTableView *deviceListView;
  QTreeWidget *featureTree;
  ImageView *imageView;
  QTreeWidget *deviceTree;
  QListWidget *deviceInfoView;

  std::string curDeviceID;
  std::string curActiveDeviceID;

  QAction *updateAct;
  QAction *connAct;
  QAction *disconnAct;
};
