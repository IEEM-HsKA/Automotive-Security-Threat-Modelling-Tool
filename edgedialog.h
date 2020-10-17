#ifndef EDGEDIALOG_H
#define EDGEDIALOG_H
#pragma once

#include <QDialog>
#include <QLabel>
#include <QLineEdit>

class QLineEdit;
class QLabel;

class EdgeDialog : public QDialog {
  Q_OBJECT
public:
  QString m_comboText;
  QStringList m_fields;
  // explicit EdgeDialog(QWidget *parent = nullptr);
  explicit EdgeDialog();

  // static QStringList getStrings(QWidget *parent, bool *ok = nullptr);
  static QStringList getStrings(bool *ok = nullptr);

private:
  QLineEdit *comLineEdit;
  QComboBox *connectionCombo;

private slots:
  void collectData();
};

#endif // EDGEDIALOG_H