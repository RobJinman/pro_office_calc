#include <algorithm>
#include <cassert>
#include <QPushButton>
#include <QButtonGroup>
#include <QPainter>
#include <vector>
#include "event_system.hpp"
#include "update_loop.hpp"
#include "effects.hpp"
#include "strings.hpp"
#include "state_ids.hpp"
#include "utils.hpp"
#include "effects.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc.hpp"
#include "fragments/f_main/f_shuffled_calc/f_shuffled_calc_spec.hpp"
#include "fragments/f_main/f_main.hpp"


using std::shuffle;


static std::mt19937 randEngine(randomSeed());


//===========================================
// idToChar
//===========================================
static QChar idToChar(int id) {
  if (id < 10) {
    return QString::number(id)[0];
  }
  switch (id) {
    case BTN_PLUS: return '+';
    case BTN_MINUS: return '-';
    case BTN_TIMES: return '*';
    case BTN_DIVIDE: return '/';
    case BTN_POINT: return '.';
    case BTN_CLEAR: return 'C';
    case BTN_EQUALS: return '=';
  }
  return '_';
}

//===========================================
// FShuffledCalc::FShuffledCalc
//===========================================
FShuffledCalc::FShuffledCalc(Fragment& parent_, FragmentData& parentData_,
  const CommonFragData& commonData)
  : QWidget(nullptr),
    Fragment("FShuffledCalc", parent_, parentData_, m_data, commonData) {

  DBG_PRINT("FShuffledCalc::FShuffledCalc\n");
}

//===========================================
// FShuffledCalc::reload
//===========================================
void FShuffledCalc::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FShuffledCalc::reload\n");

  auto& parentData = parentFragData<WidgetFragData>();

  m_origParentState.spacing = parentData.box->spacing();
  m_origParentState.margins = parentData.box->contentsMargins();

  parentData.box->setSpacing(0);
  parentData.box->setContentsMargins(0, 0, 0, 0);
  parentData.box->addWidget(this);

  QFont f = font();
  f.setPixelSize(18);
  setFont(f);

  m_wgtDigitDisplay = makeQtObjPtr<QLineEdit>(this);
  m_wgtDigitDisplay->setMaximumHeight(40);
  m_wgtDigitDisplay->setAlignment(Qt::AlignRight);
  m_wgtDigitDisplay->setReadOnly(true);

  m_wgtOpDisplay = makeQtObjPtr<QLabel>(this);
  m_wgtOpDisplay->setFixedHeight(18);
  m_wgtOpDisplay->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

  m_wgtButtonGrid = makeQtObjPtr<ButtonGrid>(this);

  m_vbox = makeQtObjPtr<QVBoxLayout>();
  m_vbox->setSpacing(0);
  m_vbox->addWidget(m_wgtDigitDisplay.get());
  m_vbox->addWidget(m_wgtOpDisplay.get());
  m_vbox->addWidget(m_wgtButtonGrid.get());
  setLayout(m_vbox.get());

  connect(m_wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(onButtonClick(int)));

  auto& spec = dynamic_cast<const FShuffledCalcSpec&>(spec_);
  m_targetValue = spec.targetValue;

  ucs4string_t symbols_ = utf8ToUcs4(spec.symbols.toStdString());
  shuffle(symbols_.begin(), symbols_.end(), randEngine);
  QString symbols(ucs4ToUtf8(symbols_).c_str());

  auto& buttons = m_wgtButtonGrid->buttons;
  auto& group = m_wgtButtonGrid->buttonGroup;

  assert(symbols.length() == static_cast<int>(buttons.size()));

  std::vector<int> ids({
    BTN_ZERO,
    BTN_ONE,
    BTN_TWO,
    BTN_THREE,
    BTN_FOUR,
    BTN_FIVE,
    BTN_SIX,
    BTN_SEVEN,
    BTN_EIGHT,
    BTN_NINE,
    BTN_PLUS,
    BTN_MINUS,
    BTN_TIMES,
    BTN_DIVIDE,
    BTN_POINT,
    BTN_CLEAR,
    BTN_EQUALS
  });

  std::vector<int> newIds = ids;
  shuffle(newIds.begin(), newIds.end(), randEngine);

  for (int i = 0; i < symbols.length(); ++i) {
    group->setId(buttons[i].get(), newIds[i]);
    QChar ch = symbols[i];
    buttons[i]->setText(ch);
    m_symbols[idToChar(newIds[i])] = ch;
  }

  DBG_PRINT("Answer = " << translateToSymbols(m_targetValue.c_str()).toStdString() << "\n");

  setColour(*m_wgtDigitDisplay, spec.displayColour, QPalette::Base);
}

