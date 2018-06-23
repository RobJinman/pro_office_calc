#ifndef __PROCALC_FRAGMENTS_F_KERNEL_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_GAME_LOGIC_HPP__


#include <string>
#include <stdexcept>
#include <map>
#include <list>
#include "raycast/component.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/events.hpp"


class EventSystem;
class Event;
class EntityManager;
class EventHandlerSystem;
class GameEvent;
class RootFactory;
class CZone;


namespace millennium_bug {


class ObjectFactory;

template<class T, int SIZE>
class FixedSizeList {
  public:
    const T& oldest() const {
      return m_list.front();
    }

    const T& newest() const {
      return m_list.back();
    }

    const T& nthNewest(unsigned int n) const {
      if (n >= m_list.size()) {
        throw std::out_of_range("Index out of range");
      }

      unsigned int i = 0;
      for (const T& e : m_list) {
        if (n + 1 + i == m_list.size()) {
          return e;
        }

        ++i;
      }

      throw std::out_of_range("Index out of range");
    }

    void push(const T& element) {
      m_list.push_back(element);

      if (m_list.size() > SIZE) {
        m_list.pop_front();
      }
    }

    int size() const {
      return m_list.size();
    }

  private:
    std::list<T> m_list;
};

class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager, RootFactory& rootFactory,
      ObjectFactory& objectFactory);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    void initialise(const std::set<Coord>& mineCoords);
    void onPlayerEnterCellInner(entityId_t cellId);
    void onCellDoorOpened(entityId_t cellId);
    void lockDoors();
    void sealDoor(entityId_t doorId);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    RootFactory& m_rootFactory;
    ObjectFactory& m_objectFactory;

    entityId_t m_entityId = -1;
    int m_setupEventId = -1;

    std::map<entityId_t, Coord> m_cellIds;

    FixedSizeList<entityId_t, 3> m_history;
};


}



#endif
