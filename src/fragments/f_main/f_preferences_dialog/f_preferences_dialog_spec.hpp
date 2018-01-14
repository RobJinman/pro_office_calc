#ifndef __PROCALC_FRAGMENTS_F_PREFERENCES_DIALOG_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_PREFERENCES_DIALOG_SPEC_HPP__


#include <QString>
#include "fragment_spec.hpp"
#include "fragments/relocatable/f_glitch/f_glitch_spec.hpp"


struct FPreferencesDialogSpec : public FragmentSpec {
  FPreferencesDialogSpec()
    : FragmentSpec("FPreferencesDialog", {
        &glitchSpec
      }) {}

  FGlitchSpec glitchSpec;

  QString titleText = "Preferences";
  int width = 640;
  int height = 480;
};


#endif