#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_COMPONENTS_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_COMPONENTS_HPP__


#include <string>
#include <list>
#include <map>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_objects.hpp"


enum class CRenderKind {
  C_REGION,
  C_WALL,
  C_JOINING_EDGE,
  C_SPRITE,
  C_FLOOR_DECAL,
  C_WALL_DECAL
};

struct CRender : public Component {
  CRender(CRenderKind kind, entityId_t entityId, entityId_t parentId)
    : Component(entityId, ComponentKind::C_RENDER),
      kind(kind),
      parentId(parentId) {}

  CRender(const CRender& cpy)
    : Component(cpy),
      kind(cpy.kind),
      parentId(-1) {}

  CRenderKind kind;
  entityId_t parentId;

  virtual ~CRender() override {}
};

typedef std::unique_ptr<CRender> pCRender_t;

class CRegion;

class CSprite : public CRender {
  public:
    CSprite(entityId_t entityId, entityId_t parentId, const Size& size, const std::string& texture)
      : CRender(CRenderKind::C_SPRITE, entityId, parentId),
        texture(texture),
        size(size) {}

    void setTransform(const Matrix& m) {
      pos.x = m.tx();
      pos.y = m.ty();
      angle = m.a();
    }

    const QRectF& textureCRegion(const Point& camPos) const {
      Vec2f v = pos - camPos;
      return animations.at("idle").currentFrame().part(PI - atan2(v.y, v.x) + angle);
    }

    CRegion* region;
    std::string texture;
    Vec2f pos;
    double angle;
    Size size;
    std::map<std::string, Animation> animations;

    void playAnimation(const std::string& name);

    virtual ~CSprite() override {}
};

typedef std::unique_ptr<CSprite> pCSprite_t;

struct CAmmo : public CSprite {
  CAmmo(entityId_t entityId, entityId_t parentId)
    : CSprite(entityId, parentId, Size(30, 15), "ammo") {

    Animation anim;
    anim.fps = 0;

    anim.frames.push_back(AnimationFrame{
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1),
      QRectF(0, 0, 1, 1)
    });

    animations["idle"] = anim;
  }

  virtual ~CAmmo() override {}
};

struct CBadGuy : public CSprite {
  CBadGuy(entityId_t entityId, entityId_t parentId)
    : CSprite(entityId, parentId, Size(70, 70), "bad_guy") {

    Animation anim;
    anim.fps = 0;

    anim.frames.push_back(AnimationFrame{
      QRectF(0, 0, 0.125, 1),
      QRectF(0.125, 0, 0.125, 1),
      QRectF(0.25, 0, 0.125, 1),
      QRectF(0.375, 0, 0.125, 1),
      QRectF(0.5, 0, 0.125, 1),
      QRectF(0.625, 0, 0.125, 1),
      QRectF(0.750, 0, 0.125, 1),
      QRectF(0.875, 0, 0.125, 1)
    });

    animations["idle"] = anim;
  }

  virtual ~CBadGuy() override {}
};

struct CEdge : public CRender {
  CEdge(CRenderKind kind, entityId_t entityId, entityId_t parentId)
    : CRender(kind, entityId, parentId) {}

  CEdge(const CEdge& cpy)
    : CRender(cpy) {

    lseg = cpy.lseg;
  }

  LineSegment lseg;

  virtual ~CEdge() {}
};

typedef std::unique_ptr<CEdge> pCEdge_t;

struct CFloorDecal : public CRender {
  CFloorDecal(entityId_t entityId, entityId_t parentId)
    : CRender(CRenderKind::C_FLOOR_DECAL, entityId, parentId) {}

  std::string texture;
  Size size;
  Matrix transform;

  virtual ~CFloorDecal() override {}
};

typedef std::unique_ptr<CFloorDecal> pCFloorDecal_t;

class CRegion;
typedef std::unique_ptr<CRegion> pCRegion_t;

struct CRegion : public CRender {
  CRegion(entityId_t entityId, entityId_t parentId)
    : CRender(CRenderKind::C_REGION, entityId, parentId) {}

  bool hasCeiling = true;
  double floorHeight = 0;
  double ceilingHeight = 100;
  std::string floorTexture;
  std::string ceilingTexture;
  std::list<pCRegion_t> children;
  std::list<CEdge*> edges;
  std::list<pCSprite_t> sprites;
  std::list<pCFloorDecal_t> floorDecals;
  CRegion* parent = nullptr;

  virtual ~CRegion() override {}
};

void forEachConstCRegion(const CRegion& region, std::function<void(const CRegion&)> fn);
void forEachCRegion(CRegion& region, std::function<void(CRegion&)> fn);

struct CWallDecal : public CRender {
  CWallDecal(entityId_t entityId, entityId_t parentId)
    : CRender(CRenderKind::C_WALL_DECAL, entityId, parentId) {}

  std::string texture;
  Size size;
  Point pos;

  virtual ~CWallDecal() {}
};

typedef std::unique_ptr<CWallDecal> pCWallDecal_t;

struct CWall : public CEdge {
  CWall(entityId_t entityId, entityId_t parentId)
    : CEdge(CRenderKind::C_WALL, entityId, parentId) {}

  std::string texture;
  CRegion* region;
  std::list<pCWallDecal_t> decals;

  double height() const {
    return region->ceilingHeight - region->floorHeight;
  }

  virtual ~CWall() {}
};

struct CJoiningEdge : public CEdge {
  CJoiningEdge(entityId_t entityId, entityId_t parentId)
    : CEdge(CRenderKind::C_JOINING_EDGE, entityId, parentId) {}

  CJoiningEdge(const CJoiningEdge& cpy)
    : CEdge(cpy) {

    topTexture = cpy.topTexture;
    bottomTexture = cpy.bottomTexture;
    regionA = cpy.regionA;
    regionB = cpy.regionB;
  }

  void mergeIn(const CJoiningEdge& other) {
    if (other.topTexture != "default") {
      topTexture = other.topTexture;
    }
    if (other.bottomTexture != "default") {
      bottomTexture = other.bottomTexture;
    }
  }

  std::string topTexture = "default";
  std::string bottomTexture = "default";

  CRegion* regionA = nullptr;
  CRegion* regionB = nullptr;

  virtual ~CJoiningEdge() {}
};


#endif
