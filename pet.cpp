#include "pet.h"

void Pet::begin() {
  _prefs.begin(NVS_NAMESPACE, false);

  _hunger   = _prefs.getFloat("hunger", 80.0f);
  _mood     = _prefs.getFloat("mood", 80.0f);
  _energy   = _prefs.getFloat("energy", 80.0f);
  _clean    = _prefs.getFloat("clean", 90.0f);
  _sleeping = _prefs.getBool("sleep", false);
  _ageSec   = _prefs.getUInt("age", 0);

  _hunger = clampStat(_hunger);
  _mood   = clampStat(_mood);
  _energy = clampStat(_energy);
  _clean  = clampStat(_clean);

  uint32_t now = millis();
  _lastTickMs = now;
  _lastAgeMs = now;
  _lastSaveMs = now;
  _dirty = false;

  Serial.printf("[pet] loaded: hunger=%.1f mood=%.1f energy=%.1f clean=%.1f age=%lus\n",
                _hunger, _mood, _energy, _clean, (unsigned long)_ageSec);
}

float Pet::clampStat(float v) {
  if (v < 0.0f) return 0.0f;
  if (v > 100.0f) return 100.0f;
  return v;
}

void Pet::update(uint32_t now) {
  // 年龄按真实秒数累计
  uint32_t ageDelta = now - _lastAgeMs;
  if (ageDelta >= 1000) {
    uint32_t secs = ageDelta / 1000;
    _ageSec += secs;
    _lastAgeMs += secs * 1000;
    _dirty = true;
  }

  // 每 30 秒结算一次；如果 loop 被卡住，会补齐遗漏的 tick
  while ((uint32_t)(now - _lastTickMs) >= TICK_MS) {
    _lastTickMs += TICK_MS;
    tick();
    _dirty = true;
  }

  // 每 60 秒写一次 NVS，避免频繁擦写闪存
  if (_dirty && (uint32_t)(now - _lastSaveMs) >= SAVE_MS) {
    save();
  }
}

void Pet::tick() {
  if (_sleeping) {
    _energy += ENERGY_RECOVER;
    _hunger -= HUNGER_DECAY * 0.5f;   // 睡觉时饿得慢
    _clean  -= CLEAN_DECAY * 0.5f;    // 脏得也慢
    _mood   += 0.05f * SPEED;
  } else {
    _hunger -= HUNGER_DECAY;
    _energy -= ENERGY_DECAY;
    _clean  -= CLEAN_DECAY;
    _mood   -= MOOD_DECAY;

    // 属性太低会额外影响心情
    if (_hunger < 30.0f) _mood -= 0.30f * SPEED;
    if (_clean < 30.0f)  _mood -= 0.20f * SPEED;
    if (_energy < 20.0f) _mood -= 0.20f * SPEED;
  }

  _hunger = clampStat(_hunger);
  _mood   = clampStat(_mood);
  _energy = clampStat(_energy);
  _clean  = clampStat(_clean);
}

PetMood Pet::moodState() const {
  if (_sleeping) return PetMood::SLEEPING;
  if (_hunger < 10.0f && _clean < 10.0f) return PetMood::SICK;
  if (_clean < 20.0f) return PetMood::DIRTY;
  if (_hunger < 20.0f) return PetMood::HUNGRY;
  if (_energy < 15.0f) return PetMood::TIRED;
  if (_mood < 25.0f) return PetMood::SAD;
  if (_mood > 70.0f) return PetMood::HAPPY;
  return PetMood::NEUTRAL;
}

void Pet::feed() {
  _sleeping = false;
  _hunger += FEED_HUNGER;
  _mood += FEED_MOOD;
  _clean += FEED_CLEAN;
  _hunger = clampStat(_hunger);
  _mood   = clampStat(_mood);
  _clean  = clampStat(_clean);
  _dirty = true;
}

void Pet::play() {
  _sleeping = false;
  _mood += PLAY_MOOD;
  _energy += PLAY_ENERGY;
  _hunger += PLAY_HUNGER;
  _mood   = clampStat(_mood);
  _energy = clampStat(_energy);
  _hunger = clampStat(_hunger);
  _dirty = true;
}

void Pet::clean() {
  _sleeping = false;
  _clean += CLEAN_GAIN;
  _mood += CLEAN_MOOD;
  _clean = clampStat(_clean);
  _mood  = clampStat(_mood);
  _dirty = true;
}

void Pet::toggleSleep() {
  _sleeping = !_sleeping;
  if (!_sleeping) {
    _mood = clampStat(_mood + 2.0f);
  }
  _dirty = true;
}

void Pet::pat() {
  _mood = clampStat(_mood + PAT_MOOD);
  _dirty = true;
}

void Pet::save(bool force) {
  if (!force && !_dirty) return;

  _prefs.putFloat("hunger", _hunger);
  _prefs.putFloat("mood", _mood);
  _prefs.putFloat("energy", _energy);
  _prefs.putFloat("clean", _clean);
  _prefs.putBool("sleep", _sleeping);
  _prefs.putUInt("age", _ageSec);

  _lastSaveMs = millis();
  _dirty = false;
  Serial.println("[pet] saved");
}
