# USB VSense Rev. 1: production files

Everything needed to have USB VSense Rev. 1 made and assembled, without opening KiCad or MPLAB.

| Path | What it is |
|---|---|
| `gerbers/main_pcba/` | Gerbers and drill file for the main board (board 4/7), which carries all the electronics |
| `gerbers/enclosure_boards/` | Gerbers and drill file for the six enclosure boards (1/7–3/7 and 5/7–7/7), drawn side by side in one set |
| `BOM_USB_VSense_r1.csv` | Complete BOM: electronics, M1.6 nuts, PCBs and screws |
| `firmware/` | Production firmware HEX for the ATtiny406, with its SHA-256 checksum (rebuilds byte-identically from the published source) |

The Gerbers are the files used for the March 2026 production run. Their internal headers still carry the old project name `005_usb_volt_tester_r1`. Zip archives of each Gerber set are attached to the GitHub Release.

## PCB specification

| | Main PCB (4/7) | Enclosure boards (1/7–3/7, 5/7–7/7) |
|---|---|---|
| Size | 18.4 × 47.6 mm | 6 boards of 18.3 × 47.5 mm, drawn side by side (set outline 120.0 × 47.6 mm) |
| Layers | **4** | **2** |
| Thickness | **0.8 mm** | **0.8 mm** |
| Surface finish | Immersion gold | Immersion gold |
| Solder mask / silkscreen | Black / white | Black / white |
| Edge plating | Yes, continuous (see fabrication notes) | Yes, continuous (see fabrication notes) |
| Assembly | All electronic parts (both sides) | Four M1.6 SMT nuts on board 7/7 only |

Notes on the files:
- **The enclosure boards are 2-layer.** The KiCad project uses one 4-layer stackup for all seven boards, so KiCad also exported empty inner-layer Gerbers for the enclosure set, plus a job file that says 4 layers. Those files were sent with the order, and the fab was told to ignore the inner layers. They have been removed from `gerbers/enclosure_boards/` so the set can't be quoted as 4-layer by mistake.
- **The main board's job file says 0.78 mm.** That is the sum of KiCad's design stackup (4 × 35 µm Cu, 0.11 / 0.40 / 0.11 mm FR4, 2 × 10 µm mask), not the ordered value. Order 0.8 mm. The fab's actual 0.8 mm 4-layer stackup is not documented here.
- **Copper weight.** The KiCad stackup uses 35 µm on all layers. **TODO (Sparsh):** confirm the copper weight that was ordered.

## Fabrication notes

These are the instructions given to the fab, taken from the order correspondence (September 2024 for Gamma, March 2026 for Rev. 1). If you order the boards yourself, pass them on.

1. **Edge plating on all seven boards.** Plate the edges wherever copper and solder mask extend to the board outline.
2. **Tabs in the same place on every board.** Because of the continuous edge plating, the fab has to add break-off tabs (attachment points). Put them in the **same position on all seven boards**. In earlier orders the tabs were placed differently on different boards, and the break-off marks spoiled the look of the assembled stack.
3. **0.2 mm corner radius on the main board.** The main board needs 0.2 mm radius corners at the USB-C plug (P1), as marked on the `User_Comments` layer. That layer (`USB_VSense_r1-User_Comments.gbr`) is included in `gerbers/main_pcba/`; send it with the order. This applies to the main board only.
4. **Board spacing.** For the 2024 Gamma enclosure order the fab increased the spacing between the six designs to 1 mm, so that the boards would not join up after edge plating.
5. **Solder-mask bridges.** With black mask, the fab's stated minimum IC pad spacing for a mask bridge is 0.22 mm (0.19 mm for other colours). Some IC pads on the main board are closer than that. For Rev. 1, as for Gamma, the bridge was dropped there and one opening used instead, after the assembly team confirmed it would not affect yield.
6. **No X-outs needed** on the enclosure boards: they have no electrical function.

## PCBA notes (main board)

These notes were given to the assembler, in addition to the BOM.

1. **P1 (USB-C plug) alignment needs special attention.** In the 32-unit 2024 batch, several plugs were soldered misaligned. That is not acceptable.
2. **Keep P1's pick-and-place cap on.** The JAE DX07P024AJ5R1500 ships with a protective pick-and-place cap. Leave it on through the whole PCBA process to keep the contacts clean.
3. **Solder P1's shield to the PCB tabs.** This was a defect in an earlier (beta) batch.
4. **D5 and D7 polarity marks differ.** D5's polarity mark goes to the **cathode (C)** marking on the PCB. D7's polarity mark goes to the **anode (A)** marking. The two bi-colour LEDs have different pin-outs, so treat them separately.
5. **R11 is 0805.** The other resistors are 0603. Don't accept a substitute for R11 in a different package (this came up in the Rev. 1 quotation).
6. **H1–H4 are not on the main board.** They are in the same KiCad project but go on enclosure board 7/7 (four per board, labelled H1–H4 on its top silkscreen). They have no orientation and only need to be placed in their holes.

