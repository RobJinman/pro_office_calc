#include <cassert>
#include <list>
#include <set>
#include <ostream>
#include <QImage>
#include "raycast/render_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/entity_manager.hpp"
#include "exception.hpp"


using std::string;
using std::list;
using std::set;
using std::vector;
using std::array;
using std::unique_ptr;
using std::ostream;
using std::function;
using std::make_pair;


static const double VIEWPORT_W = 10.0 * 320.0 / 240.0;
static const double VIEWPORT_H = 10.0;


//===========================================
// operator<<
//===========================================
ostream& operator<<(ostream& os, CRenderKind kind) {
  switch (kind) {
    case CRenderKind::REGION: os << "REGION"; break;
    case CRenderKind::WALL: os << "WALL"; break;
    case CRenderKind::JOIN: os << "JOIN"; break;
    case CRenderKind::FLOOR_DECAL: os << "FLOOR_DECAL"; break;
    case CRenderKind::WALL_DECAL: os << "WALL_DECAL"; break;
    case CRenderKind::SPRITE: os << "SPRITE"; break;
    case CRenderKind::OVERLAY: os << "OVERLAY"; break;
  }
  return os;
}

//===========================================
// getSoftEdge
//===========================================
static inline CSoftEdge& getSoftEdge(const SpatialSystem& spatialSystem, const CBoundary& b) {
  return DYNAMIC_CAST<CSoftEdge&>(spatialSystem.getComponent(b.entityId()));
}

//===========================================
// forEachRegion
//
// fn can return false to abort the loop
//===========================================
static bool forEachRegion(CRegion& region, function<bool(CRegion&)> fn) {
  if (fn(region)) {
    for (auto& child : region.children) {
      if (!forEachRegion(*child, fn)) {
        break;
      }
    }

    return true;
  }

  return false;
}

//===========================================
// RenderSystem::RenderSystem
//===========================================
RenderSystem::RenderSystem(const AppConfig& appConfig, EntityManager& entityManager, QImage& target)
  : m_appConfig(appConfig),
    m_entityManager(entityManager),
    m_renderer(appConfig, entityManager, target, rg, Size{VIEWPORT_W, VIEWPORT_H}) {}

//===========================================
// RenderSystem::textOverlayWidth
//
// Returns width in world units
//===========================================
double RenderSystem::textOverlayWidth(const CTextOverlay& overlay) const {
  double h = overlay.height * m_renderer.worldUnit_px().y;

  QFont font = m_appConfig.monoFont;
  font.setPixelSize(h);

  QFontMetrics fm{font};

  double textW_px = fm.size(Qt::TextSingleLine, overlay.text.c_str()).width();
  return textW_px / m_renderer.worldUnit_px().x;
}

//===========================================
// RenderSystem::centreTextOverlay
//===========================================
void RenderSystem::centreTextOverlay(CTextOverlay& overlay) const {
  double x = 0.5 * (VIEWPORT_W - textOverlayWidth(overlay));
  overlay.pos.x = x;
}

//===========================================
// connectSubregions
//===========================================
static void connectSubregions(const SpatialSystem& spatialSystem, CRegion& region) {
  int i = 0;
  forEachRegion(region, [&](CRegion& r) {
    for (auto jt = r.boundaries.begin(); jt != r.boundaries.end(); ++jt) {
      if ((*jt)->kind == CRenderKind::JOIN) {
        CJoin* je = DYNAMIC_CAST<CJoin*>(*jt);
        assert(je != nullptr);

        entityId_t id1 = getSoftEdge(spatialSystem, *je).joinId;

        bool hasTwin = false;
        int j = 0;
        forEachRegion(region, [&](CRegion& r_) {
          if (j >= i) {
            return false;
          }

          for (auto lt = r_.boundaries.begin(); lt != r_.boundaries.end(); ++lt) {
            if ((*lt)->kind == CRenderKind::JOIN) {
              CJoin* other = DYNAMIC_CAST<CJoin*>(*lt);

              entityId_t id2 = getSoftEdge(spatialSystem, *other).joinId;

              if (id1 == id2) {
                hasTwin = true;

                je->joinId = other->joinId;
                je->regionA = other->regionA = &r;
                je->regionB = other->regionB = &r_;

                je->mergeIn(*other);
                other->mergeIn(*je);

                return false;
              }
            }
          }

          ++j;
          return true;
        });

        if (!hasTwin) {
          je->regionA = &r;
          je->regionB = &region;
        }
      }
    }

    ++i;
    return true;
  });
}

