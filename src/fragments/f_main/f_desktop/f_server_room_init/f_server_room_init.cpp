#include <QPixmap>
#include "fragments/f_main/f_desktop/f_desktop.hpp"
#include "fragments/f_main/f_desktop/f_server_room_init/f_server_room_init.hpp"
#include "fragments/f_main/f_desktop/f_server_room_init/f_server_room_init_spec.hpp"
#include "utils.hpp"
#include "app_config.hpp"


//===========================================
// FServerRoomInit::FServerRoomInit
//===========================================
FServerRoomInit::FServerRoomInit(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : Fragment("FServerRoomInit", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FServerRoomInit::FServerRoomInit\n");
}

//===========================================
// FServerRoomInit::reload
//===========================================
void FServerRoomInit::reload(const FragmentSpec&) {
  DBG_PRINT("FServerRoomInit::reload\n");

  m_hLaunch = commonData.eventSystem.listen("launchServerRoom", [this](const Event&) {
    auto& parentData = parentFragData<FDesktopData>();

    QPixmap pix(commonData.appConfig.dataPath("youve_got_mail/procalc_dark.png").c_str());
    parentData.icons[0]->wgtButton->setIcon(pix);
  });
}

//===========================================
// FServerRoomInit::cleanUp
//===========================================
void FServerRoomInit::cleanUp() {
  DBG_PRINT("FServerRoomInit::cleanUp\n");
}

//===========================================
// FServerRoomInit::~FServerRoomInit
//===========================================
FServerRoomInit::~FServerRoomInit() {
  DBG_PRINT("FServerRoomInit::~FServerRoomInit\n");

}
