#ifndef __PROCALC_FRAGMENTS_F_APP_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_APP_DIALOG_SPEC_HPP__


#include <string>
#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"
#include "fragments/relocatable/f_calculator/f_calculator_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_mail_client/f_mail_client_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_server_room/f_server_room_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_procalc_setup/f_procalc_setup_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_text_editor/f_text_editor_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system/f_file_system_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system_2/f_file_system_2_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/f_minesweeper_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_console/f_console_spec.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/f_doomsweeper_spec.hpp"


struct FAppDialogSpec : public FragmentSpec {
  FAppDialogSpec(int id)
    : FragmentSpec("FAppDialog", id, {
        &glitchSpec,
        &calculatorSpec,
        &mailClientSpec,
        &serverRoomSpec,
        &procalcSetupSpec,
        &textEditorSpec,
        &fileSystemSpec,
        &fileSystem2Spec,
        &minesweeperSpec,
        &doomsweeperSpec,
        &consoleSpec
      }) {}

  FGlitchSpec glitchSpec;
  FCalculatorSpec calculatorSpec;
  FMailClientSpec mailClientSpec;
  FServerRoomSpec serverRoomSpec;
  FProcalcSetupSpec procalcSetupSpec;
  FTextEditorSpec textEditorSpec;
  FFileSystemSpec fileSystemSpec;
  FFileSystem2Spec fileSystem2Spec;
  FMinesweeperSpec minesweeperSpec;
  FDoomsweeperSpec doomsweeperSpec;
  FConsoleSpec consoleSpec;

  std::string name = "dialog";
  QString titleText = "Application";
  int width = 640;
  int height = 480;
  std::string showOnEvent = "doesNotExist";
};


#endif
