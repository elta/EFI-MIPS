/*++

Copyright (c) 2005 - 2006 Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution. The full text of the license may be found at         
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:
  QueryTable.c
    
Abstract:
  Build a table, each item is (Key, Info) pair.
  And give a interface of query a string out of a table.

Revision History

--*/

#include "EfiShellLib.h"
#include "QueryTable.h"
#include "PrintInfo.h"
#include "Smbios.h"

#include STRING_DEFINES_FILE

TABLE_ITEM  SystemWakeupTypeTable[] = {
  {
    0x0,
    L" Reserved"
  },
  {
    0x1,
    L" Other"
  },
  {
    0x2,
    L" Unknown"
  },
  {
    0x3,
    L" APM Timer"
  },
  {
    0x4,
    L" Modem Ring"
  },
  {
    0x5,
    L" LAN Remote"
  },
  {
    0x6,
    L" Power Switch"
  },
  {
    0x7,
    L" AC Power Restored"
  }
};

TABLE_ITEM  SystemEnclosureTypeTable[] = {
  {
    0x01,
    L"  None"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  Desktop"
  },
  {
    0x04,
    L"  Low Profile Desktop"
  },
  {
    0x05,
    L"  Pizza Box"
  },
  {
    0x06,
    L"  Mini Tower"
  },
  {
    0x07,
    L"  Tower"
  },
  {
    0x08,
    L"  Portable"
  },
  {
    0x09,
    L"  LapTop"
  },
  {
    0x0A,
    L"  Notebook"
  },
  {
    0x0B,
    L"  Hand Held"
  },
  {
    0x0C,
    L"  Docking Station"
  },
  {
    0x0D,
    L"  All in One"
  },
  {
    0x0E,
    L"  Sub Notebook"
  },
  {
    0x0F,
    L"  Space-saving"
  },
  {
    0x10,
    L"  Main Server Chassis"
  },
  {
    0x11,
    L"  Expansion Chassis"
  },
  {
    0x12,
    L"  SubChassis"
  },
  {
    0x13,
    L"  Sub Notebook"
  },
  {
    0x14,
    L"  Bus Expansion Chassis"
  },
  {
    0x15,
    L"  Peripheral Chassis"
  },
  {
    0x16,
    L"  RAID Chassis"
  },
  {
    0x17,
    L"  Rack Mount Chassis"
  },
  {
    0x18,
    L"  Sealed-case PC"
  },
};

TABLE_ITEM  SystemEnclosureStatusTable[] = {
  {
    0x1,
    L" Other"
  },
  {
    0x2,
    L" Unknown"
  },
  {
    0x3,
    L" Safe"
  },
  {
    0x4,
    L" Warning"
  },
  {
    0x5,
    L" Critical"
  },
  {
    0x6,
    L" Non-recoverable"
  }
};

TABLE_ITEM  SESecurityStatusTable[] = {
  {
    0x1,
    L" Other"
  },
  {
    0x2,
    L" Unknown"
  },
  {
    0x3,
    L" None"
  },
  {
    0x4,
    L" External interface locked out"
  },
  {
    0x5,
    L" External interface enabled"
  }
};

TABLE_ITEM  ProcessorTypeTable[] = {
  {
    0x1,
    L" Other"
  },
  {
    0x2,
    L" Unknown"
  },
  {
    0x3,
    L" Central Processor"
  },
  {
    0x4,
    L" Math Processor"
  },
  {
    0x5,
    L" DSP Processor"
  },
  {
    0x6,
    L" Video Processor "
  },
};

TABLE_ITEM  ProcessorUpgradeTable[] = {
  {
    0x01,
    L"Other"
  },
  {
    0x02,
    L"Unknown"
  },
  {
    0x03,
    L"Daughter Board"
  },
  {
    0x04,
    L"ZIF Socket"
  },
  {
    0x05,
    L"Replaceable Piggy Back"
  },
  {
    0x06,
    L"None"
  },
  {
    0x07,
    L"LIF Socket"
  },
  {
    0x08,
    L"Slot 1"
  },
  {
    0x09,
    L"Slot 2"
  },
  {
    0x0A,
    L"370-pin socket"
  },
  {
    0x0B,
    L"Slot A"
  },
  {
    0x0C,
    L"Slot M"
  },
  {
    0x0D,
    L"Socket 423"
  },
  {
    0x0E,
    L"Socket A"
  },
  {
    0x0F,
    L"Socket 478"
  },
  {
    0x10,
    L"Socket 754"
  },
  {
    0x11,
    L"Socket 940"
  },
  {
    0x12,
    L"Socket 939"
  },
  {
    0x13,
    L"Socket mPGA604"
  },
  {
    0x14,
    L"Socket LGA771"
  },
  {
    0x15,
    L"Socket LGA775"
  }
};

TABLE_ITEM  McErrorDetectMethodTable[] = {
  {
    0x01,
    L"Other"
  },
  {
    0x02,
    L"Unknown"
  },
  {
    0x03,
    L"None"
  },
  {
    0x04,
    L"8-bit Parity"
  },
  {
    0x05,
    L"32-bit ECC"
  },
  {
    0x06,
    L"64-bit ECC"
  },
  {
    0x07,
    L"128-bit ECC"
  },
  {
    0x08,
    L"CRC"
  },
};

TABLE_ITEM  McErrorCorrectCapabilityTable[] = {
  {
    0,
    L"Other"
  },
  {
    1,
    L"Unknown"
  },
  {
    2,
    L"None"
  },
  {
    3,
    L"Single Bit Error Correcting"
  },
  {
    4,
    L"Double Bit Error Correcting"
  },
  {
    5,
    L"Error Scrubbing"
  },
};

TABLE_ITEM  McInterleaveSupportTable[] = {
  {
    0x01,
    L"Other"
  },
  {
    0x02,
    L"Unknown"
  },
  {
    0x03,
    L"One Way Interleave"
  },
  {
    0x04,
    L"Two Way Interleave"
  },
  {
    0x05,
    L"Four Way Interleave"
  },
  {
    0x06,
    L"Eight Way Interleave"
  },
  {
    0x07,
    L"Sixteen Way Interleave"
  }
};

TABLE_ITEM  McMemorySpeedsTable[] = {
  {
    0,
    L" Other"
  },
  {
    1,
    L" Unknown"
  },
  {
    2,
    L" 70ns"
  },
  {
    3,
    L" 60ns"
  },
  {
    4,
    L" 50ns"
  },
};

TABLE_ITEM  MemoryModuleVoltageTable[] = {
  {
    0,
    L" 5V"
  },
  {
    1,
    L" 3.3V"
  },
  {
    2,
    L" 2.9V"
  },
};

TABLE_ITEM  MmMemoryTypeTable[] = {
  {
    0,
    L" Other"
  },
  {
    1,
    L" Unknown"
  },
  {
    2,
    L" Standard"
  },
  {
    3,
    L" Fast Page Mode"
  },
  {
    4,
    L" EDO"
  },
  {
    5,
    L" Parity"
  },
  {
    6,
    L" ECC "
  },
  {
    7,
    L" SIMM"
  },
  {
    8,
    L" DIMM"
  },
  {
    9,
    L" Burst EDO"
  },
  {
    10,
    L" SDRAM"
  }
};

TABLE_ITEM  MmErrorStatusTable[] = {
  {
    0,
    L" Uncorrectable errors received"
  },
  {
    1,
    L" Correctable errors received"
  },
  {
    2,
    L" Error Status obtained from the event log"
  }
};

