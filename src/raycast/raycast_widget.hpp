#ifndef __PROCALC_RAYCAST_RAYCAST_WIDGET_HPP__
#define __PROCALC_RAYCAST_RAYCAST_WIDGET_HPP__


#include <memory>
#include <map>
#include <QWidget>
#include <QTimer>
#include <QImage>
#include "raycast/entity_manager.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/geometry.hpp"
#include "raycast/time_service.hpp"
#include "raycast/root_factory.hpp"
#ifdef DEBUG
#  include <chrono>
#endif


class QPaintEvent;
class EventSystem;

class RaycastWidget : public QWidget {
  Q_OBJECT

  public:
    RaycastWidget(QWidget* parent, EventSystem& eventSystem);

    void initialise();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

  private slots:
    void tick();

  private:
    EventSystem& m_eventSystem;

    EntityManager m_entityManager;
    TimeService m_timeService;
    AudioService m_audioService;
    std::unique_ptr<RootFactory> m_rootFactory;
    std::unique_ptr<QTimer> m_timer;
    QImage m_buffer;
    std::map<int, bool> m_keyStates;
    bool m_mouseBtnState;
    Point m_cursor;
    bool m_cursorCaptured;
    Qt::CursorShape m_defaultCursor;

#ifdef DEBUG
    std::chrono::high_resolution_clock::time_point m_t = std::chrono::high_resolution_clock::now();
    double m_frameRate = 0;
    long m_frame = 0;
#endif

    void loadMap(const std::string& mapFilePath);
};


#endif