> **TODO (Sparsh):** add the pick-and-place (centroid) file for the main board. The 2024 Gamma order used `005_usb_volt_tester_gamma-all.pos`. One way to regenerate it from Rev. 1 (then delete the rows that aren't on the main board, such as H1–H4):
> ```
> kicad-cli pcb export pos --format csv --units mm --side both -o USB_VSense_r1-pos.csv hardware/KiCAD/USB_VSense_r1/USB_VSense_r1.kicad_pcb
> ```

## BOM notes

- **Two sets of part numbers.** "Design" columns are what the schematic specifies. "Rev. 1 run" columns are the parts listed in the assembler's final quotation (March 2026), including the substitutes that were approved. A quotation is not a purchase confirmation.
- **Unconfirmed resistors.** For R5, R9 and R14 the final quotation leaves the purchased part number blank, so those are marked **UNCONFIRMED**. Every listed option is a 1 % 0603 part of the same value.
- **Mounting nuts.** H1–H4 (Würth 9774015633R) are soldered to enclosure board 7/7, not to the main PCBA.
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

## Assembly

USB VSense is a stack of seven 0.8 mm PCBs, held together by four M1.6 screws threaded into four SMT nuts on the bottom board. The boards are marked 1/7 to 7/7 in silkscreen, top to bottom.

| Board | Role | Features |
|---|---|---|
| 1/7 | Faceplate (top) | Voltage markings (05–48), four 1.8 mm screw holes |
| 2/7 | Light guide | Eight 5.08 mm LED windows, 1.8 mm screw holes |
| 3/7 | Light guide | Eight 5.08 mm LED windows, 1.8 mm screw holes |
| 4/7 | **Main PCBA** | LEDs and USB-C plug/receptacle on top, all other parts and the UPDI pads (J2) on the bottom |
| 5/7 | Spacer | 11.8 × 20.3 mm cutout for the main board's bottom-side parts, four 3.5 mm holes for the nuts, holes for the receptacle's rear pegs |
| 6/7 | Spacer | 11.8 × 20.3 mm cutout, four 3.5 mm holes for the nuts |
| 7/7 | Base (bottom) | Four M1.6 SMT nuts (H1–H4) facing up; CE, UKCA, WEEE and RoHS marks on the underside |

How the stack fits:
- **LEDs:** the main board's LEDs sit in the windows of boards 2/7 and 3/7.
- **Bottom-side parts:** the parts on the main board's underside sit in the cutouts of boards 5/7 and 6/7.
- **Nuts:** the 1.5 mm nuts on 7/7 sit in the 3.5 mm holes of 6/7 and 5/7, which together are 1.6 mm thick.

### You need

- 1 × main PCBA (4/7), assembled
- 6 × enclosure boards (1/7–3/7, 5/7–7/7), tabs removed, with the four nuts already soldered to 7/7
- 4 × M1.6 screws (see the TODO below)
- A UPDI programmer (e.g. PICkit 4 with MPLAB IPE) and a USB-C power source

### Steps

1. **Program the main board first.** The UPDI pads (J2) are on the underside of 4/7 and are covered once the stack is assembled. Program `firmware/USB_VSense_fw_r1.hex` and verify it.
2. **Test the main board on its own.** Remove P1's pick-and-place cap if it is still fitted, then connect a USB-C source. At power-up the LEDs sweep from 48 V down to 5 V. With a plain 5 V source the 5 V LED then stays on.
3. **Lay down the base.** Place 7/7 with the nuts facing up (compliance marks facing down).
4. **Add the spacers.** Stack 6/7, then 5/7, over the nuts. The nuts pass through the 3.5 mm holes.
5. **Add the main PCBA (4/7)** with its LEDs facing up. The receptacle's rear pegs go into the holes in 5/7, and the bottom-side parts sit in the cutouts.
6. **Add the light guides and faceplate.** Stack 3/7, then 2/7, then the faceplate 1/7.
7. **Align the edges** so the tab marks line up on all seven boards (fabrication note 2).
8. **Screw the stack together.** Insert the four M1.6 screws from the faceplate side into the nuts. Tighten until the stack is closed and firm. Don't overtighten: the boards are only 0.8 mm thick.
9. **Final test.** Repeat the power-up check from step 2 on the finished unit.

> **TODO (Sparsh):**
> - Screw specification: length, head type, material. It isn't in any quotation or order email.
> - A tightening torque, if you have one.
> - An exploded photo.
