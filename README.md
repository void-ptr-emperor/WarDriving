WarDriving

ESP32 wardriving logger: scans Wi-Fi networks, tags them with GPS coordinates and writes everything to a CSV file on an SD card. Live status is shown on an OLED display with a four-button menu. Logs are downloaded over Wi-Fi — no card reader needed.

Written from scratch in C with ESP-IDF: own SSD1306 driver, own menu engine, own NMEA parser. No external libraries.

Features
Wi-Fi scan every 5 seconds, up to 20 networks per scan
GPS position attached to each network at the moment it was found
CSV log on SD card, one row per network, no duplicates
128x64 OLED menu: live Wi-Fi, live GPS, SD card status, settings
Built-in web server — connect to the device's own access point and download the log
CPU frequency, display brightness and light sleep configurable from the menu
Tasks can be stopped and restarted from the menu
Hardware
Part	Notes
ESP32	any DevKit board
SSD1306	128x64, SPI
microSD module	SPI, FAT32
GPS module	NMEA over UART, 9600 baud (tested with u-blox)
4 buttons	to GND, internal pull-ups are used
Pinout

OLED and SD share the SPI2 bus.

Signal	GPIO
SPI MOSI	23
SPI MISO	19
SPI CLK	18
OLED CS	5
OLED DC	2
OLED RESET	4
SD CS	27
GPS TX (to ESP RX)	16
GPS RX (to ESP TX)	17
Button UP	22
Button OK	14
Button DOWN	13
Button BACK	21

Pins are defined in main/spi.h, main/uart.h and main/isr.h.

Build

Requires ESP-IDF v5.3 or newer.

bash
git clone https://github.com/void-ptr-emperor/WarDriving.git
cd WarDriving
idf.py set-target esp32
idf.py build flash monitor

For the frequency and sleep menu to do anything, enable power management in idf.py menuconfig:

Component config -> Power Management -> Support for power management
Component config -> FreeRTOS -> Tickless idle support
Getting the log

The device runs its own access point while scanning.

Connect to the Wi-Fi network created by the board (SSID and password are set in main/web.c)
Open http://192.168.4.1 in a browser
data.csv is downloaded

The SD card can also be read directly in a card reader.

CSV format
ssid,rssi,bssid,auth,lat,lon,alt,fix
Field	Example	Notes
ssid	MyNetwork	network name
rssi	-67	signal strength, dBm
bssid	c4:70:0b:7a:04:40	MAC address
auth	WPA/WPA2	security type
lat	4114.42701	latitude, raw NMEA
lon	04449.39194	longitude, raw NMEA
alt	945.2	altitude, metres
fix	1	GPS fix quality, 0 means no fix

Coordinates are written in raw NMEA form: ddmm.mmmm, degrees and minutes joined together. Mapping tools expect decimal degrees, so convert before importing:

decimal = int(value / 100) + (value mod 100) / 60

For 4114.42701 that gives 41.240450.

In a spreadsheet:

=INT(E1/100) + MOD(E1,100)/60

Once converted, the file can be dropped straight into kepler.gl or Google My Maps.

How it works

Four worker tasks and one UI task, all talking through FreeRTOS primitives.

gps_task   ──(copy under mutex)──▶ wifi_task ──(queue)──▶ sd_task
                                                   
buttons ─────────────────────────▶ ui_task ──▶ OLED
wifi_task scans, picks the strongest network for the display, takes a copy of the current GPS position and pushes one record per network into a queue
gps_task reads NMEA lines, parses GGA, publishes position
sd_task pulls records off the queue and appends them to the CSV, so each network is written exactly once
ui_task is the only task that touches the display; everything else just asks it to redraw
a single mutex guards the strings shown on screen and the menu state

The display is never drawn from more than one task, and the record queue removes shared state between the scanner and the card entirely.

Menu
MAIN MENU
├── WIFI LIVE      networks found, strongest SSID, RSSI, security
├── GPS LIVE       latitude, longitude, altitude, fix
├── SD STATUS      card state, records written, size, free space
└── OTHER MENU
    ├── ESP32 SETTINGS    80 / 160 / 240 MHz, light sleep
    ├── OLED SETTINGS     brightness
    └── MODULE STATUS     start / stop logging

The menu is a table of structs in main/menu.c. Each entry holds a label, an optional pointer to a submenu, an optional pointer to a live string, and an optional callback. Adding a screen means adding a row.

License

MIT
