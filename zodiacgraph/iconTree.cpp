#include "iconTree.h"

#include <QIcon>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

IconTree::IconTree(QWidget *parent) : QWidget(parent) {
  m_tree = new QTreeWidget();
  m_mainCtrl = NULL;

  m_tree->setIconSize(QSize(40, 40));
  m_tree->setHeaderLabel("Component Library");

  connect(m_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this,
          SLOT(onTreeItemClicked(QTreeWidgetItem *)));
  connect(m_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this,
          SLOT(onTreeItemDoubleClicked(QTreeWidgetItem *)));

  // Load componentes and connections
  setTreeRoot("CPS");
  setTreeRoot("Sensor");
  setTreeRoot("Actuator");
  setTreeRoot("HMI");
  setTreeRoot("Entry Points");
  // setTreeRoot("Communication System");

  // CPS
  setTreeChild(m_tree->topLevelItem(0), "ECU", QIcon(":/icons/ECU.png"));
  setTreeChild(m_tree->topLevelItem(0), "Gateway", QIcon(":/icons/ECU.png"));

  // Sensors
  setTreeChild(m_tree->topLevelItem(1), "Sensor", QIcon(":/icons/ECU.png"));
  setTreeChild(m_tree->topLevelItem(1), "Camera", QIcon(":/icons/ECU.png"));
  setTreeChild(m_tree->topLevelItem(1), "Radar", QIcon(":/icons/ECU.png"));
  setTreeChild(m_tree->topLevelItem(1), "LiDAR", QIcon(":/icons/ECU.png"));

  // Actuators
  setTreeChild(m_tree->topLevelItem(2), "Actuator", QIcon(":/icons/ECU.png"));

  // HMI
  setTreeChild(m_tree->topLevelItem(3), "CD", QIcon(":/icons/ECU.png"));
  setTreeChild(m_tree->topLevelItem(3), "DVD", QIcon(":/icons/ECU.png"));
  setTreeChild(m_tree->topLevelItem(3), "OBD", QIcon(":/icons/ECU.png"));

  // Entry points
  setTreeChild(m_tree->topLevelItem(4), "Wireless-Entry-Point",
               QIcon(":/icons/ECU.png"));
  setTreeChild(m_tree->topLevelItem(4), "Physical-Enrty-Point",
               QIcon(":/icons/ECU.png"));
}

void IconTree::setMainCtrl(MainCtrl *mainCtrl) { m_mainCtrl = mainCtrl; }

void IconTree::setTreeRoot(QString name) {

  QTreeWidgetItem *treeItem = new QTreeWidgetItem();
  treeItem->setText(0, name);
  m_tree->addTopLevelItem(treeItem);
}

void IconTree::setTreeChild(QTreeWidgetItem *item, QString name, QIcon icon) {

  QTreeWidgetItem *childItem = new QTreeWidgetItem();
  childItem->setText(0, name);
  childItem->setIcon(0, icon);
  item->addChild(childItem);
}

void IconTree::onTreeItemClicked(QTreeWidgetItem *item) {

  //// Get the pointer to the currently selected item.
  // QTreeWidgetItem *currentItem = m_tree->currentItem();
  QTreeWidgetItem *currentItem = item;
  qDebug() << currentItem->text(0);

  if (m_mainCtrl != NULL) {
  }
}

void IconTree::onTreeItemDoubleClicked(QTreeWidgetItem *item) {

  // Get the pointer to the currently selected item.
  // QTreeWidgetItem *currentItem = m_tree->currentItem();
  QTreeWidgetItem *currentItem = item;
  qDebug() << currentItem->text(0);

  if (m_mainCtrl != NULL) {
    // Open Dialog and ask for component name
    bool ok;
    QString componentType = currentItem->text(0);
    QString name = QInputDialog::getText(this, tr("Create new component"),
                                         tr("Please enter component name:"),
                                         QLineEdit::Normal, "", &ok);

    // create new node if a component name was provided
    if (ok && !name.isEmpty()) {
      name = name.simplified();
      name.replace(" ", "_");
      NodeCtrl *nodeCtrl = m_mainCtrl->createNode(name, componentType);
      nodeCtrl->getNodeHandle().setPos(600, 700);
    }
  }
}

IconTree::~IconTree() {}