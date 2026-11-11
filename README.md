# USB VSense

A compact inline USB-C voltage indicator. It sits between a USB-C source and sink and shows which USB Power Delivery fixed-voltage level is on VBUS, with one colour-coded LED per level: **5, 9, 12, 15, 20, 28, 36 and 48 V**. The USB 2.0 data, CC and SBU lines pass through.

- Designed and produced by **Meticulous Technologies**, Eindhoven, NL
- Sold through [Crowd Supply](https://www.crowdsupply.com/meticulous-technologies/usb-vsense)
- OSHWA certified open-source hardware, UID [NL000037](https://certification.oshwa.org/nl000037.html)
- Hardware revisions:
  - **Rev. 1**: the production revision (March 2026), in `hardware/KiCAD/USB_VSense_r1/`
  - **Gamma**: the OSHWA-certified open-source release (January 2026), in `hardware/KiCAD/USB_VSense_gamma/`

> **TODO (Sparsh):** add a product photo and an exploded photo of the 7-board stack (store images in `docs/images/`).

---

## How to read the LEDs

The behaviour below comes from the firmware source (`firmware/MPLABX/usb_vsense_r1_2.X/main.c`).

| VBUS relative to the nearest PD level | Indication |
|---|---|
| Within ±5 % | That level's LED is **solid** |
| 5–10 % above | Level LED solid + **next-higher** LED blinks **slowly** |
| More than 10 % above (up to the midpoint) | Level LED solid + next-higher LED blinks **fast** |
| 5–10 % below | Level LED solid + **next-lower** LED blinks **slowly** |
| More than 10 % below (down to the midpoint) | Level LED solid + next-lower LED blinks **fast** |

Edge cases:
- **Below 5 V − 5 %:** the 5 V LED itself blinks (slowly for 5–10 % low, fast below that).
- **Above 48 V + 5 %:** the 48 V LED blinks slowly. Above about 51.6 V the ADC saturates, so the indication stays the same however high VBUS goes.
- **Power-up:** the LEDs sweep from 48 V down to 5 V, about 150 ms per LED.

Adjacent ranges are split at midpoints: 7, 10.5, 13.5, 17.5, 24, 32 and 42 V. Each window edge has 1 LSB of hysteresis.

Blink rates work out to about 1 Hz (slow) and 3 Hz (fast). These figures assume the nominal 32.768 kHz internal ULP oscillator, divided by 2, feeding TCB0. They are calculated from the code, not measured.

> **TODO (Sparsh):**
> - The Crowd Supply page says over-range (e.g. 52 V) gives a *rapidly* blinking 48 V LED. This source gives a *slow* blink. Confirm which firmware is on shipped units, then fix either the code or the page.
> - Add the LED colour for each voltage. Confirmed so far: 5 V red, 9 V orange, 48 V white.

## Specifications

| Parameter | Value | Source / status |
|---|---|---|
| Indicated levels | 5, 9, 12, 15, 20, 28, 36, 48 V | firmware |
| Pass-through | USB 2.0 D+/D−, CC, SBU, VBUS | campaign page |
| Rated pass-through | 48 V / 5 A (240 W, USB PD EPR) | campaign page |
| Maximum input voltage | 55 V | campaign page; **TODO:** state whether this is a design limit or tested |
| Measurement | ATtiny406 10-bit ADC, internal 4.3 V reference; two dividers (÷5.1 up to ~21.9 V, ÷12 up to ~51.6 V); range switch at 21.65 V rising / 21.35 V falling | firmware + BOM |
| Quiescent current | **TODO** | not in repo |
| Length | 56 mm | press coverage; **TODO:** confirm |

### Accuracy: what is and isn't guaranteed

- **Not guaranteed.** The window thresholds are computed from nominal component values. There is no per-unit calibration.
- **Reference error.** The ATtiny406 internal 4.3 V reference is specified at **±5 %** over VDD 1.8–5.5 V and −40 to 125 °C (Microchip DS40002318A, Table 33-21). The datasheet says this figure is "based on characterization and not covered by production test limits". That error alone is as large as the ±5 % "nominal" band, so a unit may light the neighbouring blink indication when VBUS is near a band edge.
- **Divider error.** The divider resistors are YAGEO RC0603FR series, ±1 % per the part-number code.
- **Supply range.** Microchip specifies the 4.3 V reference for VDD ≥ 4.75 V, and the ADC reference must stay ≤ VDD − 0.4 V. VDD comes from an MCP1792 5.0 V LDO fed from VBUS. At low VBUS (the 5 V "low" region), VDD drops below that range, so readings there are outside the datasheet conditions.
- **Typical (bench) accuracy:** **TODO (Sparsh):** add measured figures with the number of units, test equipment and temperature, labelled as typical.

## Repository layout

```
production/rev1/                       ready-to-order files: Gerbers, complete BOM, firmware HEX
hardware/KiCAD/USB_VSense_r1/          Rev. 1: production design source
    USB_VSense_r1.kicad_pro / .kicad_sch / .kicad_pcb
    USB_VSense_r1.csv                  BOM as exported from the schematic
    library/                           project symbols, footprints, faceplate artwork
hardware/KiCAD/USB_VSense_gamma/       Gamma: the OSHWA-certified release, kept for reference
    bom/ibom.html                      interactive BOM (Gamma)
hardware/LICENSE_HARDWARE.txt          CERN-OHL-S-2.0
firmware/MPLABX/usb_vsense_r1_2.X/            MPLAB X project, firmware v1.2 (main.c, gpio.c)
firmware/LICENSE_FIRMWARE.txt          GPL v3
docs/schematics/USB_VSense_gamma.pdf   schematic (applies to Gamma and Rev. 1)
docs/*DECLARATION OF CONFORMITY*.pdf   EU / UK declarations of conformity
docs/USB_VSENSE_SAFETY_GUIDE_EN_DE_FR_NL.pdf   safety guide
```

**3D models are deliberately not included.** Several component models come from manufacturers under terms that do not allow redistribution. KiCad's 3D viewer will report missing models. Download them from the respective manufacturers if you need them. This does not affect the fabrication outputs.

## Building the hardware

**To have boards made, use [`production/rev1/`](production/rev1/).** It holds the Gerbers as fabricated, a complete BOM (including the M1.6 nuts, PCBs and the parts actually used in the Rev. 1 run), the firmware HEX, and the [assembly instructions](production/rev1/README.md#assembly). The same files are attached as zips to the GitHub Release.

The device is a stack of seven 0.8 mm PCBs, marked 1/7 to 7/7 from top to bottom:
- **The main board (4/7)** is 4-layer and carries all the electronics.
- **The six enclosure boards** (faceplate, light guides, spacers and base) are 2-layer.

Four M1.6 screws from the faceplate side thread into four SMT nuts on the base board (7/7). All seven boards are drawn in a single `.kicad_pcb`.

**Schematic:** [`docs/schematics/USB_VSense_gamma.pdf`](docs/schematics/USB_VSense_gamma.pdf). The Rev. 1 schematic is electrically identical to Gamma: same components, values, footprints, part numbers and connections. Only the title block differs.

**KiCad version:** both projects are saved with **KiCad 10** and cannot be opened in KiCad 9.

The Rev. 1 Gerbers were exported with KiCad 9.0.8 before the files were converted to KiCad 10. During conversion KiCad 10 regenerated a few teardrops, so a fresh export differs very slightly from the fabricated boards in teardrop copper. The Gerbers in `production/rev1/` remain the authoritative record of what was built.

The fabrication notes (edge plating, tab placement, corner radius) and the PCBA notes for the assembler are in [`production/rev1/README.md`](production/rev1/README.md#fabrication-notes).

> **TODO (Sparsh):**
> - Explain how the two Gerber sets (main board, six enclosure boards) are produced from the single `.kicad_pcb`.
> - Add the assembly position (centroid) file used for the main PCBA.

> **Warning (Gamma only):** the Gamma `.kicad_pcb` carries `FREE SAMPLE / NOT FOR SALE` as solder-mask artwork and a third-party manufacturer logo on one enclosure board. Rev. 1 has neither. Build from Rev. 1.

## Building and flashing the firmware

The ready-to-flash HEX is [`production/rev1/firmware/USB_VSense_fw_r1.hex`](production/rev1/firmware/).

SHA-256:
```
dc750f71fc5567e7b25dc1818dd63438305cd66886e6211a805e0ac14986a6e1  USB_VSense_fw_r1.hex
```

This is the HEX built on 21 May 2026 for the Rev. 1 production run. The firmware source in this repository rebuilds to a **byte-identical** HEX with:
- XC8 **v3.10**
- XC8 **v4.00** with ATtiny_DFP **3.4.278**

So the published source and the HEX match.

To build it yourself:

1. Open `firmware/MPLABX/usb_vsense_r1_2.X` in MPLAB X.
2. Build the production configuration (Production > Clean and Build Main Project).
3. Compare the SHA-256 of `dist/Copy_of_default/production/*.hex` with the value above.

To flash it, program over **UPDI** using the UPDI pads on the board (J2), for example with a PICkit 4 and MPLAB IPE.

The fuse settings are defined in `main.h` (`FUSES = {...}`) and are embedded in the HEX, so programming the HEX also sets the fuses. The lock byte is `0xC5` (no lock), so a programmed device can be read back and compared.

> Firmware v1.2 only changes how the LED port pointers are typed, so that the code compiles on newer XC8 (`-Werror=int-conversion`). The generated code is unchanged; see the changelog in the `main.c` header.

## Licences

| Part | Licence |
|---|---|
| Hardware (`hardware/`) | [CERN-OHL-S-2.0](hardware/LICENSE_HARDWARE.txt) |
| Firmware (`firmware/`) | GPL v3, see [LICENSE_FIRMWARE.txt](firmware/LICENSE_FIRMWARE.txt) |
| Documentation (`docs/`) | [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) |

See [NOTICE](NOTICE) for the source location and [TRADEMARK.md](TRADEMARK.md) for use of the names and logos.

> **TODO (Sparsh):** the source headers say GPL-3.0-*or-later*, while the OSHWA listing says GPL-3.0-*only*. Pick one and make all three places match.

## Contact

Questions and bug reports: open a GitHub issue, or email info@meticuloustech.nl.
