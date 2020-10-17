#include "edgedialog.h"
#include "architectureElements.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QtWidgets>
#include <iostream>
#include <unordered_map>

EdgeDialog::EdgeDialog() : QDialog() {
  QStringList conElements;
  QLabel *comName;
  QLabel *comType;
  QStringListModel *connections;
  Connections connectionsElements;

  QVBoxLayout *upperLayout = new QVBoxLayout;
  QVBoxLayout *downLayout = new QVBoxLayout;
  QVBoxLayout *dialogLayout = new QVBoxLayout;
  QHBoxLayout *mainLayout = new QHBoxLayout;

  comName = new QLabel(QString("Communication Name:"), this);
  comLineEdit = new QLineEdit(this);

  connections = new QStringListModel();

  for (auto it = connectionsElements.m_mapping.begin();
       it != connectionsElements.m_mapping.end(); ++it) {
    QString qstr = QString::fromStdString(it->first);
    conElements << qstr;
  }

  connections->setStringList(conElements);

  comType = new QLabel(QString("Communication type:"), this);
  connectionCombo = new QComboBox();
  connectionCombo->setModel(connections);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

  connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this,
          SLOT(collectData()));
  connect(buttonBox, &QDialogButtonBox::accepted, this, &EdgeDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &EdgeDialog::reject);

  upperLayout->addWidget(comName);
  upperLayout->addWidget(comType);

  downLayout->addWidget(comLineEdit);
  downLayout->addWidget(connectionCombo);
  mainLayout->addLayout(upperLayout);
  mainLayout->addLayout(downLayout);
  dialogLayout->addLayout(mainLayout);
  dialogLayout->addSpacing(20);
  dialogLayout->addWidget(buttonBox);

  setLayout(dialogLayout);
}

void EdgeDialog::collectData() {
  m_fields.push_back(comLineEdit->text());
  m_fields.push_back(connectionCombo->currentText());
}

QStringList EdgeDialog::getStrings(bool *ok) {

  EdgeDialog *dialog = new EdgeDialog();

  // block other windows
  // dialog->setWindowFlags(Qt::ApplicationModal);
  QStringList list;

  const int ret = dialog->exec();

  if (ok)
    *ok = !!ret;

  if (ret) {
    list = dialog->m_fields;
  }

  dialog->deleteLater();

  return list;
}