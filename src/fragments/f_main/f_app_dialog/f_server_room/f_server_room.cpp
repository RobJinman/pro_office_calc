#include "fragments/relocatable/widget_frag_data.hpp"
#include "fragments/f_main/f_app_dialog/f_server_room/f_server_room.hpp"
#include "fragments/f_main/f_app_dialog/f_server_room/f_server_room_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_server_room/object_factory.hpp"
#include "raycast/render_system.hpp"
#include "utils.hpp"
#include "app_config.hpp"


//===========================================
// FServerRoom::FServerRoom
//===========================================
FServerRoom::FServerRoom(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FServerRoom", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FServerRoom::FServerRoom\n");
}

//===========================================
// FServerRoom::reload
//===========================================
void FServerRoom::reload(const FragmentSpec&) {
  DBG_PRINT("FServerRoom::reload\n");

  m_hLaunch = commonData.eventSystem.listen("launchServerRoom", [this](const Event&) {
    auto& parentData = parentFragData<WidgetFragData>();

    m_data.vbox = makeQtObjPtr<QVBoxLayout>();
    m_data.vbox->setSpacing(0);
    m_data.vbox->setContentsMargins(0, 0, 0, 0);

    setLayout(m_data.vbox.get());

    m_origParentState.spacing = parentData.box->spacing();
    m_origParentState.margins = parentData.box->contentsMargins();

    for (int i = 0; i < parentData.box->count(); ++i) {
      QWidget* wgt = parentData.box->itemAt(i)->widget();
      if (wgt) {
        wgt->hide();
      }
    }

    parentData.box->setSpacing(0);
    parentData.box->setContentsMargins(0, 0, 0, 0);
    parentData.box->addWidget(this);

    m_data.wgtRaycast = makeQtObjPtr<RaycastWidget>(commonData.appConfig, commonData.eventSystem);

    auto& rootFactory = m_data.wgtRaycast->rootFactory();
    auto& timeService = m_data.wgtRaycast->timeService();
    auto& audioService = m_data.wgtRaycast->audioService();
    auto& entityManager = m_data.wgtRaycast->entityManager();

    m_data.wgtCalculator = makeQtObjPtr<CalculatorWidget>(commonData.eventSystem);
    m_data.wgtCalculator->show();
    m_data.wgtCalculator->hide();

    GameObjectFactory* factory = new youve_got_mail::ObjectFactory(rootFactory, entityManager,
      timeService, audioService, *m_data.wgtCalculator);

    m_data.wgtRaycast->rootFactory().addFactory(pGameObjectFactory_t(factory));
    m_data.wgtRaycast->initialise(commonData.appConfig.dataPath("youve_got_mail/map.svg"));

    m_data.gameLogic.reset(new youve_got_mail::GameLogic(commonData.eventSystem, entityManager));

    m_data.wgtRaycast->start();

    m_data.vbox->addWidget(m_data.wgtRaycast.get());
  });
}

//===========================================
// FServerRoom::cleanUp
//===========================================
void FServerRoom::cleanUp() {
  DBG_PRINT("FServerRoom::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);

  for (int i = 0; i < parentData.box->count(); ++i) {
    QWidget* wgt = parentData.box->itemAt(i)->widget();
    if (wgt) {
      wgt->show();
    }
  }
}

//===========================================
// FServerRoom::~FServerRoom
//===========================================
FServerRoom::~FServerRoom() {
  DBG_PRINT("FServerRoom::~FServerRoom\n");
}