//===========================================
// RenderSystem::connectRegions
//
// Assumes SpatialSystem::connectZones() has already been called
//===========================================
void RenderSystem::connectRegions() {
  const SpatialSystem& spatialSystem = m_entityManager
    .system<SpatialSystem>(ComponentKind::C_SPATIAL);

  connectSubregions(spatialSystem, *rg.rootRegion);
}

//===========================================
// RenderSystem::render
//===========================================
void RenderSystem::render() {
  m_renderer.renderScene();
}

//===========================================
// addToRegion
//===========================================
static void addToRegion(RenderGraph& rg, CRegion& region, pCRender_t child) {
  switch (child->kind) {
    case CRenderKind::REGION: {
      pCRegion_t ptr(DYNAMIC_CAST<CRegion*>(child.release()));
      region.children.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::JOIN:
    case CRenderKind::WALL: {
      pCBoundary_t ptr(DYNAMIC_CAST<CBoundary*>(child.release()));
      region.boundaries.push_back(ptr.get());
      rg.boundaries.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::FLOOR_DECAL: {
      pCFloorDecal_t ptr(DYNAMIC_CAST<CFloorDecal*>(child.release()));
      region.floorDecals.push_back(std::move(ptr));
      break;
    }
    case CRenderKind::SPRITE: {
      pCSprite_t ptr(DYNAMIC_CAST<CSprite*>(child.release()));
      region.sprites.push_back(std::move(ptr));
      break;
    }
    default: {
      EXCEPTION("Cannot add component of kind " << child->kind << " to region");
    }
  }
}

//===========================================
// addToWall
//===========================================
static void addToWall(CWall& boundary, pCRender_t child) {
  switch (child->kind) {
    case CRenderKind::WALL_DECAL: {
      pCWallDecal_t ptr(DYNAMIC_CAST<CWallDecal*>(child.release()));
      boundary.decals.push_back(std::move(ptr));
      break;
    }
    default: {
      EXCEPTION("Cannot add component of kind " << child->kind << " to Wall");
    }
  }
}

//===========================================
// addToJoin
//===========================================
static void addToJoin(CJoin& boundary, pCRender_t child) {
  switch (child->kind) {
    case CRenderKind::WALL_DECAL: {
      pCWallDecal_t ptr(DYNAMIC_CAST<CWallDecal*>(child.release()));
      boundary.decals.push_back(std::move(ptr));
      break;
    }
    default: {
      EXCEPTION("Cannot add component of kind " << child->kind << " to Join");
    }
  }
}

//===========================================
// addChildToComponent
//===========================================
static void addChildToComponent(RenderGraph& rg, CRender& parent, pCRender_t child) {
  switch (parent.kind) {
    case CRenderKind::REGION:
      addToRegion(rg, DYNAMIC_CAST<CRegion&>(parent), std::move(child));
      break;
    case CRenderKind::WALL:
      addToWall(DYNAMIC_CAST<CWall&>(parent), std::move(child));
      break;
    case CRenderKind::JOIN:
      addToJoin(DYNAMIC_CAST<CJoin&>(parent), std::move(child));
      break;
    default: {
      EXCEPTION("Cannot add component of kind " << child->kind << " to component of kind "
        << parent.kind);
    }
  };
}

//===========================================
// removeFromRegion
//===========================================
static bool removeFromRegion(RenderGraph& rg, CRegion& region, const CRender& child,
  bool keepAlive) {

  bool found = false;

  switch (child.kind) {
    case CRenderKind::REGION: {
      auto it = find_if(region.children.begin(), region.children.end(), [&](const pCRegion_t& e) {
        return e.get() == DYNAMIC_CAST<const CRegion*>(&child);
      });
      if (it != region.children.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(region.children, it);
        found = true;
      }
      break;
    }
    case CRenderKind::JOIN:
    case CRenderKind::WALL: {
      auto it = find_if(region.boundaries.begin(), region.boundaries.end(),
        [&](const CBoundary* b) {

        return b == DYNAMIC_CAST<const CBoundary*>(&child);
      });
      erase(region.boundaries, it);
      auto jt = find_if(rg.boundaries.begin(), rg.boundaries.end(), [&](const pCBoundary_t& b) {
        return b.get() == DYNAMIC_CAST<const CBoundary*>(&child);
      });
      if (jt != rg.boundaries.end()) {
        if (keepAlive) {
          jt->release();
        }
        erase(rg.boundaries, jt);
        found = true;
      }
      break;
    }
    case CRenderKind::FLOOR_DECAL: {
      auto it = find_if(region.floorDecals.begin(), region.floorDecals.end(),
        [&](const pCFloorDecal_t& e) {

        return e.get() == DYNAMIC_CAST<const CFloorDecal*>(&child);
      });
      if (it != region.floorDecals.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(region.floorDecals, it);
        found = true;
      }
      break;
    }
    case CRenderKind::SPRITE: {
      auto it = find_if(region.sprites.begin(), region.sprites.end(), [&](const pCSprite_t& e) {
        return e.get() == DYNAMIC_CAST<const CSprite*>(&child);
      });
      if (it != region.sprites.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(region.sprites, it);
        found = true;
      }
      break;
    }
    default: {
      EXCEPTION("Cannot add component of kind " << child.kind << " to region");
    }
  }

  return found;
}

//===========================================
// removeFromWall
//===========================================
static bool removeFromWall(CWall& boundary, const CRender& child, bool keepAlive) {
  bool found = false;

  switch (child.kind) {
    case CRenderKind::WALL_DECAL: {
      auto it = find_if(boundary.decals.begin(), boundary.decals.end(),
        [&](const pCWallDecal_t& e) {

        return e.get() == DYNAMIC_CAST<const CWallDecal*>(&child);
      });
      if (it != boundary.decals.end()) {
        if (keepAlive) {
          it->release();
        }
        erase(boundary.decals, it);
        found = true;
      }
      break;
    }
    default: {
      EXCEPTION("Cannot remove component of kind " << child.kind << " from Wall");
    }
  }

  return found;
}

//===========================================
// RenderSystem::crossRegions
//===========================================
void RenderSystem::crossRegions(RenderGraph& rg, entityId_t entityId, entityId_t oldZone,
  entityId_t newZone) {

  auto it = m_components.find(entityId);
  if (it != m_components.end()) {
    CRender& c = *it->second;
    CRegion& oldRegion = DYNAMIC_CAST<CRegion&>(getComponent(oldZone));
    CRegion& newRegion = DYNAMIC_CAST<CRegion&>(getComponent(newZone));

    if (removeFromRegion(rg, oldRegion, c, true)) {
      addChildToComponent(rg, newRegion, pCRender_t(&c));
    }
  }
}

//===========================================
// RenderSystem::handleEvent
//===========================================
void RenderSystem::handleEvent(const GameEvent& event) {
  if (event.name == "entity_changed_zone") {
    const EChangedZone& e = DYNAMIC_CAST<const EChangedZone&>(event);
    crossRegions(rg, e.entityId, e.oldZone, e.newZone);
  }
}

//===========================================
// removeChildFromComponent
//===========================================
static void removeChildFromComponent(RenderGraph& rg, CRender& parent, const CRender& child,
  bool keepAlive = false) {

  switch (parent.kind) {
    case CRenderKind::REGION: {
      removeFromRegion(rg, DYNAMIC_CAST<CRegion&>(parent), child, keepAlive);
      break;
    }
    case CRenderKind::WALL: {
      removeFromWall(DYNAMIC_CAST<CWall&>(parent), child, keepAlive);
      break;
    }
    default: {
      EXCEPTION("Cannot remove component of kind " << child.kind << " from component of kind "
        << parent.kind);
    }
  };
}

//===========================================
// RenderSystem::hasComponent
//===========================================
bool RenderSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// RenderSystem::getComponent
//===========================================
CRender& RenderSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// RenderSystem::addComponent
//===========================================
void RenderSystem::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_RENDER) {
    EXCEPTION("Component is not of kind C_RENDER");
  }

  CRender* ptr = DYNAMIC_CAST<CRender*>(component.release());
  pCRender_t c(ptr);

  if (c->parentId == -1) {
    if (c->kind == CRenderKind::REGION) {
      if (rg.rootRegion) {
        EXCEPTION("Root region already set");
      }

      pCRegion_t z(DYNAMIC_CAST<CRegion*>(c.release()));

      rg.rootRegion = std::move(z);
      m_components.clear();
    }
    else if (c->kind == CRenderKind::OVERLAY) {
      pCOverlay_t z(DYNAMIC_CAST<COverlay*>(c.release()));

      auto it = find_if(rg.overlays.begin(), rg.overlays.end(), [&](const pCOverlay_t& o) {
        return o->entityId() == z->entityId();
      });

      if (it == rg.overlays.end()) {
        rg.overlays.insert(std::move(z));
      }
    }
    else {
      EXCEPTION("Component has no parent and is not of type REGION or OVERLAY");
    }
  }
  else {
    auto it = m_components.find(c->parentId);
    if (it == m_components.end()) {
      EXCEPTION("Could not find parent component with id " << c->parentId);
    }

    CRender* parent = it->second;
    assert(parent->entityId() == c->parentId);

    m_entityChildren[c->parentId].insert(c->entityId());
    addChildToComponent(rg, *parent, std::move(c));
  }

  m_components.insert(make_pair(ptr->entityId(), ptr));
}

