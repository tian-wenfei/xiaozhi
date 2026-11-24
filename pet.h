#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

// 宠物当前表现出来的状态，由四项属性推导
enum class PetMood : uint8_t {
  HAPPY,
  NEUTRAL,
  HUNGRY,
  TIRED,
  DIRTY,
  SAD,
  SICK,
  SLEEPING
};

class Pet {
public:
  void begin();
  void update(uint32_t now);

  void feed();
  void play();
  void clean();
  void toggleSleep();
  void pat();

  void save(bool force = false);

  float hunger() const { return _hunger; }
  float moodValue() const { return _mood; }
  float energy() const { return _energy; }
  float clean() const { return _clean; }
  bool sleeping() const { return _sleeping; }
  uint32_t ageSeconds() const { return _ageSec; }
  PetMood moodState() const;

private:
  void tick();
  static float clampStat(float v);

  Preferences _prefs;

  float _hunger = 80.0f;
  float _mood = 80.0f;
  float _energy = 80.0f;
  float _clean = 90.0f;
  bool _sleeping = false;

  uint32_t _ageSec = 0;
  uint32_t _lastTickMs = 0;
  uint32_t _lastAgeMs = 0;
  uint32_t _lastSaveMs = 0;
  bool _dirty = false;
};
