#pragma once
#ifndef ICONLIST_H
#define ICONLIST_H

#include "mainctrl.h"
#include "nodectrl.h"

#include <QDebug>
#include <QListWidget>
#include <QWidget>
#include <QtCore>

class IconList : public QWidget {
  Q_OBJECT

public:
  QListWidget *m_list;
  MainCtrl *m_mainCtrl;
  IconList(QWidget *parent = 0);
  void setMainCtrl(MainCtrl *mainCtrl);
  ~IconList();

public slots:
  void onListItemClicked(QListWidgetItem *item);
  void onListItemDoubleClicked(QListWidgetItem *item);
};

#endif // ICONLIST_H
