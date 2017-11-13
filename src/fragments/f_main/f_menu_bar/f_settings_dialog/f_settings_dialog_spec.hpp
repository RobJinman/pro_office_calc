#ifndef __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_SETTINGS_DIALOG_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_loading_screen/f_loading_screen_spec.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/f_raycast_spec.hpp"


struct FSettingsDialogSpec : public FragmentSpec {
  FSettingsDialogSpec()
    : FragmentSpec("FSettingsDialog", {
        &loadingScreenSpec,
        &raycastSpec
      }) {}

  FLoadingScreenSpec loadingScreenSpec;
  FRaycastSpec raycastSpec;

  QString titleText = "Settings";
  int width = 640;
  int height = 480;
};


#endif
