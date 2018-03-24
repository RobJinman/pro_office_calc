#ifndef __PROCALC_FRAGMENTS_F_SERVER_ROOM_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_SERVER_ROOM_GAME_LOGIC_HPP__


class EventSystem;
class Event;
class EntityManager;

namespace youve_got_mail {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    void onDivByZero(const Event& event);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    int m_eventIdx = -1;
};


}



#endif