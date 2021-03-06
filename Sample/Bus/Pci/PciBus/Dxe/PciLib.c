/*++

Copyright (c) 2004 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  PciLib.c 
  
Abstract:

  PCI Bus Driver Lib file
  It abstracts some functions that can be different 
  between light PCI bus driver and full PCI bus driver

Revision History

--*/

#include "PciBus.h"

EFI_PCI_HOTPLUG_REQUEST_PROTOCOL gPciHotPlugRequest = {
  PciHotPlugRequestNotify
};


VOID
InstallHotPlugRequestProtocol (
  IN EFI_STATUS *Status
  )
/*++

Routine Description:

Arguments:
  Status   - A pointer to the status.

Returns:

  None

--*/
{
  EFI_HANDLE  Handle;

  Handle = NULL;
  *Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiPciHotPlugRequestProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gPciHotPlugRequest
                  );
}

VOID
InstallPciHotplugGuid (
  IN  PCI_IO_DEVICE                  *PciIoDevice
  )
/*++

Routine Description:

Arguments:

  PciIoDevice    -  A pointer to the PCI_IO_DEVICE.

Returns:

  None

--*/
{
  EFI_STATUS  Status;

  if (IS_CARDBUS_BRIDGE (&PciIoDevice->Parent->Pci)) {

    Status = gBS->InstallProtocolInterface (
                    &PciIoDevice->Handle,
                    &gEfiPciHotPlugDeviceGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  }
}

VOID
UninstallPciHotplugGuid (
  IN  PCI_IO_DEVICE                  *PciIoDevice
  )
/*++

Routine Description:

Arguments:

  PciIoDevice    - A pointer to the PCI_IO_DEVICE.

Returns:

  None

--*/
{
  EFI_STATUS  Status;

  Status = gBS->OpenProtocol (
                  PciIoDevice->Handle,
                  &gEfiPciHotPlugDeviceGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  if (Status == EFI_SUCCESS) {
    //
    // This may triger CardBus driver to stop for
    // Pccard devices opened the GUID via BY_DRIVER
    //
    Status = gBS->UninstallProtocolInterface (
                    PciIoDevice->Handle,
                    &gEfiPciHotPlugDeviceGuid,
                    NULL
                    );
  }
}

VOID
GetBackPcCardBar (
  IN  PCI_IO_DEVICE                  *PciIoDevice
  )
/*++

Routine Description:


Arguments:

  PciIoDevice      - A pointer to the PCI_IO_DEVICE.

Returns:

  None

--*/
{
  UINT32  Address;

  //
  // Read PciBar information from the bar register
  //
  if (!gFullEnumeration) {

    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                            &(PciIoDevice->PciIo),
                            EfiPciIoWidthUint32,
                            0x1c,
                            1,
                            &Address
                            );

    (PciIoDevice->PciBar)[P2C_MEM_1].BaseAddress  = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_MEM_1].Length       = 0x2000000;
    (PciIoDevice->PciBar)[P2C_MEM_1].BarType      = PciBarTypeMem32;

    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                            &(PciIoDevice->PciIo),
                            EfiPciIoWidthUint32,
                            0x20,
                            1,
                            &Address
                            );
    (PciIoDevice->PciBar)[P2C_MEM_2].BaseAddress  = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_MEM_2].Length       = 0x2000000;
    (PciIoDevice->PciBar)[P2C_MEM_2].BarType      = PciBarTypePMem32;

    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                            &(PciIoDevice->PciIo),
                            EfiPciIoWidthUint32,
                            0x2c,
                            1,
                            &Address
                            );
    (PciIoDevice->PciBar)[P2C_IO_1].BaseAddress = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_IO_1].Length      = 0x100;
    (PciIoDevice->PciBar)[P2C_IO_1].BarType     = PciBarTypeIo16;

    Address = 0;
    PciIoDevice->PciIo.Pci.Read (
                            &(PciIoDevice->PciIo),
                            EfiPciIoWidthUint32,
                            0x34,
                            1,
                            &Address
                            );
    (PciIoDevice->PciBar)[P2C_IO_2].BaseAddress = (UINT64) (Address);
    (PciIoDevice->PciBar)[P2C_IO_2].Length      = 0x100;
    (PciIoDevice->PciBar)[P2C_IO_2].BarType     = PciBarTypeIo16;

  }

  if (gPciHotPlugInit != NULL) {
    GetResourcePaddingForHpb (PciIoDevice);
  }
}

