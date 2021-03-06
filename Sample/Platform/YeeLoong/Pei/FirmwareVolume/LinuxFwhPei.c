/*++

Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:
  LinuxFwhPei.c
    
Abstract:
  PEIM to abstract construction of firmware volume in a Windows NT environment.

Revision History

--*/

#include "Tiano.h"
#include "FlashLayout.h"
#include "Pei.h"
#include "PeiLib.h"

#include EFI_PPI_DEFINITION (LinuxFwh)

EFI_STATUS
EFIAPI
PeimInitializeLinuxFwhPei (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  );

EFI_PEIM_ENTRY_POINT (PeimInitializeLinuxFwhPei)

EFI_STATUS
EFIAPI
PeimInitializeLinuxFwhPei (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:
  Perform a call-back into the SEC simulator to get address of the Firmware Hub

Arguments:
  FfsHeader   - Ffs Header availible to every PEIM
  PeiServices - General purpose services available to every PEIM.
    
Returns:
  None

--*/
{
  EFI_STATUS                  Status;
  EFI_PEI_PPI_DESCRIPTOR      *PpiDescriptor;
  LINUX_FWH_PPI               *FwhPpi;
  EFI_PHYSICAL_ADDRESS        FdBase;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;
  UINT64                      FdSize;
  UINTN                       Index;

  //
  // Get the Fwh Information PPI
  //
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gLinuxFwhPpiGuid, // GUID
                            0,              // INSTANCE
                            &PpiDescriptor, // EFI_PEI_PPI_DESCRIPTOR
                            &FwhPpi         // PPI
                            );
  ASSERT_PEI_ERROR (PeiServices, Status);

  Index = 0;
  do {
    //
    // Get information about all the FD's in the system
    //
    Status = FwhPpi->LinuxFwh (Index, &FdBase, &FdSize);
    if (!EFI_ERROR (Status)) {
      //
      // Assume the FD starts with an FV header
      //
      FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FdBase;

      //
      // Make an FV Hob for the first FV in the FD
      //
      Status = PeiBuildHobFv (PeiServices, FdBase, FvHeader->FvLength);
      ASSERT_PEI_ERROR (PeiServices, Status);

      if (Index == 0) {
        //
        // Assume the first FD was produced by the NT32.DSC
        //  All these strange offests are needed to keep in
        //  sync with the FlashMap and NT32.dsc file
        //
        Status = PeiBuildHobResourceDescriptor (
                  PeiServices,
                  EFI_RESOURCE_FIRMWARE_DEVICE,
                  (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
                  FdBase,
                  (FvHeader->FvLength + EFI_LINUX_RUNTIME_UPDATABLE_LENGTH + EFI_LINUX_FTW_SPARE_BLOCK_LENGTH)
                  );
        ASSERT_PEI_ERROR (PeiServices, Status);

        //
        // Hard code the address of the spare block and variable services.
        //  Assume it's a hard coded offset from FV0 in FD0.
        //
        FdBase  = FdBase + EFI_LINUX_RUNTIME_UPDATABLE_OFFSET;
        FdSize  = EFI_LINUX_RUNTIME_UPDATABLE_LENGTH + EFI_LINUX_FTW_SPARE_BLOCK_LENGTH;

        Status  = PeiBuildHobFv (PeiServices, FdBase, FdSize);
        ASSERT_PEI_ERROR (PeiServices, Status);
      } else {
        //
        // For other FD's just map them in.
        //
        Status = PeiBuildHobResourceDescriptor (
                  PeiServices,
                  EFI_RESOURCE_FIRMWARE_DEVICE,
                  (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
                  FdBase,
                  FdSize
                  );
        ASSERT_PEI_ERROR (PeiServices, Status);
      }
    }

    Index++;
  } while (!EFI_ERROR (Status));

  return Status;
}
