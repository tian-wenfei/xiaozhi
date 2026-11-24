# 桌宠 Zhuochong v1（离线版）

> **注意：这是旧方案（T-Display 按键版）。** 需求已改为“免按键语音桌宠”，新方案见 [VOICE_PET_PLAN.md](VOICE_PET_PLAN.md)。本文件保留作学习参考，不再作为主方案。

一个跑在 **LilyGO TTGO T-Display** 上的桌面电子宠物。屏幕里是一只纯代码绘制的小生物：会眨眼、呼吸、随饥饿/心情/精力/清洁度改变表情。两个按键就能喂食、玩耍、清洁、睡觉。

v1 不联网；联网 + AI 对话放在 v2。

> 验证状态：已用 Arduino CLI + ESP32 core 2.0.14 + LilyGO TFT_eSPI 2.2.20 编译通过（Flash 338 KB / 25%，全局变量 22 KB / 6%）。还没有在实物板子上跑过；第一次烧录如果报错，把 Arduino IDE 下方的完整报错或串口输出发给我。

---

## 一、采购清单

| 物品 | 说明 | 大概价格 |
|---|---|---|
| LilyGO TTGO T-Display（ESP32 版） | 1.14 寸 135×240 彩屏、2 个按键、USB-C、4MB Flash | 60–100 元 |
| USB-C 数据线 | **必须能传数据**，很多充电线不行 | 5–15 元 |
| 3.7V 锂电池（可选） | v1 用 USB 供电即可，以后做无线版再买 | 15–30 元 |

搜索关键词：`LilyGO T-Display ESP32 1.14`。
**不要买** T-Display-S3、T-Display-AMOLED，引脚和开发板设置都不一样。

不需要：面包板、杜邦线、电烙铁。

---

## 二、安装开发环境（只做一次）

### 1. 安装 Arduino IDE

到 https://www.arduino.cc/en/software 下载 **Arduino IDE 2.x**，按默认选项安装。

### 2. 安装 ESP32 开发板支持 2.0.14

1. 打开 Arduino IDE → 文件 → 首选项
2. 在「附加开发板管理器网址」里填入：
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. 工具 → 开发板 → 开发板管理器
4. 搜索 `esp32`，找到 **esp32 by Espressif Systems**
5. 点版本下拉框，**选择 2.0.14**，点安装

> 必须用 2.0.14。不要装 3.x，T-Display 的示例库和它不兼容。

### 3. 安装 TFT_eSPI（用 LilyGO 仓库里的版本）

1. 打开 https://github.com/Xinyuan-LilyGO/TTGO-T-Display
2. 点绿色 **Code → Download ZIP**
3. 解压后，把里面的 **`TFT_eSPI` 文件夹**整个复制到：
   ```
   C:\Users\22773\Documents\Arduino\libraries\
   ```
4. 确认路径长这样（不能再套一层）：
   ```
   C:\Users\22773\Documents\Arduino\libraries\TFT_eSPI\library.properties
   ```
5. LilyGO 仓库里的 `User_Setup_Select.h` **已经默认启用了 Setup25（T-Display）**，不需要改任何引脚。

> 如果以前用「库管理器」装过 TFT_eSPI，请先把 `libraries` 里旧的 TFT_eSPI 文件夹删掉，避免版本冲突。

### 4. 装 USB 驱动

插上板子，打开「设备管理器 → 端口」：

- 出现 `COMx`：正常，跳过
- 出现黄色感叹号：去 WCH 官网下载 CH340/CH343 驱动安装（LilyGO README 里也有链接）
- 有些批次是 CP210x 芯片，装 Silicon Labs 的 CP210x 驱动

---

## 三、烧录

1. 用 USB-C 数据线把板子连到电脑
2. Arduino IDE → 文件 → 打开 → `C:\Users\22773\Desktop\zhuochong\zhuochong.ino`
3. 工具菜单按下面设置：

| 菜单项 | 选择 |
|---|---|
| 开发板 | ESP32 Arduino → **ESP32 Dev Module** |
| Flash Size | **4MB (32Mb)** |
| PSRAM | **Disable** |
| Partition Scheme | **Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)** |
| Upload Speed | 921600（不稳定就换 460800） |
| Port | 你的 COMx |

4. 点左上角 **→**（上传）
5. 看到 `Connecting........_____` 一直连不上时：**按住板子上的 BOOT 键（GPIO0），点上传，等出现 `Writing at...` 再松开**

烧录成功后，屏幕先显示 `ZHUO CHONG`，约 1 秒后进入宠物画面。串口监视器设为 **115200** 可以看到运行日志。

---

## 四、怎么玩

板子上有两个按键：

- **A 键**：标着 `35` / `IO35` 的那个
- **B 键**：标着 `0` / `BOOT` 的那个