EFI_STATUS
RemoveRejectedPciDevices (
  EFI_HANDLE        RootBridgeHandle,
  IN PCI_IO_DEVICE  *Bridge
  )
/*++

Routine Description:


Arguments:

  RootBridgeHandle   - An efi handle.
  Bridge             - An pointer to the PCI_IO_DEVICE.

Returns:

  None

--*/
// TODO:    EFI_SUCCESS - add return value to function comment
{
  PCI_IO_DEVICE   *Temp;
  EFI_LIST_ENTRY  *CurrentLink;
  EFI_LIST_ENTRY  *LastLink;

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink && CurrentLink != &Bridge->ChildList) {

    Temp = PCI_IO_DEVICE_FROM_LINK (CurrentLink);

    if (IS_PCI_BRIDGE (&Temp->Pci)) {
      //
      // Remove rejected devices recusively
      //
      RemoveRejectedPciDevices (RootBridgeHandle, Temp);
    } else {
      //
      // Skip rejection for all PPBs, while detect rejection for others
      //
      if (IsPciDeviceRejected (Temp)) {
                
        //
        // For P2C, remove all devices on it
        //
                
        if (!IsListEmpty (&Temp->ChildList)) {
          RemoveAllPciDeviceOnBridge (RootBridgeHandle, Temp);
        }
        
        //
        // Finally remove itself
        //
        
        LastLink = CurrentLink->BackLink;
        RemoveEntryList (CurrentLink);
        FreePciDevice (Temp);

        CurrentLink = LastLink;
      }
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciHostBridgeResourceAllocator (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *PciResAlloc
  )
/*++

Routine Description:

  Host brige resource allocator.

Arguments:

  PciResAlloc  - A pointer to the EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL.

Returns:

  EFI Status.

--*/
// TODO:    EFI_NOT_FOUND - add return value to function comment
// TODO:    EFI_NOT_FOUND - add return value to function comment
// TODO:    EFI_NOT_FOUND - add return value to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  PCI_IO_DEVICE                         *RootBridgeDev;
  EFI_HANDLE                            RootBridgeHandle;
  VOID                                  *AcpiConfig;
  EFI_STATUS                            Status;
  UINT64                                IoBase;
  UINT64                                Mem32Base;
  UINT64                                PMem32Base;
  UINT64                                Mem64Base;
  UINT64                                PMem64Base;
  UINT64                                IoResStatus;
  UINT64                                Mem32ResStatus;
  UINT64                                PMem32ResStatus;
  UINT64                                Mem64ResStatus;
  UINT64                                PMem64ResStatus;
  UINT64                                MaxOptionRomSize;
  PCI_RESOURCE_NODE                     *IoBridge;
  PCI_RESOURCE_NODE                     *Mem32Bridge;
  PCI_RESOURCE_NODE                     *PMem32Bridge;
  PCI_RESOURCE_NODE                     *Mem64Bridge;
  PCI_RESOURCE_NODE                     *PMem64Bridge;
  PCI_RESOURCE_NODE                     IoPool;
  PCI_RESOURCE_NODE                     Mem32Pool;
  PCI_RESOURCE_NODE                     PMem32Pool;
  PCI_RESOURCE_NODE                     Mem64Pool;
  PCI_RESOURCE_NODE                     PMem64Pool;
  BOOLEAN                               ReAllocate;
  EFI_DEVICE_HANDLE_EXTENDED_DATA       HandleExtendedData;
  EFI_RESOURCE_ALLOC_FAILURE_ERROR_DATA AllocFailExtendedData;

  //
  // Reallocate flag
  //
  ReAllocate = FALSE;

  //
  // It will try several times if the resource allocation fails
  //
  while (TRUE) {

    //
    // Initialize resource pool
    //    
    InitializeResourcePool (&IoPool, PciBarTypeIo16);
    InitializeResourcePool (&Mem32Pool, PciBarTypeMem32);
    InitializeResourcePool (&PMem32Pool, PciBarTypePMem32);
    InitializeResourcePool (&Mem64Pool, PciBarTypeMem64);
    InitializeResourcePool (&PMem64Pool, PciBarTypePMem64);

    RootBridgeDev     = NULL;
    RootBridgeHandle  = 0;

    while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

      //
      // Get RootBridg Device by handle
      //
      RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);

      if (RootBridgeDev == NULL) {
        return EFI_NOT_FOUND;
      }

      //
      // Create the entire system resource map from the information collected by
      // enumerator. Several resource tree was created
      //

      IoBridge = CreateResourceNode (
                  RootBridgeDev,
                  0,
                  0xFFF,
                  0,
                  PciBarTypeIo16,
                  PciResUsageTypical
                  );

      Mem32Bridge = CreateResourceNode (
                      RootBridgeDev,
                      0,
                      0xFFFFF,
                      0,
                      PciBarTypeMem32,
                      PciResUsageTypical
                      );

      PMem32Bridge = CreateResourceNode (
                      RootBridgeDev,
                      0,
                      0xFFFFF,
                      0,
                      PciBarTypePMem32,
                      PciResUsageTypical
                      );

      Mem64Bridge = CreateResourceNode (
                      RootBridgeDev,
                      0,
                      0xFFFFF,
                      0,
                      PciBarTypeMem64,
                      PciResUsageTypical
                      );

      PMem64Bridge = CreateResourceNode (
                      RootBridgeDev,
                      0,
                      0xFFFFF,
                      0,
                      PciBarTypePMem64,
                      PciResUsageTypical
                      );

      //
      // Create resourcemap by going through all the devices subject to this root bridge
      //
      Status = CreateResourceMap (
                RootBridgeDev,
                IoBridge,
                Mem32Bridge,
                PMem32Bridge,
                Mem64Bridge,
                PMem64Bridge
                );

      //
      // Get the max ROM size that the root bridge can process
      //
      RootBridgeDev->RomSize = Mem32Bridge->Length;

      //
      // Skip to enlarge the resource request during realloction
      //
      if (!ReAllocate) {
        //
        // Get Max Option Rom size for current root bridge
        //
        MaxOptionRomSize = GetMaxOptionRomSize (RootBridgeDev);

        //
        // Enlarger the mem32 resource to accomdate the option rom
        // if the mem32 resource is not enough to hold the rom
        //
        if (MaxOptionRomSize > Mem32Bridge->Length) {

          Mem32Bridge->Length     = MaxOptionRomSize;
          RootBridgeDev->RomSize  = MaxOptionRomSize;

          //
          // Alignment should be adjusted as well
          //
          if (Mem32Bridge->Alignment < MaxOptionRomSize - 1) {
            Mem32Bridge->Alignment = MaxOptionRomSize - 1;
          }
        }
      }    
      
      //
      // Based on the all the resource tree, contruct ACPI resource node to
      // submit the resource aperture to pci host bridge protocol
      //
      Status = ConstructAcpiResourceRequestor (
                RootBridgeDev,
                IoBridge,
                Mem32Bridge,
                PMem32Bridge,
                Mem64Bridge,
                PMem64Bridge,
                &AcpiConfig
                );

      //
      // Insert these resource nodes into the database
      //
      InsertResourceNode (&IoPool, IoBridge);
      InsertResourceNode (&Mem32Pool, Mem32Bridge);
      InsertResourceNode (&PMem32Pool, PMem32Bridge);
      InsertResourceNode (&Mem64Pool, Mem64Bridge);
      InsertResourceNode (&PMem64Pool, PMem64Bridge);

      if (Status == EFI_SUCCESS) {
        //
        // Submit the resource requirement
        //
        Status = PciResAlloc->SubmitResources (
                                PciResAlloc,
                                RootBridgeDev->Handle,
                                AcpiConfig
                                );
      }

      //
      // Free acpi resource node
      //
      if (AcpiConfig != NULL) {
        gBS->FreePool (AcpiConfig);
      }

      if (EFI_ERROR (Status)) {
        //
        // Destroy all the resource tree
        //
        DestroyResourceTree (&IoPool);
        DestroyResourceTree (&Mem32Pool);
        DestroyResourceTree (&PMem32Pool);
        DestroyResourceTree (&Mem64Pool);
        DestroyResourceTree (&PMem64Pool);
        return Status;
      }
    }

    //
    // Notify pci bus driver starts to program the resource
    //
   
    Status = NotifyPhase (PciResAlloc, EfiPciHostBridgeAllocateResources);

    if (!EFI_ERROR (Status)) {
      //
      // Allocation succeed, then continue the following
      //
      break;
    }
      
    //
    // If the resource allocation is unsuccessful, free resources on bridge
    //
            
    RootBridgeDev     = NULL;
    RootBridgeHandle  = 0;

    IoResStatus       = EFI_RESOURCE_SATISFIED;
    Mem32ResStatus    = EFI_RESOURCE_SATISFIED;
    PMem32ResStatus   = EFI_RESOURCE_SATISFIED;
    Mem64ResStatus    = EFI_RESOURCE_SATISFIED;
    PMem64ResStatus   = EFI_RESOURCE_SATISFIED;

    while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {
      //
      // Get RootBridg Device by handle
      //
      RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);
      if (RootBridgeDev == NULL) {
        return EFI_NOT_FOUND;
      }
 
      //
      // Get host bridge handle for status report
      //
      HandleExtendedData.Handle = RootBridgeDev->PciRootBridgeIo->ParentHandle;

      //
      // Get acpi resource node for all the resource types
      //
      AcpiConfig = NULL;

      Status = PciResAlloc->GetProposedResources (
                              PciResAlloc,
                              RootBridgeDev->Handle,
                              &AcpiConfig
                              );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      if (AcpiConfig != NULL) {
        //
        // Adjust resource allocation policy for each RB
        //
        GetResourceAllocationStatus (
          AcpiConfig,
          &IoResStatus,
          &Mem32ResStatus,
          &PMem32ResStatus,
          &Mem64ResStatus,
          &PMem64ResStatus
          );
        gBS->FreePool (AcpiConfig);
      }
    }
    //
    // End while
    //

    //
    // Raise the EFI_IOB_EC_RESOURCE_CONFLICT status code
    //
    AllocFailExtendedData.DataHeader.HeaderSize = sizeof (EFI_STATUS_CODE_DATA);
    AllocFailExtendedData.DataHeader.Size = sizeof (EFI_RESOURCE_ALLOC_FAILURE_ERROR_DATA) -
    sizeof (EFI_STATUS_CODE_DATA);

    EfiCommonLibCopyMem (
      &AllocFailExtendedData.DataHeader.Type,
      &gEfiStatusCodeSpecificDataGuid,
      sizeof (EFI_GUID)
      );

    //
    // It is very difficult to follow the spec here
    // Device path , Bar index can not be get here
    //
    AllocFailExtendedData.DevicePath  = NULL;
    AllocFailExtendedData.Bar         = 0;

    //
    // Need to get the required resource
    //
    AllocFailExtendedData.ReqRes    = NULL;
    AllocFailExtendedData.AllocRes  = NULL;

    EfiLibReportStatusCode (
      EFI_PROGRESS_CODE,
      EFI_IO_BUS_PCI | EFI_IOB_EC_RESOURCE_CONFLICT,
      0,
      &gEfiCallerIdGuid,
      (EFI_STATUS_CODE_DATA *) &AllocFailExtendedData
      );

    Status = PciHostBridgeAdjustAllocation (
              &IoPool,
              &Mem32Pool,
              &PMem32Pool,
              &Mem64Pool,
              &PMem64Pool,
              IoResStatus,
              Mem32ResStatus,
              PMem32ResStatus,
              Mem64ResStatus,
              PMem64ResStatus
              );

    //
    // Destroy all the resource tree
    //
    DestroyResourceTree (&IoPool);
    DestroyResourceTree (&Mem32Pool);
    DestroyResourceTree (&PMem32Pool);
    DestroyResourceTree (&Mem64Pool);
    DestroyResourceTree (&PMem64Pool);

    NotifyPhase (PciResAlloc, EfiPciHostBridgeFreeResources);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    ReAllocate = TRUE;

  }
  //
  // End main while
  //

  //
  // Raise the EFI_IOB_PCI_RES_ALLOC status code
  //
  HandleExtendedData.DataHeader.HeaderSize  = sizeof (EFI_STATUS_CODE_DATA);
  HandleExtendedData.DataHeader.Size        = 
  sizeof (EFI_RESOURCE_ALLOC_FAILURE_ERROR_DATA) - sizeof (EFI_STATUS_CODE_DATA);

  EfiCommonLibCopyMem (
    &HandleExtendedData.DataHeader.Type,
    &gEfiStatusCodeSpecificDataGuid,
    sizeof (EFI_GUID)
    );

  EfiLibReportStatusCode (
    EFI_PROGRESS_CODE,
    EFI_IO_BUS_PCI | EFI_IOB_PCI_PC_RES_ALLOC,
    0,
    &gEfiCallerIdGuid,
    (EFI_STATUS_CODE_DATA *) &HandleExtendedData
    );

  //
  // Notify pci bus driver starts to program the resource
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeSetResources);

  RootBridgeDev     = NULL;

  RootBridgeHandle  = 0;

  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

    //
    // Get RootBridg Device by handle
    //
    RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_NOT_FOUND;
    }
    
    //
    // Get acpi resource node for all the resource types
    //
    AcpiConfig = NULL;
    Status = PciResAlloc->GetProposedResources (
                            PciResAlloc,
                            RootBridgeDev->Handle,
                            &AcpiConfig
                            );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Get the resource base by interpreting acpi resource node
    //
    //
    GetResourceBase (
      AcpiConfig,
      &IoBase,
      &Mem32Base,
      &PMem32Base,
      &Mem64Base,
      &PMem64Base
      );

    //
    // Process option rom for this root bridge
    //
    Status = ProcessOptionRom (RootBridgeDev, Mem32Base, RootBridgeDev->RomSize);

    //
    // Create the entire system resource map from the information collected by
    // enumerator. Several resource tree was created
    //
    Status = GetResourceMap (
              RootBridgeDev,
              &IoBridge,
              &Mem32Bridge,
              &PMem32Bridge,
              &Mem64Bridge,
              &PMem64Bridge,
              &IoPool,
              &Mem32Pool,
              &PMem32Pool,
              &Mem64Pool,
              &PMem64Pool
              );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Program IO resources
    //
    ProgramResource (
      IoBase,
      IoBridge
      );

    //
    // Program Mem32 resources
    //
    ProgramResource (
      Mem32Base,
      Mem32Bridge
      );

    //
    // Program PMem32 resources
    //
    ProgramResource (
      PMem32Base,
      PMem32Bridge
      );

    //
    // Program Mem64 resources
    //
    ProgramResource (
      Mem64Base,
      Mem64Bridge
      );

    //
    // Program PMem64 resources
    //
    ProgramResource (
      PMem64Base,
      PMem64Bridge
      );

    if (AcpiConfig != NULL) {
      gBS->FreePool (AcpiConfig);
    }
  }

  //
  // Destroy all the resource tree
  //
  DestroyResourceTree (&IoPool);
  DestroyResourceTree (&Mem32Pool);
  DestroyResourceTree (&PMem32Pool);
  DestroyResourceTree (&Mem64Pool);
  DestroyResourceTree (&PMem64Pool);

  //
  // Notify the resource allocation phase is to end
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeEndResourceAllocation);

  return EFI_SUCCESS;
}

