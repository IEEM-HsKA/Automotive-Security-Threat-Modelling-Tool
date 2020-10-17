#include "mainwindow.h"
#include "analysegraph\graphmanager.h"

#include "dragwidget.h"
#include "mainctrl.h"
#include "nodectrl.h"
#include "propertyeditor.h"
#include "zodiacgraph/baseedge.h"
#include "zodiacgraph/edgearrow.h"
#include "zodiacgraph/edgelabel.h"
#include "zodiacgraph/node.h"
#include "zodiacgraph/nodelabel.h"
#include "zodiacgraph/perimeter.h"
#include "zodiacgraph/plug.h"
#include "zodiacgraph/pluglabel.h"
#include "zodiacgraph/scene.h"
#include "zodiacgraph/view.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDockWidget>
#include <QHeaderView>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QSplitterHandle>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTableWidget>
#include <QToolBar>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("ASTMT - Automotive Security Threat Modeling Tool");
  setWindowIcon(QIcon(":/icons/zodiac_logo.png"));
  showMaximized();
  saved = false;

  createActions();
  createMenues();

  // create the Property Editor
  PropertyEditor *propertyEditor = new PropertyEditor(this);

  // create the main toolbar
  QToolBar *mainToolBar = new QToolBar(this);
  mainToolBar->setStyleSheet("QToolBar {border: 0px;}");
  mainToolBar->setIconSize(QSize(12, 12));
  mainToolBar->setMovable(false);
  mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  addToolBar(Qt::TopToolBarArea, mainToolBar);

  elementPropertieDock = new QDockWidget(tr("Element Properties"), this);
  stencilsDock = new QDockWidget(tr("Stencils"), this);
  diagramTab = new QTabWidget;
  tabIndex = 1;

  elementPropArea = new QScrollArea;
  // elementPropArea->setBackgroundRole(QPalette::Light);
  elementPropArea->setFixedSize(300, 200);
  elementPropArea->setAutoFillBackground(true);

  stencilsArea = new QScrollArea;
  // stencilsArea->setBackgroundRole(QPalette::Light);
  stencilsArea->setFixedSize(300, 200);

  elementPropertieDock->setAllowedAreas(Qt::LeftDockWidgetArea);
  elementPropertieDock->setBackgroundRole(QPalette::Dark);
  elementPropertieDock->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Expanding);
  elementPropertieDock->setWidget(propertyEditor);

  stencilsDock->setAllowedAreas(Qt::LeftDockWidgetArea);
  stencilsDock->setBackgroundRole(QPalette::Dark);
  stencilsDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  stencilsDock->setWidget(stencilsArea);

  QDockWidget *messageDock = new QDockWidget(tr("Messages"), this);
  messageDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // create the status bar
  statusBar();

  m_messageTable = new QTableWidget(this);
  QStringList messageTableColumns;
  messageTableColumns << "Severity"
                      << "Path";
  m_messageTable->setColumnCount(2);
  m_messageTable->setHorizontalHeaderLabels(messageTableColumns);
  m_messageTable->setWindowTitle("Label");
  messageDock->setBackgroundRole(QPalette::Light);
  m_messageTable->setBackgroundRole(QPalette::Light);
  messageDock->setWidget(m_messageTable);

  // create the Zodiac graph
  zodiac::Scene *zodiacScene = new zodiac::Scene(this);
  zodiac::View *zodiacView = new zodiac::View(this);
  zodiacView->setScene(zodiacScene);

  // create the Main Controller
  m_mainCtrl = new MainCtrl(this, zodiacScene, propertyEditor);

  // setup the main splitter
  m_mainSplitter = new QSplitter(Qt::Horizontal, this);
  m_mainSplitter->setSizes({300, 900});
  subSplitter = new QSplitter(Qt::Vertical);
  subSplitter->setSizes({300, 450});
  messageSplitter = new QSplitter(Qt::Vertical, this);
  messageSplitter->setSizes({900, 200});

  diagramTab->addTab(zodiacView, tr("Diagram 1"));

  subSplitter->addWidget(stencilsDock);
  subSplitter->addWidget(elementPropertieDock);
  messageSplitter->addWidget(diagramTab);
  messageSplitter->addWidget(messageDock);
  m_mainSplitter->addWidget(subSplitter);
  m_mainSplitter->addWidget(messageSplitter);

  // create global actions
  QAction *newNodeAction =
      new QAction(QIcon(":/icons/plus.svg"), tr("&Add Node"), this);
  newNodeAction->setShortcuts(QKeySequence::New);
  newNodeAction->setStatusTip(tr("Create a new Node"));
  mainToolBar->addAction(newNodeAction);
  connect(newNodeAction, SIGNAL(triggered()), m_mainCtrl,
          SLOT(createDefaultNode()));

  QWidget *emptySpacer = new QWidget();
  emptySpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mainToolBar->addWidget(emptySpacer);

  posit = messageDock->pos();
  messageDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                               Qt::RightDockWidgetArea);

  // initialize the GUI
  setCentralWidget(m_mainSplitter);
  readSettings();
  zodiacScene->updateStyle();
  zodiacView->updateStyle();

  messageDock->setBackgroundRole(QPalette::Light);
  m_messageTable->setBackgroundRole(QPalette::Light);


  QSettings windoSet("IEEM", "ASTMT");
  windoSet.beginGroup("WindowLayout");
  windoSet.setValue("MainSplitter", m_mainSplitter->saveState());
  windoSet.setValue("MessageSplitter", messageSplitter->saveState());
  windoSet.setValue("SubSplitter", subSplitter->saveState());
  windoSet.endGroup();

  IconTree *components = new IconTree();
  stencilsDock->setWidget(components->m_tree);
  components->setMainCtrl(m_mainCtrl);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (m_mainCtrl->shutdown()) {
    // if the user accepted, close the application
    writeSettings();
    event->accept();

  } else {
    // if the user objected, do nothing
    event->ignore();
  }
}

