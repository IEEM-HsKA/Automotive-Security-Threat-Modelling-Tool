#include "astmtSGM.h"
#include "architectureElements.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <unordered_map>

#define testmode 0

/// <summary>Config of the graphical user interface for SGM table</summary>
SGMTable::SGMTable(GraphManager EEArchitecture, QWidget *parent)
    : m_EEArchitecture(EEArchitecture), QWidget(parent) {
  m_EEArchitecture.buildBGLGraph();
  m_EEArchitecture.printGraph();

  m_graph = m_EEArchitecture.getBGLGraph().getDiaGraph();

  m_pTableWidget = new QTableWidget(this);
  m_pTableWidget->setRowCount(1);
  m_pTableWidget->setColumnCount(7);
  m_TableHeader << "Threat-ID"
                << "Hazard"
                << "can be triggered by"
                << "message or function"
                << "on component or system"
                << "on entry point"
                << "Severity";
  m_pTableWidget->setHorizontalHeaderLabels(m_TableHeader);
  m_pTableWidget->verticalHeader()->setVisible(false);

  m_pTableWidget->AdjustToContents;
  m_pTableWidget->setShowGrid(true);
  m_pTableWidget->setStyleSheet(
      "QTableView {selection-background-color: grey;}");
  m_pTableWidget->setColumnWidth(1, 200);

  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addWidget(m_pTableWidget);
  this->setLayout(mainLayout);
  m_buttonBox = new QDialogButtonBox(Qt::Vertical);
  m_startAnalyse = new QPushButton("Start analysis", this);
  m_newEntry = new QPushButton("New entry", this);
  m_buttonBox->addButton(m_startAnalyse, QDialogButtonBox::ActionRole);
  m_buttonBox->addButton(m_newEntry, QDialogButtonBox::ActionRole);

  mainLayout->addWidget(m_buttonBox);

  connect(m_startAnalyse, SIGNAL(clicked(bool)), this, SLOT(startAnalyse()));
  connect(m_newEntry, SIGNAL(clicked(bool)), this, SLOT(addRow()));
  connect(m_pTableWidget, SIGNAL(cellDoubleClicked(int, int)), this,
          SLOT(cellSelected(int, int)));

  QStringList guidewords;
  guidewords << "disclosure of information"
             << "reset of component"
             << "manipulation of message"
             << "manipulation of firmware"
             << "disconnection of component"
             << "delay of message or signal"
             << "deletion of message or signal"
             << "stopping of component"
             << "activation of(diagnostic) function"
             << "insertion of message or signal";
  m_guidewordsM = new QStringListModel();
  m_guidewordsM->setStringList(guidewords);

  QStringList components;
  QStringList allElements;
  std::vector<std::string> tmpEdges = m_EEArchitecture.getEdges();
  std::vector<std::string> tmpNodes = m_EEArchitecture.getNodes();

  for (auto it = tmpNodes.begin(); it != tmpNodes.end(); ++it) {

    components << QString::fromStdString(*it);
  }
  allElements << components;

  for (auto it = tmpEdges.begin(); it != tmpEdges.end(); ++it) {

    allElements << QString::fromStdString(*it);
  }

  m_componentM = new QStringListModel();
  m_componentM->setStringList(components);

  m_allElementsM = new QStringListModel();
  m_allElementsM->setStringList(allElements);

  QStringList severity;
  severity << "S1"
           << "S2"
           << "S3";
  m_severityM = new QStringListModel();
  m_severityM->setStringList(severity);

  for (int row = 0; row < m_pTableWidget->rowCount(); ++row) {

    for (int col = 0; col < m_pTableWidget->columnCount(); ++col) {
      if (col == 0) {

        m_pTableWidget->setItem(row, col,
                                new QTableWidgetItem(QString::number(row)));
      } else if (col == 2) {
        QComboBox *combo_c2 = new QComboBox();
        combo_c2->setModel(m_guidewordsM);
        m_pTableWidget->setCellWidget(row, col, combo_c2);

      } else if (col == 4) {
        QComboBox *combo_c4 = new QComboBox();
        combo_c4->setModel(m_componentM);
        m_pTableWidget->setCellWidget(row, col, combo_c4);
      } else if (col == 5) {
        QComboBox *combo_c5 = new QComboBox();
        combo_c5->setModel(m_allElementsM);
        m_pTableWidget->setCellWidget(row, col, combo_c5);
      } else if (col == 6) {
        QComboBox *combo_c6 = new QComboBox();
        combo_c6->setModel(m_severityM);
        m_pTableWidget->setCellWidget(row, col, combo_c6);
      } else {
        m_pTableWidget->setItem(row, col, new QTableWidgetItem(""));
      }
    }
  }
  m_pTableWidget->resizeColumnsToContents();
}

