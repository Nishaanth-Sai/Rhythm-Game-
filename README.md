# Rhythm-Game-

A rhythm-based music game using an Arduino, VS1053 MP3 shield, and a TFT display. Players select a song, and falling blocks must be matched with button inputs timed to the music.

## 🎮 Game Flow

1. **Startup Screen**: Displays the game title.
2. **Song Selection Screen**: Choose from available songs stored on the SD card.
3. **Gameplay**:
   - Music starts playing through the VS1053 MP3 shield.
   - Blocks fall in sync with the music.
   - Players press buttons to "catch" the blocks at the right time.

## 🔁 Master-Slave Configuration

This project uses a two-Arduino setup connected via I²C:

| Role          | Board          | Responsibilities                                             |
|---------------|----------------|--------------------------------------------------------------|
| **Master**    | Arduino MEGA   | Handles game state, graphics (TFT), user input, and sends commands to the UNO to play music. |
| **Slave**     | Arduino UNO    | Handles music playback using the VS1053 MP3 shield.          |

This modular approach improves performance and separates visual/game logic from audio playback.

## 🛠 Hardware Used

| Component                | Description                                        |
|-------------------------|----------------------------------------------------|
| Arduino UNO             | Controls the MP3 shield (slave)                    |
| Arduino MEGA            | Manages the display, inputs, game states (master) |
| VS1053 MP3 Shield       | Plays MP3 music from SD card                       |
| SD Card                 | Stores music files and level data                  |
| 2.1" TFT Display Shield | Displays game UI and falling blocks                |
| Buttons (x4)            | Used to interact with falling blocks (like notes) |

## 💾 SD Card Structure

Place the songs of your choice in the SD card name it as 0001.mp3,0002.mp3 and so on.

## 🧾 Files Overview

- `Display.ino`: Runs on the MEGA. Handles display, button input, block timing, and sends playback commands to the UNO.
- `Sound.ino`: Runs on the UNO. Handles audio playback with the VS1053 MP3 shield upon receiving commands from the MEGA.

## 🧩 Libraries Required

Make sure to install the following Arduino libraries:
- `Adafruit_GFX`
- `MCUFRIEND_kbv` (for the TFT display)
- `Adafruit_VS1053`
- `Wire` (for I²C communication)
- `SPI`
- `SD`

## 🔌 Pin Configuration (Example)

### MEGA (Master):
| Function         | Pin             | Notes                          |
|------------------|------------------|--------------------------------|
| I²C SDA          | Pin 20           | To UNO SDA                     |
| I²C SCL          | Pin 21           | To UNO SCL                     |
| TFT Display      | SPI + digital    | Uses MCUFRIEND library         |
| Buttons          | D2, D3, D4, D5   | One per lane/button            |

### UNO (Slave):
| Function         | Pin             | Notes                          |
|------------------|------------------|--------------------------------|
| I²C SDA          | A4               | From MEGA SDA                  |
| I²C SCL          | A5               | From MEGA SCL                  |
| VS1053 MP3 Shield| SPI + DREQ/CS/etc| For sound playback             |

## 🚀 Getting Started

1. Wire up your hardware according to the pin tables above.
2. Format the SD card as FAT32 and upload the `.mp3`
3. Upload `Display.ino` to the MEGA and `Sound.ino` to the UNO.
4. Power both boards and reset to begin the game.