void MainWindow::displayAbout() {
  QMessageBox aboutBox;
  aboutBox.setWindowTitle("About the ASTMT Application");
  aboutBox.setText(
      "<h3>About this Application</h3>"
      "This tool can be used to design and analyse threat models for "
      "automotive domain. This can help to identify security threats"
      " in early development stages. The application was developed by Juergen"
	  " Duerrwang (main application) and Florian Sommer (GUI parts)."
      " The application uses <b>ZodiacGraph</b> which was developed by "
      "<a href=\"https://github.com/clemenssielaff/ZodiacGraph\" "
      "title=\"Clements Sielaff\">Clemens Sielaff</a> ."

      "<h3>License</h3>"
	  "The <b>Automotive Threat Modeling Tool</b> is released under the terms of"
	  " <a href=\"https://opensource.org/licenses/MIT\" "
	  "title=\"MIT Licence\">MIT Licence</a> ."

      "<h3>Icons</h3>"
      "Icons from <a href=\"http://www.flaticon.com\" "
      "title=\"Flaticon\">www.flaticon.com</a>, licensed under <a "
      "href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative "
      "Commons BY 3.0\">CC BY 3.0</a><br>"
      "&#8594; Arrows, Plus, Minus & Bucket Icons made by <a "
      "href=\"http://www.freepik.com\" title=\"Freepik\">Freepik</a><br>"
      "&#8594; Door Icons made by <a href=\"http://www.icomoon.io\" "
      "title=\"Icomoon\">Icomoon</a><br>"
      "&#8594; Questionmark Icon made by <a href=\"http://www.danielbruce.se\" "
      "title=\"Daniel Bruce\">Daniel Bruce</a><br>"
      "&#8594; Play Icon made by <a href=\"http://yanlu.de\" "
      "title=\"Yannick\">Yannick</a>");
  aboutBox.exec();
}

