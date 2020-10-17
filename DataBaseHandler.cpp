#include "DataBaseHandler.h"

#include <QtCore\QRegularExpression.h>
#include <QtCore\qdebug.h>
#include <QtCore\qfile.h>
#include <QtSql\qsqlerror.h>
#include <QtSql\qsqlrecord.h>

/// <summary>Handels SQL database connection</summary>
/// <returns>SQL-Handle</returns> 
DatabaseHandler::DatabaseHandler(QString dbName) {
  m_db = QSqlDatabase::addDatabase("QSQLITE");

  if (QFile::exists(dbName)) {
    qDebug() << "Database already exists, database will be opened.";
    m_db.setDatabaseName(dbName);
    m_db.open();
  } else {
    qDebug() << "Database does not exist, database will be created.";
    m_db.setDatabaseName(dbName);
    m_db.open();

    QSqlQuery query(m_db);
    query.prepare(
        "CREATE TABLE IF NOT EXISTS Attacks (ID TEXT, Description TEXT, "
        "Reference TEXT, Year TEXT, AttackMotivation TEXT,"
        "AttackClass TEXT, AttackBase TEXT, AttackType TEXT, "
        "ViolatedSecurityProperty TEXT, AffectedAsset TEXT, Vulnerability TEXT,"
        "Interface TEXT,Consequences TEXT, AttackPath TEXT, Requirement TEXT, "
        "RequiredPrivileges Text, AcquiredPrivileges TEXT, Restriction TEXT, "
        "AttackLevel TEXT,"
        "Vehicle TEXT, Component TEXT, Tools TEXT, VulnerabilityDatabase TEXT, "
        "CVSSv3BaseMetric TEXT, CVSSv3Exploitability TEXT)");

    query.exec();
    qDebug() << "Database created" << query.lastError();
  }

  if (m_db.isOpen()) {
    dataBaseOpen = true;

    qDebug() << "Connected to database.";

  } else {
    dataBaseOpen = false;

    qDebug() << "Not connected to database.";
  }
}
 
