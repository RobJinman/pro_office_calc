#ifndef __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__
#define __PROCALC_FRAGMENTS_F_MAIL_CLIENT_HPP__


#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMargins>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "fragments/relocatable/widget_frag_data.hpp"


struct FMailClientData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<QLabel> wgtLabel;
};

class FMailClient : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FMailClient(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMailClient() override;

  private:
    FMailClientData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