void MainWindow::readSettings() {
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     qApp->organizationName(), qApp->applicationName());

  // apply custom palette and style
  QPalette palette;
  settings.beginGroup("palette");
  palette.setColor(QPalette::Window,
                   settings.value("window", "#353535").toString());
  palette.setColor(QPalette::WindowText,
                   settings.value("windowText", "#E0E2E4").toString());
  palette.setColor(QPalette::Base,
                   settings.value("base", "#191919").toString());
  palette.setColor(QPalette::AlternateBase,
                   settings.value("alternateBase", "#353535").toString());
  palette.setColor(QPalette::ToolTipBase,
                   settings.value("toolTipBase", "#000000").toString());
  palette.setColor(QPalette::ToolTipText,
                   settings.value("toolTipText", "#808080").toString());
  palette.setColor(QPalette::Text,
                   settings.value("text", "#E0E2E4").toString());
  palette.setColor(QPalette::Button,
                   settings.value("button", "#353535").toString());
  palette.setColor(QPalette::ButtonText,
                   settings.value("buttonText", "#E0E2E4").toString());
  palette.setColor(QPalette::BrightText,
                   settings.value("brightText", "#ffffff").toString());
  palette.setColor(QPalette::Link,
                   settings.value("link", "#2a82da").toString());
  palette.setColor(QPalette::Highlight,
                   settings.value("highlight", "#2a82da").toString());
  palette.setColor(QPalette::HighlightedText,
                   settings.value("highlightedText", "#000000").toString());
  settings.endGroup();
  qApp->setPalette(palette);
  qApp->setStyle(QStyleFactory::create(
      settings.value("style", QString("Fusion")).toString()));

  // apply window position and size
  settings.beginGroup("mainWindow");
  resize(settings.value("size", QSize(1080, 600)).toSize());
  move(settings.value("pos", QPoint(200, 200)).toPoint());
  m_mainSplitter->restoreState(
      settings.value("propertySplitter").toByteArray());
  settings.endGroup();

  // apply ZodiacGraph settings
  settings.beginGroup("zodiac");

  settings.beginGroup("baseEdge");
  zodiac::BaseEdge::setBaseWidth(settings.value("width", 2.5).toReal());
  zodiac::BaseEdge::setBaseColor(settings.value("color", "#cc5d4e").toString());
  settings.endGroup();

  settings.beginGroup("edgeArrow");
  zodiac::EdgeArrow::defineArrow(settings.value("length", 12).toReal(),
                                 settings.value("width", 15).toReal());
  zodiac::EdgeArrow::setArrowColor(
      settings.value("color", "#cc5d4e").toString());
  settings.endGroup();

  settings.beginGroup("edgeLabel");
  zodiac::EdgeLabel::setFontFamily(
      settings.value("fontFamily", "DejaVu Sans Mono").toString());
  zodiac::EdgeLabel::setPointSize(settings.value("pointSize", 10).toReal());
  zodiac::EdgeLabel::setWeight(
      QFont::Weight(settings.value("weight", 75).toInt()));
  zodiac::EdgeLabel::setColor(settings.value("color", "#c8c8c8").toString());
  zodiac::EdgeLabel::setTransparency(
      settings.value("transparency", 0.7).toReal());
  zodiac::EdgeLabel::setVerticalOffset(
      settings.value("verticalOffset", 0.5).toReal());
  settings.endGroup();

  settings.beginGroup("node");
  zodiac::Node::setCoreRadius(settings.value("coreRadius", 25).toReal());
  zodiac::Node::setIdleColor(settings.value("idleColor", "#4b77a7").toString());
  zodiac::Node::setSelectedColor(
      settings.value("selectedColor", "#62abfa").toString());
  zodiac::Node::setOutlineColor(
      settings.value("outlineColor", "#cdcdcd").toString());
  zodiac::Node::setOutlineWidth(settings.value("outlineWidth", 3).toReal());
  zodiac::Node::setPlugSweep(settings.value("plugSweep", 32.5).toReal());
  zodiac::Node::setGapSweep(settings.value("gapSweep", 8.125).toReal());
  settings.endGroup();

  settings.beginGroup("nodeLabel");
  zodiac::NodeLabel::setTextColor(
      settings.value("textColor", "#ffffff").toString());
  zodiac::NodeLabel::setBackgroundColor(
      settings.value("backgroundColor", "#426998").toString());
  zodiac::NodeLabel::setLineColor(
      settings.value("lineColor", "#cdcdcd").toString());
  zodiac::NodeLabel::setLineWidth(settings.value("lineWidth", 1.5).toReal());
  zodiac::NodeLabel::setCornerRadius(
      settings.value("cornerRadius", 8).toReal());
  zodiac::NodeLabel::setVerticalMargin(
      settings.value("verticalMargin", 2).toReal());
  zodiac::NodeLabel::setHorizontalMargin(
      settings.value("horizontalMargin", 4).toReal());
  zodiac::NodeLabel::setFontFamily(
      settings.value("fontFamily", "DejaVu Sans Mono").toString());
  zodiac::NodeLabel::setPointSize(settings.value("pointSize", 9).toReal());
  zodiac::NodeLabel::setWeight(
      QFont::Weight(settings.value("weight", 63).toInt()));
  settings.endGroup();

  settings.beginGroup("perimeter");
  zodiac::Perimeter::setColor(settings.value("color", "#2b517d").toString());
  settings.endGroup();

  settings.beginGroup("plug");
  zodiac::Plug::setWidth(settings.value("width", 12).toReal());
  zodiac::Plug::setInColor(settings.value("inColor", "#728872").toString());
  zodiac::Plug::setOutColor(settings.value("outColor", "#887272").toString());
  zodiac::Plug::setHighlightColor(
      settings.value("highlightColor", "#d1d7db").toString());
  settings.endGroup();

  settings.beginGroup("plugLabel");
  zodiac::PlugLabel::setFontFamily(
      settings.value("fontFamily", "DejaVu Sans Mono").toString());
  zodiac::PlugLabel::setPointSize(settings.value("pointSize", 10).toReal());
  zodiac::PlugLabel::setWeight(
      QFont::Weight(settings.value("weight", 75).toInt()));
  zodiac::PlugLabel::setColor(settings.value("color", "#828688").toString());
  zodiac::PlugLabel::setLabelDistance(settings.value("distance", 15).toReal());
  settings.endGroup();

  settings.beginGroup("view");
  zodiac::View::setBackgroundColor(
      settings.value("backgroundColor", "#191919").toString());
  settings.endGroup();

  settings.endGroup(); // zodiac
}

