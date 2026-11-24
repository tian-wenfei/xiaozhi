#include "input.h"

void buttonBegin(Button &b, uint8_t pin, bool usePullup) {
  b.pin = pin;
  pinMode(pin, usePullup ? INPUT_PULLUP : INPUT);
  b.stableLevel = digitalRead(pin);
  b.lastRaw = b.stableLevel;
  b.lastChangeMs = millis();
  b.pressStartMs = millis();
  b.longFired = false;
}

// 每个 loop 调用一次。返回 SHORT 或 LONG，每次按下最多各触发一次。
BtnEvent buttonPoll(Button &b, uint32_t now) {
  bool raw = digitalRead(b.pin);

  if (raw != b.lastRaw) {
    b.lastRaw = raw;
    b.lastChangeMs = now;
  }

  // 电平稳定 DEBOUNCE_MS 后才承认变化
  if ((uint32_t)(now - b.lastChangeMs) >= DEBOUNCE_MS && raw != b.stableLevel) {
    b.stableLevel = raw;
    if (b.stableLevel == LOW) {
      b.pressStartMs = now;
      b.longFired = false;
    } else {
      // 松手：没触发过长按才算短按
      if (!b.longFired) return BtnEvent::SHORT;
    }
  }

  // 按住超过阈值，立刻触发一次长按
  if (b.stableLevel == LOW && !b.longFired &&
      (uint32_t)(now - b.pressStartMs) >= LONG_PRESS_MS) {
    b.longFired = true;
    return BtnEvent::LONG;
  }

  return BtnEvent::NONE;
}