//===========================================
// FShuffledCalc::cleanUp
//===========================================
void FShuffledCalc::cleanUp() {
  DBG_PRINT("FShuffledCalc::cleanUp\n");

  auto& parentData = parentFragData<WidgetFragData>();

  parentData.box->setSpacing(m_origParentState.spacing);
  parentData.box->setContentsMargins(m_origParentState.margins);
  parentData.box->removeWidget(this);
}

//===========================================
// FShuffledCalc::translateToSymbols
//===========================================
QString FShuffledCalc::translateToSymbols(const QString& str) const {
  QString result;

  for (int i = 0; i < str.length(); ++i) {
    auto it = m_symbols.find(str[i]);
    result += (it == m_symbols.end() ? str[i] : it->second);
  }

  return result;
}

//===========================================
// FShuffledCalc::onButtonClick
//===========================================
void FShuffledCalc::onButtonClick(int id) {
  m_wgtOpDisplay->setText("");

  if (id <= 9) {
    m_calculator.number(id);
  }
  else {
    auto btn = dynamic_cast<const QPushButton*>(m_wgtButtonGrid->buttonGroup->button(id));
    QString symbol = btn->text();

    switch (id) {
      case BTN_PLUS: {
        m_calculator.plus();
        m_wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_MINUS: {
        m_calculator.minus();
        m_wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_TIMES: {
        m_calculator.times();
        m_wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_DIVIDE: {
        m_calculator.divide();
        m_wgtOpDisplay->setText(symbol);
        break;
      }
      case BTN_POINT: {
        m_calculator.point();
        break;
      }
      case BTN_EQUALS: {
        m_calculator.equals();
        break;
      }
      case BTN_CLEAR: {
        m_calculator.clear();
        break;
      }
    }
  }

  QString symbols = translateToSymbols(m_calculator.display().c_str());
  m_wgtDigitDisplay->setText(symbols);

  if (m_calculator.display() == m_targetValue) {
    auto& buttons = m_wgtButtonGrid->buttons;
    int i = 0;
    int n = commonData.updateLoop.fps() / 2;

    commonData.updateLoop.add([&, i, n]() mutable {
      for (auto it = buttons.begin(); it != buttons.end(); ++it) {
        QSize sz = (*it)->size();
        (*it)->resize(sz * 0.8);

        if (i == n - 1) {
          (*it)->setVisible(false);
        }
      }

      ++i;
      return i < n;
    }, [&]() {
      m_wgtDigitDisplay->setText("");
      int i = 0;
      int n = commonData.updateLoop.fps() / 2;

      commonData.updateLoop.add([&, i, n]() mutable {
        int x = m_wgtDigitDisplay->geometry().x();
        int y = m_wgtDigitDisplay->geometry().y();
        m_wgtDigitDisplay->move(x, y + 30);

        ++i;
        return i < n;
      },
      [&]() {
        commonData.eventSystem.fire(pEvent_t(new RequestStateChangeEvent(ST_ARE_YOU_SURE)));
      });
    });
  }
}

//===========================================
// FShuffledCalc::~FShuffledCalc
//===========================================
FShuffledCalc::~FShuffledCalc() {
  DBG_PRINT("FShuffledCalc::~FShuffledCalc\n");
}
