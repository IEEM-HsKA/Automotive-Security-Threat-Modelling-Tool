#include "iconList.h"

#include <QIcon>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

IconList::IconList(QWidget *parent) : QWidget(parent) {
  m_list = new QListWidget;
  m_mainCtrl = NULL;

  m_list->setIconSize(QSize(60, 60));

  connect(m_list, SIGNAL(itemClicked(QListWidgetItem *)), this,
          SLOT(onListItemClicked(QListWidgetItem *)));
  connect(m_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
          SLOT(onListItemDoubleClicked(QListWidgetItem *)));

  const QIcon *icon1 = new QIcon(":/icons/ECU.png");
  QListWidgetItem *pItem1 = new QListWidgetItem(*icon1, "ECU", m_list);

  const QIcon *icon2 = new QIcon(":/icons/CAN.png");
  QListWidgetItem *pItem2 = new QListWidgetItem(*icon2, "CAN", m_list);

  /*QString iconPath;
  QString iconText
  QIcon icon;*/

  // for (int i = 0; i <= numList; i++)
  //{
  //	//const QIcon *icon = new QIcon(iconPath);
  //	QListWidgetItem * listItem = new QListWidgetItem(*icon, iconText,
  //m_list); 	m_list->addItem(listItem);
  //}

  m_list->addItem(pItem1);
  m_list->addItem(pItem2);
}

void IconList::setMainCtrl(MainCtrl *mainCtrl) { m_mainCtrl = mainCtrl; }

void IconList::onListItemClicked(QListWidgetItem *item) {

  // Get the pointer to the currently selected item.
  QListWidgetItem *currentItem = m_list->currentItem();
  qDebug() << currentItem->text();

  if (m_mainCtrl != NULL) {
  }
}

void IconList::onListItemDoubleClicked(QListWidgetItem *item) {

  // Get the pointer to the currently selected item.
  QListWidgetItem *currentItem = m_list->currentItem();
  qDebug() << currentItem->text();

  if (m_mainCtrl != NULL) {
    NodeCtrl *nodeCtrl = m_mainCtrl->createNode(currentItem->text());
    nodeCtrl->getNodeHandle().setPos(600, 700);
  }
}

IconList::~IconList() {}