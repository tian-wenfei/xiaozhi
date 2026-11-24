# 小智语音桌宠 - 烧录脚本（OLED 128x64 版）
# 用法：在 PowerShell 里运行
#   powershell -ExecutionPolicy Bypass -File .\flash.ps1

$ErrorActionPreference = "Stop"

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "  小智语音桌宠 烧录助手" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

$firmware = Join-Path $PSScriptRoot "firmware\merged-binary.bin"
if (-not (Test-Path $firmware)) {
    Write-Host "找不到固件：$firmware" -ForegroundColor Red
    Write-Host "请确认 firmware\merged-binary.bin 存在。" -ForegroundColor Yellow
    exit 1
}

if (-not (Get-Command uv -ErrorAction SilentlyContinue)) {
    Write-Host "没有找到 uv 命令。请先安装 uv，或改用官方图形化烧录工具。" -ForegroundColor Red
    exit 1
}

Write-Host "固件：$firmware"
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
Write-Host "请现在让板子进入下载模式：" -ForegroundColor Yellow
Write-Host "  1. 按住 BOOT 键不放"
Write-Host "  2. 点一下 RST / EN 键"
Write-Host "  3. 松开 BOOT 键"
Write-Host ""
Read-Host "准备好后按回车开始烧录"

Write-Host ""
Write-Host "开始烧录..." -ForegroundColor Cyan
uv tool run esptool --chip esp32s3 -p $port -b 460800 write-flash 0x0 $firmware

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "烧录成功！" -ForegroundColor Green
    Write-Host "拔掉 USB，重新插上，设备会开机。" -ForegroundColor Green
    Write-Host "然后按屏幕提示连 2.4GHz WiFi，并到 xiaozhi.me 绑定。" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "烧录失败，请检查：" -ForegroundColor Red
    Write-Host "  - USB 线是不是数据线"
    Write-Host "  - 端口是不是选对了"
    Write-Host "  - 板子有没有按 BOOT + RST 进入下载模式"
    Write-Host "  - 设备管理器里有没有黄色感叹号（驱动没装好）"
}
