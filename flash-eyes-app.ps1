# 小智语音桌宠 - 只更新应用（保留 WiFi 配置和配网信息）
# 用法：powershell -ExecutionPolicy Bypass -File .\flash-eyes-app.ps1
#
# 和 flash-eyes.ps1 的区别：
#   flash-eyes-app.ps1  只写 app 分区（0x20000），WiFi/绑定信息不动，刷得快
#   flash-eyes.ps1      整包写入（0x0），会顺带把 NVS 覆盖成 0xFF，WiFi 要重配

$ErrorActionPreference = "Stop"

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "  小智语音桌宠（眼睛版）应用更新" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

$firmware = Join-Path $PSScriptRoot "firmware\xiaozhi-eyes.bin"
if (-not (Test-Path $firmware)) {
    Write-Host "找不到应用固件：$firmware" -ForegroundColor Red
    Write-Host "请确认 firmware\xiaozhi-eyes.bin 存在。" -ForegroundColor Yellow
    exit 1
}

if (-not (Get-Command uv -ErrorAction SilentlyContinue)) {
    Write-Host "没有找到 uv 命令。请先安装 uv，或改用官方图形化烧录工具。" -ForegroundColor Red
    exit 1
}

Write-Host "固件：$firmware（只写 app 分区，保留 WiFi 配置）"
Write-Host ""

$ports = [System.IO.Ports.SerialPort]::GetPortNames()
if ($ports.Count -eq 0) {
    Write-Host "没有检测到串口。" -ForegroundColor Red
    Write-Host "请先插上 USB-C 数据线，并确认设备管理器里出现 COM 口。" -ForegroundColor Yellow
    exit 1
}

Write-Host "检测到的串口：" -ForegroundColor Green
$ports | ForEach-Object { Write-Host "  $_" }
Write-Host ""

$port = (Read-Host "请输入要烧录的端口（例如 COM5）").Trim().ToUpper()
if ($port -notmatch '^COM\d+$') {
    Write-Host "端口格式不对，应该是 COM5 这种形式。" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "开始烧录（通常 30 秒左右）..." -ForegroundColor Cyan
uv tool run esptool --chip esp32s3 -p $port -b 460800 write-flash 0x20000 $firmware

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "烧录成功！设备会自动重启，WiFi 配置保留。" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "烧录失败，请检查：" -ForegroundColor Red
    Write-Host "  - USB 线是不是数据线"
    Write-Host "  - 端口是不是选对了"
    Write-Host "  - 设备管理器里有没有黄色感叹号（驱动没装好）"
    Write-Host "  - 如果提示分区不对，说明设备还没刷过完整固件，先运行 flash-eyes.ps1"
}