TABLE_ITEM  CacheSRAMTypeTable[] = {
  {
    0,
    L" Other"
  },
  {
    1,
    L" Unknown"
  },
  {
    2,
    L" Non-Burst"
  },
  {
    3,
    L" Burst"
  },
  {
    4,
    L" Pipeline Burst"
  },
  {
    5,
    L" Synchronous"
  },
  {
    6,
    L" Asynchronous"
  },
};

TABLE_ITEM  CacheErrCorrectingTypeTable[] = {
  {
    0x01,
    L"Other"
  },
  {
    0x02,
    L"Unknown"
  },
  {
    0x03,
    L"None"
  },
  {
    0x04,
    L"Parity"
  },
  {
    0x05,
    L"Single-bit ECC"
  },
  {
    0x06,
    L"Multi-bit ECC"
  },
  {
    0x07,
    L"Sixteen Way Interleave"
  }
};

TABLE_ITEM  CacheSystemCacheTypeTable[] = {
  {
    0x01,
    L"Other"
  },
  {
    0x02,
    L"Unknown"
  },
  {
    0x03,
    L"Instruction"
  },
  {
    0x04,
    L"Data"
  },
  {
    0x05,
    L"Unified"
  }
};

TABLE_ITEM  CacheAssociativityTable[] = {
  {
    0x01,
    L"Other"
  },
  {
    0x02,
    L"Unknown"
  },
  {
    0x03,
    L"Direct Mapped"
  },
  {
    0x04,
    L"2-way Set-Associative"
  },
  {
    0x05,
    L"4-way Set-Associative"
  },
  {
    0x06,
    L"Fully Associative"
  }
};

TABLE_ITEM  PortConnectorTypeTable[] = {
  {
    0x00,
    L"None"
  },
  {
    0x01,
    L"Centronics"
  },
  {
    0x02,
    L"Mini Centronics"
  },
  {
    0x03,
    L"Proprietary"
  },
  {
    0x04,
    L"DB-25 pin male"
  },
  {
    0x05,
    L"DB-25 pin female"
  },
  {
    0x06,
    L"DB-15 pin male"
  },
  {
    0x07,
    L"DB-15 pin female"
  },
  {
    0x08,
    L"DB-9 pin male"
  },
  {
    0x09,
    L"DB-9 pin female"
  },
  {
    0x0A,
    L"RJ-11"
  },
  {
    0x0B,
    L"RJ-45"
  },
  {
    0x0C,
    L"50 Pin MiniSCSI"
  },
  {
    0x0D,
    L"Mini-DIN"
  },
  {
    0x0E,
    L"Micro-DIN"
  },
  {
    0x0F,
    L"PS/2"
  },
  {
    0x10,
    L"Infrared"
  },
  {
    0x11,
    L"HP-HIL"
  },
  {
    0x12,
    L"Access Bus (USB)"
  },
  {
    0x13,
    L"SSA SCSI"
  },
  {
    0x14,
    L"Circular DIN-8 male"
  },
  {
    0x15,
    L"Circular DIN-8 female"
  },
  {
    0x16,
    L"On Board IDE"
  },
  {
    0x17,
    L"On Board Floppy"
  },
  {
    0x18,
    L"9 Pin Dual Inline (pin 10 cut)"
  },
  {
    0x19,
    L"25 Pin Dual Inline (pin 26 cut)"
  },
  {
    0x1A,
    L"50 Pin Dual Inline"
  },
  {
    0x1B,
    L"68 Pin Dual Inline"
  },
  {
    0x1C,
    L"On Board Sound Input from CD-ROM"
  },
  {
    0x1D,
    L"Mini-Centronics Type-14"
  },
  {
    0x1E,
    L"Mini-Centronics Type-26"
  },
  {
    0x1F,
    L"Mini-jack (headphones)"
  },
  {
    0x20,
    L"BNC"
  },
  {
    0x21,
    L"1394"
  },
  {
    0xA0,
    L"PC-98"
  },
  {
    0xA1,
    L"PC-98Hireso"
  },
  {
    0xA2,
    L"PC-H98"
  },
  {
    0xA3,
    L"PC-98Note"
  },
  {
    0xA4,
    L"PC-98Full"
  },
  {
    0xFF,
    L"Other"
  },
};

TABLE_ITEM  PortTypeTable[] = {
  {
    0x00,
    L"None"
  },
  {
    0x01,
    L"Parallel Port XT/AT Compatible"
  },
  {
    0x02,
    L"Parallel Port PS/2"
  },
  {
    0x03,
    L"Parallel Port ECP"
  },
  {
    0x04,
    L"Parallel Port EPP"
  },
  {
    0x05,
    L"Parallel Port ECP/EPP"
  },
  {
    0x06,
    L"Serial Port XT/AT Compatible"
  },
  {
    0x07,
    L"Serial Port 16450 Compatible"
  },
  {
    0x08,
    L"Serial Port 16550 Compatible"
  },
  {
    0x09,
    L"Serial Port 16550A Compatible"
  },
  {
    0x0A,
    L"SCSI Port"
  },
  {
    0x0B,
    L"MIDI Port"
  },
  {
    0x0C,
    L"Joy Stick Port"
  },
  {
    0x0D,
    L"Keyboard Port"
  },
  {
    0x0E,
    L"Mouse Port"
  },
  {
    0x0F,
    L"SSA SCSI"
  },
  {
    0x10,
    L"USB"
  },
  {
    0x11,
    L"FireWire (IEEE P1394)"
  },
  {
    0x12,
    L"PCMCIA Type II"
  },
  {
    0x13,
    L"PCMCIA Type II"
  },
  {
    0x14,
    L"PCMCIA Type III"
  },
  {
    0x15,
    L"Cardbus"
  },
  {
    0x16,
    L"Access Bus Port"
  },
  {
    0x17,
    L"SCSI II"
  },
  {
    0x18,
    L"SCSI Wide"
  },
  {
    0x19,
    L"PC-98"
  },
  {
    0x1A,
    L"PC-98-Hireso"
  },
  {
    0x1B,
    L"PC-H98"
  },
  {
    0x1C,
    L"Video Port"
  },
  {
    0x1D,
    L"Audio Port"
  },
  {
    0x1E,
    L"Modem Port"
  },
  {
    0x1F,
    L"Network Port"
  },
  {
    0xA0,
    L"8251 Compatible"
  },
  {
    0xA1,
    L"8251 FIFO Compatible"
  },
  {
    0xFF,
    L"Other "
  },
};

TABLE_ITEM  SystemSlotTypeTable[] = {
  {
    0x01,
    L"Other"
  },
  {
    0x02,
    L"Unknown"
  },
  {
    0x03,
    L"ISA"
  },
  {
    0x04,
    L"MCA"
  },
  {
    0x05,
    L"EISA"
  },
  {
    0x06,
    L"PCI"
  },
  {
    0x07,
    L"PC Card (PCMCIA)"
  },
  {
    0x08,
    L"VL-VESA"
  },
  {
    0x09,
    L"Proprietary"
  },
  {
    0x0A,
    L"Processor Card Slot"
  },
  {
    0x0B,
    L"Proprietary Memory Card Slot"
  },
  {
    0x0C,
    L"I/O Riser Card Slot"
  },
  {
    0x0D,
    L"NuBus"
  },
  {
    0x0E,
    L"PCI - 66MHz Capable"
  },
  {
    0x0F,
    L"AGP"
  },
  {
    0x10,
    L"AGP 2X"
  },
  {
    0x11,
    L"AGP 4X"
  },
  {
    0x12,
    L"PCI-X"
  },
  {
    0xA0,
    L"PC-98/C20 "
  },
  {
    0xA1,
    L"PC-98/C24 "
  },
  {
    0xA2,
    L"PC-98/E "
  },
  {
    0xA3,
    L"PC-98/Local Bus "
  },
  {
    0xA4,
    L"PC-98/Card "
  },
  {
    0xA5,
    L"PCI Express "
  },
};