void MainWindow::writeSettings() {
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     qApp->organizationName(), qApp->applicationName());

  // write out current palette
  QPalette palette = qApp->palette();
  settings.beginGroup("palette");
  settings.setValue("window", palette.color(QPalette::Window).name());
  settings.setValue("windowText", palette.color(QPalette::WindowText).name());
  settings.setValue("base", palette.color(QPalette::Base).name());
  settings.setValue("alternateBase",
                    palette.color(QPalette::AlternateBase).name());
  settings.setValue("toolTipBase", palette.color(QPalette::ToolTipBase).name());
  settings.setValue("toolTipText", palette.color(QPalette::ToolTipText).name());
  settings.setValue("text", palette.color(QPalette::Text).name());
  settings.setValue("button", palette.color(QPalette::Button).name());
  settings.setValue("buttonText", palette.color(QPalette::ButtonText).name());
  settings.setValue("brightText", palette.color(QPalette::BrightText).name());
  settings.setValue("link", palette.color(QPalette::Link).name());
  settings.setValue("highlight", palette.color(QPalette::Highlight).name());
  settings.setValue("highlightedText",
                    palette.color(QPalette::HighlightedText).name());
  settings.endGroup();

  // write out MainWindow position and size
  settings.beginGroup("mainWindow");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  settings.setValue("propertySplitter", m_mainSplitter->saveState());
  settings.endGroup();

  // write out ZodiacGraph settings
  settings.beginGroup("zodiac");

  settings.beginGroup("baseEdge");
  settings.setValue("width", zodiac::BaseEdge::getBaseWidth());
  settings.setValue("color", zodiac::BaseEdge::getBaseColor().name());
  settings.endGroup();

  settings.beginGroup("edgeArrow");
  settings.setValue("width", zodiac::EdgeArrow::getArrowWidth());
  settings.setValue("length", zodiac::EdgeArrow::getArrowLength());
  settings.setValue("color", zodiac::EdgeArrow::getArrowColor().name());
  settings.endGroup();

  settings.beginGroup("edgeLabel");
  settings.setValue("fontFamily", zodiac::EdgeLabel::getFontFamily());
  settings.setValue("pointSize", zodiac::EdgeLabel::getPointSize());
  settings.setValue("weight", zodiac::EdgeLabel::getWeight());
  settings.setValue("color", zodiac::EdgeLabel::getColor().name());
  settings.setValue("transparency", zodiac::EdgeLabel::getTransparency());
  settings.setValue("verticalOffset", zodiac::EdgeLabel::getVerticalOffset());
  settings.endGroup();

  settings.beginGroup("node");
  settings.setValue("coreRadius", zodiac::Node::getCoreRadius());
  settings.setValue("idleColor", zodiac::Node::getIdleColor().name());
  settings.setValue("selectedColor", zodiac::Node::getSelectedColor().name());
  settings.setValue("outlineColor", zodiac::Node::getOutlineColor().name());
  settings.setValue("outlineWidth", zodiac::Node::getOutlineWidth());
  settings.setValue("plugSweep", zodiac::Node::getPlugSweep());
  settings.setValue("gapSweep", zodiac::Node::getGapSweep());
  settings.endGroup();

  settings.beginGroup("nodeLabel");
  settings.setValue("textColor", zodiac::NodeLabel::getTextColor().name());
  settings.setValue("backgroundColor",
                    zodiac::NodeLabel::getBackgroundColor().name());
  settings.setValue("lineColor", zodiac::NodeLabel::getLineColor().name());
  settings.setValue("lineWidth", zodiac::NodeLabel::getLineWidth());
  settings.setValue("cornerRadius", zodiac::NodeLabel::getCornerRadius());
  settings.setValue("verticalMargin", zodiac::NodeLabel::getVerticalMargin());
  settings.setValue("horizontalMargin",
                    zodiac::NodeLabel::getHorizontalMargin());
  settings.setValue("fontFamily", zodiac::NodeLabel::getFontFamily());
  settings.setValue("pointSize", zodiac::NodeLabel::getPointSize());
  settings.setValue("weight", zodiac::NodeLabel::getWeight());
  settings.endGroup();

  settings.beginGroup("perimeter");
  settings.setValue("color", zodiac::Perimeter::getColor().name());
  settings.endGroup();

  settings.beginGroup("plug");
  settings.setValue("width", zodiac::Plug::getWidth());
  settings.setValue("inColor", zodiac::Plug::getInColor().name());
  settings.setValue("outColor", zodiac::Plug::getOutColor().name());
  settings.setValue("highlightColor", zodiac::Plug::getHighlightColor().name());
  settings.endGroup();

  settings.beginGroup("plugLabel");
  settings.setValue("fontFamily", zodiac::PlugLabel::getFontFamily());
  settings.setValue("pointSize", zodiac::PlugLabel::getPointSize());
  settings.setValue("weight", zodiac::PlugLabel::getWeight());
  settings.setValue("color", zodiac::PlugLabel::getColor().name());
  settings.setValue("distance", zodiac::PlugLabel::getLabelDistance());
  settings.endGroup();

  settings.beginGroup("view");
  settings.setValue("backgroundColor",
                    zodiac::View::getBackgroundColor().name());
  settings.endGroup();

  settings.endGroup(); // zodiac
}


