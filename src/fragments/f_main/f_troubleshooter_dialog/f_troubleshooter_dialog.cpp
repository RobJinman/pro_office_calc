#include <cassert>
#include <QMessageBox>
#include <QMenuBar>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/f_troubleshooter_dialog.hpp"
#include "fragments/f_main/f_troubleshooter_dialog/f_troubleshooter_dialog_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"
#include "app_config.hpp"


using its_raining_tetrominos::GameLogic;


//===========================================
// FTroubleshooterDialog::FTroubleshooterDialog
//===========================================
FTroubleshooterDialog::FTroubleshooterDialog(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FTroubleshooterDialog", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FTroubleshooterDialog::FTroubleshooterDialog\n");
}

//===========================================
// FTroubleshooterDialog::reload
//===========================================
void FTroubleshooterDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FTroubleshooterDialog::reload\n");

  auto& spec = dynamic_cast<const FTroubleshooterDialogSpec&>(spec_);
  auto& parentData = parentFragData<FMainData>();

  m_data.vbox = makeQtObjPtr<QVBoxLayout>();
  setLayout(m_data.vbox.get());

  m_data.actPreferences = makeQtObjPtr<QAction>("Troubleshooter", this);
  parentData.mnuHelp->addAction(m_data.actPreferences.get());

  m_data.wgtTabs = makeQtObjPtr<QTabWidget>(this);
  m_data.vbox->addWidget(m_data.wgtTabs.get());

  setupTab1(spec.raycastWidth, spec.raycastHeight);
  setupTab2();
  setupTab3(spec.raycastWidth, spec.raycastHeight);

  m_data.wgtTabs->addTab(m_data.tab1.page.get(), "Fix automatically");

  connect(m_data.actPreferences.get(), SIGNAL(triggered()), this, SLOT(showTroubleshooterDialog()));

  setWindowTitle(spec.titleText);
}

//===========================================
// FTroubleshooterDialog::setupTab1
//===========================================
void FTroubleshooterDialog::setupTab1(int raycastWidth, int raycastHeight) {
  auto& tab = m_data.tab1;

  auto margins = contentsMargins();
  int paddingW = margins.left() + margins.right();
  int paddingH = margins.top() + margins.bottom();

  tab.page = makeQtObjPtr<QWidget>();
  tab.vbox = makeQtObjPtr<QVBoxLayout>();
  tab.page->setLayout(tab.vbox.get());
  tab.page->setFixedSize(raycastWidth - paddingW, raycastHeight - paddingH);
  tab.wgtCaption = makeQtObjPtr<QLabel>("Attempt to fix problem automatically");
  tab.wgtRunTroubleshooter = makeQtObjPtr<QPushButton>("Run troubleshooter");
  tab.wgtProgressBar = makeQtObjPtr<QProgressBar>();
  tab.wgtGroupbox = makeQtObjPtr<QGroupBox>("Result");
  tab.resultsVbox = makeQtObjPtr<QVBoxLayout>();
  tab.btnsHbox = makeQtObjPtr<QHBoxLayout>();
  tab.wgtNoProblemsFound = makeQtObjPtr<QLabel>("> No problems found");
  tab.wgtProblemResolved = makeQtObjPtr<QLabel>("Has this resolved the problem?");
  tab.wgtYes = makeQtObjPtr<QPushButton>("Yes");
  tab.wgtNo = makeQtObjPtr<QPushButton>("No");

  tab.btnsHbox->addWidget(tab.wgtYes.get());
  tab.btnsHbox->addWidget(tab.wgtNo.get());

  tab.resultsVbox->addWidget(tab.wgtNoProblemsFound.get());
  tab.resultsVbox->addWidget(tab.wgtProblemResolved.get());
  tab.resultsVbox->addLayout(tab.btnsHbox.get());

  tab.timer = makeQtObjPtr<QTimer>();

  tab.wgtGroupbox->setLayout(tab.resultsVbox.get());
  tab.wgtGroupbox->setVisible(false);

  auto sp = tab.wgtGroupbox->sizePolicy();
  sp.setRetainSizeWhenHidden(true);
  tab.wgtGroupbox->setSizePolicy(sp);

  tab.wgtProgressBar->setVisible(false);
  tab.wgtProgressBar->setRange(0, 10);

  tab.vbox->addWidget(tab.wgtCaption.get());
  tab.vbox->addWidget(tab.wgtRunTroubleshooter.get());
  tab.vbox->addWidget(tab.wgtProgressBar.get());
  tab.vbox->addWidget(tab.wgtGroupbox.get(), 1);

  connect(tab.wgtRunTroubleshooter.get(), SIGNAL(clicked()), this, SLOT(onRunTroubleshooter()));
  connect(tab.timer.get(), SIGNAL(timeout()), this, SLOT(onTick()));
  connect(tab.wgtNo.get(), SIGNAL(clicked()), this, SLOT(onNoClick()));
  connect(tab.wgtYes.get(), SIGNAL(clicked()), this, SLOT(onYesClick()));
}

