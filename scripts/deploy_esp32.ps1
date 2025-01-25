# Configuration
$COM_PORT = "COM3"
$BAUD_RATE = 115200
$FIRMWARE_PATH = "C:\Users\simen\ACDC2025\.pio\build\esp32dev\firmware.bin"  # Use absolute path for testing

# Verify firmware path
if (Test-Path $FIRMWARE_PATH) {
    Write-Host "Firmware file found. Proceeding with flashing."
} else {
    Write-Host "Firmware file not found. Check path and try again."
    exit
}

# Erase flash memory (optional step)
Write-Host "Erasing flash memory..."
python -m esptool --chip esp32 --port $COM_PORT --baud $BAUD_RATE erase_flash

# Flash new firmware
Write-Host "Flashing new firmware..."
python -m esptool --chip esp32 --port $COM_PORT --baud $BAUD_RATE write_flash -z 0x1000 $FIRMWARE_PATH

if ($?) {
    Write-Host "ESP32 deployment successful!"
} else {
    Write-Host "Deployment failed. Check the ESP32 connection."
}