TABLE_ITEM  SystemSlotDataBusWidthTable[] = {
  {
    0x01,
    L" Other"
  },
  {
    0x02,
    L" Unknown"
  },
  {
    0x03,
    L" 8 bit"
  },
  {
    0x04,
    L" 16 bit"
  },
  {
    0x05,
    L" 32 bit"
  },
  {
    0x06,
    L" 64 bit"
  },
  {
    0x07,
    L" 128 bit"
  },
};

TABLE_ITEM  SystemSlotCurrentUsageTable[] = {
  {
    0x01,
    L" Other"
  },
  {
    0x02,
    L" Unknown"
  },
  {
    0x03,
    L" Available"
  },
  {
    0x04,
    L" In use"
  },
};

TABLE_ITEM  SystemSlotLengthTable[] = {
  {
    0x01,
    L" Other"
  },
  {
    0x02,
    L" Unknown"
  },
  {
    0x03,
    L" Short length"
  },
  {
    0x04,
    L" Long Length"
  },
};

TABLE_ITEM  SlotCharacteristics1Table[] = {
  {
    0,
    L" Characteristics Unknown"
  },
  {
    1,
    L" Provides 5.0 Volts"
  },
  {
    2,
    L" Provides 3.3 Volts"
  },
  {
    3,
    L" Slot's opening is shared with another slot, e.g. PCI/EISA shared slot."
  },

  {
    4,
    L" PC Card slot supports PC Card-16"
  },
  {
    5,
    L" PC Card slot supports CardBus"
  },
  {
    6,
    L" PC Card slot supports Zoom Video "
  },
  {
    7,
    L" PC Card slot supports Modem Ring Resume "
  }
};

TABLE_ITEM  SlotCharacteristics2Table[] = {
  {
    0,
    L" PCI slot supports Power Management Enable (PME#) signal"
  },
  {
    1,
    L" Slot supports hot-plug devices"
  },
  {
    2,
    L" PCI slot supports SMBus signal"
  }
};

TABLE_ITEM  OnboardDeviceTypesTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  Video"
  },
  {
    0x04,
    L"  SCSI Controller"
  },
  {
    0x05,
    L"  Ethernet"
  },
  {
    0x06,
    L"  Token Ring"
  },
  {
    0x07,
    L"  Sound"
  },
};

TABLE_ITEM  SELTypesTable[] = {
  {
    0x00,
    L" Reserved."
  },
  {
    0x01,
    L" Single-bit ECC memory error"
  },
  {
    0x02,
    L" Multi-bit ECC memory error"
  },
  {
    0x03,
    L" Parity memory error"
  },
  {
    0x04,
    L" Bus time-out"
  },
  {
    0x05,
    L" I/O Channel Check"
  },
  {
    0x06,
    L" Software NMI"
  },
  {
    0x07,
    L" POST Memory Resize"
  },
  {
    0x08,
    L" POST Error"
  },
  {
    0x09,
    L" PCI Parity Error"
  },
  {
    0x0A,
    L" PCI System Error"
  },
  {
    0x0B,
    L" CPU Failure"
  },
  {
    0x0C,
    L" EISA FailSafe Timer time-out"
  },
  {
    0x0D,
    L" Correctable memory log disabled"
  },
  {
    0x0E,
    L" Logging disabled for a specific Event Type"
  },
  {
    0x0F,
    L" Reserved"
  },
  {
    0x10,
    L" System Limit Exceeded"
  },
  {
    0x11,
    L" Asynchronous hardware timer expired and issued a system reset"
  },
  {
    0x12,
    L" System configuration information"
  },
  {
    0x13,
    L" Hard-disk information"
  },
  {
    0x14,
    L" System reconfigured"
  },
  {
    0x15,
    L" Uncorrectable CPU-complex error"
  },
  {
    0x16,
    L" Log Area Reset/Cleared"
  },
  {
    0x17,
    L" System boot"
  },
  {
    0x7F18,
    L" Unused by SMBIOS specification"
  },
  {
    0xFE80,
    L" System and OEM specified"
  },
  {
    0xFF,
    L" End-of-log"
  },
};

TABLE_ITEM  SELVarDataFormatTypeTable[] = {
  {
    0x00,
    L" None "
  },
  {
    0x01,
    L" Handle "
  },
  {
    0x02,
    L" Multiple-Event "
  },
  {
    0x03,
    L" Multiple-Event Handle "
  },
  {
    0x04,
    L" POST Results Bitmap "
  },
  //
  // Defined below
  //
  {
    0x05,
    L" System Management Type"
  },
  //
  // Defined below
  //
  {
    0x06,
    L" Multiple-Event System Management Type "
  },
  {
    0x7F07,
    L" Unused "
  },
  {
    0xFF80,
    L" OEM assigned "
  },
};

TABLE_ITEM  PostResultsBitmapDw1Table[] = {
  {
    0,
    L" Channel 2 Timer error "
  },
  {
    1,
    L" Master PIC (8259 #1) error "
  },
  {
    2,
    L" Slave PIC (8259 #2) error "
  },
  {
    3,
    L" CMOS Battery Failure "
  },
  {
    4,
    L" CMOS System Options Not Set "
  },
  {
    5,
    L" CMOS Checksum Error "
  },
  {
    6,
    L" CMOS Configuration Error "
  },
  {
    7,
    L" Mouse and Keyboard Swapped "
  },
  {
    8,
    L" Keyboard Locked "
  },
  {
    9,
    L" Keyboard Not Functional "
  },
  {
    10,
    L" Keyboard Controller Not Functional "
  },
  {
    11,
    L" CMOS Memory Size Different "
  },
  {
    12,
    L" Memory Decreased in Size "
  },
  {
    13,
    L" Cache Memory Error "
  },
  {
    14,
    L" Floppy Drive 0 Error "
  },
  {
    15,
    L" Floppy Drive 1 Error "
  },
  {
    16,
    L" Floppy Controller Failure "
  },
  {
    17,
    L" Number of ATA Drives Reduced Error "
  },
  {
    18,
    L" CMOS Time Not Set "
  },
  {
    19,
    L" DDC Monitor Configuration Change "
  },
  {
    20,
    L" Reserved, set to 0 "
  },
  {
    21,
    L" Reserved, set to 0 "
  },
  {
    22,
    L" Reserved, set to 0 "
  },
  {
    23,
    L" Reserved, set to 0 "
  },
  {
    24,
    L" Second DWORD has valid data "
  },
  {
    25,
    L" Reserved, set to 0 "
  },
  {
    26,
    L" Reserved, set to 0 "
  },
  {
    27,
    L" Reserved, set to 0 "
  },
  {
    28,
    L" Normally 0; available for OEM assignment "
  },
  {
    29,
    L" Normally 0; available for OEM assignment "
  },
  {
    30,
    L" Normally 0; available for OEM assignment "
  },
  {
    31,
    L" Normally 0; available for OEM assignment "
  },
};

