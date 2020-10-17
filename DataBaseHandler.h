#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H
#pragma once
#include "architectureElements.h"
#include "vulnerability.h"

#include <QtCore\qvariant.h>
#include <QtSql\qsqlquery.h>
#include <QtSql\qtsqlglobal.h>
#include <Qtcore\QString.h>
#include <Qtcore\QStringList.h>
#include <unordered_map>

class DatabaseHandler {

public:
  bool dataBaseOpen = false;
  QSqlDatabase m_db;
  DatabaseHandler(QString dbName);
  ~DatabaseHandler();

  void readFromDataBase(std::vector<QStringList> &entries);
  void readVulnerabilityFromDataBase(
      std::vector<QStringList> &dataBaseVuls, ComputeUnits compute,
      Connections connect,
      std::unordered_map<unsigned int, Vulnerability> &globalVulMap);
  unsigned short checkPrivileges(QString privileges);

private:
};

#endif // DATABASEHANDLER_H
