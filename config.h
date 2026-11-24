#pragma once
#include <Arduino.h>

// ==================== 硬件引脚（TTGO T-Display，官方 Setup25） ====================
constexpr uint8_t PIN_BTN_A = 35;   // 左键：只能输入，板上已有外部上拉
constexpr uint8_t PIN_BTN_B = 0;    // 右键：BOOT 键，运行时可当普通按键

// ==================== 屏幕 ====================
constexpr int16_t PANEL_W = 135;          // 面板原生宽（竖屏）
constexpr int16_t PANEL_H = 240;          // 面板原生高（竖屏）
constexpr int16_t SCREEN_W = 240;         // setRotation(1) 后的宽
constexpr int16_t SCREEN_H = 135;         // setRotation(1) 后的高
constexpr uint8_t SCREEN_ROTATION = 1;    // 画面上下颠倒时改成 3

// ==================== 时间 ====================
constexpr uint32_t TICK_MS = 30000;       // 每 30 秒结算一次属性
constexpr uint32_t FRAME_MS = 40;         // 约 25 帧/秒
constexpr uint32_t SAVE_MS = 60000;       // 每 60 秒自动存档
constexpr uint32_t LONG_PRESS_MS = 800;   // 长按判定
constexpr uint32_t DEBOUNCE_MS = 25;      // 按键消抖

// ==================== 调试开关 ====================
// 改成 1：所有属性变化加快 10 倍，几分钟就能看到宠物饿/脏/困。
// 正常使用时保持 0。
#define TEST_FAST 0
constexpr float SPEED = TEST_FAST ? 10.0f : 1.0f;

// ==================== 属性衰减（每个 tick = 30 秒） ====================
constexpr float HUNGER_DECAY   = 0.25f * SPEED;   // 约 0.5/分钟
constexpr float ENERGY_DECAY   = 0.15f * SPEED;   // 约 0.3/分钟
constexpr float CLEAN_DECAY    = 0.125f * SPEED;  // 约 0.25/分钟
constexpr float MOOD_DECAY     = 0.10f * SPEED;   // 约 0.2/分钟
constexpr float ENERGY_RECOVER = 1.00f * SPEED;   // 睡觉时每个 tick 恢复

// ==================== 动作效果 ====================
constexpr float FEED_HUNGER = 40.0f;
constexpr float FEED_MOOD   = 5.0f;
constexpr float FEED_CLEAN  = -4.0f;
constexpr float PLAY_MOOD   = 25.0f;
constexpr float PLAY_ENERGY = -15.0f;
constexpr float PLAY_HUNGER = -6.0f;
constexpr float CLEAN_GAIN  = 50.0f;
constexpr float CLEAN_MOOD  = 8.0f;
constexpr float PAT_MOOD    = 8.0f;

// ==================== 存档 ====================
constexpr char NVS_NAMESPACE[] = "zhuochong";