TABLE_ITEM  PostResultsBitmapDw2Table[] = {
  {
    0,
    L" Normally 0; available for OEM assignment "
  },
  {
    1,
    L" Normally 0; available for OEM assignment "
  },
  {
    2,
    L" Normally 0; available for OEM assignment "
  },
  {
    3,
    L" Normally 0; available for OEM assignment "
  },
  {
    4,
    L" Normally 0; available for OEM assignment "
  },
  {
    5,
    L" Normally 0; available for OEM assignment "
  },
  {
    6,
    L" Normally 0; available for OEM assignment "
  },
  {
    7,
    L" PCI Memory Conflict "
  },
  {
    8,
    L" PCI I/O Conflict "
  },
  {
    9,
    L" PCI IRQ Conflict "
  },
  {
    10,
    L" PNP Memory Conflict "
  },
  {
    11,
    L" PNP 32 bit Memory Conflict "
  },
  {
    12,
    L" PNP I/O Conflict "
  },
  {
    13,
    L" PNP IRQ Conflict "
  },
  {
    14,
    L" PNP DMA Conflict "
  },
  {
    15,
    L" Bad PNP Serial ID Checksum "
  },
  {
    16,
    L" Bad PNP Resource Data Checksum "
  },
  {
    17,
    L" Static Resource Conflict "
  },
  {
    18,
    L" NVRAM Checksum Error, NVRAM Cleared "
  },
  {
    19,
    L" System Board Device Resource Conflict "
  },
  {
    20,
    L" Primary Output Device Not Found "
  },
  {
    21,
    L" Primary Input Device Not Found "
  },
  {
    22,
    L" Primary Boot Device Not Found "
  },
  {
    23,
    L" NVRAM Cleared By Jumper "
  },
  {
    24,
    L" NVRAM Data Invalid, NVRAM Cleared "
  },
  {
    25,
    L" FDC Resource Conflict "
  },
  {
    26,
    L" Primary ATA Controller Resource Conflict "
  },
  {
    27,
    L" Secondary ATA Controller Resource Conflict "
  },
  {
    28,
    L" Parallel Port Resource Conflict "
  },
  {
    29,
    L" Serial Port 1 Resource Conflict "
  },
  {
    30,
    L" Serial Port 2 Resource Conflict "
  },
  {
    31,
    L" Audio Resource Conflict "
  },
};

TABLE_ITEM  SELSysManagementTypesTable[] = {
  {
    0x01,
    L" +2.5V Out of range, #2 "
  },
  {
    0x02,
    L" +3.3V Out of range "
  },
  {
    0x03,
    L" +5V Out of range "
  },
  {
    0x04,
    L" -5V Out of range "
  },
  {
    0x05,
    L" +12V Out of range "
  },
  {
    0x06,
    L" -12V Out of range "
  },
  {
    0x0F07,
    L" Reserved for future out-of-range voltage levels "
  },
  {
    0x10,
    L" System board temperature out of range "
  },
  {
    0x11,
    L" Processor #1 temperature out of range "
  },
  {
    0x12,
    L" Processor #2 temperature out of range "
  },
  {
    0x13,
    L" Processor #3 temperature out of range "
  },
  {
    0x14,
    L" Processor #4 temperature out of range "
  },
  {
    0x1F15,
    L" Reserved for future out-of-range temperatures"
  },
  {
    0x2720,
    L" Fan n (n = 0 to 7) Out of range "
  },
  {
    0x2F28,
    L" Reserved for future assignment via this specification "
  },
  {
    0x30,
    L" Chassis secure switch activated "
  },
};

TABLE_ITEM  PMALocationTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  System board or motherboard"
  },
  {
    0x04,
    L"  ISA add-on card"
  },
  {
    0x05,
    L"  EISA add-on card"
  },
  {
    0x06,
    L"  PCI add-on card"
  },
  {
    0x07,
    L"  MCA add-on card"
  },
  {
    0x08,
    L"  PCMCIA add-on card"
  },
  {
    0x09,
    L"  Proprietary add-on card"
  },
  {
    0x0A,
    L"  NuBus"
  },
  {
    0xA0,
    L"  PC-98/C20 add-on card"
  },
  {
    0xA1,
    L"  PC-98/C24 add-on card"
  },
  {
    0xA2,
    L"  PC-98/E add-on card"
  },
  {
    0xA3,
    L"  PC-98/Local bus add-on card"
  }
};

TABLE_ITEM  PMAUseTable[] = {
  {
    0x01,
    L" Other"
  },
  {
    0x02,
    L" Unknown"
  },
  {
    0x03,
    L" System memory"
  },
  {
    0x04,
    L" Video memory"
  },
  {
    0x05,
    L" Flash memory"
  },
  {
    0x06,
    L" Non-volatile RAM"
  },
  {
    0x07,
    L" Cache memory"
  }
};

TABLE_ITEM  PMAErrorCorrectionTypesTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  None"
  },
  {
    0x04,
    L"  Parity"
  },
  {
    0x05,
    L"  Single-bit ECC"
  },
  {
    0x06,
    L"  Multi-bit ECC"
  },
  {
    0x07,
    L"  CRC"
  }
};

TABLE_ITEM  MemoryDeviceFormFactorTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  SIMM"
  },
  {
    0x04,
    L"  SIP"
  },
  {
    0x05,
    L"  Chip"
  },
  {
    0x06,
    L"  DIP"
  },
  {
    0x07,
    L"  ZIP"
  },
  {
    0x08,
    L"  Proprietary Card"
  },
  {
    0x09,
    L"  DIMM"
  },
  {
    0x0A,
    L"  TSOP"
  },
  {
    0x0B,
    L"  Row of chips"
  },
  {
    0x0C,
    L"  RIMM"
  },
  {
    0x0D,
    L"  SODIMM"
  },
  {
    0x0E,
    L"  SRIMM"
  },
  {
    0x0F,
    L"  FB-DIMM"
  }
};

TABLE_ITEM  MemoryDeviceTypeTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  DRAM"
  },
  {
    0x04,
    L"  EDRAM"
  },
  {
    0x05,
    L"  VRAM"
  },
  {
    0x06,
    L"  SRAM"
  },
  {
    0x07,
    L"  RAM"
  },
  {
    0x08,
    L"  ROM"
  },
  {
    0x09,
    L"  FLASH"
  },
  {
    0x0A,
    L"  EEPROM"
  },
  {
    0x0B,
    L"  FEPROM"
  },
  {
    0x0C,
    L"  EPROM"
  },
  {
    0x0D,
    L"  CDRAM"
  },
  {
    0x0E,
    L"  3DRAM"
  },
  {
    0x0F,
    L"  SDRAM"
  },
  {
    0x10,
    L"  SGRAM"
  },
  {
    0x11,
    L"  RDRAM"
  },
  {
    0x12,
    L"  DDR"
  },
  {
    0x13,
    L"  DDR2"
  },
  {
    0x14,
    L"  DDR2 FB-DIMM"
  }
};

TABLE_ITEM  MemoryDeviceTypeDetailTable[] = {
  {
    1,
    L" Other"
  },
  {
    2,
    L" Unknown"
  },
  {
    3,
    L" Fast-paged"
  },
  {
    4,
    L" Static column"
  },
  {
    5,
    L" Pseudo-STATIC"
  },
  {
    6,
    L" RAMBUS "
  },
  {
    7,
    L" Synchronous"
  },
  {
    8,
    L" CMOS"
  },
  {
    9,
    L" EDO"
  },
  {
    10,
    L" Window DRAM"
  },
  {
    11,
    L" Cache DRAM"
  },
  {
    12,
    L" Non-volatile"
  },
};

