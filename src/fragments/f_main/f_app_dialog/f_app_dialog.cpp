#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_app_dialog.hpp"
#include "fragments/f_main/f_app_dialog/f_app_dialog_spec.hpp"
#include "event_system.hpp"
#include "utils.hpp"


//===========================================
// FAppDialog::FAppDialog
//===========================================
FAppDialog::FAppDialog(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FAppDialog", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FAppDialog::FAppDialog\n");

  setLayout(m_data.box);
}

//===========================================
// FAppDialog::reload
//===========================================
void FAppDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FAppDialog::reload\n");

  auto& spec = dynamic_cast<const FAppDialogSpec&>(spec_);

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);

  if (m_eventIdx != -1) {
    commonData.eventSystem.forget(m_eventIdx);
  }

  commonData.eventSystem.listen(spec.showOnEvent, [this](const Event& event) {
    show();
  }, m_eventIdx);
}

//===========================================
// FAppDialog::cleanUp
//===========================================
void FAppDialog::cleanUp() {
  DBG_PRINT("FAppDialog::cleanUp\n");

  commonData.eventSystem.forget(m_eventIdx);
}

//===========================================
// FAppDialog::~FAppDialog
//===========================================
FAppDialog::~FAppDialog() {
  DBG_PRINT("FAppDialog::~FAppDialog\n");
}