EFI_STATUS
PciScanBus (
  IN PCI_IO_DEVICE                      *Bridge,
  IN UINT8                              StartBusNumber,
  OUT UINT8                             *SubBusNumber,
  OUT UINT8                             *PaddedBusRange
  )
/*++

Routine Description:

  This routine is used to assign bus number to the given PCI bus system

Arguments:

  Bridge         - A pointer to the PCI_IO_DEVICE structure.
  StartBusNumber - The start bus number.
  SubBusNumber   - A pointer to the sub bus number.
  PaddedBusRange - A pointer to the padded bus range.

Returns:

  None

--*/
// TODO:    EFI_DEVICE_ERROR - add return value to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_STATUS                        Status;
  PCI_TYPE00                        Pci;
  UINT8                             Device;
  UINT8                             Func;
  UINT64                            Address;
  UINTN                             SecondBus;
  UINT16                            Register;
  UINTN                             HpIndex;
  PCI_IO_DEVICE                     *PciDevice;
  EFI_EVENT                         Event;
  EFI_HPC_STATE                     State;
  UINT64                            PciAddress;
  EFI_HPC_PADDING_ATTRIBUTES        Attributes;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  UINT16                            BusRange;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo;
  BOOLEAN                           BusPadding;

  PciRootBridgeIo = Bridge->PciRootBridgeIo;
  SecondBus       = 0;
  Register        = 0;
  State           = 0;
  Attributes      = 0;
  BusRange        = 0;

  ResetAllPpbBusReg (Bridge, StartBusNumber);

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
    for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {

      //
      // Check to see whether a pci device is present
      //
      Status = PciDevicePresent (
                PciRootBridgeIo,
                &Pci,
                StartBusNumber,
                Device,
                Func
                );

      if (EFI_ERROR (Status)) {
        if (Func == 0) {
          //
          // Skip sub functions, this is not a multi function device
          //
          Func = PCI_MAX_FUNC;
        }

        continue;
      }
      
      //
      // Get the PCI device information
      //
      Status = PciSearchDevice (
                Bridge,
                &Pci,
                StartBusNumber,
                Device,
                Func,
                &PciDevice
                );

      ASSERT (!EFI_ERROR (Status));

      PciAddress = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, 0);

      if (!IS_PCI_BRIDGE (&Pci)) {
        //
        // PCI bridges will be called later
        // Here just need for PCI device or PCI to cardbus controller
        // EfiPciBeforeChildBusEnumeration for PCI Device Node
        //
        PreprocessController (
            PciDevice,
            PciDevice->BusNumber,
            PciDevice->DeviceNumber,
            PciDevice->FunctionNumber,
            EfiPciBeforeChildBusEnumeration
            );
      }
      
      //
      // For Pci Hotplug controller devcie only
      //
      if (gPciHotPlugInit != NULL) {
        //
        // Check if it is a Hotplug PCI controller
        //
        if (IsRootPciHotPlugController (PciDevice->DevicePath, &HpIndex)) {

          if (!gPciRootHpcData[HpIndex].Initialized) {

            Status = CreateEventForHpc (HpIndex, &Event);

            ASSERT (!EFI_ERROR (Status));

            Status = gPciHotPlugInit->InitializeRootHpc (
                                        gPciHotPlugInit,
                                        gPciRootHpcPool[HpIndex].HpcDevicePath,
                                        PciAddress,
                                        Event,
                                        &State
                                        );
                                        
            PreprocessController (
              PciDevice,
              PciDevice->BusNumber,
              PciDevice->DeviceNumber,
              PciDevice->FunctionNumber,
              EfiPciBeforeChildBusEnumeration
            );                                        
            continue;
          }
        }
      }

      if (IS_PCI_BRIDGE (&Pci) || IS_CARDBUS_BRIDGE (&Pci)) {
        //
        // For PPB
        // Get the bridge information
        //
        BusPadding = FALSE;
        if (gPciHotPlugInit != NULL) {

          if (IsRootPciHotPlugBus (PciDevice->DevicePath, &HpIndex)) {
          
            //
            // If it is initialized, get the padded bus range
            //
            Status = gPciHotPlugInit->GetResourcePadding (
                                        gPciHotPlugInit,
                                        gPciRootHpcPool[HpIndex].HpbDevicePath,
                                        PciAddress,
                                        &State,
                                        (VOID **) &Descriptors,
                                        &Attributes
                                        );

            if (EFI_ERROR (Status)) {
              return Status;
            }

            BusRange = 0;
            Status = PciGetBusRange (
                      &Descriptors,
                      NULL,
                      NULL,
                      &BusRange
                      );

            gBS->FreePool (Descriptors);

            if (EFI_ERROR (Status)) {
              return Status;
            }

            BusPadding = TRUE;
          }
        }

        (*SubBusNumber)++;
        SecondBus = *SubBusNumber;

        Register  = (UINT16) ((SecondBus << 8) | (UINT16) StartBusNumber);
        Address   = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, 0x18);

        Status = PciRootBridgeIo->Pci.Write (
                                        PciRootBridgeIo,
                                        EfiPciWidthUint16,
                                        Address,
                                        1,
                                        &Register
                                        );


        //
        // If it is PPB, resursively search down this bridge
        //
        if (IS_PCI_BRIDGE (&Pci)) {
          
          //
          // Initialize SubBusNumber to Maximum bus number
          //
          Register  = 0xFF;
          Address   = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, 0x1A);
          Status = PciRootBridgeIo->Pci.Write (
                                          PciRootBridgeIo,
                                          EfiPciWidthUint8,
                                          Address,
                                          1,
                                          &Register
                                          );

          //
          // Nofify EfiPciBeforeChildBusEnumeration for PCI Brige
          //
          PreprocessController (
            PciDevice,
            PciDevice->BusNumber,
            PciDevice->DeviceNumber,
            PciDevice->FunctionNumber,
            EfiPciBeforeChildBusEnumeration
            );

          Status = PciScanBus (
                    PciDevice,
                    (UINT8) (SecondBus),
                    SubBusNumber,
                    PaddedBusRange
                    );

          if (EFI_ERROR (Status)) {
            return EFI_DEVICE_ERROR;
          }
        }

        if (BusPadding) {
          //
          // Ensure the device is enabled and initialized
          //
          if ((Attributes == EfiPaddingPciRootBridge) &&
              (State & EFI_HPC_STATE_ENABLED)         &&
              (State & EFI_HPC_STATE_INITIALIZED)     ) {
            *PaddedBusRange = (UINT8) ((UINT8) (BusRange) +*PaddedBusRange);
          } else {
            *SubBusNumber = (UINT8) ((UINT8) (BusRange) +*SubBusNumber);
          }
        }

        //
        // Set the current maximum bus number under the PPB
        //
        Address = EFI_PCI_ADDRESS (StartBusNumber, Device, Func, 0x1A);

        Status = PciRootBridgeIo->Pci.Write (
                                        PciRootBridgeIo,
                                        EfiPciWidthUint8,
                                        Address,
                                        1,
                                        SubBusNumber
                                        );
      }

      if (Func == 0 && !IS_PCI_MULTI_FUNC (&Pci)) {

        //
        // Skip sub functions, this is not a multi function device
        //
        Func = PCI_MAX_FUNC;
      }

    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciRootBridgeP2CProcess (
  IN PCI_IO_DEVICE *Bridge
  )