TABLE_ITEM  MemoryErrorTypeTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  OK"
  },
  {
    0x04,
    L"  Bad read"
  },
  {
    0x05,
    L"  Parity error"
  },
  {
    0x06,
    L"  Single-bit error"
  },
  {
    0x07,
    L"  Double-bit error"
  },
  {
    0x08,
    L"  Multi-bit error"
  },
  {
    0x09,
    L"  Nibble error"
  },
  {
    0x0A,
    L"  Checksum error"
  },
  {
    0x0B,
    L"  CRC error"
  },
  {
    0x0C,
    L"  Corrected single-bit error"
  },
  {
    0x0D,
    L"  Corrected error"
  },
  {
    0x0E,
    L"  Uncorrectable error"
  },
};

TABLE_ITEM  MemoryErrorGranularityTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  Device level"
  },
  {
    0x04,
    L"  Memory partition level"
  },
};

TABLE_ITEM  MemoryErrorOperationTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  Read"
  },
  {
    0x04,
    L"  Write"
  },
  {
    0x05,
    L"  Partial Write"
  },
};

TABLE_ITEM  PointingDeviceTypeTable[] = {
  {
    0x01,
    L"  Other"
  },
  {
    0x02,
    L"  Unknown"
  },
  {
    0x03,
    L"  Mouse"
  },
  {
    0x04,
    L"  Track Ball"
  },
  {
    0x05,
    L"  Track Point"
  },
  {
    0x06,
    L"  Glide Point"
  },
  {
    0x07,
    L"  Touch Pad"
  },
};

TABLE_ITEM  PointingDeviceInterfaceTable[] = {
  {
    0x01,
    L" Other"
  },
  {
    0x02,
    L" Unknown"
  },
  {
    0x03,
    L" Serial"
  },
  {
    0x04,
    L" PS/2"
  },
  {
    0x05,
    L" Infrared"
  },
  {
    0x06,
    L" HP-HIL"
  },
  {
    0x07,
    L" Bus mouse"
  },
  {
    0x08,
    L" ADB(Apple Desktop Bus"
  },
  {
    0xA0,
    L" Bus mouse DB-9"
  },
  {
    0xA1,
    L" Bus mouse mirco-DIN"
  },
  {
    0xA2,
    L" USB"
  },
};

TABLE_ITEM  PBDeviceChemistryTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" Lead Acid "
  },
  {
    0x04,
    L" Nickel Cadmium "
  },
  {
    0x05,
    L" Nickel metal hydride "
  },
  {
    0x06,
    L" Lithium-ion "
  },
  {
    0x07,
    L" Zinc air "
  },
  {
    0x08,
    L" Lithium Polymer "
  },
};

TABLE_ITEM  VPLocationTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" OK "
  },
  {
    0x04,
    L" Non-critical "
  },
  {
    0x05,
    L" Critical "
  },
  {
    0x06,
    L" Non-recoverable "
  },
};

TABLE_ITEM  VPStatusTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" Processor "
  },
  {
    0x04,
    L" Disk "
  },
  {
    0x05,
    L" Peripheral Bay "
  },
  {
    0x06,
    L" System Management Module "
  },
  {
    0x07,
    L" Motherboard "
  },
  {
    0x08,
    L" Memory Module "
  },
  {
    0x09,
    L" Processor Module "
  },
  {
    0x0A,
    L" Power Unit "
  },
  {
    0x0B,
    L" Add-in Card "
  },
};

TABLE_ITEM  CoolingDeviceStatusTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" OK "
  },
  {
    0x04,
    L" Non-critical "
  },
  {
    0x05,
    L" Critical "
  },
  {
    0x06,
    L" Non-recoverable "
  },
};

TABLE_ITEM  CoolingDeviceTypeTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" Fan "
  },
  {
    0x04,
    L" Centrifugal Blower "
  },
  {
    0x05,
    L" Chip Fan "
  },
  {
    0x06,
    L" Cabinet Fan "
  },
  {
    0x07,
    L" Power Supply Fan "
  },
  {
    0x08,
    L" Heat Pipe "
  },
  {
    0x09,
    L" Integrated Refrigeration "
  },
  {
    0x0A,
    L" Active Cooling "
  },
  {
    0x0B,
    L" Passive Cooling "
  },
};

TABLE_ITEM  TemperatureProbeStatusTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" OK "
  },
  {
    0x04,
    L" Non-critical "
  },
  {
    0x05,
    L" Critical "
  },
  {
    0x06,
    L" Non-recoverable "
  },
};

TABLE_ITEM  TemperatureProbeLocTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" Processor "
  },
  {
    0x04,
    L" Disk "
  },
  {
    0x05,
    L" Peripheral Bay "
  },
  {
    0x06,
    L" System Management Module "
  },
  {
    0x07,
    L" Motherboard "
  },
  {
    0x08,
    L" Memory Module "
  },
  {
    0x09,
    L" Processor Module "
  },
  {
    0x0A,
    L" Power Unit "
  },
  {
    0x0B,
    L" Add-in Card "
  },
};

TABLE_ITEM  ECPStatusTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" OK "
  },
  {
    0x04,
    L" Non-critical "
  },
  {
    0x05,
    L" Critical "
  },
  {
    0x06,
    L" Non-recoverable "
  },
};

TABLE_ITEM  ECPLocTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" Processor "
  },
  {
    0x04,
    L" Disk "
  },
  {
    0x05,
    L" Peripheral Bay "
  },
  {
    0x06,
    L" System Management Module "
  },
  {
    0x07,
    L" Motherboard "
  },
  {
    0x08,
    L" Memory Module "
  },
  {
    0x09,
    L" Processor Module "
  },
  {
    0x0A,
    L" Power Unit "
  },
  {
    0x0B,
    L" Add-in Card "
  },
};

TABLE_ITEM  MDTypeTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" National Semiconductor LM75 "
  },
  {
    0x04,
    L" National Semiconductor LM78 "
  },
  {
    0x05,
    L" National Semiconductor LM79 "
  },
  {
    0x06,
    L" National Semiconductor LM80 "
  },
  {
    0x07,
    L" National Semiconductor LM81 "
  },
  {
    0x08,
    L" Analog Devices ADM9240 "
  },
  {
    0x09,
    L" Dallas Semiconductor DS1780 "
  },
  {
    0x0A,
    L" Maxim 1617 "
  },
  {
    0x0B,
    L" Genesys GL518SM "
  },
  {
    0x0C,
    L" Winbond W83781D "
  },
  {
    0x0D,
    L" Holtek HT82H791 "
  },
};

TABLE_ITEM  MDAddressTypeTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" I/O Port "
  },
  {
    0x04,
    L" Memory "
  },
  {
    0x05,
    L" SM Bus "
  },
};

TABLE_ITEM  MemoryChannelTypeTable[] = {
  {
    0x01,
    L" Other "
  },
  {
    0x02,
    L" Unknown "
  },
  {
    0x03,
    L" RamBus "
  },
  {
    0x04,
    L" SyncLink "
  },
};

