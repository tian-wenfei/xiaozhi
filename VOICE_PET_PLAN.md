# 低成本语音桌宠（几十元 DIY）

## 已验证的工作配置（2026-11-13）

- 主控：ESP32-S3 DevKitC-1 N16R8（16MB Flash + 8MB OPI PSRAM）
- 固件：xiaozhi v2.5.2，`bread-compact-wifi-128x64`
- 屏幕：0.96 寸 SSD1306 128×64 I2C，SDA→GPIO41，SCL→GPIO42
- 麦克风：INMP441，VDD→3V3，GND→GND，L/R→GND，SD→GPIO6，SCK→GPIO5，WS→GPIO4
- 功放：MAX98357A，VIN→5V，GND→GND，SD→3V3，GAIN→悬空，DIN→GPIO7，BCLK→GPIO15，LRC→GPIO16
- 喇叭：8Ω 2W，接功放绿色端子 +/-
- 网络：2.4GHz WiFi，已在 xiaozhi.me 绑定
- 状态：语音对话、屏幕表情、麦克风、喇叭均正常

## OLED 眼睛版固件（2026-11-13 重做表情）

- 固件：`firmware/merged-binary-eyes.bin`（ESP32-S3 / 16MB）
- SHA-256：`2f16b00c784fb38c1650303ae957d94c070cff39c00797d796b0cf995b213ba8`
- 首次/整包烧录：`powershell -ExecutionPolicy Bypass -File .\flash-eyes.ps1`
- 日常只更新表情：`powershell -ExecutionPolicy Bypass -File .\flash-eyes-app.ps1`（只写 app 分区，保留 WiFi 配置，约 30 秒）
- **注意**：整包固件里 NVS 区域是空的（0xFF），所以 `flash-eyes.ps1` 会清掉 WiFi 配网信息，刷完要重新连一次 WiFi；`flash-eyes-app.ps1` 不会。
- 平时屏幕只显示**两颗发光的小圆眼睛**；配网、绑定、通知时自动切回原来的文字界面。
- 造型参考 [Emotion Ball 表情馆](https://www.kianaria.cn/emotion-ball/)：没有瞳孔，默认是两颗小圆加一张小嘴，情绪靠眼睛的大小、倾斜、位置和形状变化，加上嘴的弯曲/开合表达；左右眼故意不完全对称。
- 待机：目光每 2–4 秒轻轻游走，偶尔眨眼，偶尔小跳一下；另外每 5–15 秒会随机做一个小动作——瞥向一侧、单眼眯一下、打哈欠、咧嘴笑一下或轻轻歪头，持续 1–2 秒。
- 听你说话：眼睛睁大、略微向上看。
- 说话：目光持续左右移动、眨眼。
- 表情细分到 20 多种：开心 / 大笑 / 恋爱 / 亲亲 / 眨眼 / 鬼脸 / 好吃 / 自信 / 难过 / 哭 / 生气 / 困 / 累 / 放松 / 思考 / 惊讶 / 震惊 / 疑惑 / 头晕 / 害羞 / 尴尬。
- 每种表情都有专门的五官组合和特效：眉毛（生气、难过、惊讶、思考时变化）、腮红（开心、害羞、恋爱、亲亲）、泪滴流下（难过一滴、哭四滴）、汗滴（累、尴尬、头晕、震惊）、飘爱心（恋爱、亲亲）、闪光（大笑、好吃、惊讶）、怒气符号（生气）、问号（疑惑）、感叹号（震惊）、旋转螺旋 + 眼睛上下晃（头晕、疑惑）、思考光点、睡眠 z z Z。
- 开心是弯弯的笑眼 + 张开的笑嘴 + 腮红，头上不再有闪烁的小星星。
- 生气、震惊、头晕时整个脸会轻微抖动；开心、大笑、恋爱、震惊等会弹跳一下。
- 说话时嘴巴会随声音一张一合，眼睛继续左右看；脸部整体上移，屏幕上下留白更均匀。
- 表情保持：小智说完后，表情会在屏幕上保留 6 秒再回默认；重新开始说话时立即切回聆听表情。串口会打印 `SetEmotion('xxx')`，方便核对服务器发来的情绪名。
- 哭：闭眼横线 + 往下掉的泪滴；睡眠：横线眼 + 右上角飘 z z Z。
- AI 回复只出声，不在屏幕上显示文字（这是“只显示眼睛”的取舍）。
- 回退到原版：运行 `flash.ps1`，它烧录的是 `firmware/merged-binary.bin`。
- 源码改动：`firmware/oled-eyes.patch`（含 OLED 显示代码和 Windows 编译参数）。
- 效果预览：`docs/eyes-preview.png`（按同样的几何参数模拟的 128×64 画面）。
- 注意：屏幕大部分是黑的，只有两颗小眼睛发光，夜里不刺眼。

### 重新编译（改代码后）

这套固件是在 xiaozhi-esp32 **v2.5.2（commit 49b4380）** 基础上打补丁编译的：

```powershell
# 1. 下载源码（如果临时目录还在可以跳过）
git clone https://github.com/78/xiaozhi-esp32.git
cd xiaozhi-esp32
git checkout 49b4380
git apply C:\Users\22773\Desktop\zhuochong\firmware\oled-eyes.patch

# 2. 编译（要用 C:\Espressif\build-eyes.bat 里配置好的 ESP-IDF 环境）
cmd /c C:\Espressif\build-eyes.bat
```

编译产物有两个：
- `build\merged-binary.bin` → 复制到 `firmware\merged-binary-eyes.bin`，用 `flash-eyes.ps1` 整包烧录（会清 WiFi）
- `build\xiaozhi.bin` → 复制到 `firmware\xiaozhi-eyes.bin`，用 `flash-eyes-app.ps1` 只更新应用（保留 WiFi）

当前编译用的临时源码目录：`C:\Users\22773\AppData\Local\Temp\xiaozhi-build\xiaozhi-esp32`。
如果源码换到别的位置，需要改 `C:\Espressif\build-eyes.bat` 里那一行 `cd /d ...`。

目标：不按任何键，说“你好小智”就能对话；屏幕有表情。用现成的 **xiaozhi-esp32** 固件，不写代码。

总价：
- **OLED 版约 ¥70–100**（最便宜、接线最少）
- **彩屏版约 ¥90–130**（屏幕大、表情好看）

## 零、到货前准备（现在就能做）

- [x] 固件已下载：`firmware/merged-binary.bin`（v2.5.2，OLED 128×64 版，ESP32-S3 / 16MB）
- [x] esptool 已安装（uv 隔离环境）
- [ ] 注册 [xiaozhi.me](https://xiaozhi.me) 账号
- [ ] 确认家里 WiFi 有 2.4GHz（ESP32 不支持 5GHz）
- [ ] 收到板子后核对型号：ESP32-S3 **N16R8**，16MB Flash + 8MB OPI PSRAM

收到货后先别通电，核对这几样：

- 主控板丝印或商品页写着 **N16R8 / 16MB / 8MB PSRAM**
- OLED 是 4 针 I2C：VCC、GND、SDA、SCL
- INMP441 是 6 针：VDD、GND、SD、SCK、WS、L/R
- MAX98357A 是 7 针：VIN、GND、DIN、BCLK、LRC、SD、GAIN，另有 OUT+/OUT-
- 喇叭是 8Ω 2W
- 模块排针已经焊好（没焊的话需要电烙铁，或者换已焊版本）

---

## 一、采购清单

### 必备

| 物品 | 型号 / 搜索词 | 数量 | 约价 | 注意 |
|---|---|---|---|---|
| 主控板 | **ESP32-S3 DevKitC-1 N16R8**（16MB Flash + 8MB **OPI** PSRAM） | 1 | ¥35–50 | 预编译固件要求 16MB Flash + 8MB OPI PSRAM；不要买 4MB/8MB Flash 或 QSPI PSRAM 版 |
| 麦克风 | **INMP441** I2S 麦克风模块 | 1 | ¥8–12 | 买已焊排针的 |
| 功放 | **MAX98357A** I2S 功放模块 | 1 | ¥8–12 | 买已焊排针的 |
| 喇叭 | 8Ω 2W 小喇叭 | 1 | ¥3–6 | 带两根线即可 |
| 屏幕（二选一） | **0.96 寸 SSD1306 128×64 I2C OLED** | 1 | ¥8–12 | 最便宜，单色，表情简单 |
|  | 或 **2.4 寸 ST7789 240×320 SPI 彩屏** | 1 | ¥25–35 | 表情更好看，接线多 3 根 |
| 面包板 | 400 孔 | 1 | ¥5–10 | 免焊 |
| 杜邦线 | 公对公 / 公对母 | 若干 | ¥5–10 | 建议买一包 |
| USB-C 数据线 | 能传数据 | 1 | ¥5–10 | 充电线不行 |

### 可选

- 5V/2A 手机充电器（给功放和板子供电，避免声音大时重启）
- 100µF 电解电容（并在功放 5V/GND 上，减少爆音）
- 亚克力/3D 打印外壳

---

## 二、接线

> 接线前先拔掉 USB。所有模块的 GND 必须连在一起（共地）。
> 下面写的是 ESP32-S3 板子上印的 GPIO 编号。

### 1. INMP441 麦克风

| INMP441 | 接到 ESP32-S3 |
|---|---|
| VDD | 3V3 |
| GND | GND |
| L/R | GND（选左声道） |
| WS | GPIO4 |
| SCK | GPIO5 |
| SD | GPIO6 |

### 2. MAX98357A 功放 + 喇叭

| MAX98357A | 接到 ESP32-S3 |
|---|---|
| VIN | 5V |
| GND | GND |
| DIN | GPIO7 |
| BCLK | GPIO15 |
| LRC | GPIO16 |
| SD | 3V3（使能） |
| GAIN | 悬空（默认增益） |
| OUT+ / OUT- | 喇叭两根线 |

### 3. OLED 版屏幕（SSD1306 128×64）

| OLED | 接到 ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO41 |
| SCL | GPIO42 |

### 4. 彩屏版屏幕（ST7789 240×320，2.4 寸）

| ST7789 | 接到 ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SCL / SCK | GPIO21 |
| SDA / MOSI | GPIO47 |
| CS | GPIO41 |
| DC / RS | GPIO40 |
| RST / RES | GPIO45 |
| BL / LED | GPIO42 |

> 彩屏版不要接按键；这个固件只做语音，不需要触摸/音量键。
> 官方面包板效果图见 [docs/breadboard-wiring.jpg](docs/breadboard-wiring.jpg)。

---

## 三、刷固件

> 我已经把 OLED 版固件下载到 `firmware/merged-binary.bin`，到货后可以直接运行 `flash.ps1`。

### 1. 下载对应版本

打开 https://github.com/78/xiaozhi-esp32/releases

在最新版本的 **Assets** 里找：

| 你的屏幕 | 下载文件名包含 |
|---|---|
| 0.96 寸 OLED 128×64 | `bread-compact-wifi-128x64` |
| 2.4 寸 ST7789 240×320 彩屏 | `bread-compact-wifi-lcd` |

下载 ZIP，解压得到 `merged-binary.bin`。

> 官方预编译固件是按 **16MB Flash + 8MB OPI PSRAM** 构建的。买错板子（4MB/8MB Flash、无 PSRAM、QSPI PSRAM）会出现烧录失败或开机重启。

### 2. 烧录

**方式 A：官方新手教程（推荐）**

https://ccnphfhqs21z.feishu.cn/wiki/Zpz4wXBtdimBrLk25WdcXzxcnNS

**方式 B：本地 esptool**

```powershell
python -m pip install esptool
```

板子按住 **BOOT**，点一下 **RST/EN**，松开 BOOT，进入下载模式。然后：

```powershell
uv tool run esptool --chip esp32s3 -p COM5 -b 460800 write-flash 0x0 firmware\merged-binary.bin
```

把 `COM5` 换成设备管理器里的实际端口。

**方式 C：一键脚本（推荐，固件已准备好）**

```powershell
powershell -ExecutionPolicy Bypass -File .\flash.ps1
```

脚本会列出串口、提示你按 BOOT+RST，然后自动烧录。

---

## 屏幕/喇叭没到货时先做什么

只插主控板，**不要接任何模块**。

1. 用 USB-C 数据线把板子连到电脑（如果板子有两个 USB-C 口，优先插标着 **UART** 的那个）
2. 打开 PowerShell，列出串口：

```powershell
[System.IO.Ports.SerialPort]::GetPortNames()
```

3. 确认板子型号和 Flash 大小：

```powershell
uv tool run esptool --chip esp32s3 -p COM5 flash-id
```

把 `COM5` 换成实际端口。期望看到：

- `Detected chip type... ESP32-S3`
- `Detected flash size: 16MB`

4. 读一下 MAC 地址，确认通信正常：

```powershell
uv tool run esptool --chip esp32s3 -p COM5 read-mac
```

如果连不上：按住 **BOOT**，点一下 **RST/EN**，松开 BOOT，再运行命令。
把这两条命令的完整输出发我，我帮你确认板子对不对。

---

## 四、首次开机

1. 插上 USB，屏幕亮起
2. 设备会开一个 WiFi 热点（或显示二维码）
3. 手机连这个热点，按提示选择家里的 **2.4GHz WiFi** 并输入密码
4. 到 [xiaozhi.me](https://xiaozhi.me) 注册账号，按屏幕提示的验证码绑定设备
5. 说“**你好小智**”，然后直接说话

---

## 五、验收标准

- 不按任何键，说唤醒词能开始对话
- 屏幕有表情变化
- 喇叭能出声，麦克风能听清
- 连续对话 10 分钟不重启
- 断网时设备不崩溃

---

## 六、最容易踩的坑

| 现象 | 检查 |
|---|---|
| 完全没声音 | 功放 VIN 有没有接 5V；SD 有没有接 3V3；喇叭接的是 OUT+/OUT- |
| 声音很小/爆音 | 换 5V/2A 电源；喇叭用 8Ω 2W；在功放电源并 100µF 电容 |
| 麦克风没反应 | L/R 必须接 GND；WS/SCK/SD 是否接对；买的是 INMP441 不是模拟麦 |
| 屏幕不亮 | OLED 接 3V3 不是 5V；彩屏检查 CS/DC/RST/BL；固件版本要和屏幕匹配 |
| 屏幕花屏/偏移 | 彩屏必须是 ST7789 240×320；其他尺寸需要自己编译固件 |
| 连不上 WiFi | ESP32 只支持 2.4GHz，5GHz 不行 |
| 烧录失败 | 换数据线；按 BOOT+RST 进下载模式；装 CH340/CP210x 驱动 |
| 一直重启 | 供电不足；换 USB 口或 5V/2A 充电器；确认板子是 N16R8 + OPI PSRAM |

---

## 七、先说清楚

小智固件是 **“AI 语音助手 + 表情”**，不是喂食、清洁、成长、进化的养成游戏。你要的“能对话、能动、有表情”它能满足；如果之后想加养成玩法，需要二次开发，那是下一阶段。

原来的 T-Display 按键版代码保留在 `zhuochong/`，作为学习参考，不再是主方案。
