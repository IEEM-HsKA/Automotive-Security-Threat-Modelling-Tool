#pragma once
#ifndef ICONTREE_H
#define ICONTREE_H

#include "mainctrl.h"
#include "nodectrl.h"

#include <QDebug>
#include <QInputDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QtCore>

class IconTree : public QWidget {
  Q_OBJECT

public:
  QTreeWidget *m_tree;
  MainCtrl *m_mainCtrl;
  IconTree(QWidget *parent = 0);
  void setTreeRoot(QString name);
  void setTreeChild(QTreeWidgetItem *item, QString name, QIcon icon);
  void setMainCtrl(MainCtrl *mainCtrl);
  ~IconTree();

public slots:
  void onTreeItemClicked(QTreeWidgetItem *item);
  void onTreeItemDoubleClicked(QTreeWidgetItem *item);
};

#endif // ICONTREE_H
