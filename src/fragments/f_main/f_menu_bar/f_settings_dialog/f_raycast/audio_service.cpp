#include <QFileInfo>
#include <QUrl>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/audio_service.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"


using std::make_pair;
using std::string;
using std::unique_ptr;


//===========================================
// AudioService::AudioService
//===========================================
AudioService::AudioService(EntityManager& entityManager)
  : m_entityManager(entityManager) {

  m_musicVolume = 0.5;
  setMasterVolume(0.5);

  m_mediaPlayer.setPlaylist(&m_playlist);
}

//===========================================
// AudioService::addSound
//===========================================
void AudioService::addSound(const string& name, const string& resourcePath) {
  pSoundEffect_t sound(new SoundEffect);

  QString absPath = QFileInfo(resourcePath.c_str()).absoluteFilePath();
  sound->sound.setSource(QUrl::fromLocalFile(absPath));

  m_sounds.insert(make_pair(name, std::move(sound)));
}

//===========================================
// AudioService::addMusicTrack
//===========================================
void AudioService::addMusicTrack(const string& name, const string& resourcePath) {
  QString absPath = QFileInfo(resourcePath.c_str()).absoluteFilePath();
  m_musicTracks[name] = QMediaContent(QUrl::fromLocalFile(absPath));
}

//===========================================
// AudioService::playSound
//===========================================
void AudioService::playSound(const string& name) {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    sound.sound.setVolume(m_masterVolume * sound.volume);
    sound.sound.play();
  }
}

//===========================================
// AudioService::playSoundAtPos
//===========================================
void AudioService::playSoundAtPos(const string& name, const Point& pos) {
  auto it = m_sounds.find(name);
  if (it != m_sounds.end()) {
    SoundEffect& sound = *it->second;

    const SpatialSystem& spatialSystem = m_entityManager
      .system<SpatialSystem>(ComponentKind::C_SPATIAL);

    double d = distance(spatialSystem.sg.player->camera().pos, pos);
    double v = 1.0 - clipNumber(d, Range(0, 2000)) / 2000;

    sound.sound.setVolume(m_masterVolume * sound.volume * v);
    sound.sound.play();
  }
}

//===========================================
// AudioService::playMusic
//===========================================
void AudioService::playMusic(const string& name) {
  auto it = m_musicTracks.find(name);

  if (it != m_musicTracks.end()) {
    m_playlist.clear();
    m_playlist.addMedia(it->second);
    m_playlist.setPlaybackMode(QMediaPlaylist::Loop);

    m_mediaPlayer.play();
  }
}

//===========================================
// AudioService::stopMusic
//===========================================
void AudioService::stopMusic() {
  m_mediaPlayer.stop();
}

//===========================================
// AudioService::setMusicVolume
//===========================================
void AudioService::setMusicVolume(double volume) {
  m_musicVolume = volume;
  m_mediaPlayer.setVolume(m_masterVolume * m_musicVolume * 100);
}

//===========================================
// AudioService::setMasterVolume
//===========================================
void AudioService::setMasterVolume(double volume) {
  m_masterVolume = volume;
  setMusicVolume(m_musicVolume);
}