/*++

Copyright (c) 2004 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

    ConPlatform.c
    
Abstract:

--*/

#include "ConPlatform.h"

EFI_DRIVER_BINDING_PROTOCOL gConPlatformTextInDriverBinding = {
  ConPlatformTextInDriverBindingSupported,
  ConPlatformTextInDriverBindingStart,
  ConPlatformDriverBindingStop,
  0xa,
  NULL,
  NULL
};

EFI_DRIVER_BINDING_PROTOCOL gConPlatformTextOutDriverBinding = {
  ConPlatformTextOutDriverBindingSupported,
  ConPlatformTextOutDriverBindingStart,
  ConPlatformDriverBindingStop,
  0xa,
  NULL,
  NULL
};

EFI_DRIVER_ENTRY_POINT (ConPlatformDriverEntry)

EFI_STATUS
EFIAPI
ConPlatformDriverEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

Arguments:
  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

Returns: 
  EFI_STATUS

--*/
{
  EFI_STATUS  Status;

  //
  // Install driver binding protocols and component name protocols
  //
  Status = INSTALL_ALL_DRIVER_PROTOCOLS (
            ImageHandle,
            SystemTable,
            &gConPlatformTextOutDriverBinding,
            ImageHandle,
            &gConPlatformComponentName,
            NULL,
            NULL
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return INSTALL_ALL_DRIVER_PROTOCOLS (
          ImageHandle,
          SystemTable,
          &gConPlatformTextInDriverBinding,
          NULL,
          &gConPlatformComponentName,
          NULL,
          NULL
          );
}

STATIC
EFI_STATUS
EFIAPI
ConPlatformTextInDriverBindingSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
/*++

Routine Description:
  Supported 

Arguments:
  (Standard DriverBinding Protocol Supported() function)

Returns:

  None

--*/
{
  return ConPlatformDriverBindingSupported (
          This,
          ControllerHandle,
          RemainingDevicePath,
          &gEfiSimpleTextInProtocolGuid
          );
}

STATIC
EFI_STATUS
EFIAPI
ConPlatformTextOutDriverBindingSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
/*++

Routine Description:
  Supported 

Arguments:
  (Standard DriverBinding Protocol Supported() function)

Returns:

  None

--*/
{
  return ConPlatformDriverBindingSupported (
          This,
          ControllerHandle,
          RemainingDevicePath,
          &gEfiSimpleTextOutProtocolGuid
          );
}

EFI_STATUS
ConPlatformDriverBindingSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath,
  IN  EFI_GUID                     *ProtocolGuid
  )
