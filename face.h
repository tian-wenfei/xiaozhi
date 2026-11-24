#pragma once
#include <TFT_eSPI.h>
#include "pet.h"

// 界面层的临时状态：菜单、飘心、提示文字
struct UiState {
  bool menuOpen = false;
  uint8_t selected = 0;
  uint32_t heartUntil = 0;
  char toast[20] = {0};
  uint32_t toastUntil = 0;
};

// 把整个画面画到 g 上。g 可以是 TFT_eSPI，也可以是 TFT_eSprite。
void renderScene(TFT_eSPI &g, const Pet &pet, const UiState &ui, uint32_t now);

// 在底部栏显示一句话，持续 ms 毫秒
void showToast(UiState &ui, const char *msg, uint32_t now, uint32_t ms = 1600);