void MainWindow::createMenues() {

  fileMenu = menuBar()->addMenu(tr("&File"));
  // editMenu = menuBar()->addMenu(tr("&Edit"));
  // viewMenu = menuBar()->addMenu(tr("&View"));
  diagramMenu = menuBar()->addMenu(tr("&Diagram"));
  SGMMenu = menuBar()->addMenu(tr("&SGM-Analysis"));
  reportsMenu = menuBar()->addMenu(tr("&Report"));
  helpMenu = menuBar()->addMenu(tr("&Help"));

  // fileMenu->addAction(newAct);
  // fileMenu->addAction(openAct);
  // fileMenu->addAction(saveAct);
  // fileMenu->addAction(saveAsAct);
  // fileMenu->addAction(applyTemplateAct);
  // fileMenu->addAction(threatModelInformationAct);
  // fileMenu->addAction(closeModelAct);
  fileMenu->addAction(printGraphAct);
  fileMenu->addAction(exitAct);
  // editMenu->addAction(undoAct);
  // editMenu->addAction(redoAct);
  // editMenu->addAction(copyAct);
  // editMenu->addAction(cutAct);
  // editMenu->addAction(pasteAct);
  // viewMenu->addAction(designViewAct);
  // viewMenu->addAction(analysisViewAct);
  // viewMenu->addAction(zoomInAct);
  // viewMenu->addAction(zoomOutAct);
  // viewMenu->addAction(stencilsAct);
  // viewMenu->addAction(messageAct);
  // viewMenu->addAction(noteAct);
  // viewMenu->addAction(threatListAct);
  // viewMenu->addAction(threatPropertiesAct);
  // viewMenu->addAction(elementPropertiesAct);
  diagramMenu->addAction(newDiagramAct);
  //diagramMenu->addAction(deleteDiagramAct);
  SGMMenu->addAction(newSGMTableAct);
  reportsMenu->addAction(createFullReportAct);
  //helpMenu->addAction(gettingStartedAct);
  helpMenu->addAction(aboutAct);
}