TABLE_ITEM  IPMIDIBMCInterfaceTypeTable[] = {
  {
    0x00,
    L" Unknown "
  },
  {
    0x01,
    L" KCS: Keyboard Controller Style "
  },
  {
    0x02,
    L" SMIC: Server Management Interface Chip "
  },
  {
    0x03,
    L" BT: Block Transfer "
  },
  {
    0xFF04,
    L" Reserved for future assignment by this specification "
  },
};

TABLE_ITEM  StructureTypeInfoTable[] = {
  {
    0,
    L" BIOS Information"
  },
  {
    1,
    L" System Information"
  },
  {
    2,
    L" Base Board Information"
  },
  {
    3,
    L" System Enclosure"
  },
  {
    4,
    L" Processor Information"
  },
  {
    5,
    L" Memory Controller Information "
  },
  {
    6,
    L" Memory Module Information "
  },
  {
    7,
    L" Cache Information "
  },
  {
    8,
    L" Port Connector Information "
  },
  {
    9,
    L" System Slots "
  },
  {
    10,
    L" On Board Devices Information  "
  },
  {
    11,
    L" OEM Strings"
  },
  {
    12,
    L" System Configuration Options "
  },
  {
    13,
    L" BIOS Language Information  "
  },
  {
    14,
    L" Group Associations "
  },
  {
    15,
    L" System Event Log "
  },
  {
    16,
    L" Physical Memory Array "
  },
  {
    17,
    L" Memory Device "
  },
  {
    18,
    L" 32-bit Memory Error Information "
  },
  {
    19,
    L" Memory Array Mapped Address "
  },
  {
    20,
    L" Memory Device Mapped Address  "
  },
  {
    21,
    L" Built-in Pointing Device "
  },
  {
    22,
    L" Portable Battery "
  },
  {
    23,
    L" System Reset "
  },
  {
    24,
    L" Hardware Security "
  },
  {
    25,
    L" System Power Controls "
  },
  {
    26,
    L" Voltage Probe "
  },
  {
    27,
    L" Cooling Device "
  },
  {
    28,
    L" Temperature Probe "
  },
  {
    29,
    L" Electrical Current Probe "
  },
  {
    30,
    L" Out-of-Band Remote Access  "
  },
  {
    31,
    L" Boot Integrity Services (BIS) Entry Point"
  },
  {
    32,
    L" System Boot Information "
  },
  {
    33,
    L" 64-bit Memory Error Information "
  },
  {
    34,
    L" Management Device "
  },
  {
    35,
    L" Management Device Component "
  },
  {
    36,
    L" Management Device Threshold Data "
  },
  {
    37,
    L" Memory Channel "
  },
  {
    38,
    L" IPMI Device Information "
  },
  {
    39,
    L" System Power Supply"
  },
  {
    0x7E,
    L" Inactive"
  },
  {
    0x7F,
    L" End-of-Table "
  },
};


UINT8
QueryTable (
  IN  TABLE_ITEM    *Table,
  IN  UINTN         Number,
  IN  UINT8         Key,
  IN  OUT CHAR16    *Info
  )
/*++
Routine Description:
  Function Description
    Given a table and a Key, return the responding info.

  Arguments:
    Table   -  The begin address of table
    Number  -  The number of table items
    Key     -  The query Key
    Info    -  Input as empty buffer; output as data buffer.
  
  Returns:
    if Key found   - return found Key and Info
    if Key unfound - return QUERY_TABLE_UNFOUND, Info=NULL

  Notes:
    Table[Index].Key is change from UINT8 to UINT16, 
    in order to deal with "0xaa - 0xbb". 

    For example:
      DisplaySELVariableDataFormatTypes(UINT8 Type, UINT8 Option)
    has a item: 
      "0x07-0x7F,   Unused"
    Now define Key = 0x7F07, that is to say: High = 0x7F, Low = 0x07.
    Then all the Key Value between Low and High gets the same string 
    L"Unused".

--*/
{
  UINTN Index;
  //
  // High byte and Low byte of word
  //
  UINT8 High;
  UINT8 Low;

  for (Index = 0; Index < Number; Index++) {
    High  = (UINT8) (Table[Index].Key >> 8);
    Low   = (UINT8) (Table[Index].Key & 0x00FF);
    //
    // Check if Key is in the range
    //
    if (High > Low && Key >= Low && Key <= High) {
      StrCpy (Info, Table[Index].Info);
      StrCat (Info, L"\n");
      return Key;
    }
    //
    // Check if Key == Value in the table
    //
    if (Table[Index].Key == Key) {
      StrCpy (Info, Table[Index].Info);
      StrCat (Info, L"\n");
      return Key;
    }
  }

  StrCpy (Info, L"Undefined Value\n");
  return QUERY_TABLE_UNFOUND;
}

VOID
PrintBitsInfo (
  IN  TABLE_ITEM    *Table,
  IN  UINTN         Number,
  IN  UINT32        Bits
  )
/*++

  Routine Description:
      Given a table of bit info and a Key, 
      return the responding info to the Key.

  Arguments:
    Table     - Point to a table which maintains a map of 'bit' to 'message'
    Number    - Number of table items.
    Bits      - The Key of query the bit map information.
  
  Returns:
    None

--*/
{
  //
  // Get certain bit of 'Value':
  //
#define BIT(Value, bit) ((Value) & ((UINT32) 1) << (bit))
  //
  // Clear certain bit of 'Value':
  //
#define CLR_BIT(Value, bit) ((Value) -= (BIT (Value, bit)))

  UINTN   Index;
  UINT32  Value;
  UINT8   NoInfo;

  NoInfo  = TRUE;
  Value   = Bits;
  //
  // query the table and print information
  //
  for (Index = 0; Index < Number; Index++) {
    if (BIT (Value, Table[Index].Key) != 0) {
      Print (Table[Index].Info);
      Print (L" | ");

      NoInfo = FALSE;
      //
      // clear the bit, for reserved bits test
      //
      CLR_BIT (Value, Table[Index].Key);
    }
  }

  if (NoInfo == TRUE) {
    PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_NO_INFO), HiiHandle);
  }

  if (Value != 0) {
    PrintToken (
      STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_RSVD_BITS_SET),
      HiiHandle,
      Value
      );
  }

  Print (L"\n");
}
//
// //////////////////////////////////////////////////////////////////
//
// Following uses QueryTable functions to simplify the coding.
// QueryTable(), PrintBitsInfo()
//
//
#define PRINT_TABLE_ITEM(Table, Key) \
  do { \
    UINTN   Num; \
    CHAR16  Info[64]; \
    Num = sizeof (Table) / sizeof (TABLE_ITEM); \
    ZeroMem (Info, sizeof (Info)); \
    QueryTable (Table, Num, Key, Info); \
    Print (Info); \
  } while (0);

#define PRINT_BITS_INFO(Table, bits) \
  do { \
    UINTN Num; \
    Num = sizeof (Table) / sizeof (TABLE_ITEM); \
    PrintBitsInfo (Table, Num, (UINT32) bits); \
  } while (0);

//
////////////////////////////////////////////////////////////////////
//
// System Information (Type 1)
//
VOID
DisplaySystemWakeupType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_WAKEUP_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (SystemWakeupTypeTable, Type);
}
//
// System Enclosure (Type 3)
//
VOID
DisplaySystemEnclosureType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_CHASSIS_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  //
  // query table and print info
  //
  PRINT_TABLE_ITEM (SystemEnclosureTypeTable, Type);

  if (BIT (Type, 7) != 0) {
    PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_CHASSIS_LOCK_PRESENT), HiiHandle);
  }
}

