/*++

Copyright (c) 2005 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  PciOptionRomSupport.c
  
Abstract:

  PCI Bus Driver

Revision History

--*/

#include "PciBus.h"


EFI_STATUS
RomDecode (
  IN PCI_IO_DEVICE   *PciDevice,
  IN UINT8           RomBarIndex,
  IN UINT32          RomBar,
  IN BOOLEAN         Enable
);

EFI_STATUS
GetOpRomInfo (
  IN PCI_IO_DEVICE    *PciIoDevice
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{
  UINT8                           RomBarIndex;
  UINT32                          AllOnes;
  UINT64                          Address;
  EFI_STATUS                      Status;
  UINT8                           Bus;
  UINT8                           Device;
  UINT8                           Function;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;

  Bus             = PciIoDevice->BusNumber;
  Device          = PciIoDevice->DeviceNumber;
  Function        = PciIoDevice->FunctionNumber;

  PciRootBridgeIo = PciIoDevice->PciRootBridgeIo;

  //
  // offset is 0x30 if is not ppb
  //

  //
  // 0x30
  //
  RomBarIndex = PCI_DEVICE_ROMBAR;

  if (IS_PCI_BRIDGE (&PciIoDevice->Pci)) {
    //
    // if is ppb
    //

    //
    // 0x38
    //
    RomBarIndex = PCI_BRIDGE_ROMBAR;
  }
  //
  // the bit0 is 0 to prevent the enabling of the Rom address decoder
  //
  AllOnes = 0xfffffffe;
  Address = EFI_PCI_ADDRESS (Bus, Device, Function, RomBarIndex);

  Status = PciRootBridgeIo->Pci.Write (
                                  PciRootBridgeIo,
                                  EfiPciWidthUint32,
                                  Address,
                                  1,
                                  &AllOnes
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  //
  // read back
  //
  Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,
                                  EfiPciWidthUint32,
                                  Address,
                                  1,
                                  &AllOnes
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Bits [1, 10] are reserved
  //
  AllOnes &= 0xFFFFF800;
  if ((AllOnes == 0) || (AllOnes == 0xFFFFF800)) {
    return EFI_NOT_FOUND;
  }
  
  DEBUG ((EFI_D_ERROR, "PCIBUS: GetOpRomInfo: OPROM detected!\n"));
  DEBUG ((EFI_D_ERROR, "PCIBUS: GetOpRomInfo: B-%x, D-%x, F-%x\n", (UINTN)Bus, (UINTN)Device, (UINTN)Function));

  PciIoDevice->RomSize = (UINT64) ((~AllOnes) + 1);
  return EFI_SUCCESS;
}

EFI_STATUS
LoadOpRomImage (
  IN PCI_IO_DEVICE   *PciDevice,
  IN UINT64          ReservedMemoryBase
  )
/*++

Routine Description:
 
    Load option rom image for specified PCI device

Arguments:

Returns:

--*/
{
  UINT8                     RomBarIndex;
  UINT8                     Indicator;
  UINT16                    OffsetPcir;
  UINT32                    RomBarOffset;
  UINT32                    RomBar;
  UINT64                    Temp;
  EFI_STATUS                retStatus;
  BOOLEAN                   FirstCheck;
  UINT8                     *Image;
  PCI_EXPANSION_ROM_HEADER  *RomHeader;
  PCI_DATA_STRUCTURE        *RomPcir;
  UINT64                    RomSize;
  UINT64                    RomImageSize;
  UINT8                     *RomInMemory;
  UINT8                     CodeType;

  RomSize       = PciDevice->RomSize;

  Indicator     = 0;
  RomImageSize  = 0;
  RomInMemory   = NULL;
  Temp          = 0;
  CodeType      = 0xFF;

  //
  // Get the RomBarIndex
  //

  //
  // 0x30
  //
  RomBarIndex = PCI_DEVICE_ROMBAR;
  if (IS_PCI_BRIDGE (&(PciDevice->Pci))) {
    //
    // if is ppb
    //

    //
    // 0x38
    //
    RomBarIndex = PCI_BRIDGE_ROMBAR;
  }
  //
  // Allocate memory for Rom header and PCIR
  //
  RomHeader = EfiLibAllocatePool (sizeof (PCI_EXPANSION_ROM_HEADER));
  if (RomHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  RomPcir = EfiLibAllocatePool (sizeof (PCI_DATA_STRUCTURE));
  if (RomPcir == NULL) {
    gBS->FreePool (RomHeader);
    return EFI_OUT_OF_RESOURCES;
  }

  RomBar = (UINT32)ReservedMemoryBase;  

  //
  // Enable RomBar
  //
  RomDecode (PciDevice, RomBarIndex, RomBar, TRUE);

  RomBarOffset  = RomBar;
  retStatus     = EFI_NOT_FOUND;
  FirstCheck    = TRUE;

  do {
    PciDevice->PciRootBridgeIo->Mem.Read (
                                      PciDevice->PciRootBridgeIo,
                                      EfiPciWidthUint8,
                                      RomBarOffset,
                                      sizeof (PCI_EXPANSION_ROM_HEADER),
                                      (UINT8 *) RomHeader
                                      );

    if (RomHeader->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
      RomBarOffset = RomBarOffset + 512;
      if (FirstCheck) {
        break;
      } else {
        RomImageSize = RomImageSize + 512;
        continue;
      }
    }

    FirstCheck  = FALSE;
    OffsetPcir  = RomHeader->PcirOffset;
    PciDevice->PciRootBridgeIo->Mem.Read (
                                      PciDevice->PciRootBridgeIo,
                                      EfiPciWidthUint8,
                                      RomBarOffset + OffsetPcir,
                                      sizeof (PCI_DATA_STRUCTURE),
                                      (UINT8 *) RomPcir
                                      );
    if (RomPcir->CodeType == PCI_CODE_TYPE_PCAT_IMAGE) {
      CodeType = PCI_CODE_TYPE_PCAT_IMAGE;
    }
    Indicator     = RomPcir->Indicator;
    RomImageSize  = RomImageSize + RomPcir->ImageLength * 512;
    RomBarOffset  = RomBarOffset + RomPcir->ImageLength * 512;
  } while (((Indicator & 0x80) == 0x00) && ((RomBarOffset - RomBar) < RomSize));

  //
  // Some Legacy Cards do not report the correct ImageLength so used the maximum
  // of the legacy length and the PCIR Image Length
  //
  if (CodeType == PCI_CODE_TYPE_PCAT_IMAGE) {
    RomImageSize = EFI_MAX(RomImageSize, (((EFI_LEGACY_EXPANSION_ROM_HEADER *)RomHeader)->Size512 * 512));
  }

  if (RomImageSize > 0) {
    retStatus = EFI_SUCCESS;
    Image     = EfiLibAllocatePool ((UINT32) RomImageSize);
    if (Image == NULL) {
      RomDecode (PciDevice, RomBarIndex, RomBar, FALSE);
      gBS->FreePool (RomHeader);
      gBS->FreePool (RomPcir);
      return EFI_OUT_OF_RESOURCES;
    }
    
    //
    // Copy Rom image into memory
    //
    PciDevice->PciRootBridgeIo->Mem.Read (
                                      PciDevice->PciRootBridgeIo,
                                      EfiPciWidthUint8,
                                      RomBar,
                                      (UINT32) RomImageSize,
                                      Image
                                      );
    RomInMemory = Image;
  }

  RomDecode (PciDevice, RomBarIndex, RomBar, FALSE);

  PciDevice->PciIo.RomSize  = RomImageSize;
  PciDevice->PciIo.RomImage = RomInMemory;

  //
  // Free allocated memory
  //
  gBS->FreePool (RomHeader);
  gBS->FreePool (RomPcir);

  return retStatus;
}

EFI_STATUS
RomDecode (
  IN PCI_IO_DEVICE   *PciDevice,
  IN UINT8           RomBarIndex,
  IN UINT32          RomBar,
  IN BOOLEAN         Enable
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
{
  UINT16                          CommandValue;
  UINT32                          Value32;
  UINT64                          Address;
  //EFI_STATUS                      Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  
  PciRootBridgeIo = PciDevice->PciRootBridgeIo;
  if (Enable) {
    Address = EFI_PCI_ADDRESS (PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber, RomBarIndex);
    //
    // set the Rom base address: now is hardcode
    //
    PciRootBridgeIo->Pci.Write(
                               PciRootBridgeIo, 
                               EfiPciWidthUint32, 
                               Address, 
                               1, 
                               &RomBar);
  
    //
    // enable its decoder
    //
    Value32 = RomBar | 0x1;
    PciRootBridgeIo->Pci.Write(
                               PciRootBridgeIo, 
                               EfiPciWidthUint32, 
                               Address, 
                               1, 
                               &Value32);
    
    //
    //setting the memory space bit in the function's command register
    //
    Address = EFI_PCI_ADDRESS (PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber, 0x04);
    PciRootBridgeIo->Pci.Read(
                              PciRootBridgeIo, 
                              EfiPciWidthUint16, 
                              Address, 
                              1, 
                              &CommandValue);
    
    CommandValue = (UINT16)(CommandValue | 0x0002); //0x0003
    PciRootBridgeIo->Pci.Write(
                               PciRootBridgeIo, 
                               EfiPciWidthUint16, 
                               Address, 
                               1, 
                               &CommandValue);
  } else {
    //
    // disable rom decode
    //  
    Address = EFI_PCI_ADDRESS (PciDevice->BusNumber, PciDevice->DeviceNumber, PciDevice->FunctionNumber, RomBarIndex);
    Value32 = 0xfffffffe;
    PciRootBridgeIo->Pci.Write(
                               PciRootBridgeIo, 
                               EfiPciWidthUint32, 
                               Address, 
                               1, 
                               &Value32);
  }

  return EFI_SUCCESS;

}

EFI_STATUS
ProcessOpRomImage (
  PCI_IO_DEVICE   *PciDevice
  )
/*++

Routine Description:

  Process the oprom image.
  
Arguments:
  PciDevice       A pointer to a pci device.

Returns:

  EFI Status.
  
--*/
{
  UINT8                         Indicator;
  UINT32                        ImageSize;
  UINT16                        ImageOffset;
  VOID                          *RomBar;
  UINT8                         *RomBarOffset;
  EFI_HANDLE                    ImageHandle;
  EFI_STATUS                    Status;
  EFI_STATUS                    retStatus;
  BOOLEAN                       FirstCheck;
  BOOLEAN                       SkipImage;
  UINT32                        DestinationSize;
  UINT32                        ScratchSize;
  UINT8                         *Scratch;
  VOID                          *ImageBuffer;
  VOID                          *DecompressedImageBuffer;
  UINT32                        ImageLength;
  EFI_DECOMPRESS_PROTOCOL       *Decompress;
  EFI_PCI_EXPANSION_ROM_HEADER  *EfiRomHeader;
  PCI_DATA_STRUCTURE            *Pcir;

  Indicator = 0;

  //
  // Get the Address of the Rom image
  //
  RomBar        = PciDevice->PciIo.RomImage;
  RomBarOffset  = (UINT8 *) RomBar;
  retStatus     = EFI_NOT_FOUND;
  FirstCheck    = TRUE;

  do {
    EfiRomHeader = (EFI_PCI_EXPANSION_ROM_HEADER *) RomBarOffset;
    if (EfiRomHeader->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
      RomBarOffset = RomBarOffset + 512;
      if (FirstCheck) {
        break;
      } else {
        continue;
      }
    }

    FirstCheck  = FALSE;
    Pcir        = (PCI_DATA_STRUCTURE *) (RomBarOffset + EfiRomHeader->PcirOffset);
    ImageSize   = (UINT32) (Pcir->ImageLength * 512);
    Indicator   = Pcir->Indicator;

    if ((Pcir->CodeType == PCI_CODE_TYPE_EFI_IMAGE) && 
        (EfiRomHeader->EfiSignature == EFI_PCI_EXPANSION_ROM_HEADER_EFISIGNATURE)) {

      if ((EfiRomHeader->EfiSubsystem == EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER)  ||
          (EfiRomHeader->EfiSubsystem == EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER)) {

        ImageOffset             = EfiRomHeader->EfiImageHeaderOffset;
        ImageSize               = (UINT32) (EfiRomHeader->InitializationSize * 512);

        ImageBuffer             = (VOID *) (RomBarOffset + ImageOffset);
        ImageLength             = ImageSize - (UINT32)ImageOffset;
        DecompressedImageBuffer = NULL;

        //
        // decompress here if needed
        //
        SkipImage = FALSE;
        if (EfiRomHeader->CompressionType > EFI_PCI_EXPANSION_ROM_HEADER_COMPRESSED) {
          SkipImage = TRUE;
        }

        if (EfiRomHeader->CompressionType == EFI_PCI_EXPANSION_ROM_HEADER_COMPRESSED) {
          Status = gBS->LocateProtocol (&gEfiDecompressProtocolGuid, NULL, (VOID **) &Decompress);
          if (EFI_ERROR (Status)) {
            SkipImage = TRUE;
          } else {
            SkipImage = TRUE;
            Status = Decompress->GetInfo (
                                  Decompress,
                                  ImageBuffer,
                                  ImageLength,
                                  &DestinationSize,
                                  &ScratchSize
                                  );
            if (!EFI_ERROR (Status)) {
              DecompressedImageBuffer = NULL;
              DecompressedImageBuffer = EfiLibAllocatePool (DestinationSize);
              if (DecompressedImageBuffer != NULL) {
                Scratch = EfiLibAllocatePool (ScratchSize);
                if (Scratch != NULL) {
                  Status = Decompress->Decompress (
                                        Decompress,
                                        ImageBuffer,
                                        ImageLength,
                                        DecompressedImageBuffer,
                                        DestinationSize,
                                        Scratch,
                                        ScratchSize
                                        );
                  if (!EFI_ERROR (Status)) {
                    ImageBuffer = DecompressedImageBuffer;
                    ImageLength = DestinationSize;
                    SkipImage   = FALSE;
                  }

                  gBS->FreePool (Scratch);
                }
              }
            }
          }
        }

        if (!SkipImage) {
          //
          // load image and start image
          //
          Status = gBS->LoadImage (
                          FALSE,
                          gPciBusDriverBinding.DriverBindingHandle,
                          NULL,
                          ImageBuffer,
                          ImageLength,
                          &ImageHandle
                          );
          if (!EFI_ERROR (Status)) {
            Status = gBS->StartImage (ImageHandle, NULL, NULL);
            if (!EFI_ERROR (Status)) {
              AddDriver (PciDevice, ImageHandle);
              retStatus = EFI_SUCCESS;
            }
          }
        }

        RomBarOffset = RomBarOffset + ImageSize;
      } else {
        RomBarOffset = RomBarOffset + ImageSize;
      }
    } else {
      RomBarOffset = RomBarOffset + ImageSize;
    }

  } while (((Indicator & 0x80) == 0x00) && ((UINTN) (RomBarOffset - (UINT8 *) RomBar) < PciDevice->RomSize));

  return retStatus;

}