void MainWindow::createActions() {
  newAct = new QAction(tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

  openAct = new QAction(tr("&Open"), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open a file"));
  connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

  saveAct = new QAction(tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save a file"));
  connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

  saveAsAct = new QAction(tr("&Save As..."), this);
  saveAsAct->setShortcuts(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Save a file as ..."));
  connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveFileAs);

  applyTemplateAct = new QAction(tr("&Apply Template..."), this);
  applyTemplateAct->setStatusTip(tr("Apply a template to the model"));
  connect(applyTemplateAct, &QAction::triggered, this,
          &MainWindow::applyTemplate);

  threatModelInformationAct =
      new QAction(tr("&Threat Model Information..."), this);
  threatModelInformationAct->setStatusTip(
      tr("Display information about the threat model"));
  connect(threatModelInformationAct, &QAction::triggered, this,
          &MainWindow::threatModelInformation);

  closeModelAct = new QAction(tr("&Close Model"), this);
  closeModelAct->setShortcuts(QKeySequence::Close);
  closeModelAct->setStatusTip(tr("Close a file"));
  connect(closeModelAct, &QAction::triggered, this, &MainWindow::closeModel);

  printGraphAct = new QAction(tr("&Print Graph"), this);
  printGraphAct->setStatusTip("Print graph to PDF");
  connect(printGraphAct, &QAction::triggered, this, &MainWindow::printGraph);

  exitAct = new QAction(tr("&Exit"), this);
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, &QAction::triggered, this, &MainWindow::exitSlot);

  undoAct = new QAction(tr("&Undo"), this);
  undoAct->setShortcuts(QKeySequence::Undo);
  undoAct->setStatusTip(tr("Undo an action"));
  connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

  redoAct = new QAction(tr("&Redo"), this);
  redoAct->setShortcuts(QKeySequence::Redo);
  redoAct->setStatusTip(tr("Redo an action"));
  connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

  copyAct = new QAction(tr("&Copy"), this);
  copyAct->setShortcuts(QKeySequence::Copy);
  copyAct->setStatusTip(tr("Copy an element"));
  connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

  cutAct = new QAction(tr("&Cut"), this);
  cutAct->setShortcuts(QKeySequence::Cut);
  cutAct->setStatusTip(tr("Cut an element"));
  connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

  pasteAct = new QAction(tr("&Paste"), this);
  pasteAct->setShortcuts(QKeySequence::Paste);
  pasteAct->setStatusTip(tr("Paste an element"));
  connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

  designViewAct = new QAction(tr("&Design View"), this);
  designViewAct->setStatusTip(tr("Switch to the design view"));
  connect(designViewAct, &QAction::triggered, this, &MainWindow::designView);

  analysisViewAct = new QAction(tr("&Analysis View"), this);
  analysisViewAct->setStatusTip(tr("Switch to the analysis view"));
  connect(analysisViewAct, &QAction::triggered, this,
          &MainWindow::analysisView);

  zoomInAct = new QAction(tr("&Zoom In"), this);
  zoomInAct->setShortcuts(QKeySequence::ZoomIn);
  zoomInAct->setStatusTip(tr("Zoom into the view"));
  connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);

  zoomOutAct = new QAction(tr("&Zoom Out"), this);
  zoomOutAct->setShortcuts(QKeySequence::ZoomOut);
  zoomOutAct->setStatusTip(tr("Zoom out of the view"));
  connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);

  stencilsAct = new QAction(tr("&Stencils"), this);
  stencilsAct->setStatusTip(tr("Show the stencils"));
  connect(stencilsAct, &QAction::triggered, this, &MainWindow::stencils);

  messageAct = new QAction(tr("&Message"), this);
  messageAct->setStatusTip(tr("Show the messages"));
  connect(messageAct, &QAction::triggered, this, &MainWindow::message);

  noteAct = new QAction(tr("&Note"), this);
  noteAct->setStatusTip(tr("Show notes"));
  connect(noteAct, &QAction::triggered, this, &MainWindow::note);

  threatListAct = new QAction(tr("&Threat List"), this);
  threatListAct->setStatusTip(tr("Show the threat list"));
  connect(threatListAct, &QAction::triggered, this, &MainWindow::threatList);

  threatPropertiesAct = new QAction(tr("&Threat Properties"), this);
  threatPropertiesAct->setStatusTip(tr("Show the threat properties"));
  connect(threatPropertiesAct, &QAction::triggered, this,
          &MainWindow::threatProperties);

  elementPropertiesAct = new QAction(tr("&Element Properties"), this);
  elementPropertiesAct->setStatusTip(tr("Show the element properties"));
  connect(elementPropertiesAct, &QAction::triggered, this,
          &MainWindow::elementProperties);

  newDiagramAct = new QAction(tr("&New Diagram"), this);
  newDiagramAct->setStatusTip(tr("Create a new threat model diagram"));
  connect(newDiagramAct, &QAction::triggered, this, &MainWindow::newDiagram);

  deleteDiagramAct = new QAction(tr("&Delete Diagram"), this);
  deleteDiagramAct->setStatusTip(tr("Delete the current threat model diagram"));
  connect(deleteDiagramAct, &QAction::triggered, this,
          &MainWindow::deleteDiagram);

  newSGMTableAct = new QAction(tr("&SGM Table"), this);
  newSGMTableAct->setStatusTip(tr("Create a new SGM table"));
  connect(newSGMTableAct, &QAction::triggered, this, &MainWindow::newSGMTable);

  createFullReportAct = new QAction(tr("&Create Full Report"), this);
  createFullReportAct->setStatusTip(tr("Create a full report"));
  connect(createFullReportAct, &QAction::triggered, this,
          &MainWindow::createFullReport);

  gettingStartedAct = new QAction(tr("&Getting Started Guide"), this);
  gettingStartedAct->setStatusTip(tr("Call the Getting Started Guide"));
  connect(gettingStartedAct, &QAction::triggered, this,
          &MainWindow::gettingStarted);

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application information"));
  connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::newFile() { qDebug("New"); }

void MainWindow::openFile() { qDebug("Open file"); }

void MainWindow::saveFile() { qDebug("Save file"); }

void MainWindow::saveFileAs() { qDebug("Save file as"); }

void MainWindow::applyTemplate() { qDebug("Apply template"); }

void MainWindow::threatModelInformation() {
  qDebug("Threat model information");
}

void MainWindow::closeModel() {
  int indexCount = diagramTab->count();
  if (saved == false) {
    QMessageBox messageBox;

    messageBox.setWindowTitle("Close model");
    messageBox.setText("Do want to save the model?");
    messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                                  QMessageBox::Cancel);
    messageBox.setDefaultButton(QMessageBox::Cancel);

    int ret = messageBox.exec();

    if (ret == QMessageBox::Save) {
      saveFile();
    } else if (ret == QMessageBox::Discard) {
      for (int i = 0; i < indexCount; i++) {
        diagramTab->removeTab(diagramTab->currentIndex());
      }
    }

  } else {
    for (int i = 0; i < indexCount; i++) {
      diagramTab->removeTab(diagramTab->currentIndex());
    }
  }
}