//===========================================
// FTroubleshooterDialog::setupTab2
//===========================================
void FTroubleshooterDialog::setupTab2() {
  auto& tab = m_data.tab2;

  tab.page = makeQtObjPtr<QWidget>();
  tab.vbox = makeQtObjPtr<QVBoxLayout>();
  tab.wgtTextBrowser = makeQtObjPtr<QTextBrowser>();

  tab.vbox->addWidget(tab.wgtTextBrowser.get());

  tab.wgtTextBrowser->setSearchPaths(QStringList()
    << commonData.appConfig.dataPath("its_raining_tetrominos").c_str());
  tab.wgtTextBrowser->setSource(QUrl("troubleshooter1.html"));

  tab.page->setLayout(tab.vbox.get());
}

//===========================================
// FTroubleshooterDialog::setupTab3
//===========================================
void FTroubleshooterDialog::setupTab3(int raycastWidth, int raycastHeight) {
  auto& tab = m_data.tab3;

  tab.page = makeQtObjPtr<QWidget>();
  tab.page->setContentsMargins(0, 0, 0, 0);
  tab.vbox = makeQtObjPtr<QVBoxLayout>();
  tab.vbox->setSpacing(0);
  tab.vbox->setContentsMargins(0, 0, 0, 0);
  tab.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.appConfig, commonData.eventSystem,
    raycastWidth, raycastHeight);
  tab.wgtRaycast->initialise(commonData.appConfig.dataPath("its_raining_tetrominos/map.svg"));
  tab.gameLogic.reset(new GameLogic(commonData.eventSystem, tab.wgtRaycast->entityManager()));

  tab.wgtRaycast->start();

  tab.vbox->addWidget(tab.wgtRaycast.get());
  tab.page->setLayout(tab.vbox.get());
}

//===========================================
// FTroubleshooterDialog::onNoClick
//===========================================
void FTroubleshooterDialog::onNoClick() {
  assert(m_data.wgtTabs->count() == 2);

  m_data.tab1.wgtGroupbox->setVisible(false);

  m_data.wgtTabs->insertTab(1, m_data.tab2.page.get(), "Common problems");
  m_data.wgtTabs->setCurrentIndex(1);
  m_data.tab2.wgtTextBrowser->setSource(QUrl("troubleshooter1.html"));
}

//===========================================
// FTroubleshooterDialog::onYesClick
//===========================================
void FTroubleshooterDialog::onYesClick() {
  auto& tab = m_data.tab1;

  tab.wgtGroupbox->setVisible(false);
}

//===========================================
// FTroubleshooterDialog::onTick
//===========================================
void FTroubleshooterDialog::onTick() {
  auto& tab = m_data.tab1;

  if (tab.wgtProgressBar->value() == 10) {
    tab.timer->stop();

    tab.wgtProgressBar->setVisible(false);
    tab.wgtGroupbox->setVisible(true);
    tab.wgtRunTroubleshooter->setDisabled(false);
    m_data.wgtTabs->addTab(m_data.tab3.page.get(), "⚙");

    commonData.eventSystem.fire(pEvent_t(new Event("increaseTetrominoRain")));
  }
  else {
    tab.wgtProgressBar->setValue(tab.wgtProgressBar->value() + 1);
  }
}

//===========================================
// FTroubleshooterDialog::onRunTroubleshooter
//===========================================
void FTroubleshooterDialog::onRunTroubleshooter() {
  auto& tab = m_data.tab1;

  m_data.wgtTabs->removeTab(2);
  m_data.wgtTabs->removeTab(1);

  tab.wgtProgressBar->setVisible(true);
  tab.wgtGroupbox->setVisible(false);
  tab.wgtRunTroubleshooter->setDisabled(true);

  tab.wgtProgressBar->setValue(0);
  tab.timer->start(300);
}

//===========================================
// FTroubleshooterDialog::cleanUp
//===========================================
void FTroubleshooterDialog::cleanUp() {
  DBG_PRINT("FTroubleshooterDialog::cleanUp\n");

  auto& parentData = parentFragData<FMainData>();

  parentData.mnuHelp->removeAction(m_data.actPreferences.get());
}

//===========================================
// FTroubleshooterDialog::showTroubleshooterDialog
//===========================================
void FTroubleshooterDialog::showTroubleshooterDialog() {
  show();
}

//===========================================
// FTroubleshooterDialog::~FTroubleshooterDialog
//===========================================
FTroubleshooterDialog::~FTroubleshooterDialog() {
  DBG_PRINT("FTroubleshooterDialog::~FTroubleshooterDialog\n");
}
