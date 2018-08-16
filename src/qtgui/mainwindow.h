/*  -*- mode: c++ -*- */
#pragma once

#include <QMainWindow>
#include <QLabel>
#include <GenApi/GenApi.h>

class QTableView;
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
class QListWidget;
class QScrollArea;
class QDockWidget;

namespace pal {
  class ImageViewer;
};

class RectLabel : public QLabel
{
  Q_OBJECT
public:
  RectLabel();
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();
  void createActions();
  void createDeviceWindow();

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
  pal::ImageViewer *imageViewer;

  QDockWidget *deviceDock;
  QListWidget *deviceInfoView;
  QTreeWidget *deviceTree;
  QTreeWidget *featureTree;

  std::string curDeviceID;
  std::string curActiveDeviceID;

  QToolBar *toolbar;
  QAction *updateAct;
  QAction *connAct;
  QAction *disconnAct;
  QAction *imageStartAct;
  QAction *imageStopAct;
  QAction *imagePauseAct;
};