void MainWindow::printGraph() { m_mainCtrl->printZodiacScene(); }

void MainWindow::exitSlot() {
  qDebug("Exit");
  QApplication::quit();
}

void MainWindow::undo() { qDebug("Undo"); }

void MainWindow::redo() { qDebug("Redo"); }

void MainWindow::copy() { qDebug("Copy"); }

void MainWindow::cut() { qDebug("Cut"); }

void MainWindow::paste() { qDebug("Paste"); }

void MainWindow::designView() { qDebug("Design view"); }

void MainWindow::analysisView() { qDebug("Analysis view"); }

void MainWindow::zoomIn() { qDebug("Zoom in"); }

void MainWindow::zoomOut() { qDebug("Zoom out"); }

void MainWindow::stencils() { qDebug("Stencils"); }

void MainWindow::message() { qDebug("Message"); }

void MainWindow::note() { qDebug("Note"); }

void MainWindow::threatList() { qDebug("Threat list"); }

void MainWindow::threatProperties() { qDebug("Threat Properties"); }

void MainWindow::elementProperties() { qDebug("Element properties"); }

void MainWindow::newDiagram() {
  tabIndex++;
  QString tabName = "Diagram " + QString::number(tabIndex);
  diagramTab->addTab(new QWidget(), tabName);
}

