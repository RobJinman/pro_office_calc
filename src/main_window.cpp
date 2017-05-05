#include <sstream>
#include <QMenuBar>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "utils.hpp"
#include "main_window.hpp"
#include "types.hpp"
#include "app_config.hpp"
#include "app_state.hpp"


//===========================================
// numberToWord
//===========================================
static QString numberToWord(int n) {
  switch (n) {
    case 1: return "one";
    case 2: return "two";
    case 3: return "three";
    case 4: return "four";
    case 5: return "five";
    case 6: return "six";
    case 7: return "seven";
    case 8: return "eight";
    case 9: return "nine";
    case 10: return "ten";
    default: return "?";
  }
}

//===========================================
// MainWindow::MainWindow
//===========================================
MainWindow::MainWindow(const AppConfig& appConfig, AppState& appState)
  : QMainWindow(nullptr),
    m_appConfig(appConfig),
    m_appState(appState),
    m_mnuFile(nullptr),
    m_mnuHelp(nullptr),
    m_actAbout(nullptr),
    m_actQuit(nullptr),
    m_wgtCentral(nullptr) {

  resize(600, 300);

  setWindowTitle("Pro Office Calculator");

  m_actQuit.reset(new QAction("Quit", this));

  m_mnuFile.reset(menuBar()->addMenu("File"));
  m_mnuFile->addAction(m_actQuit.get());

  m_actAbout.reset(new QAction("About", this));
  m_mnuHelp.reset(menuBar()->addMenu("Help"));
  m_mnuHelp->addAction(m_actAbout.get());

  m_wgtCentral.reset(new QWidget(this));
  setCentralWidget(m_wgtCentral.get());

  QHBoxLayout* hbox = new QHBoxLayout;
  m_wgtCentral->setLayout(hbox);

  connect(m_actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));
  connect(m_actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));
}

//===========================================
// MainWindow::closeEvent
//===========================================
void MainWindow::closeEvent(QCloseEvent*) {
  DBG_PRINT("Quitting\n");

  m_appState.count--;
  m_appState.persist(m_appConfig);
}

//===========================================
// MainWindow::showAbout
//===========================================
void MainWindow::showAbout() {
   QString msg;
   msg = msg + "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
     "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
     "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
     "<i>" + numberToWord(m_appState.count) + "</i>";

  QMessageBox msgBox(this);
  msgBox.setWindowTitle("About");
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setText(msg);
  msgBox.exec();
}

//===========================================
// MainWindow::~MainWindow
//===========================================
MainWindow::~MainWindow() {}
