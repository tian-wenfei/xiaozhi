#include "face.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// ==================== 颜色 ====================
constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

namespace {

constexpr uint16_t C_SKY[9] = {
  rgb565(10, 16, 30), rgb565(12, 19, 35), rgb565(14, 22, 41),
  rgb565(17, 26, 48), rgb565(20, 30, 55), rgb565(23, 34, 62),
  rgb565(26, 38, 69), rgb565(29, 42, 76), rgb565(32, 46, 83)
};

constexpr uint16_t C_GROUND    = rgb565(18, 28, 45);
constexpr uint16_t C_SHADOW    = rgb565(9, 14, 24);
constexpr uint16_t C_STAR_DIM  = rgb565(70, 86, 110);
constexpr uint16_t C_PANEL     = rgb565(24, 36, 56);
constexpr uint16_t C_PANEL_HI  = rgb565(44, 62, 92);
constexpr uint16_t C_TEXT      = rgb565(232, 241, 242);
constexpr uint16_t C_DIM       = rgb565(132, 152, 174);
constexpr uint16_t C_ACCENT    = rgb565(255, 209, 102);
constexpr uint16_t C_BAR_BG    = rgb565(38, 52, 74);

constexpr uint16_t C_BODY       = rgb565(111, 231, 200);
constexpr uint16_t C_BODY_TIRED = rgb565(140, 170, 230);
constexpr uint16_t C_BODY_SAD   = rgb565(120, 190, 210);
constexpr uint16_t C_BODY_DIRTY = rgb565(150, 178, 150);
constexpr uint16_t C_BODY_SICK  = rgb565(160, 172, 140);
constexpr uint16_t C_BODY_SLEEP = rgb565(70, 140, 125);

constexpr uint16_t C_WHITE  = rgb565(255, 255, 255);
constexpr uint16_t C_EYE    = rgb565(26, 26, 46);
constexpr uint16_t C_CHEEK  = rgb565(255, 150, 170);
constexpr uint16_t C_MOUTH  = rgb565(60, 48, 66);
constexpr uint16_t C_DIRT   = rgb565(150, 110, 70);
constexpr uint16_t C_SWEAT  = rgb565(120, 200, 255);
constexpr uint16_t C_HEART  = rgb565(255, 107, 157);

constexpr uint16_t C_HUNGER = rgb565(255, 159, 67);
constexpr uint16_t C_MOOD   = rgb565(255, 107, 157);
constexpr uint16_t C_ENERGY = rgb565(255, 209, 102);
constexpr uint16_t C_CLEAN  = rgb565(78, 205, 196);

constexpr uint16_t C_FOOD      = rgb565(255, 92, 92);
constexpr uint16_t C_STEM      = rgb565(120, 80, 50);
constexpr uint16_t C_LEAF      = rgb565(96, 200, 120);
constexpr uint16_t C_BALL_LINE = rgb565(240, 240, 240);
constexpr uint16_t C_BUBBLE    = rgb565(180, 235, 255);

// ==================== 小工具 ====================
uint16_t shiftColor(uint16_t c, int dr, int dg, int db) {
  int r = (c >> 11) & 0x1F;
  int g = (c >> 5) & 0x3F;
  int b = c & 0x1F;
  r += dr; g += dg; b += db;
  if (r < 0) r = 0; if (r > 31) r = 31;
  if (g < 0) g = 0; if (g > 63) g = 63;
  if (b < 0) b = 0; if (b > 31) b = 31;
  return (uint16_t)((r << 11) | (g << 5) | b);
}

void drawHeart(TFT_eSPI &g, int x, int y, int s, uint16_t color) {
  g.fillCircle(x - s / 2, y, s / 2 + 1, color);
  g.fillCircle(x + s / 2, y, s / 2 + 1, color);
  g.fillTriangle(x - s, y + s / 4, x + s, y + s / 4, x, y + s, color);
}

// 用多条短线拼出一条抛物线：depth > 0 是笑，depth < 0 是哭
void drawMouthCurve(TFT_eSPI &g, int cx, int cy, int half, int depth, uint16_t color) {
  int px = cx - half;
  int py = cy;
  for (int i = 1; i <= 8; i++) {
    float t = (float)i / 8.0f * 2.0f - 1.0f;
    int x = cx + (int)(t * half);
    int y = cy + (int)(depth * (1.0f - t * t));
    g.drawLine(px, py, x, y, color);
    g.drawLine(px, py + 1, x, y + 1, color);
    px = x;
    py = y;
  }
}

// ==================== 背景 ====================
void drawBackground(TFT_eSPI &g, uint32_t now) {
  for (int i = 0; i < 9; i++) {
    g.fillRect(0, i * 10, SCREEN_W, 10, C_SKY[i]);
  }
  g.fillRect(0, 90, SCREEN_W, SCREEN_H - 90, C_GROUND);

  static const int16_t sx[7] = {16, 40, 66, 92, 118, 132, 24};
  static const int16_t sy[7] = {10, 22, 8, 18, 9, 28, 34};
  for (int i = 0; i < 7; i++) {
    uint16_t c = ((now / 600 + i) % 3 == 0) ? C_WHITE : C_STAR_DIM;
    g.fillRect(sx[i], sy[i], 2, 2, c);
  }
}

// ==================== 眼睛 ====================
void drawOpenEye(TFT_eSPI &g, int x, int y, int r, int lookX, int lookY, bool blink) {
  if (blink) {
    g.drawLine(x - r, y, x, y + 2, C_EYE);
    g.drawLine(x, y + 2, x + r, y, C_EYE);
    return;
  }
  g.fillCircle(x, y, r, C_WHITE);
  g.fillCircle(x + lookX, y + lookY, 4, C_EYE);
  g.fillCircle(x + lookX - 2, y + lookY - 2, 1, C_WHITE);
}

void drawEyes(TFT_eSPI &g, PetMood mood, int cx, int cy, bool blink, uint16_t body) {
  const int ex1 = cx - 15;
  const int ex2 = cx + 15;
  const int ey = cy - 4;
  const int r = 9;
  const int lookX = (int)(sinf(millis() / 1700.0f) * 2.0f);
  const int lookY = (int)(cosf(millis() / 2100.0f) * 1.5f);

  switch (mood) {
    case PetMood::SLEEPING:
      g.drawLine(ex1 - r, ey, ex1, ey + 3, C_EYE);
      g.drawLine(ex1, ey + 3, ex1 + r, ey, C_EYE);
      g.drawLine(ex2 - r, ey, ex2, ey + 3, C_EYE);
      g.drawLine(ex2, ey + 3, ex2 + r, ey, C_EYE);
      break;

    case PetMood::DIRTY:
      // > <
      g.drawLine(ex1 - r, ey - r, ex1 + r, ey, C_EYE);
      g.drawLine(ex1 - r, ey + r, ex1 + r, ey, C_EYE);
      g.drawLine(ex2 + r, ey - r, ex2 - r, ey, C_EYE);
      g.drawLine(ex2 + r, ey + r, ex2 - r, ey, C_EYE);
      break;

    case PetMood::SICK:
      // x x
      g.drawLine(ex1 - r, ey - r, ex1 + r, ey + r, C_EYE);
      g.drawLine(ex1 - r, ey + r, ex1 + r, ey - r, C_EYE);
      g.drawLine(ex2 - r, ey - r, ex2 + r, ey + r, C_EYE);
      g.drawLine(ex2 - r, ey + r, ex2 + r, ey - r, C_EYE);
      break;

    case PetMood::TIRED:
      // 半睁眼：画完整眼睛，再用身体颜色盖住上半部分
      g.fillCircle(ex1, ey, r, C_WHITE);
      g.fillCircle(ex1 + lookX, ey + lookY + 2, 4, C_EYE);
      g.fillRect(ex1 - r - 1, ey - r - 1, r * 2 + 2, r + 1, body);
      g.drawLine(ex1 - r, ey, ex1 + r, ey, C_EYE);

      g.fillCircle(ex2, ey, r, C_WHITE);
      g.fillCircle(ex2 + lookX, ey + lookY + 2, 4, C_EYE);
      g.fillRect(ex2 - r - 1, ey - r - 1, r * 2 + 2, r + 1, body);
      g.drawLine(ex2 - r, ey, ex2 + r, ey, C_EYE);
      break;

    default:
      drawOpenEye(g, ex1, ey, r, lookX, lookY, blink);
      drawOpenEye(g, ex2, ey, r, lookX, lookY, blink);
      break;
  }
}

// ==================== 嘴 ====================
void drawMouth(TFT_eSPI &g, PetMood mood, int cx, int cy) {
  switch (mood) {
    case PetMood::HAPPY:
      drawMouthCurve(g, cx, cy + 15, 12, 6, C_MOUTH);
      break;
    case PetMood::NEUTRAL:
      drawMouthCurve(g, cx, cy + 15, 7, 2, C_MOUTH);
      break;
    case PetMood::HUNGRY:
      g.fillRoundRect(cx - 6, cy + 11, 12, 10, 5, C_MOUTH);
      break;
    case PetMood::TIRED:
      g.drawLine(cx - 6, cy + 16, cx + 6, cy + 16, C_MOUTH);
      break;
    case PetMood::DIRTY:
      g.drawLine(cx - 8, cy + 15, cx - 3, cy + 13, C_MOUTH);
      g.drawLine(cx - 3, cy + 13, cx + 3, cy + 17, C_MOUTH);
      g.drawLine(cx + 3, cy + 17, cx + 8, cy + 15, C_MOUTH);
      break;
    case PetMood::SAD:
      drawMouthCurve(g, cx, cy + 18, 8, -5, C_MOUTH);
      break;
    case PetMood::SICK:
      drawMouthCurve(g, cx, cy + 17, 7, -3, C_MOUTH);
      break;
    case PetMood::SLEEPING:
      g.fillCircle(cx, cy + 15, 3, C_MOUTH);
      break;
  }
}

// ==================== 宠物本体 ====================
void drawPet(TFT_eSPI &g, const Pet &pet, const UiState &ui, uint32_t now) {
  static uint32_t nextBlink = 0;
  static uint32_t blinkUntil = 0;

  if (nextBlink == 0) nextBlink = now + 1500;
  if ((int32_t)(now - nextBlink) >= 0) {
    blinkUntil = now + 120;
    nextBlink = now + 2200 + random(1800);
  }
  bool blinking = (int32_t)(now - blinkUntil) < 0;

  const PetMood mood = pet.moodState();
  const int cx = 72;
  const int cy = 58 + (int)(sinf(now / 650.0f) * 2.0f);

  uint16_t body = C_BODY;
  switch (mood) {
    case PetMood::TIRED:    body = C_BODY_TIRED; break;
    case PetMood::SAD:      body = C_BODY_SAD;   break;
    case PetMood::DIRTY:    body = C_BODY_DIRTY; break;
    case PetMood::SICK:     body = C_BODY_SICK;  break;
    case PetMood::SLEEPING: body = C_BODY_SLEEP; break;
    default: break;
  }
  const uint16_t bodyDark = shiftColor(body, -5, -9, -5);
  const uint16_t belly = shiftColor(body, +9, +11, +7);

  // 地面阴影
  g.fillRoundRect(cx - 26, 89, 52, 6, 3, C_SHADOW);

  // 触角（画在身体后面）
  g.drawLine(cx - 20, cy - 26, cx - 27, cy - 45, bodyDark);
  g.fillCircle(cx - 27, cy - 45, 5, C_ACCENT);
  g.drawLine(cx + 20, cy - 26, cx + 27, cy - 45, bodyDark);
  g.fillCircle(cx + 27, cy - 45, 5, C_ACCENT);

  // 身体 + 肚子
  g.fillRoundRect(cx - 42, cy - 31, 84, 62, 26, body);
  g.fillRoundRect(cx - 25, cy + 3, 50, 26, 13, belly);

  // 脏污
  if (mood == PetMood::DIRTY) {
    g.fillCircle(cx - 30, cy + 18, 3, C_DIRT);
    g.fillCircle(cx + 30, cy + 16, 3, C_DIRT);
    g.fillCircle(cx + 4, cy + 26, 3, C_DIRT);
  }

  // 生病时额头的汗滴
  if (mood == PetMood::SICK) {
    g.fillCircle(cx + 32, cy - 20, 3, C_SWEAT);
    g.fillTriangle(cx + 29, cy - 22, cx + 35, cy - 22, cx + 32, cy - 30, C_SWEAT);
  }

  // 腮红
  g.fillCircle(cx - 29, cy + 9, 5, C_CHEEK);
  g.fillCircle(cx + 29, cy + 9, 5, C_CHEEK);

  drawEyes(g, mood, cx, cy, blinking, body);
  drawMouth(g, mood, cx, cy);

  // 睡觉的 Z
  if (mood == PetMood::SLEEPING) {
    int off = (int)((now / 500) % 3);
    g.setTextDatum(TL_DATUM);
    g.setTextFont(1);
    g.setTextColor(C_WHITE);
    g.drawString("z", cx + 30, cy - 30 - off * 3);
    g.drawString("z", cx + 40, cy - 36 - off * 3);
    g.drawString("Z", cx + 50, cy - 44 - off * 2);
  }

  // 摸摸头时飘心
  if (ui.heartUntil > now) {
    for (int i = 0; i < 3; i++) {
      uint32_t t = ((now / 220) + i * 2) % 6;
      int x = cx - 22 + i * 22;
      int y = cy - 34 - (int)t * 4;
      drawHeart(g, x, y, 5, C_HEART);
    }
  }
}

// ==================== 右侧属性栏 ====================
void drawStatBar(TFT_eSPI &g, int x, int y, const char *label, float value, uint16_t color) {
  g.setTextDatum(TL_DATUM);
  g.setTextFont(1);
  g.setTextColor(C_DIM);
  g.drawString(label, x, y);

  const int w = 78;
  g.fillRoundRect(x, y + 9, w, 6, 3, C_BAR_BG);
  int fill = (int)(w * value / 100.0f + 0.5f);
  if (fill < 0) fill = 0;
  if (fill > w) fill = w;
  if (fill > 0) g.fillRoundRect(x, y + 9, fill, 6, 3, color);
}

void drawStats(TFT_eSPI &g, const Pet &pet) {
  g.fillRoundRect(146, 4, 90, 94, 8, C_PANEL);
  g.drawRoundRect(146, 4, 90, 94, 8, C_PANEL_HI);

  drawStatBar(g, 152, 10, "HUNGER", pet.hunger(), C_HUNGER);
  drawStatBar(g, 152, 30, "MOOD",   pet.moodValue(), C_MOOD);
  drawStatBar(g, 152, 50, "ENERGY", pet.energy(), C_ENERGY);
  drawStatBar(g, 152, 70, "CLEAN",  pet.clean(), C_CLEAN);

  char buf[24];
  uint32_t d = pet.ageSeconds() / 86400UL;
  uint32_t h = (pet.ageSeconds() % 86400UL) / 3600UL;
  snprintf(buf, sizeof(buf), "AGE %lud %luh", (unsigned long)d, (unsigned long)h);

  g.setTextDatum(TL_DATUM);
  g.setTextFont(1);
  g.setTextColor(C_DIM);
  g.drawString(buf, 152, 88);
}

// ==================== 底部菜单栏 ====================
void drawMenuIcon(TFT_eSPI &g, int idx, int cx, int cy, uint16_t bg) {
  switch (idx) {
    case 0:  // 苹果
      g.fillCircle(cx, cy + 2, 8, C_FOOD);
      g.drawLine(cx, cy - 6, cx + 1, cy - 11, C_STEM);
      g.fillTriangle(cx + 1, cy - 10, cx + 8, cy - 13, cx + 5, cy - 6, C_LEAF);
      g.fillCircle(cx - 3, cy - 1, 2, C_WHITE);
      break;
    case 1:  // 球
      g.fillCircle(cx, cy, 8, C_ACCENT);
      g.drawLine(cx - 6, cy - 3, cx + 6, cy + 3, C_BALL_LINE);
      g.drawLine(cx - 6, cy + 3, cx + 6, cy - 3, C_BALL_LINE);
      break;
    case 2:  // 泡泡
      g.drawCircle(cx - 4, cy + 3, 6, C_BUBBLE);
      g.drawCircle(cx + 5, cy - 3, 4, C_BUBBLE);
      g.fillCircle(cx - 4, cy + 3, 1, C_BUBBLE);
      break;
    case 3:  // 月亮（用单元格背景色挖出月牙）
      g.fillCircle(cx, cy, 8, C_ACCENT);
      g.fillCircle(cx + 4, cy - 3, 7, bg);
      break;
  }
}

void drawBottomBar(TFT_eSPI &g, const UiState &ui, uint32_t now) {
  g.fillRect(0, 102, SCREEN_W, SCREEN_H - 102, C_PANEL);
  g.drawLine(0, 102, SCREEN_W - 1, 102, C_PANEL_HI);

  if (ui.menuOpen) {
    const char *labels[4] = {"FEED", "PLAY", "CLEAN", "SLEEP"};
    for (uint8_t i = 0; i < 4; i++) {
      int x = 4 + i * 58;
      int y = 104;
      bool sel = (i == ui.selected);
      if (sel) {
        g.fillRoundRect(x, y, 54, 29, 7, C_PANEL_HI);
        g.drawRoundRect(x, y, 54, 29, 7, C_ACCENT);
      }
      drawMenuIcon(g, i, x + 27, 112, sel ? C_PANEL_HI : C_PANEL);
      g.setTextDatum(MC_DATUM);
      g.setTextFont(1);
      g.setTextColor(sel ? C_ACCENT : C_TEXT);
      g.drawString(labels[i], x + 27, 124);
    }
    return;
  }

  g.setTextDatum(MC_DATUM);
  if (ui.toastUntil > now && ui.toast[0] != '\0') {
    g.setTextFont(2);
    g.setTextColor(C_ACCENT);
    g.drawString(ui.toast, SCREEN_W / 2, 118);
  } else {
    g.setTextFont(1);
    g.setTextColor(C_DIM);
    g.drawString("A=PAT   B=MENU   HOLD B=SLEEP", SCREEN_W / 2, 118);
  }
}

}  // namespace

// ==================== 对外接口 ====================
void renderScene(TFT_eSPI &g, const Pet &pet, const UiState &ui, uint32_t now) {
  drawBackground(g, now);
  drawPet(g, pet, ui, now);
  drawStats(g, pet);
  drawBottomBar(g, ui, now);
}

void showToast(UiState &ui, const char *msg, uint32_t now, uint32_t ms) {
  strncpy(ui.toast, msg, sizeof(ui.toast) - 1);
  ui.toast[sizeof(ui.toast) - 1] = '\0';
  ui.toastUntil = now + ms;
}