/*++

Routine Description:
  
    Process Option Rom on this host bridge

Arguments:

Returns:

  None

--*/
// TODO:    Bridge - add argument and description to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_LIST_ENTRY  *CurrentLink;
  PCI_IO_DEVICE   *Temp;
  EFI_HPC_STATE   State;
  UINT64          PciAddress;
  EFI_STATUS      Status;

  CurrentLink = Bridge->ChildList.ForwardLink;

  while (CurrentLink && CurrentLink != &Bridge->ChildList) {

    Temp = PCI_IO_DEVICE_FROM_LINK (CurrentLink);

    if (IS_CARDBUS_BRIDGE (&Temp->Pci)) {

      if (gPciHotPlugInit && Temp->Allocated) {
        
        //
        // Raise the EFI_IOB_PCI_HPC_INIT status code
        //
        ReportStatusCodeWithDevicePath (
          EFI_PROGRESS_CODE,
          EFI_IO_BUS_PCI | EFI_IOB_PCI_PC_HPC_INIT,
          0,
          &gEfiCallerIdGuid,
          Temp->DevicePath
          );

        PciAddress = EFI_PCI_ADDRESS (Temp->BusNumber, Temp->DeviceNumber, Temp->FunctionNumber, 0);
        Status = gPciHotPlugInit->InitializeRootHpc (
                                    gPciHotPlugInit,
                                    Temp->DevicePath,
                                    PciAddress,
                                    NULL,
                                    &State
                                    );

        if (!EFI_ERROR (Status)) {
          Status = PciBridgeEnumerator (Temp);

          if (EFI_ERROR (Status)) {
            return Status;
          }
        }

        CurrentLink = CurrentLink->ForwardLink;
        continue;

      }
    }

    if (!IsListEmpty (&Temp->ChildList)) {
      Status = PciRootBridgeP2CProcess (Temp);
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciHostBridgeP2CProcess (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *PciResAlloc
  )
/*++

Routine Description:
  
Arguments:

Returns:

  None

--*/
// TODO:    PciResAlloc - add argument and description to function comment
// TODO:    EFI_NOT_FOUND - add return value to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_HANDLE    RootBridgeHandle;
  PCI_IO_DEVICE *RootBridgeDev;
  EFI_STATUS    Status;

  RootBridgeHandle = NULL;

  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

    //
    // Get RootBridg Device by handle
    //
    RootBridgeDev = GetRootBridgeByHandle (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_NOT_FOUND;
    }

    Status = PciRootBridgeP2CProcess (RootBridgeDev);

    if (EFI_ERROR (Status)) {
      return Status;
    }

  }

  return EFI_SUCCESS;
}