VOID
DisplaySystemEnclosureStatus (
  UINT8 Status,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_CHASSIS_STATUS), HiiHandle);
  PRINT_INFO_OPTION (Status, Option);
  PRINT_TABLE_ITEM (SystemEnclosureStatusTable, Status);
}

VOID
DisplaySESecurityStatus (
  UINT8 Status,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_CHASSIS_SECURITY), HiiHandle);
  PRINT_INFO_OPTION (Status, Option);
  PRINT_TABLE_ITEM (SESecurityStatusTable, Status);
}
//
// Processor Information (Type 4)
//
VOID
DisplayProcessorType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PROC_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (ProcessorTypeTable, Type);
}

VOID
DisplayProcessorUpgrade (
  UINT8 Upgrade,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PROC_UPDATE), HiiHandle);
  PRINT_INFO_OPTION (Upgrade, Option);
  PRINT_TABLE_ITEM (ProcessorUpgradeTable, Upgrade);
}
//
// Memory Controller Information (Type 5)
//
VOID
DisplayMcErrorDetectMethod (
  UINT8 Method,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_DETECTMETHOD), HiiHandle);
  PRINT_INFO_OPTION (Method, Option);
  PRINT_TABLE_ITEM (McErrorDetectMethodTable, Method);
}

VOID
DisplayMcErrorCorrectCapability (
  UINT8 Capability,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_CORRECT_CAPABILITY), HiiHandle);
  PRINT_INFO_OPTION (Capability, Option);
  PRINT_BITS_INFO (McErrorCorrectCapabilityTable, Capability);
}

VOID
DisplayMcInterleaveSupport (
  UINT8 Support,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_INTERLEAVE_SUPPORT), HiiHandle);
  PRINT_INFO_OPTION (Support, Option);
  PRINT_TABLE_ITEM (McInterleaveSupportTable, Support);
}

VOID
DisplayMcMemorySpeeds (
  UINT16  Speed,
  UINT8   Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_MEMORY_SPEED), HiiHandle);
  PRINT_INFO_OPTION (Speed, Option);
  PRINT_BITS_INFO (McMemorySpeedsTable, Speed);
}

VOID
DisplayMemoryModuleVoltage (
  UINT8 Voltage,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_REQUIRED_VOLTAGES), HiiHandle);
  PRINT_INFO_OPTION (Voltage, Option);
  PRINT_BITS_INFO (MemoryModuleVoltageTable, Voltage);
}
//
// Memory Module Information (Type 6)
//
VOID
DisplayMmMemoryType (
  UINT16  Type,
  UINT8   Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_MODULE_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_BITS_INFO (MmMemoryTypeTable, Type);
}

VOID
DisplayMmErrorStatus (
  UINT8 Status,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_MODULE_ERROR_STATUS), HiiHandle);
  PRINT_INFO_OPTION (Status, Option);
  PRINT_BITS_INFO (MmErrorStatusTable, Status);
}
//
// Cache Information (Type 7)
//
VOID
DisplayCacheSRAMType (
  UINT16  Type,
  UINT8   Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_CACHE_SRAM_TYPE), HiiHandle);
  PRINT_INFO_OPTION ((UINT8) Type, Option);
  PRINT_BITS_INFO (CacheSRAMTypeTable, (UINT8) Type);
}

VOID
DisplayCacheErrCorrectingType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_CACHE_ERROR_CORRECTING), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (CacheErrCorrectingTypeTable, Type);
}

VOID
DisplayCacheSystemCacheType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_CACHE_SYSTEM_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (CacheSystemCacheTypeTable, Type);
}

VOID
DisplayCacheAssociativity (
  UINT8 Associativity,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_CACHE_ASSOCIATIVITY), HiiHandle);
  PRINT_INFO_OPTION (Associativity, Option);
  PRINT_TABLE_ITEM (CacheAssociativityTable, Associativity);
}
//
// Port Connector Information (Type 8)
//
VOID
DisplayPortConnectorType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PORT_CONNECTOR_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (PortConnectorTypeTable, Type);
}

VOID
DisplayPortType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PORT_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (PortTypeTable, Type);
}
//
// System Slots (Type 9)
//
VOID
DisplaySystemSlotType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_SLOT_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (SystemSlotTypeTable, Type);
}

VOID
DisplaySystemSlotDataBusWidth (
  UINT8 Width,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_SLOT_DATA), HiiHandle);
  PRINT_INFO_OPTION (Width, Option);
  PRINT_TABLE_ITEM (SystemSlotDataBusWidthTable, Width);
}

VOID
DisplaySystemSlotCurrentUsage (
  UINT8 Usage,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_SLOT_CURRENT_USAGE), HiiHandle);
  PRINT_INFO_OPTION (Usage, Option);
  PRINT_TABLE_ITEM (SystemSlotCurrentUsageTable, Usage);
}

VOID
DisplaySystemSlotLength (
  UINT8 Length,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_SLOT_LENGTH), HiiHandle);
  PRINT_INFO_OPTION (Length, Option);
  PRINT_TABLE_ITEM (SystemSlotLengthTable, Length);
}

VOID
DisplaySlotCharacteristics1 (
  UINT8 Chara1,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SLOT_CHARACTERISTICS), HiiHandle);
  PRINT_INFO_OPTION (Chara1, Option);
  PRINT_BITS_INFO (SlotCharacteristics1Table, Chara1);
}

VOID
DisplaySlotCharacteristics2 (
  UINT8 Chara2,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SLOT_CHARACTERISTICS_2), HiiHandle);
  PRINT_INFO_OPTION (Chara2, Option);
  PRINT_BITS_INFO (SlotCharacteristics2Table, Chara2);
}
//
// On Board Devices Information (Type 10)
//
VOID
DisplayOnboardDeviceTypes (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_ONBOARD_DEVICE_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (OnboardDeviceTypesTable, Type);
}
//
// System Event Log (Type 15)
//
VOID
DisplaySELTypes (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_EVENT_LOG_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (SELTypesTable, Type);
}

VOID
DisplaySELVarDataFormatType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_EVENT_LOG_VAR_DATA_FORMAT), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (SELVarDataFormatTypeTable, Type);
}

VOID
DisplayPostResultsBitmapDw1 (
  UINT32  Key,
  UINT8   Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_POST_RESULTS_BITMAP), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_BITS_INFO (PostResultsBitmapDw1Table, Key);
}

VOID
DisplayPostResultsBitmapDw2 (
  UINT32  Key,
  UINT8   Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_POST_RESULTS_SECOND_DWORD), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_BITS_INFO (PostResultsBitmapDw2Table, Key);
}

VOID
DisplaySELSysManagementTypes (
  UINT32  SMType,
  UINT8   Option
  )
{
  UINT8       Temp;

  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_MANAGEMENT_TYPES), HiiHandle);
  PRINT_INFO_OPTION (SMType, Option);

  //
  // Deal with wide range Value
  //
  if (SMType >= 0x80000000) {
    PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_OEM_ASSIGNED), HiiHandle);
  } else if (SMType >= 0x00020000) {
    PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_RSVD_FOR_FUTURE_ASSIGN), HiiHandle);
  } else if (SMType >= 0x00010000) {
    PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_SYSTEM_MANAGEMENT_PROBE), HiiHandle);
  } else if (SMType >= 0x31) {
    PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_RSVD_FOR_FUTURE_ASSIGN), HiiHandle);
  } else {
    //
    // Deal with One byte data
    //
    Temp = (UINT8) (SMType & 0x3F);
    PRINT_TABLE_ITEM (SELSysManagementTypesTable, Temp);
  }
}
//
// Physical Memory Array (Type 16)
//
VOID
DisplayPMALocation (
  UINT8 Location,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PHYS_MEM_ARRAY_LOCATION), HiiHandle);
  PRINT_INFO_OPTION (Location, Option);
  PRINT_TABLE_ITEM (PMALocationTable, Location);
}

