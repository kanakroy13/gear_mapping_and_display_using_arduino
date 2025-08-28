# gear_mapping_and_display_using_arduino
Detect gearbox position using four Hall sensors on Arduino UNO R3 and display the current gear (R, N, 1–5) on a 128×64 SSD1306 OLED via U8g2 bitmaps. Includes wiring, assets placeholders, and a simple debug/fullscreen display mode


# Gear Mapping and Display using Arduino (UNO R3)

Detect gearbox position from **four Hall sensors** and show the current gear on a **128×64 SSD1306 OLED** using **U8g2** bitmaps. Supports two layouts:
- **Fullscreen images** (default when `mode` pin is HIGH)
- **Debug side image** (when `mode` pin is LOW)

> ⚠️ This project targets **Arduino UNO R3** (ATmega328P). It uses hardware I²C (SDA=A4, SCL=A5).

---

## Demo Shots
![WhatsApp Image 2025-08-28 at 11 02 21_c23cf8c8](https://github.com/user-attachments/assets/78212a06-fa1b-44b8-a2df-814aa0a90e6a)

![WhatsApp Image 2025-08-28 at 11 02 22_45fce5c8](https://github.com/user-attachments/assets/5cdb3faa-6f63-4980-9384-36be34ea6141)

![WhatsApp Image 2025-08-28 at 11 02 22_488a2ff1](https://github.com/user-attachments/assets/8bf6c673-fae0-4532-b7af-6e6fd07b9cc5)

![WhatsApp Image 2025-08-28 at 11 02 22_2223831f](https://github.com/user-attachments/assets/53078548-4c3c-447a-a63e-9d90659ad83e)

![WhatsApp Image 2025-08-28 at 11 02 23_3f20c806](https://github.com/user-attachments/assets/cffab1ff-226e-441c-a34f-96080d2468b4)

![WhatsApp Image 2025-08-28 at 11 03 59_4be1a410](https://github.com/user-attachments/assets/96d5396f-579d-459c-9f26-d56a74451a8c)


## Hardware

- Arduino **UNO R3**
- 4 × Hall effect sensors (analog output) connected to A0–A3
- 128×64 SSD1306 OLED (I²C)
- Optional: switch/jumper to select display **mode** on digital pin **3**
- Wires, power, magnets on gear selector mechanism

### Pin map (UNO R3)

| Function                  | UNO Pin | Notes                                      |
|--------------------------|---------|--------------------------------------------|
| Hall sensor 0 (e.g., 1st)| A0      | Analog read                                |
| Hall sensor 1 (e.g., 2nd)| A1      | Analog read                                |
| Hall sensor 2 (e.g., R)  | A2      | Analog read                                |
| Hall sensor 3 (e.g., 5th)| A3      | Analog read                                |
| Mode select (0=debug,1=fs)| D3     | Use external pull-up or pull-down as needed|
| OLED SDA                 | A4      | I²C data                                   |
| OLED SCL                 | A5      | I²C clock                                  |

> Ensure your Hall sensors are powered correctly and share GND with the UNO.  
> If your mode switch floats, add a proper pull-up or pull-down resistor.

---

## Libraries

Install via **Arduino IDE → Tools → Manage Libraries…**

- **U8g2** by olikraus

Board selection: **Tools → Board → Arduino AVR Boards → Arduino Uno**

---

## How it works (logic overview)

- Each Hall sensor is read from **A0–A3**.
- Readings are converted to a “percent deviation from center (512)” to make the threshold simple and robust.
- A basic rule map assigns the gear:
  - If **A0 & A3** triggered → **3rd**
  - If **A1 & A2** triggered → **4th**
  - Else single A0→1st, A1→2nd, A2→Reverse, A3→5th
  - Else **Neutral**
- Display renders the corresponding **bitmap** from `gear_bitmaps_fullscreen.h`.
- `mode` pin (D3): **LOW=debug** (right-side icon), **HIGH=fullscreen**.

> Tune the `THRESHOLD_PERCENT` if your magnets/sensors behave differently.

---

## Project workflow

1. **Mechanical setup**  
   Mount magnets/sensors so each gear position gives a stable and **distinct** analog response.

2. **Wiring**  
   Connect sensors to **A0–A3**, OLED to **A4/A5**, and `mode` switch to **D3** with suitable pull-up/down.

3. **Library & IDE setup**  
   Install **U8g2**, select **Arduino Uno**, set correct COM port.

4. **Upload**  
   Open `src/main.ino`, compile and upload.

5. **Verify**  
   Move the shifter through **R, N, 1–5** and confirm the icon changes.

6. **Tune threshold (optional)**  
   If mis-detections occur, adjust `THRESHOLD_PERCENT` in `main.ino`.

7. **Document**  
   Add your photos to `assets/`:
   - `assets/oled_display.jpg`
   - `assets/wiring.jpg`

---

## File structure

- `src/main.ino` — reads sensors, computes gear, chooses bitmap, draws with U8g2.
- `src/gear_bitmaps_fullscreen.h` — all gear bitmaps stored in PROGMEM and an index array.

---

## Troubleshooting

- **Nothing on OLED**: check I²C wiring (SDA=A4, SCL=A5) and your OLED address. U8g2’s SSD1306 NONAME works for most 0x3C modules.
- **Wrong gear detected**: swap sensor inputs or tweak `THRESHOLD_PERCENT`.
- **Flicker between gears**: add mechanical hysteresis or increase threshold slightly.

---

## License

MIT (feel free to change). See `LICENSE`.

---


