# USB VSense Rev. 1: production files

Everything needed to have USB VSense Rev. 1 made, without opening KiCad or MPLAB.

| Path | What it is |
|---|---|
| `gerbers/main_pcba/` | Gerbers and drill files for the main board, which carries all the electronics |
| `gerbers/stack_panel/` | Gerbers and drill files for the six remaining stack boards, panelised |
| `BOM_USB_VSense_r1.csv` | Complete BOM: electronics, M1.6 nuts, PCBs and mechanical parts |
| `firmware/` | Production firmware HEX for the ATtiny406, with its SHA-256 checksum (rebuilds byte-identically from the published source) |

The Gerbers are byte-identical to the files used for the March 2026 production run. Their internal headers still carry the old project name `005_usb_volt_tester_r1`. Zip archives of each Gerber set are attached to the GitHub Release.

## PCB specification (from the Gerber job files)

| | Main PCB | Stack panel |
|---|---|---|
| Size | 18.4 × 47.6 mm | 120.0 × 47.6 mm (6 boards) |
| Layers | 4 | 4 |
| Finished thickness | 0.78 mm | 0.78 mm |
| Stackup | 35 µm Cu / 0.11 mm prepreg / 0.40 mm core / 0.11 mm prepreg / 35 µm Cu | same |
| Surface finish | Immersion gold | Immersion gold |
| Minimum clearance / track | 0.18 mm / 0.20 mm | 0.18 mm / 0.18 mm |
| PCBWay job (Rev. 1 run) | W470267AS1N85 | W470267AS1N87 |
| PCBWay assembly quote | T-1N86W470267A | T-1N88W470267A (M1.6 nuts only) |

> **TODO (Sparsh):** add mask colour, copper weight if not 1 oz, and the edge/side plating used for heatsinking.

## BOM notes

- **Two sets of part numbers.** "Design" columns are what the schematic specifies. "Rev. 1 run" columns are the parts listed in PCBWay's final quotation (27 March 2026), including the substitutes approved in the quote replies. A quotation is not a purchase confirmation.
- **Unconfirmed resistors.** For R5, R9 and R14 the final quotation leaves the purchased part number blank, so those are marked **UNCONFIRMED**. Every listed option is a 1 % 0603 part of the same value.
- **Mounting nuts.** H1–H4 (Würth 9774015633R) are fitted on the stack panel, not on the main PCBA.
- **Programming pads.** J2 is a footprint only, never populated.

## Firmware

Program U1 over UPDI with `firmware/USB_VSense_fw_r1.hex`. The fuse settings are part of the HEX, so programming it also sets the fuses.

SHA-256 (also in `firmware/USB_VSense_fw_r1.hex.sha256`):
```
dc750f71fc5567e7b25dc1818dd63438305cd66886e6211a805e0ac14986a6e1  USB_VSense_fw_r1.hex
```

Provenance:
- This HEX was built on 21 May 2026 for the Rev. 1 production run.
- Rebuilding `firmware/MPLABX/usb_vsense_r1_2.X` with XC8 v3.10 and with XC8 v4.00 gives a byte-identical file.
- Contents: 3,610 bytes of flash, fuses `00 00 7D 00 00 F6 FF 00 00`, lock byte `0xC5` (no lock).

> **TODO (Sparsh):** optionally confirm that a shipped unit matches this HEX by reading it back with MPLAB IPE (the device is not locked).

## Assembly

> **TODO (Sparsh):** add the stack order, screw specification (length, head type, quantity) and an exploded photo.