/*++

Routine Description:
  Supported 

Arguments:
  (Standard DriverBinding Protocol Supported() function)

Returns:

  None

--*/
{
  EFI_STATUS  Status;
  VOID        *Interface;

  //
  // Test to see if this is a physical device by checking to see if
  // it has a Device Path Protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Test to see if this device supports the Simple Text Output Protocol
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  ProtocolGuid,
                  (VOID **) &Interface,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
        ControllerHandle,
        ProtocolGuid,
        This->DriverBindingHandle,
        ControllerHandle
        );

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
ConPlatformTextInDriverBindingStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  )
/*++

Routine Description:


Arguments:
  (Standard DriverBinding Protocol Start() function)

Returns:


--*/
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_SIMPLE_TEXT_IN_PROTOCOL *TextIn;

  //
  // Get the Device Path Protocol so the environment variables can be updated
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Open the Simple Input Protocol BY_DRIVER
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiSimpleTextInProtocolGuid,
                  (VOID **) &TextIn,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Check the device handle, if it is a hot plug device,
  // do not put the device path into ConInDev, and install
  // gEfiConsoleInDeviceGuid to the device handle directly.
  // The policy is, make hot plug device plug in and play immediately.
  //
  if (IsHotPlugDevice (This->DriverBindingHandle, ControllerHandle)) {
    gBS->InstallMultipleProtocolInterfaces (
          &ControllerHandle,
          &gEfiConsoleInDeviceGuid,
          NULL,
          NULL
          );
  } else {
    //
    // Append the device path to the ConInDev environment variable
    //
    ConPlatformUpdateDeviceVariable (
      VarConsoleInpDev,
      DevicePath,
      APPEND
      );

    //
    // If the device path is an instance in the ConIn environment variable,
    // then install EfiConsoleInDeviceGuid onto ControllerHandle
    //
    Status = ConPlatformUpdateDeviceVariable (
              VarConsoleInp,
              DevicePath,
              CHECK
              );

    if (!EFI_ERROR (Status)) {
      gBS->InstallMultipleProtocolInterfaces (
            &ControllerHandle,
            &gEfiConsoleInDeviceGuid,
            NULL,
            NULL
            );
    } else {
      gBS->CloseProtocol (
            ControllerHandle,
            &gEfiSimpleTextInProtocolGuid,
            This->DriverBindingHandle,
            ControllerHandle
            );
    }
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
ConPlatformTextOutDriverBindingStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    ControllerHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  )
/*++

Routine Description:


Arguments:
  (Standard DriverBinding Protocol Start() function)

Returns:


--*/
{
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_SIMPLE_TEXT_OUT_PROTOCOL  *TextOut;
  BOOLEAN                       NeedClose;

  NeedClose = TRUE;

  //
  // Get the Device Path Protocol so the environment variables can be updated
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Open the Simple Text Output Protocol BY_DRIVER
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID **) &TextOut,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Check the device handle, if it is a hot plug device,
  // do not put the device path into ConOutDev and StdErrDev,
  // and install gEfiConsoleOutDeviceGuid to the device handle directly.
  // The policy is, make hot plug device plug in and play immediately.
  //
  if (IsHotPlugDevice (This->DriverBindingHandle, ControllerHandle)) {
    gBS->InstallMultipleProtocolInterfaces (
          &ControllerHandle,
          &gEfiConsoleOutDeviceGuid,
          NULL,
          NULL
          );
  } else {
    //
    // Append the device path to the ConOutDev environment variable
    //
    ConPlatformUpdateDeviceVariable (
      VarConsoleOutDev,
      DevicePath,
      APPEND
      );
    //
    // Append the device path to the StdErrDev environment variable
    //
    ConPlatformUpdateDeviceVariable (
      VarErrorOutDev,
      DevicePath,
      APPEND
      );

    //
    // If the device path is an instance in the ConOut environment variable,
    // then install EfiConsoleOutDeviceGuid onto ControllerHandle
    //
    Status = ConPlatformUpdateDeviceVariable (
              VarConsoleOut,
              DevicePath,
              CHECK
              );

    if (!EFI_ERROR (Status)) {
      NeedClose = FALSE;
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &ControllerHandle,
                      &gEfiConsoleOutDeviceGuid,
                      NULL,
                      NULL
                      );
    }
    //
    // If the device path is an instance in the StdErr environment variable,
    // then install EfiStandardErrorDeviceGuid onto ControllerHandle
    //
    Status = ConPlatformUpdateDeviceVariable (
              VarErrorOut,
              DevicePath,
              CHECK
              );
    if (!EFI_ERROR (Status)) {
      NeedClose = FALSE;
      gBS->InstallMultipleProtocolInterfaces (
            &ControllerHandle,
            &gEfiStandardErrorDeviceGuid,
            NULL,
            NULL
            );
    }

    if (NeedClose) {
      gBS->CloseProtocol (
            ControllerHandle,
            &gEfiSimpleTextOutProtocolGuid,
            This->DriverBindingHandle,
            ControllerHandle
            );
    }
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
ConPlatformDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  )
/*++

Routine Description:

Arguments:
  (Standard DriverBinding Protocol Stop() function)

Returns:

  None

--*/
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  //
  // hot plug device is not included into the console associated variables,
  // so no need to check variable for those hot plug devices.
  //
  if (!IsHotPlugDevice (This->DriverBindingHandle, ControllerHandle)) {
    //
    // Get the Device Path Protocol so the environment variables can be updated
    //
    Status = gBS->OpenProtocol (
                    ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DevicePath,
                    This->DriverBindingHandle,
                    ControllerHandle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Remove DevicePath from ConInDev, ConOutDev, and StdErrDev
      //
      ConPlatformUpdateDeviceVariable (
        VarConsoleInpDev,
        DevicePath,
        DELETE
        );
      ConPlatformUpdateDeviceVariable (
        VarConsoleOutDev,
        DevicePath,
        DELETE
        );
      ConPlatformUpdateDeviceVariable (
        VarErrorOutDev,
        DevicePath,
        DELETE
        );
    }
  }
  //
  // Uninstall the Console Device GUIDs from Controller Handle
  //
  ConPlatformUnInstallProtocol (
    This,
    ControllerHandle,
    &gEfiConsoleInDeviceGuid
    );

  ConPlatformUnInstallProtocol (
    This,
    ControllerHandle,
    &gEfiConsoleOutDeviceGuid
    );

  ConPlatformUnInstallProtocol (
    This,
    ControllerHandle,
    &gEfiStandardErrorDeviceGuid
    );

  //
  // Close the Simple Input and Simple Text Output Protocols
  //
  gBS->CloseProtocol (
        ControllerHandle,
        &gEfiSimpleTextInProtocolGuid,
        This->DriverBindingHandle,
        ControllerHandle
        );

  gBS->CloseProtocol (
        ControllerHandle,
        &gEfiSimpleTextOutProtocolGuid,
        This->DriverBindingHandle,
        ControllerHandle
        );

  return EFI_SUCCESS;
}

