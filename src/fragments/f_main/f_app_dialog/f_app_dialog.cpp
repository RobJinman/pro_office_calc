#include <QKeyEvent>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_app_dialog/f_app_dialog.hpp"
#include "fragments/f_main/f_app_dialog/f_app_dialog_spec.hpp"
#include "utils.hpp"


//===========================================
// FAppDialog::FAppDialog
//===========================================
FAppDialog::FAppDialog(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FAppDialog", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FAppDialog::FAppDialog\n");

  setLayout(m_data.box.get());
}

//===========================================
// FAppDialog::reload
//===========================================
void FAppDialog::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FAppDialog::reload\n");

  auto& spec = dynamic_cast<const FAppDialogSpec&>(spec_);

  m_name = spec.name;

  setWindowTitle(spec.titleText);
  setFixedSize(spec.width, spec.height);

  hide();

  m_hShow = commonData.eventSystem.listen(spec.showOnEvent, [this](const Event&) {
    show();
  });
}

//===========================================
// FAppDialog::keyPressEvent
//===========================================
void FAppDialog::keyPressEvent(QKeyEvent* e) {
  if (e->key() == Qt::Key_Escape) {}
}

//===========================================
// FAppDialog::closeEvent
//===========================================
void FAppDialog::closeEvent(QCloseEvent*) {
  commonData.eventSystem.fire(pEvent_t(new DialogClosedEvent(m_name)));
}

//===========================================
// FAppDialog::cleanUp
//===========================================
void FAppDialog::cleanUp() {
  DBG_PRINT("FAppDialog::cleanUp\n");
}

//===========================================
// FAppDialog::~FAppDialog
//===========================================
FAppDialog::~FAppDialog() {
  DBG_PRINT("FAppDialog::~FAppDialog\n");
}