/// <summary>Read content of the SQL database</summary>
/// <returns>Relevant entries of the attack database</returns> 
void DatabaseHandler::readFromDataBase(std::vector<QStringList> &entries) {
  // get generic vulnerabilities
  QSqlQuery queryGenVulnerabilities(m_db);
  queryGenVulnerabilities.prepare(
      "SELECT "
      "ID,Element,AcquiredPrivileges,Description,CVSSv3Exploitability,"
      "RequiredPrivileges, ViolatedSecurityProperty, Interface, EntryPoint "
      "FROM GenericVulnerabilities");
  queryGenVulnerabilities.exec();
  qDebug() << "Read generic vulnerabilities from database"
           << queryGenVulnerabilities.lastError();

  while (queryGenVulnerabilities.next()) {
    // represents table row
    QStringList row;

    QString idVulnerabilitiy = queryGenVulnerabilities.value(0).toString();
    // Component
    QString componentVulnerabilitiy =
        queryGenVulnerabilities.value(1).toString();
    // Acquired_Privileges
    QString acPrivilegesVulnerabilitiy =
        queryGenVulnerabilities.value(2).toString();
    // Description (CWE)
    QString descriptionVulnerabilitiy =
        queryGenVulnerabilities.value(3).toString();
    descriptionVulnerabilitiy = descriptionVulnerabilitiy.split('\n').at(0);
    // CVSS exploitability value
    QString exploitabiliyVulnerabilitiy =
        queryGenVulnerabilities.value(4).toString();
    // Required privileges
    QString rePrivilegesVulnerabilitiy =
        queryGenVulnerabilities.value(5).toString();
    // Violated Security Properties
    QString violatedPropertiesVulnerabilitiy =
        queryGenVulnerabilities.value(6).toString();
    // Interfaces
    QString interfacesVulnerabilitiy =
        queryGenVulnerabilities.value(7).toString();
    // EntryPoint
    QString enrtyPointVulnerabilitiy =
        queryGenVulnerabilities.value(8).toString();

    // save each cell of a row in a string list
    row.push_back(idVulnerabilitiy);
    row.push_back(componentVulnerabilitiy);
    row.push_back(acPrivilegesVulnerabilitiy);
    row.push_back(descriptionVulnerabilitiy);
    row.push_back(exploitabiliyVulnerabilitiy);
    row.push_back(rePrivilegesVulnerabilitiy);
    row.push_back(violatedPropertiesVulnerabilitiy);
    row.push_back(interfacesVulnerabilitiy);
    row.push_back(enrtyPointVulnerabilitiy);

    entries.push_back(row);
  }

  QSqlQuery queryAttacks(m_db);
  queryAttacks.prepare(
      "SELECT "
      "ID,Component,AcquiredPrivileges,Description,CVSSv3Exploitability,"
      "RequiredPrivileges, ViolatedSecurityProperty, Interface, EntryPoint "
      "FROM Attacks");
  queryAttacks.exec();
  qDebug() << "Read attacks from database" << queryAttacks.lastError();

  while (queryAttacks.next()) {
    // represents table row
    QStringList row;
    // ID
    QString idAttack = queryAttacks.value(0).toString();
    // Component
    QString componentAttack = queryAttacks.value(1).toString();
    componentAttack = componentAttack.split("\n").at(0);
    // Acquired_Privileges
    QString acPrivilegesAttack = queryAttacks.value(2).toString();
    // Description (CWE)
    QString descriptionAttack = queryAttacks.value(3).toString();
    descriptionAttack = descriptionAttack.split('\n').at(0);
    // CVSS exploitability value
    QString exploitabiliyAttack = queryAttacks.value(4).toString();
    // Required privileges
    QString rePrivilegesAttack = queryAttacks.value(5).toString();
    // Violated Security Properties
    QString violatedPropertiesAttack = queryAttacks.value(6).toString();
    violatedPropertiesAttack = violatedPropertiesAttack.split('\n').at(0);
    // Interfaces
    QString interfacesAttack = queryAttacks.value(7).toString();
    // EntryPoint
    QString enrtyPointAttack = queryAttacks.value(8).toString();

    // save each cell of a row in a string list
    row.push_back(idAttack);
    row.push_back(componentAttack);
    row.push_back(acPrivilegesAttack);
    row.push_back(descriptionAttack);
    row.push_back(exploitabiliyAttack);
    row.push_back(rePrivilegesAttack);
    row.push_back(violatedPropertiesAttack);
    row.push_back(interfacesAttack);
    row.push_back(enrtyPointAttack);

    entries.push_back(row);
  }
}