VOID
ConPlatformUnInstallProtocol (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Handle,
  IN  EFI_GUID                     *ProtocolGuid
  )
{
  EFI_STATUS  Status;

  Status = gBS->OpenProtocol (
                  Handle,
                  ProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Handle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  if (!EFI_ERROR (Status)) {
    gBS->UninstallMultipleProtocolInterfaces (
          Handle,
          ProtocolGuid,
          NULL,
          NULL
          );
  }

  return ;
}

VOID *
ConPlatformGetVariable (
  IN  CHAR16    *Name
  )
/*++

Routine Description:
  Read the EFI variable (Name) and return a dynamically allocated
  buffer, and the size of the buffer. On failure return NULL.

Arguments:
  Name       - String part of EFI variable name

Returns:
  Dynamically allocated memory that contains a copy of the EFI variable.
  Caller is repsoncible freeing the buffer.

  NULL - Variable was not read
  
--*/
{
  EFI_STATUS  Status;
  VOID        *Buffer;
  UINTN       BufferSize;

  BufferSize  = 0;
  Buffer      = NULL;

  //
  // Test to see if the variable exists.  If it doesn't reuturn NULL
  //
  Status = gRT->GetVariable (
                  Name,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BufferSize,
                  Buffer
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, &Buffer);
    if (EFI_ERROR (Status)) {
      return NULL;
    }
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (
                    Name,
                    &gEfiGlobalVariableGuid,
                    NULL,
                    &BufferSize,
                    Buffer
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }
  }

  return Buffer;
}

EFI_STATUS
ConPlatformMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  * Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  * Single,
  IN  EFI_DEVICE_PATH_PROTOCOL  **NewDevicePath OPTIONAL,
  IN  BOOLEAN                   Delete
  )
/*++

Routine Description:
  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

Arguments:
  Multi        - A pointer to a multi-instance device path data structure.

  Single       - A pointer to a single-instance device path data structure.
  
  NewDevicePath - If Delete is TRUE, this parameter must not be null, and it
                  points to the remaining device path data structure. 
                  (remaining device path = Multi - Single.)
  
  Delete        - If TRUE, means removing Single from Multi.
                  If FALSE, the routine just check whether Single matches 
                  with any instance in Multi.

Returns:

  The function returns EFI_SUCCESS if the Single is contained within Multi.  
  Otherwise, EFI_NOT_FOUND is returned.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath1;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath2;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  //
  // The passed in DevicePath should not be NULL
  //
  if ((!Multi) || (!Single)) {
    return EFI_NOT_FOUND;
  }
  //
  // if performing Delete operation, the NewDevicePath must not be NULL.
  //
  TempDevicePath1 = NULL;

  DevicePath      = Multi;
  DevicePathInst  = EfiDevicePathInstance (&DevicePath, &Size);

  //
  // search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst) {
    if (EfiCompareMem (Single, DevicePathInst, Size) == 0) {
      if (!Delete) {
        gBS->FreePool (DevicePathInst);
        return EFI_SUCCESS;
      }
    } else {
      if (Delete) {
        TempDevicePath2 = EfiAppendDevicePathInstance (
                            TempDevicePath1,
                            DevicePathInst
                            );
        gBS->FreePool (TempDevicePath1);
        TempDevicePath1 = TempDevicePath2;
      }
    }

    gBS->FreePool (DevicePathInst);
    DevicePathInst = EfiDevicePathInstance (&DevicePath, &Size);
  }

  if (Delete) {
    *NewDevicePath = TempDevicePath1;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
ConPlatformUpdateDeviceVariable (
  IN  CHAR16                    *VariableName,
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN  CONPLATFORM_VAR_OPERATION Operation
  )
/*++

Routine Description:
  

Arguments:

Returns:

  None

--*/
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *VariableDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewVariableDevicePath;

  VariableDevicePath    = NULL;
  NewVariableDevicePath = NULL;

  //
  // Get Variable according to variable name.
  // The memory for Variable is allocated within ConPlatformGetVarible(),
  // it is the caller's responsibility to free the memory before return.
  //
  VariableDevicePath = ConPlatformGetVariable (VariableName);

  if (Operation != DELETE) {

    Status = ConPlatformMatchDevicePaths (
              VariableDevicePath,
              DevicePath,
              NULL,
              FALSE
              );

    if ((Operation == CHECK) || (!EFI_ERROR (Status))) {
      //
      // The device path is already in the variable
      //
      gBS->FreePool (VariableDevicePath);

      return Status;
    }
    //
    // The device path is not in variable. Append DevicePath to the
    // environment variable that is a multi-instance device path.
    //
    Status = EFI_SUCCESS;
    NewVariableDevicePath = EfiAppendDevicePathInstance (
                              VariableDevicePath,
                              DevicePath
                              );
    if (NewVariableDevicePath == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
    }

  } else {
    //
    // Remove DevicePath from the environment variable that
    // is a multi-instance device path.
    //
    Status = ConPlatformMatchDevicePaths (
              VariableDevicePath,
              DevicePath,
              &NewVariableDevicePath,
              TRUE
              );
  }

  gBS->FreePool (VariableDevicePath);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  EfiDevicePathSize (NewVariableDevicePath),
                  NewVariableDevicePath
                  );

  gBS->FreePool (NewVariableDevicePath);

  return Status;
}

BOOLEAN
IsHotPlugDevice (
  EFI_HANDLE    DriverBindingHandle,
  EFI_HANDLE    ControllerHandle
  )
{
  EFI_STATUS  Status;

  //
  // HotPlugDeviceGuid indicates ControllerHandle stands for a hot plug device.
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiHotPlugDeviceGuid,
                  NULL,
                  DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return TRUE;
}
