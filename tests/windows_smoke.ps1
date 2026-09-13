$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$demoPath = Join-Path $projectRoot 'build-win\GLVMEngine.exe'
$logPath = Join-Path $projectRoot 'build-win\GLVMEngine.log'
$errorPath = Join-Path $projectRoot 'build-win\GLVMEngine-error.log'
Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class GlvmWindowProbe {
    [DllImport("user32.dll")] public static extern IntPtr SetThreadDpiAwarenessContext(IntPtr context);
    [StructLayout(LayoutKind.Sequential)] public struct Rect { public int Left, Top, Right, Bottom; }
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out Rect rect);
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out Rect rect);
    [DllImport("user32.dll")] public static extern bool SetWindowPos(IntPtr h, IntPtr after, int x, int y, int w, int height, uint flags);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr key, IntPtr data);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int command);
}
'@
[void][GlvmWindowProbe]::SetThreadDpiAwarenessContext([IntPtr](-4))
$probeStart = Get-Date
$demo = Start-Process -FilePath $demoPath -ArgumentList @('--frames','100000') -WorkingDirectory $projectRoot -WindowStyle Normal -PassThru
function Wait-DemoLog([string]$pattern) {
    $deadline = (Get-Date).AddSeconds(30)
    while ((Get-Date) -lt $deadline) {
        $demo.Refresh()
        if ($demo.HasExited) { throw "Demo exited early: $($demo.ExitCode). $(Get-Content $errorPath -Raw)" }
        if ((Test-Path $logPath) -and (Get-Item $logPath).LastWriteTime -ge $probeStart -and
            (Get-Content $logPath -Raw) -match $pattern) { return }
        Start-Sleep -Milliseconds 50
    }
    throw "Timed out waiting for $pattern"
}
function Resize-Demo([int]$width, [int]$height) {
    $client = New-Object GlvmWindowProbe+Rect
    $outer = New-Object GlvmWindowProbe+Rect
    [void][GlvmWindowProbe]::GetClientRect($demo.MainWindowHandle, [ref]$client)
    [void][GlvmWindowProbe]::GetWindowRect($demo.MainWindowHandle, [ref]$outer)
    $borderWidth = ($outer.Right - $outer.Left) - $client.Right
    $borderHeight = ($outer.Bottom - $outer.Top) - $client.Bottom
    if (![GlvmWindowProbe]::SetWindowPos($demo.MainWindowHandle, [IntPtr]::Zero, 0, 0, $width + $borderWidth, $height + $borderHeight, 6)) {
        throw 'SetWindowPos failed'
    }
    Wait-DemoLog "Swapchain resized: ${width}x${height}"
}
function Press-DemoKey([int]$key) {
    [void][GlvmWindowProbe]::PostMessage($demo.MainWindowHandle, 0x100, [IntPtr]$key, [IntPtr]::Zero)
    [void][GlvmWindowProbe]::PostMessage($demo.MainWindowHandle, 0x101, [IntPtr]$key, [IntPtr]::Zero)
}
try {
    Wait-DemoLog 'Vulkan ready'
    $demo.Refresh()
    if ($demo.MainWindowHandle -eq 0) { throw 'No native game window' }
    Resize-Demo 960 540
    Press-DemoKey 0x49
    Wait-DemoLog 'Inventory: open'
    Press-DemoKey 0x49
    Wait-DemoLog 'Inventory: closed'
    Press-DemoKey 0x4F
    Wait-DemoLog 'Collisions: on'
    Press-DemoKey 0x4F
    Wait-DemoLog 'Collisions: off'
    for ($cycle = 0; $cycle -lt 3; ++$cycle) {
        [void][GlvmWindowProbe]::ShowWindow($demo.MainWindowHandle, 6)
        Start-Sleep -Milliseconds 200
        [void][GlvmWindowProbe]::ShowWindow($demo.MainWindowHandle, 9)
        Start-Sleep -Milliseconds 100
    }
    Resize-Demo 1280 720
    [void][GlvmWindowProbe]::PostMessage($demo.MainWindowHandle, 0x10, [IntPtr]::Zero, [IntPtr]::Zero)
    if (!$demo.WaitForExit(10000)) { throw 'Demo did not close' }
    if ($demo.ExitCode -ne 0) { throw "Demo failed: $($demo.ExitCode)" }
    $errors = Get-Content $errorPath -Raw
    if ($errors) { throw $errors }
    if ((Get-Content $logPath -Raw) -match 'Swapchain(?: resized)?: (?:0x\d+|\d+x0)\b') {
        throw 'Renderer created a zero-sized swapchain while minimized'
    }
    Copy-Item -LiteralPath $logPath -Destination (Join-Path $projectRoot 'build-win\windows-smoke.log')
    Write-Output 'PASS native Windows window, resize twice, inventory, collision overlay, minimize/restore and close'
} finally {
    $demo.Refresh()
    if (!$demo.HasExited) { Stop-Process -Id $demo.Id -Force }
}