EFI_STATUS
PciHostBridgeEnumerator (
  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL  *PciResAlloc
  )
/*++

Routine Description:

  This function is used to enumerate the entire host bridge 
  in a given platform

Arguments:

  PciResAlloc   - A pointer to the resource allocate protocol.

Returns:

  None

--*/
// TODO:    EFI_OUT_OF_RESOURCES - add return value to function comment
// TODO:    EFI_OUT_OF_RESOURCES - add return value to function comment
// TODO:    EFI_OUT_OF_RESOURCES - add return value to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_HANDLE                        RootBridgeHandle;
  PCI_IO_DEVICE                     *RootBridgeDev;
  EFI_STATUS                        Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo;
  UINT16                            MinBus;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;

  InitializeHotPlugSupport ();

  //
  // Notify the bus allocation phase is about to start
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeBeginBusAllocation);

  RootBridgeHandle = NULL;
  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

    //
    // if a root bridge instance is found, create root bridge device for it
    //

    RootBridgeDev = CreateRootBridge (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Enumerate all the buses under this root bridge
    //

    Status = PciRootBridgeEnumerator (
              PciResAlloc,
              RootBridgeDev
              );

    DestroyRootBridge (RootBridgeDev);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //                                                            
  // Notify the bus allocation phase is finished for the first time
  //                                                            
  NotifyPhase (PciResAlloc, EfiPciHostBridgeEndBusAllocation);
    
                  
  if (gPciHotPlugInit != NULL) {
    //
    // Wait for all HPC initialized
    //
    Status = AllRootHPCInitialized (STALL_1_SECOND * 15);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Notify the bus allocation phase is about to start for the 2nd time
    //
    NotifyPhase (PciResAlloc, EfiPciHostBridgeBeginBusAllocation);
  
    RootBridgeHandle = NULL;
    while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

      //
      // if a root bridge instance is found, create root bridge device for it
      //

      RootBridgeDev = CreateRootBridge (RootBridgeHandle);

      if (RootBridgeDev == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // Enumerate all the buses under this root bridge
      //

      Status = PciRootBridgeEnumerator (
                PciResAlloc,
                RootBridgeDev
                );

      DestroyRootBridge (RootBridgeDev);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
      
    //
    // Notify the bus allocation phase is to end
    //
    NotifyPhase (PciResAlloc, EfiPciHostBridgeEndBusAllocation);
  }

  //
  // Notify the resource allocation phase is to start
  //
  NotifyPhase (PciResAlloc, EfiPciHostBridgeBeginResourceAllocation);

  RootBridgeHandle = NULL;
  while (PciResAlloc->GetNextRootBridge (PciResAlloc, &RootBridgeHandle) == EFI_SUCCESS) {

    //
    // if a root bridge instance is found, create root bridge device for it
    //

    RootBridgeDev = CreateRootBridge (RootBridgeHandle);

    if (RootBridgeDev == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = StartManagingRootBridge (RootBridgeDev);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    PciRootBridgeIo = RootBridgeDev->PciRootBridgeIo;
    Status          = PciRootBridgeIo->Configuration (PciRootBridgeIo, (VOID **) &Descriptors);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = PciGetBusRange (&Descriptors, &MinBus, NULL, NULL);

    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Determine root bridge attribute by calling interface of Pcihostbridge
    // protocol
    //
    DetermineRootBridgeAttributes (
      PciResAlloc,
      RootBridgeDev
      );

    //
    // Collect all the resource information under this root bridge
    // A database that records all the information about pci device subject to this
    // root bridge will then be created
    //
    Status = PciPciDeviceInfoCollector (
              RootBridgeDev,
              (UINT8) MinBus
              );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    InsertRootBridge (RootBridgeDev);

    //
    // Record the hostbridge handle
    //
    AddHostBridgeEnumerator (RootBridgeDev->PciRootBridgeIo->ParentHandle);
  }

  return EFI_SUCCESS;
}
