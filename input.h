#pragma once
#include <Arduino.h>
#include "config.h"

enum class BtnEvent : uint8_t {
  NONE,
  SHORT,
  LONG
};

struct Button {
  uint8_t pin = 0;
  bool stableLevel = HIGH;     // 消抖后的电平
  bool lastRaw = HIGH;         // 上一次原始读数
  uint32_t lastChangeMs = 0;   // 原始电平最后一次变化的时间
  uint32_t pressStartMs = 0;   // 按下时刻
  bool longFired = false;      // 本次按下是否已经触发过长按
};

void buttonBegin(Button &b, uint8_t pin, bool usePullup);
BtnEvent buttonPoll(Button &b, uint32_t now);