SGMTable::~SGMTable() {}

void SGMTable::cellSelected(int nRow, int nCol) {
  // QMessageBox::information(this, "", "Cell at row " + QString::number(nRow) +
  // " column " + QString::number(nCol) + " was double clicked.");
}

void SGMTable::collectTableData() {
  std::vector<QList<QString>> tableContent;
  QList<QString> itemList;
  QString temp;

  for (int row = 0; row < m_pTableWidget->rowCount(); ++row) {
    itemList.clear();
    for (int col = 0; col < m_pTableWidget->columnCount(); ++col) {
      if (col == 2) {
        QComboBox *combo_c2 = (QComboBox *)m_pTableWidget->cellWidget(row, col);
        temp = combo_c2->currentText();
      } else if (col == 4) {
        QComboBox *combo_c4 = (QComboBox *)m_pTableWidget->cellWidget(row, col);
        temp = combo_c4->currentText();
      } else if (col == 5) {
        QComboBox *combo_c5 = (QComboBox *)m_pTableWidget->cellWidget(row, col);
        temp = combo_c5->currentText();
      } else if (col == 6) {
        QComboBox *combo_c6 = (QComboBox *)m_pTableWidget->cellWidget(row, col);
        temp = combo_c6->currentText();
      } else {
        if (m_pTableWidget->item(row, col)->text() == "") {
          temp = "-";
        } else {
          temp = m_pTableWidget->item(row, col)->text();
        }
      }
      itemList.push_back(temp);
    }
    tableContent.push_back(itemList);
  }
  m_tableContent = tableContent;
}

void SGMTable::addRow() {

  m_pTableWidget->insertRow(m_pTableWidget->rowCount());

  for (int col = 0; col < m_pTableWidget->columnCount(); ++col) {
    if (col == 0) {
      m_pTableWidget->setItem(m_pTableWidget->rowCount() - 1, col,
                              new QTableWidgetItem(QString::number(
                                  m_pTableWidget->rowCount() - 1)));
    } else if (col == 2) {
      QComboBox *combo_c2 = new QComboBox();
      combo_c2->setModel(m_guidewordsM);
      m_pTableWidget->setCellWidget(m_pTableWidget->rowCount() - 1, col,
                                    combo_c2);

    } else if (col == 4) {
      QComboBox *combo_c4 = new QComboBox();
      combo_c4->setModel(m_componentM);
      m_pTableWidget->setCellWidget(m_pTableWidget->rowCount() - 1, col,
                                    combo_c4);
    } else if (col == 5) {
      QComboBox *combo_c5 = new QComboBox();
      combo_c5->setModel(m_allElementsM);
      m_pTableWidget->setCellWidget(m_pTableWidget->rowCount() - 1, col,
                                    combo_c5);
    } else if (col == 6) {
      QComboBox *combo_c6 = new QComboBox();
      combo_c6->setModel(m_severityM);
      m_pTableWidget->setCellWidget(m_pTableWidget->rowCount() - 1, col,
                                    combo_c6);
    } else {
      m_pTableWidget->setItem(m_pTableWidget->rowCount() - 1, col,
                              new QTableWidgetItem(""));
    }
  }
  m_pTableWidget->resizeColumnsToContents();
}

void SGMTable::setEEArchitecture(GraphManager *EEArchitecture) {
  m_EEArchitecture = *EEArchitecture;
}

void SGMTable::startAnalyse() {
  std::unordered_map<unsigned int, std::string> componetClasses;
  ComputeUnits computeElements;
  Connections connectionElements;

  for (auto it = computeElements.m_mapping.begin();
       it != computeElements.m_mapping.end(); ++it) {
    componetClasses.insert(std::make_pair(it->second, it->first));
  }

  for (auto it = connectionElements.m_mapping.begin();
       it != connectionElements.m_mapping.end(); ++it) {
    componetClasses.insert(std::make_pair(it->second, it->first));
  }

  collectTableData();

  // QFuture<void> ASTMT_thread = QtConcurrent::run(startASTMTcore);
  // ASTMT_thread.waitForFinished();

  bool state =
      ASTMT::startASTMTcore(m_graph, componetClasses, m_tableContent, m_result);

  if (state) {
    // everything fine
    emit astmtResultsReady();
  }
}
