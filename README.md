# USB VSense

USB VSense is an inline USB-C voltage indicator. Plug it in between a USB-C power source and the device it powers. One LED per USB Power Delivery fixed voltage shows the VBUS voltage: **5, 9, 12, 15, 20, 28, 36 and 48 V**. Blinking LEDs show how far VBUS is from nominal.

VBUS, GND, USB 2.0 data (D+/D−), CC and SBU pass straight through. The SuperSpeed pairs are not connected.

- Designed and made by **Meticulous Technologies**, Eindhoven, NL. Available from [Crowd Supply](https://www.crowdsupply.com/meticulous-technologies/usb-vsense).
- OSHWA-certified open-source hardware, UID [NL000037](https://certification.oshwa.org/nl000037.html).

## Reading the LEDs

| Level | 5 V | 9 V | 12 V | 15 V | 20 V | 28 V | 36 V | 48 V |
|---|---|---|---|---|---|---|---|---|
| LED colour | red | orange | yellow | green | lime (blue + green bi-colour) | blue | blue + red (bi-colour) | white |

| VBUS relative to the nearest level | What you see |
|---|---|
| Within ±5 % | That level's LED is **solid** |
| 5–10 % above | Level LED solid, **next-higher** LED blinks **slowly** |
| More than 10 % above (up to the midpoint) | Level LED solid, next-higher LED blinks **fast** |
| 5–10 % below | Level LED solid, **next-lower** LED blinks **slowly** |
| More than 10 % below (down to the midpoint) | Level LED solid, next-lower LED blinks **fast** |

- **Range boundaries.** Adjacent levels are split at the midpoints: 7, 10.5, 13.5, 17.5, 24, 32 and 42 V.
- **Below 5 V − 5 %.** The 5 V LED itself blinks: slowly for 5–10 % low, fast below that.
- **Above 48 V + 5 % (50.4 V).** The 48 V LED blinks slowly. Above about 51.6 V the ADC saturates, so the indication doesn't change up to the 55 V maximum.
- **At power-up.** The LEDs sweep from 48 V down to 5 V, about 150 ms per LED.
- **Blink rates.** About 1 Hz (slow) and 3 Hz (fast). These are calculated from the firmware for the nominal 32.768 kHz internal oscillator, not measured.

This behaviour is what the published firmware does (`firmware/MPLABX/usb_vsense_r1_2.X/main.c`).

## Specifications

| | |
|---|---|
| Indicated levels | 5, 9, 12, 15, 20, 28, 36, 48 V |
| Maximum input voltage | 55 V. This is the operating limit of the MCP1792 LDO that powers the circuit (datasheet: 4.5–55 V; 70 V absolute maximum). |
| Pass-through rating | 48 V / 5 A (USB PD Extended Power Range, 240 W) |
| Current consumption | < 6 mA |
| Measurement | ATtiny406 10-bit ADC with its internal 4.3 V reference. Two dividers: ÷5.1 (full scale ≈ 21.9 V) and ÷12 (≈ 51.6 V). The firmware switches range at 21.65 V rising and 21.35 V falling. |
| Size | 56 × 18.5 × 5.6 mm: a stack of seven 0.8 mm PCBs (board outline 47.5 × 18.3 mm) |

### Accuracy

The thresholds are calculated from nominal component values, and **there is no per-unit calibration**.

- **ADC reference.** The ATtiny406's internal 4.3 V reference is specified at **±5 %** over VDD 1.8–5.5 V and −40 to 125 °C (Microchip DS40002318A, Table 33-21). The datasheet notes this figure is based on characterisation, not production-tested. That error alone is as large as the ±5 % "solid" band, so near a band edge a unit can show the neighbouring blink indication.
- **Dividers.** The divider resistors are 1 %.
- **Low VBUS.** The circuit runs from a 5.0 V LDO fed by VBUS. Microchip specifies the 4.3 V reference for VDD ≥ 4.75 V and requires the ADC reference to stay ≤ VDD − 0.4 V. With VBUS near 5 V, the LDO is in dropout and VDD falls below that range, so readings in the 5 V region are outside the datasheet conditions.

## Repository contents

| Path | Contents |
|---|---|
| [`production/rev1/`](production/rev1/) | Everything needed to have Rev. 1 made: Gerbers and drill files as manufactured, BOM, placement file, firmware HEX, fabrication and assembly instructions |
| `hardware/KiCAD/USB_VSense_r1/` | Rev. 1 design source (KiCad 10): schematic, PCB and project library |
| `hardware/KiCAD/USB_VSense_gamma/` | Gamma design source: the OSHWA-certified release, kept for reference |
| `firmware/MPLABX/usb_vsense_r1_2.X/` | Firmware source (MPLAB X project, XC8), v1.2 |
| `docs/schematics/` | Schematics as PDF: Rev. 1 and Gamma (electrically identical) |
| `docs/` | EU and UK declarations of conformity, safety guide |

## Building one

Everything you need is in [`production/rev1/`](production/rev1/). Its [README](production/rev1/README.md) covers ordering the PCBs, the assembly instructions for the main PCBA, programming, and putting the seven-board stack together. The Gerber sets, BOM, placement file and firmware are also attached to the GitHub Release.

## Firmware

The production firmware is [`production/rev1/firmware/USB_VSense_fw_r1.hex`](production/rev1/firmware/):

```
dc750f71fc5567e7b25dc1818dd63438305cd66886e6211a805e0ac14986a6e1  USB_VSense_fw_r1.hex
```

Building the source in this repository reproduces this HEX **byte for byte**. That has been checked with XC8 v3.10, and with XC8 v4.00 plus ATtiny_DFP 3.4.278.

**To build:**
1. Open `firmware/MPLABX/usb_vsense_r1_2.X` in MPLAB X.
2. Run *Production > Clean and Build Main Project*.
3. Compare the SHA-256 of `dist/Copy_of_default/production/*.hex` with the value above.

**To program:**
- Program the ATtiny406 (U1) over UPDI through J2 on the main board, for example with a PICkit 4 and MPLAB IPE. The pin-out and procedure are in the [production README](production/rev1/README.md#programming).
- The fuses are defined in `main.h` and embedded in the HEX, so programming the HEX also sets them.
- The lock byte is `0xC5` (not locked), so a programmed part can be read back and verified.

The firmware is short and well commented. To change the thresholds or blink rates, edit the window definitions at the top of `main.c`.

## Modifying the hardware

- **KiCad 10.** Both projects are saved in KiCad 10 format and won't open in KiCad 9.
- **All seven boards are in one board file.** `USB_VSense_r1.kicad_pcb` holds all seven boards side by side on a 20.32 mm pitch, in stack order 7/7 … 1/7, with the main board (4/7) in the middle.
- **The fab needs two separate sets.** They are made as two orders:
  - the main board on its own (4-layer)
  - the six enclosure boards side by side, without the main board (2-layer)

  Plotting the file as it is puts all seven outlines into one set, so plot each set separately. The sets in `production/rev1/gerbers/` show the result.
- **The published Gerbers are the reference.** They were plotted from KiCad 9.0.8, before the project was converted to KiCad 10. During the conversion KiCad regenerated a few teardrops, so a fresh plot differs very slightly in teardrop copper from the boards that were made.
- **3D models are not included.** Several component models come from manufacturers under terms that don't allow redistribution, so KiCad's 3D viewer will report them missing. Download them from the manufacturers if you need them. This doesn't affect the fabrication outputs.
- **Build Rev. 1, not Gamma.** The Gamma board is kept as the certified reference. Its PCB carries `FREE SAMPLE / NOT FOR SALE` solder-mask artwork and a third-party manufacturer's logo on one enclosure board.

## Licences

| Part | Licence |
|---|---|
| Hardware (`hardware/`) | [CERN-OHL-S-2.0](hardware/LICENSE_HARDWARE.txt) |
| Firmware (`firmware/`) | GPL-3.0-only, see [LICENSE_FIRMWARE.txt](firmware/LICENSE_FIRMWARE.txt) |
| Schematic PDFs (`docs/schematics/`) | [CERN-OHL-S-2.0](hardware/LICENSE_HARDWARE.txt) |
| Other documentation (`docs/`) | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) |

See [NOTICE](NOTICE) for the source location. The names "Meticulous Technologies" and "USB VSense" and the logos are not covered by these licences; see [TRADEMARK.md](TRADEMARK.md).

## Contact

For questions and bug reports, open a GitHub issue or email info@meticuloustech.nl.