void MainWindow::deleteDiagram() {
  QMessageBox messageBox;

  messageBox.setWindowTitle("Delete Diagram");
  messageBox.setText("Are you sure you want to delete the current diagram?");
  messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  messageBox.setDefaultButton(QMessageBox::Cancel);

  int ret = messageBox.exec();

  if (ret == QMessageBox::Yes) {
    diagramTab->removeTab(diagramTab->currentIndex());
  }
}

void MainWindow::newSGMTable() {
  static bool alreadyOpen = false;

  if (!alreadyOpen) {
    alreadyOpen = true;
    m_sgmTable = new SGMTable(
        *m_mainCtrl->getSceneHandle()->getScene()->getBGLgraph(), this);
    connect(m_sgmTable, SIGNAL(astmtResultsReady()), this,
            SLOT(receiveAstmtResults()));
    QString tabName = "SGM-Table";
    diagramTab->addTab(m_sgmTable, tabName);
    alreadyOpen = false;
  }
}

void MainWindow::showASTMTReport() { qDebug("ASTMT results are available"); }

void MainWindow::createFullReport() {
  qDebug("Create full report");
  QSettings windoSet("IEEM_Bruchsal", "ASTMT");
  windoSet.beginGroup("WindowLayout");
  m_mainSplitter->restoreState(windoSet.value("MainSplitter").toByteArray());
  subSplitter->restoreState(windoSet.value("SubSplitter").toByteArray());
  messageSplitter->restoreState(
      windoSet.value("MessageSplitter").toByteArray());
  windoSet.endGroup();
  ;
}

void MainWindow::gettingStarted() { qDebug("Getting started"); }

void MainWindow::about() {
  qDebug("About");
  displayAbout();
}

void MainWindow::receiveAstmtResults() {
  unsigned int rowCounter = m_messageTable->rowCount();

  for (auto it = std::begin(m_sgmTable->m_result);
       it != std::end(m_sgmTable->m_result); ++it) {
    m_messageTable->insertRow(rowCounter);
    m_messageTable->setItem(
        rowCounter, 0, new QTableWidgetItem(QString::fromStdString(it->first)));
    m_messageTable->setItem(
        rowCounter, 1,
        new QTableWidgetItem(QString::fromStdString(it->second)));
    rowCounter++;
  }
  m_messageTable->resizeColumnsToContents();
}
