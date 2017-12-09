#include <QMouseEvent>
#include "fragments/f_main/f_preferences_dialog/f_preferences_dialog.hpp"
#include "fragments/f_main/f_preferences_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_preferences_dialog/f_config_maze/f_config_maze_spec.hpp"
#include "utils.hpp"


//===========================================
// FConfigMaze::FConfigMaze
//===========================================
FConfigMaze::FConfigMaze(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FConfigMaze", parent_, parentData_, m_data) {}

//===========================================
// FConfigMaze::rebuild
//===========================================
void FConfigMaze::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FPreferencesDialog>();
  auto& parentData = parentFragData<FPreferencesDialogData>();

  m_data.eventSystem = parentData.eventSystem;
  m_data.updateLoop = parentData.updateLoop;

  parentData.vbox->addWidget(this);

  setMouseTracking(true);

  auto& spec = dynamic_cast<const FConfigMazeSpec&>(spec_);

  m_data.wgtButton.reset(new EvasiveButton("Click me!", QPoint(200, 200), this));

  connect(m_data.wgtButton.get(), SIGNAL(pressed()), this, SLOT(onBtnClick()));

  Fragment::rebuild(spec_);
}

//===========================================
// FConfigMaze::onBtnClick
//===========================================
void FConfigMaze::onBtnClick() {
  DBG_PRINT("Button clicked!\n");
}

//===========================================
// FConfigMaze::mouseMoveEvent
//===========================================
void FConfigMaze::mouseMoveEvent(QMouseEvent*) {
  m_data.wgtButton->onMouseMove();
}

//===========================================
// FConfigMaze::cleanUp
//===========================================
void FConfigMaze::cleanUp() {
  auto& parentData = parentFragData<FPreferencesDialogData>();

  parentData.vbox->removeWidget(this);
}