//===========================================
// RenderSystem::isRoot
//===========================================
bool RenderSystem::isRoot(const CRender& c) const {
  if (c.kind != CRenderKind::REGION) {
    return false;
  }
  if (rg.rootRegion == nullptr) {
    return false;
  }
  const CRegion* ptr = DYNAMIC_CAST<const CRegion*>(&c);
  return ptr == rg.rootRegion.get();
}

//===========================================
// RenderSystem::removeEntity_r
//===========================================
void RenderSystem::removeEntity_r(entityId_t id) {
  m_components.erase(id);

  auto it = m_entityChildren.find(id);
  if (it != m_entityChildren.end()) {
    set<entityId_t>& children = it->second;

    for (auto jt = children.begin(); jt != children.end(); ++jt) {
      removeEntity_r(*jt);
    }
  }

  m_entityChildren.erase(id);
}

//===========================================
// RenderSystem::removeEntity
//===========================================
void RenderSystem::removeEntity(entityId_t id) {
  auto it = m_components.find(id);
  if (it == m_components.end()) {
    return;
  }

  CRender& c = *it->second;
  auto jt = m_components.find(c.parentId);

  if (jt != m_components.end()) {
    CRender& parent = *jt->second;
    removeChildFromComponent(rg, parent, c);
  }
  else {
    if (c.kind == CRenderKind::REGION) {
      assert(isRoot(c));
    }
    else if (c.kind == CRenderKind::OVERLAY) {
      auto it = find_if(rg.overlays.begin(), rg.overlays.end(), [&](const pCOverlay_t& o) {
        return o->entityId() == id;
      });
      erase(rg.overlays, it);
    }
  }

  removeEntity_r(id);
}