// version for global vulnerability map and ee-elements
void DatabaseHandler::readVulnerabilityFromDataBase(
    std::vector<QStringList> &dataBaseVuls, ComputeUnits compute,
    Connections connect,
    std::unordered_map<unsigned int, Vulnerability> &globalVulMap) {

  std::vector<QRegularExpression> computeMatches;
  std::vector<QRegularExpression> connectionMatches;

  // create vector of regular expressions for text matching (compute units)
  for (auto compute_it : boost::make_iterator_range(compute.m_mapping)) {
    QRegularExpression temp(QString::fromStdString(compute_it.first));
    computeMatches.push_back(temp);
  }

  // create vector of regular expressions for text matching (connections)
  for (auto interface_it : boost::make_iterator_range(connect.m_mapping)) {
    QRegularExpression temp(QString::fromStdString(interface_it.first));
    connectionMatches.push_back(temp);
  }

  // Vioaleted information Assets
  QRegularExpression integrity("Integrity");
  QRegularExpression availability("Availability");
  QRegularExpression confidentiality("Confidentiality");
  QRegularExpression authentication("Authentication");
  QRegularExpression authorization("Authorization");

  // CWE
  QRegularExpression CWE("(CWE-\\d+)");

  int i = 0;
  // iterrate over vulnerabilities in database and extract relevant information
  for (std::vector<QStringList>::iterator itList = dataBaseVuls.begin();
       itList != dataBaseVuls.end(); itList++) {
    if (i >= 412) {
      continue;
    }
    QStringList temp = *itList;

    // Vioalted information assets
    QRegularExpressionMatch matchInteg = integrity.match(temp.at(6));
    QRegularExpressionMatch matchAvail = availability.match(temp.at(6));
    QRegularExpressionMatch matchConfi = confidentiality.match(temp.at(6));
    QRegularExpressionMatch matchAuthen = authentication.match(temp.at(6));
    QRegularExpressionMatch matchAutor = authorization.match(temp.at(6));
    // QRegularExpressionMatch matchCWE = CWE.match(temp.at(8));

    if (temp.size() > 0) {
      QString str = temp.at(0);

      // gerate hash ID from table ID
      Vulnerability vul(str.toUtf8().constData());

      // match compute units
      for (auto computeMatch_it : boost::make_iterator_range(computeMatches)) {
        QRegularExpressionMatch match = computeMatch_it.match(temp.at(1));
        if (match.hasMatch()) {
          vul.m_componentType =
              compute.m_mapping.at(match.captured(0).toUtf8().constData());
          continue;
        }
      }

      // match connections
      for (auto connectingMatch_it :
           boost::make_iterator_range(connectionMatches)) {
        QRegularExpressionMatch match = connectingMatch_it.match(temp.at(7));
        if (match.hasMatch()) {
          vul.m_interfaceType =
              connect.m_mapping.at(match.captured(0).toUtf8().constData());

          continue;
        }
      }
      unsigned short accPrivi = checkPrivileges(temp.at(2));
      unsigned short reqPrivi = checkPrivileges(temp.at(5));

      vul.m_accPriveleges = accPrivi;
      vul.m_reqPriveleges = reqPrivi;

      if ((reqPrivi > 1) && (accPrivi > 1)) {
        // component <-> component
        vul.m_vulClass = 1;
      }
      if ((reqPrivi == 1) && (accPrivi > 1)) {
        // connection -> component
        vul.m_vulClass = 2;
      }
      if ((reqPrivi == 1) && (accPrivi == 1)) {
        // connection -> connection
        vul.m_vulClass = 3;
      }
      if ((reqPrivi > 1) && (accPrivi == 1)) {
        // component <-> connection
        vul.m_vulClass = 4;
      }

      if (matchInteg.hasMatch()) {
        vul.m_integrity = true;
      }

      if (matchAvail.hasMatch()) {
        vul.m_availability = true;
      }

      if (matchConfi.hasMatch()) {
        vul.m_confidentiality = true;
      }

      if (matchAuthen.hasMatch()) {
        vul.m_authentication = true;
      }

      if (matchAutor.hasMatch()) {
        vul.m_authorization = true;
      }

      // put vulnerability to que
      QString tempExpl = temp.at(4);
      vul.m_description = temp.at(3).toLocal8Bit().constData();
      vul.m_exploitVal = tempExpl.replace(",", ".").toFloat();
      globalVulMap.insert(std::make_pair(vul.m_ID, vul));
    }
    i++;
  }
}

/// <summary>Checks the privilege level</summary>
/// <returns>Integer value for the related privilege level</returns> 
unsigned short DatabaseHandler::checkPrivileges(QString privileges) {
  privileges = privileges.toLower();

  if (privileges.contains("read/write (functional communication link)")) {
    return 1;
  } else if (privileges.contains("execute (functional component)")) {
    return 2;
  } else if (privileges.contains("read (functional component)")) {
    return 3;
  } else if (privileges.contains("write (functional component)")) {
    return 4;
  } else if (privileges.contains("full control (functional component)")) {
    return 5;
  }

  return 0;
}


DatabaseHandler::~DatabaseHandler() {
  if (dataBaseOpen)
    m_db.close();
}