VOID
DisplayPMAUse (
  UINT8 Use,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PHYS_MEM_ARRAY_LOCATION), HiiHandle);
  PRINT_INFO_OPTION (Use, Option);
  PRINT_TABLE_ITEM (PMAUseTable, Use);
}

VOID
DisplayPMAErrorCorrectionTypes (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PHYS_MEM_ARRAY_ERROR), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (PMAErrorCorrectionTypesTable, Type);
}
//
// Memory Device (Type 17)
//
VOID
DisplayMemoryDeviceFormFactor (
  UINT8 FormFactor,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_DEVICE_FORM_FACTOR), HiiHandle);
  PRINT_INFO_OPTION (FormFactor, Option);
  PRINT_TABLE_ITEM (MemoryDeviceFormFactorTable, FormFactor);
}

VOID
DisplayMemoryDeviceType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_DEVICE_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (MemoryDeviceTypeTable, Type);
}

VOID
DisplayMemoryDeviceTypeDetail (
  UINT16  para,
  UINT8   Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_DEVICE_TYPE_DETAIL), HiiHandle);
  PRINT_INFO_OPTION (para, Option);
  PRINT_BITS_INFO (MemoryDeviceTypeDetailTable, para);
}
//
// 32-bit Memory Error Information (Type 18)
//
VOID
DisplayMemoryErrorType (
  UINT8 ErrorType,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_ERROR_INFO), HiiHandle);
  PRINT_INFO_OPTION (ErrorType, Option);
  PRINT_TABLE_ITEM (MemoryErrorTypeTable, ErrorType);
}

VOID
DisplayMemoryErrorGranularity (
  UINT8 Granularity,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_ERROR_GRANULARITY), HiiHandle);
  PRINT_INFO_OPTION (Granularity, Option);
  PRINT_TABLE_ITEM (MemoryErrorGranularityTable, Granularity);
}

VOID
DisplayMemoryErrorOperation (
  UINT8 Operation,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_ERROR_OP), HiiHandle);
  PRINT_INFO_OPTION (Operation, Option);
  PRINT_TABLE_ITEM (MemoryErrorOperationTable, Operation);
}
//
// Built-in Pointing Device (Type 21)
//
VOID
DisplayPointingDeviceType (
  UINT8 Type,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_POINTING_DEVICE_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (PointingDeviceTypeTable, Type);
}

VOID
DisplayPointingDeviceInterface (
  UINT8   Interface,
  UINT8   Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_POINTING_DEVICE_INTERFACE), HiiHandle);
  PRINT_INFO_OPTION (Interface, Option);
  PRINT_TABLE_ITEM (PointingDeviceInterfaceTable, Interface);
}
//
// Portable Battery (Type 22)
//
VOID
DisplayPBDeviceChemistry (
  UINT8 Key,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_PORTABLE_BATT_DEV_CHEM), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_TABLE_ITEM (PBDeviceChemistryTable, Key);
}
//
// Voltage Probe (Type 26)
//
VOID
DisplayVPLocation (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Loc;

  Loc = (UINT8) ((Key & 0xE0) >> 5);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_VOLTAGE_PROBE_LOC), HiiHandle);
  PRINT_INFO_OPTION (Loc, Option);
  PRINT_TABLE_ITEM (VPLocationTable, Loc);
}

VOID
DisplayVPStatus (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Status;

  Status = (UINT8) (Key & 0x1F);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_VOLTAGE_PROBE_STATUS), HiiHandle);
  PRINT_INFO_OPTION (Status, Option);
  PRINT_TABLE_ITEM (VPStatusTable, Status);
}
//
// Voltage Probe (Type 27)
//
VOID
DisplayCoolingDeviceStatus (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Status;

  Status = (UINT8) ((Key & 0xE0) >> 5);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_COOLING_DEV_STATUS), HiiHandle);
  PRINT_INFO_OPTION (Status, Option);
  PRINT_TABLE_ITEM (CoolingDeviceStatusTable, Status);
}

VOID
DisplayCoolingDeviceType (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Type;

  Type = (UINT8) (Key & 0x1F);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_COOLING_DEV_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Type, Option);
  PRINT_TABLE_ITEM (CoolingDeviceTypeTable, Type);
}
//
// Temperature Probe  (Type 28)
//
VOID
DisplayTemperatureProbeStatus (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Status;

  Status = (UINT8) ((Key & 0xE0) >> 5);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_TEMP_PROBE), HiiHandle);
  PRINT_INFO_OPTION (Status, Option);
  PRINT_TABLE_ITEM (TemperatureProbeStatusTable, Status);
}

VOID
DisplayTemperatureProbeLoc (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Loc;

  Loc = (UINT8) (Key & 0x1F);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_VOLTAGE_PROBE_LOC), HiiHandle);
  PRINT_INFO_OPTION (Loc, Option);
  PRINT_TABLE_ITEM (TemperatureProbeLocTable, Loc);
}
//
// Electrical Current Probe (Type 29)
//
VOID
DisplayECPStatus (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Status;

  Status = (UINT8) ((Key & 0xE0) >> 5);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_ELEC_PROBE_STATUS), HiiHandle);
  PRINT_INFO_OPTION (Status, Option);
  PRINT_TABLE_ITEM (ECPStatusTable, Status);
}

VOID
DisplayECPLoc (
  UINT8 Key,
  UINT8 Option
  )
{
  UINT8       Loc;

  Loc = (UINT8) (Key & 0x1F);
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_ELEC_PROBE_LOC), HiiHandle);
  PRINT_INFO_OPTION (Loc, Option);
  PRINT_TABLE_ITEM (ECPLocTable, Loc);
}
//
// Management Device (Type 34)
//
VOID
DisplayMDType (
  UINT8 Key,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MANAGEMENT_DEV_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_TABLE_ITEM (MDTypeTable, Key);
}

VOID
DisplayMDAddressType (
  UINT8 Key,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MANAGEMENT_DEV_ADDR_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_TABLE_ITEM (MDAddressTypeTable, Key);
}
//
// Memory Channel (Type 37)
//
VOID
DisplayMemoryChannelType (
  UINT8 Key,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_MEM_CHANNEL_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_TABLE_ITEM (MemoryChannelTypeTable, Key);
}
//
// IPMI Device Information (Type 38)
//
VOID
DisplayIPMIDIBMCInterfaceType (
  UINT8 Key,
  UINT8 Option
  )
{
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_BMC_INTERFACE_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_TABLE_ITEM (IPMIDIBMCInterfaceTypeTable, Key);
}

VOID
DisplayStructureTypeInfo (
  UINT8 Key,
  UINT8 Option
  )
{
  //
  // display
  //
  PrintToken (STRING_TOKEN (STR_SMBIOSVIEW_QUERYTABLE_STRUCT_TYPE), HiiHandle);
  PRINT_INFO_OPTION (Key, Option);
  PRINT_TABLE_ITEM (StructureTypeInfoTable, Key);
}
