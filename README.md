## M6x09-II-SBC

A minature seventeen-component singleboard computer to experiment with the `x09` architecture.

_Preface_

So often, engineers go crazy on adding features to create the perfect singleboard computer. In the 8-bit world, it is really most interesting to get to the nut of the matter: Can we create the simplest hardware system to test those things we are building-out, such as the bits of a novel kernel or memory management, or evolution to multiprocessing?

This project is what we can discern is the _minimal_ system using modern connetions to a laptop running debian where we can get to understanding how the _ideal_ is in terms of _novel metal_.

_Insistences_

The ideal project should NOT be exploited into higher monetary platitudes simply because people love it. Rather, what open-source tools are the most trustworthy (free of manipulation by intellectual corruption) that will have any engineer who replicates this project have the same success in whatever the future time may be?

_The board_

Layout (v1.1)

![image](/design/Layout_M6809-II-SBC.png)

The manufactured ouput (unassembled version)

![image](/design/bare-board.jpg)

_Parts list_

ID,Name,Designator,Footprint,Quantity,Part Number,Manufacturer,Supplier,Supplier Part Number,Unit Price (EUR)
1,10uF,C1,CAP-D3.0XF1.5,1,ECA-1VM100,Panasonic,Mouser,667-ECA-1VM100,0.167
2,18pF,"C2,C3",RAD-0.1,2,K180J15C0GF5TL2,Vishay,Mouser,594-K180J15C0GF5TL2,0.214
3,0.1uF,"C4,C5,C6,C7,C8",RAD-0.1,5,K104K15X7RH53H5G,Vishay,Mouser,594-K104K15X7RH53H5G,0.698
4,1N914,D1,DO-35,1,1N914,Fairchild,Mouser,512-1N914,0.093
5,USB POWER JUMPER,H1,HDR-2X1/2.54,1,826629-2,TE Connectivity,Mouser,571-826629-2,0.316
6,EXPANSION HEADER,H2,HDR-16X2/2.54,1,5-146257-8,TE Connectivity,Mouser,571-5-146257-8,2.25
7,POWER,LED1,LED-3MM/2.54,1,HLMP-1503,Broadcom,Mouser,630-HLMP-1503,0.428
8,USB FTDI,P1,HDR-6X1/2.54,1,5-146281-6,TE Connectivity,Mouser,571-1032396,0.693
9,330,R1,AXIAL-0.3,1,RN60D3300FB14,Vishay,Mouser,71-RN60D3300F,0.285
10,2.2K,RN1,SIP-9,1,4609X-101-222LF,Bourns,Mouser,652-4609X-1LF-2.2K,0.428
11,RESET,SW1,TACTILE-PTH-EZ,1,TS02-66-50-BK-100-LCR-D,CUI Devices,Mouser,179-TS026650BK100LCR,0.093
12,68B50,U1,DIP24-600,1,DILB24P-224TLF,Amphenol,Mouser,649-DILB24P-224TLF,0.446
13,27C128,U2,DIP28-600,1,1-2199299-2,TE Connectivity,Mouser,571-1-2199299-2,0.688
14,62256P,U3,DIP28-600,1,1-2199299-2,TE Connectivity,Mouser,571-1-2199299-2,0.688
15,68B09,U4,DIP40-600-Ladder,1,1-2199299-5,TE Connectivity,Mouser,571-1-2199299-5,0.874
16,7.3728 MHz,X1,OSC-49S-1,1,ABL-7.3728MHZ-B2,Abracon,Mouser,815-ABL-7.3728-B2,0.307
17,74LS00,U5,DIP14-300,1,1-2199298-3,TE Connectivity,Mouser,571-1-2199298-3,0.205

#### Reference

Refer to [this](https://jefftranter.blogspot.com/2019/01/a-6809-single-board-computer.html).
EPROM programmer [software](https://drive.proton.me/urls/3ZPDKVPCFG#tx9o8VUVD5vE).