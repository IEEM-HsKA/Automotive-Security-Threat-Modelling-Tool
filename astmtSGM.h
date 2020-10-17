#ifndef ASTMT_SGM_H
#define ASTMT_SGM_H

#include "analysegraph\bglgraph.h"
#include "analysegraph\graphmanager.h"
#include "astmtCore.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QPushButton>
#include <QString.h>
#include <QStringListModel>
#include <QTableWidget>

/// <summary>User interface for the SGM artefacts (SGM tabelle)</summary>
class SGMTable : public QWidget {
  Q_OBJECT

public:
  SGMTable(GraphManager EEArchitecture, QWidget *parent = 0);
  ~SGMTable();

  void setEEArchitecture(GraphManager *EEArchitecture);
  std::vector<std::pair<std::string, std::string>> m_result;

signals:
  void astmtResultsReady();

private slots:
  void cellSelected(int nRow, int nCol);
  void collectTableData();
  void startAnalyse();
  void addRow();

private:
  QTableWidget *m_pTableWidget;
  QStringList m_TableHeader;
  QPushButton *m_submit;
  QPushButton *m_startAnalyse;
  QPushButton *m_newEntry;
  QStringListModel *m_guidewordsM;
  QStringListModel *m_componentM;
  QStringListModel *m_allElementsM;
  QStringListModel *m_severityM;
  QDialogButtonBox *m_buttonBox;
  std::vector<QList<QString>> m_tableContent;
  GraphManager m_EEArchitecture;
  diGraph m_graph;
};

#endif ASTMT_SGM_H
