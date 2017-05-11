#include <QWidget>
#include "effects.hpp"
#include "update_loop.hpp"
#include "exception.hpp"


//===========================================
// tweenColour
//===========================================
QColor tweenColour(const QColor& a, const QColor& b, double i) {
  if (i < 0.0 || i > 1.0) {
    EXCEPTION("Tween val of " << i << " is out of range");
  }

  return QColor(a.red() + (b.red() - a.red()) * i,
                a.green() + (b.green() - a.green()) * i,
                a.blue() + (b.blue() - a.blue()) * i);
}

//===========================================
// transitionColour
//===========================================
void transitionColour(UpdateLoop& updateLoop, QWidget& widget, const QColor& colour,
  QPalette::ColorRole colourRole, double duration) {

  QPalette palette = widget.palette();
  QColor origCol = palette.color(colourRole);

  int frames = updateLoop.fps() * duration;
  int i = 0;

  updateLoop.add([=,&widget]() mutable {
    double f = static_cast<double>(i) / static_cast<double>(frames);

    QColor c = tweenColour(origCol, colour, f);
    palette.setColor(colourRole, c);
    widget.setPalette(palette);

    ++i;
    return i <= frames;
  });
}

//===========================================
// setBackgroundImage
//===========================================
void setBackgroundImage(QWidget& widget, const QString& path) {
  QPixmap bg(path);
  bg = bg.scaled(widget.size(), Qt::IgnoreAspectRatio);
  QPalette palette = widget.palette();
  palette.setBrush(QPalette::Background, bg);
  widget.setPalette(palette);
}
