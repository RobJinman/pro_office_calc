#include <QMainWindow>
#include <QPushButton>
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger.hpp"
#include "fragments/f_main/f_calculator/f_normal_calc_trigger/f_normal_calc_trigger_spec.hpp"
#include "fragments/f_main/f_calculator/f_calculator.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"
#include "effects.hpp"


//===========================================
// FNormalCalcTrigger::FNormalCalcTrigger
//===========================================
FNormalCalcTrigger::FNormalCalcTrigger(Fragment& parent_, FragmentData& parentData_)
  : Fragment("FNormalCalcTrigger", parent_, parentData_, m_data),
    m_stateId(0) {

  auto& parentData = parentFragData<FCalculatorData>();
  auto& parent = parentFrag<FCalculator>();

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
}

//===========================================
// FNormalCalcTrigger::rebuild
//===========================================
void FNormalCalcTrigger::rebuild(const FragmentSpec& spec_) {
  auto& spec = dynamic_cast<const FNormalCalcTriggerSpec&>(spec_);

  m_stateId = spec.stateId;
  m_targetWindowColour = spec.targetWindowColour;
  m_targetDisplayColour = spec.targetDisplayColour;
  m_symbols = spec.symbols;

  Fragment::rebuild(spec_);
}

//===========================================
// FNormalCalcTrigger::cleanUp
//===========================================
void FNormalCalcTrigger::cleanUp() {
  auto& parentData = parentFragData<FCalculatorData>();
  auto& parent = parentFrag<FCalculator>();

  disconnect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this,
    SLOT(onButtonClick(int)));
  connect(parentData.wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), &parent,
    SLOT(onButtonClick(int)));
}

//===========================================
// FNormalCalcTrigger:onButtonClick
//===========================================
void FNormalCalcTrigger::onButtonClick(int id) {
  auto& data = parentFragData<FCalculatorData>();

  if (m_stateId == ST_NORMAL_CALCULATOR_10 && id == BTN_EQUALS) {
    double result = data.calculator.equals();
    data.wgtDigitDisplay->setText(data.calculator.display().c_str());

    if (std::isinf(result)) {
      QColor origCol = data.window->palette().color(QPalette::Window);
      int i = 0;

      data.updateLoop->add([=,&data]() mutable {
        auto& buttons = data.wgtButtonGrid->buttons;
        ++i;

        int j = 0;
        for (auto it = buttons.begin(); it != buttons.end(); ++it) {
          QChar ch = m_symbols[(i + j) % m_symbols.length()];
          (*it)->setText(ch);
          ++j;
        }

        return i < data.updateLoop->fps() * 1.5;
      });

      data.updateLoop->add([=,&data]() mutable {
        ++i;
        if (i % 2) {
          setColour(*data.window, origCol, QPalette::Window);
        }
        else {
          setColour(*data.window, Qt::white, QPalette::Window);
        }

        return i < data.updateLoop->fps();
      }, [&]() {
        transitionColour(*data.updateLoop, *data.window, m_targetWindowColour, QPalette::Window,
          0.5, [&]() {

          data.eventSystem->fire(RequestStateChangeEvent(ST_SHUFFLED_KEYS));
        });

        transitionColour(*data.updateLoop, *data.wgtDigitDisplay, m_targetDisplayColour,
          QPalette::Base, 0.5);
      });
    }
  }
  else {
    parentFrag<FCalculator>().onButtonClick(id);
  }
}