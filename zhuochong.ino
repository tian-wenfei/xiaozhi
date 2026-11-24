#include <TFT_eSPI.h>
#include "config.h"
#include "pet.h"
#include "input.h"
#include "face.h"

TFT_eSPI tft(PANEL_W, PANEL_H);            // 面板原生 135x240，setRotation(1) 后是 240x135
TFT_eSprite spr(&tft);                      // 离屏缓冲，防闪烁
Pet pet;
UiState ui;
Button btnA;
Button btnB;

bool spriteReady = false;
uint32_t lastFrameMs = 0;

void handleInput(BtnEvent evA, BtnEvent evB, uint32_t now);
void applyAction(uint8_t idx, uint32_t now);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("==========================================");
  Serial.println("  Zhuochong desktop pet  v1 (offline)");
  Serial.println("  A = pat   B = menu   hold B = sleep");
  Serial.println("==========================================");

  tft.init();
  tft.setRotation(SCREEN_ROTATION);
  tft.fillScreen(TFT_BLACK);

  // 开机画面
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("ZHUO CHONG", SCREEN_W / 2, SCREEN_H / 2 - 10);
  tft.setTextFont(1);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString("v1  offline pet", SCREEN_W / 2, SCREEN_H / 2 + 18);
  delay(600);

  // 240x135x16bit = 约 65KB 内存；失败就退化成直接画屏（会有轻微闪烁）
  spr.setColorDepth(16);
  spriteReady = (spr.createSprite(SCREEN_W, SCREEN_H) != nullptr);

  buttonBegin(btnA, PIN_BTN_A, false);  // GPIO35 板上已有外部上拉
  buttonBegin(btnB, PIN_BTN_B, true);   // GPIO0 是 BOOT 键，用内部上拉

  pet.begin();

  Serial.printf("[sys] sprite=%s  free heap=%u\n",
                spriteReady ? "OK" : "FAIL", (unsigned)ESP.getFreeHeap());
  if (!spriteReady) {
    Serial.println("[sys] sprite 分配失败，改为直接绘制");
  }

  lastFrameMs = millis();
}

void loop() {
  uint32_t now = millis();

  pet.update(now);

  BtnEvent evA = buttonPoll(btnA, now);
  BtnEvent evB = buttonPoll(btnB, now);
  if (evA != BtnEvent::NONE || evB != BtnEvent::NONE) {
    handleInput(evA, evB, now);
  }

  if ((uint32_t)(now - lastFrameMs) >= FRAME_MS) {
    lastFrameMs = now;
    if (spriteReady) {
      renderScene(spr, pet, ui, now);
      spr.pushSprite(0, 0);
    } else {
      renderScene(tft, pet, ui, now);
    }
  }
}

void handleInput(BtnEvent evA, BtnEvent evB, uint32_t now) {
  if (ui.menuOpen) {
    // 菜单打开时：B 切换项目，A 执行，长按 B 关闭
    if (evB == BtnEvent::SHORT) {
      ui.selected = (ui.selected + 1) % 4;
    } else if (evB == BtnEvent::LONG) {
      ui.menuOpen = false;
    } else if (evA == BtnEvent::SHORT) {
      applyAction(ui.selected, now);
      ui.menuOpen = false;
    }
    return;
  }

  // 主界面：A 摸摸，B 打开菜单，长按 B 睡觉/唤醒
  if (evA == BtnEvent::SHORT) {
    pet.pat();
    ui.heartUntil = now + 1500;
    showToast(ui, "PAT PAT", now, 1200);
    pet.save(true);
  } else if (evB == BtnEvent::SHORT) {
    ui.menuOpen = true;
    ui.selected = 0;
  } else if (evB == BtnEvent::LONG) {
    pet.toggleSleep();
    showToast(ui, pet.sleeping() ? "ZZZ..." : "WAKE UP", now);
    pet.save(true);
  }
}

void applyAction(uint8_t idx, uint32_t now) {
  switch (idx) {
    case 0:
      pet.feed();
      showToast(ui, "YUM!", now);
      break;
    case 1:
      pet.play();
      showToast(ui, "FUN!", now);
      ui.heartUntil = now + 1200;
      break;
    case 2:
      pet.clean();
      showToast(ui, "CLEAN!", now);
      break;
    case 3:
      pet.toggleSleep();
      showToast(ui, pet.sleeping() ? "ZZZ..." : "WAKE UP", now);
      break;
  }
  pet.save(true);
}