| 操作 | 按键 | 效果 |
|---|---|---|
| 摸摸头 | 主界面 A 短按 | 心情 +8，头顶飘心 |
| 打开菜单 | 主界面 B 短按 | 底部出现 FEED / PLAY / CLEAN / SLEEP |
| 切换菜单项 | 菜单中 B 短按 | 选中下一项 |
| 执行 | 菜单中 A 短按 | 执行当前项并关闭菜单 |
| 关闭菜单 | 菜单中 B 长按 | 返回主界面 |
| 睡觉 / 唤醒 | 主界面 B 长按 | 切换睡眠状态 |

菜单项效果：

| 菜单 | 效果 |
|---|---|
| FEED | 饥饿 +40，心情 +5，清洁 -4 |
| PLAY | 心情 +25，精力 -15，饥饿 -6 |
| CLEAN | 清洁 +50，心情 +8 |
| SLEEP | 进入/退出睡眠；睡眠时精力恢复，饿得慢 |

右侧四条状态条：**HUNGER（饥饿）、MOOD（心情）、ENERGY（精力）、CLEAN（清洁）**。
任意一条掉到很低，宠物会变成对应的表情（饿、困、脏、难过、生病）。

> v1 的界面用英文，因为中文字库要占几百 KB Flash，放在 v2 一起做。

---

## 五、想改数值 / 加速测试

打开 `config.h`：

```cpp
#define TEST_FAST 0   // 改成 1：所有属性变化快 10 倍，几分钟就能看到宠物变饿/变脏
```

其他常用参数：

| 参数 | 作用 |
|---|---|
| `SCREEN_ROTATION` | 画面上下颠倒时，从 1 改成 3 |
| `HUNGER_DECAY` 等 | 各项属性每 30 秒掉多少 |
| `SAVE_MS` | 自动存档间隔（默认 60 秒） |
| `LONG_PRESS_MS` | 长按判定时间（默认 800ms） |

改完重新点上传即可。

---

## 六、文件说明

```
zhuochong.ino    主程序：setup/loop、按键分发、动作执行
config.h         所有可调参数：引脚、时间、衰减速度、动作效果
pet.h / pet.cpp  宠物的四项属性、状态机、NVS 存档
face.h / face.cpp 画面：背景、眼睛、嘴、表情、状态条、底部菜单
input.h / input.cpp 按键消抖 + 短按/长按识别
README.md        本文件
```

数据存在 ESP32 的 NVS 里：拔电重启后属性、年龄、睡眠状态都会保留。
**注意**：v1 没有 RTC，关机期间宠物不衰减；v2 联网后用网络时间补算。

---

## 七、常见问题

| 现象 | 原因 / 解决 |
|---|---|
| 编译报 `TFT_eSPI.h: No such file` | 库没放在 `Documents\Arduino\libraries\TFT_eSPI\`，或路径多套了一层 |
| 编译报一大堆 TFT_eSPI 错误 | ESP32 core 不是 2.0.14，或 libraries 里有两个 TFT_eSPI |
| 上传失败 / 找不到端口 | 先换一根**数据线**；按住 BOOT 再上传；关掉串口监视器 |
| 屏幕白屏或花屏 | `User_Setup_Select.h` 没有启用 Setup25；或买成了 S3 版本 |
| 画面上下颠倒 | `config.h` 里 `SCREEN_ROTATION` 改成 3 |
| 颜色发蓝/发红 | 在 `Setup25_TTGO_T_Display.h` 里试 `TFT_RGB_ORDER TFT_BGR` 或 `TFT_INVERSION_ON` |
| 按键没反应 | 打开串口看有没有 `[sys]` 日志；A 键不要用 `INPUT_PULLUP`（GPIO35 只能输入） |
| 屏幕一直闪 | 开串口看 `sprite=FAIL`，说明内存分配失败；先关掉 WiFi 相关代码再试 |
| 烧录后不断重启 | 供电不足。换电脑 USB 口或足量 5V 电源，别用劣质充电宝 |

---

## 八、下一步（v2）

1. WiFi 配网：手机连设备热点，填家里 WiFi（只支持 2.4GHz）
2. 手机网页聊天：浏览器打开设备 IP，输入中文 → 调大模型 API → 回复显示在屏幕上
3. 中文字库：`U8g2_for_TFT_eSPI` + GB2312 12/14px 字体，分区改 **Huge APP (3MB No OTA)**
4. AI 人设：把当前饥饿/心情/精力拼进 system prompt，让回复符合宠物状态
5. 进阶：WiFiManager、TLS 根证书、NTP 补算离线时间、电池和外壳

API Key 会放在单独的 `secrets.h`，不会提交到任何仓库。

---

## 九、参考项目

- [LilyGO TTGO T-Display 官方仓库](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)（引脚、示例、TFT_eSPI）
- [TamaPoke](https://github.com/socquique/TamaPoke)——圆屏宝可梦宠物，玩法设计参考
- [TamaFi](https://github.com/cifertech/TamaFi)——ESP32 桌宠，纯代码画脸和状态机参考
- [ESP32-C3 DIY Tamagotchi](https://github.com/derdacavga/Esp32-Tamagotchi)——模块化代码结构参考
