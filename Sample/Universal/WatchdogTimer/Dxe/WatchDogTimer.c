/*++

Copyright (c) 2004 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  WatchDogTimer.c

Abstract:

 Generic watchdog timer implemenetation using EFI APIs

Revision History

--*/

#include "WatchDogTimer.h"

//
// Handle for the Watchdog Timer Architectural Protocol instance produced by this driver
//
EFI_HANDLE                        mWatchdogTimerHandle = NULL;

//
// The Watchdog Timer Architectural Protocol instance produced by this driver
//
EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  mWatchdogTimer = {
  WatchdogTimerDriverRegisterHandler,
  WatchdogTimerDriverSetTimerPeriod,
  WatchdogTimerDriverGetTimerPeriod
};

//
// The watchdog timer period in 100 nS units
//
UINT64                            mWatchdogTimerPeriod = 0;

//
// The notification function to call if the watchdig timer fires
//
EFI_WATCHDOG_TIMER_NOTIFY         mWatchdogTimerNotifyFunction = NULL;

//
// The one-shot timer event that is armed when the watchdog timer is enabled
//
EFI_EVENT                         mWatchdogTimerEvent;

//
// Worker Functions
//
VOID
EFIAPI
WatchdogTimerDriverExpires (
  IN EFI_EVENT    Timer,
  IN VOID         *Context
  )
/*++

  Routine Description:

    Notification function that is called if the watchdog timer is fired.  If a 
    handler has been registered with the Watchdog Timer Architectural Protocol,
    then that handler is called passing in the time period that has passed that
    cause the watchdog timer to fire.  Then, a call to the Runtime Service 
    ResetSystem() is made to reset the platform.
    
  Arguments:

    Timer   - The one-shot timer event that was signaled when the watchdog timer 
              expired.

    Context - The context that was registered when the event Timer was created.

  Returns:

    None.

--*/
{
  //
  // Report error code before exiting
  //
  EfiLibReportStatusCode (
    EFI_ERROR_CODE | EFI_ERROR_MINOR,
    (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_TIMER_EXPIRED),
    0,
    &gEfiCallerIdGuid,
    NULL
    );

  //
  // If a notification function has been registered, then call it
  //
  if (mWatchdogTimerNotifyFunction != NULL) {
    mWatchdogTimerNotifyFunction (mWatchdogTimerPeriod);
  }
  //
  // Reset the platform
  //
  gRT->ResetSystem (EfiResetCold, EFI_TIMEOUT, 0, NULL);
}

EFI_STATUS
EFIAPI
WatchdogTimerDriverRegisterHandler (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN EFI_WATCHDOG_TIMER_NOTIFY         NotifyFunction
  )
/*++

Routine Description:

  This function registers a handler that is to be invoked when the watchdog 
  timer fires.  By default, the EFI_WATCHDOG_TIMER protocol will call the 
  Runtime Service ResetSystem() when the watchdog timer fires.  If a 
  NotifyFunction is registered, then the NotifyFunction will be called before 
  the Runtime Service ResetSystem() is called.  If NotifyFunction is NULL, then 
  the watchdog handler is unregistered.  If a watchdog handler is registered, 
  then EFI_SUCCESS is returned.  If an attempt is made to register a handler 
  when a handler is already registered, then EFI_ALREADY_STARTED is returned.  
  If an attempt is made to uninstall a handler when a handler is not installed, 
  then return EFI_INVALID_PARAMETER.

Arguments:

  This           - The EFI_WATCHDOG_TIMER_ARCH_PROTOCOL instance.

  NotifyFunction - The function to call when the watchdog timer fires.  If this
                   is NULL, then the handler will be unregistered.

Returns: 

  EFI_SUCCESS           - The watchdog timer handler was registered or 
                          unregistered.

  EFI_ALREADY_STARTED   - NotifyFunction is not NULL, and a handler is already 
                          registered.

  EFI_INVALID_PARAMETER - NotifyFunction is NULL, and a handler was not 
                          previously registered.

--*/
{
  if (NotifyFunction == NULL && mWatchdogTimerNotifyFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (NotifyFunction != NULL && mWatchdogTimerNotifyFunction != NULL) {
    return EFI_ALREADY_STARTED;
  }

  mWatchdogTimerNotifyFunction = NotifyFunction;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
WatchdogTimerDriverSetTimerPeriod (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                            TimerPeriod
  )
/*++

Routine Description:

  This function sets the amount of time to wait before firing the watchdog 
  timer to TimerPeriod 100 nS units.  If TimerPeriod is 0, then the watchdog 
  timer is disabled.

Arguments:

  This        - The EFI_WATCHDOG_TIMER_ARCH_PROTOCOL instance.

  TimerPeriod - The amount of time in 100 nS units to wait before the watchdog 
                timer is fired.  If TimerPeriod is zero, then the watchdog 
                timer is disabled.
  
Returns: 

  EFI_SUCCESS      - The watchdog timer has been programmed to fire in Time 
                     100 nS units.

  EFI_DEVICE_ERROR - A watchdog timer could not be programmed due to a device 
                     error.

--*/
{
  mWatchdogTimerPeriod = TimerPeriod;

  return gBS->SetTimer (
                mWatchdogTimerEvent,
                (mWatchdogTimerPeriod == 0) ? TimerCancel : TimerRelative,
                mWatchdogTimerPeriod
                );
}

EFI_STATUS
EFIAPI
WatchdogTimerDriverGetTimerPeriod (
  IN EFI_WATCHDOG_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                            *TimerPeriod
  )
/*++

Routine Description:

  This function retrieves the amount of time the system will wait before firing 
  the watchdog timer.  This period is returned in TimerPeriod, and EFI_SUCCESS 
  is returned.  If TimerPeriod is NULL, then EFI_INVALID_PARAMETER is returned.

Arguments:

  This        - The EFI_WATCHDOG_TIMER_ARCH_PROTOCOL instance.

  TimerPeriod - A pointer to the amount of time in 100 nS units that the system 
                will wait before the watchdog timer is fired.  If TimerPeriod of
                zero is returned, then the watchdog timer is disabled.
  
Returns: 

  EFI_SUCCESS           - The amount of time that the system will wait before 
                          firing the watchdog timer was returned in TimerPeriod.

  EFI_INVALID_PARAMETER - TimerPeriod is NULL.

--*/
{
  if (TimerPeriod == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *TimerPeriod = mWatchdogTimerPeriod;

  return EFI_SUCCESS;
}

EFI_DRIVER_ENTRY_POINT (WatchdogTimerDriverInitialize)

EFI_STATUS
EFIAPI
WatchdogTimerDriverInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  Initialize the Watchdog Timer Architectural Protocol driver

Arguments:

  ImageHandle - ImageHandle of the loaded driver

  SystemTable - Pointer to the System Table

Returns:

  EFI_SUCCESS           - Timer Architectural Protocol created

  EFI_OUT_OF_RESOURCES  - Not enough resources available to initialize driver.
  
  EFI_DEVICE_ERROR      - A device error occured attempting to initialize the driver.

--*/
{
  EFI_STATUS  Status;

  //
  // Initialize the EFI Driver Library
  //
  EfiInitializeDriverLib (ImageHandle, SystemTable);

  EfiLibReportStatusCode (
    EFI_PROGRESS_CODE,
    (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_SW_PC_INIT_BEGIN),
    0,
    &gEfiCallerIdGuid,
    NULL
    );
  //
  // Make sure the Watchdog Timer Architectural Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiWatchdogTimerArchProtocolGuid);

  //
  // Create the timer event used to implement a simple watchdog timer
  //
  Status = gBS->CreateEvent (
                  EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
                  EFI_TPL_NOTIFY,
                  WatchdogTimerDriverExpires,
                  NULL,
                  &mWatchdogTimerEvent
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Install the Watchdog Timer Arch Protocol onto a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mWatchdogTimerHandle,
                  &gEfiWatchdogTimerArchProtocolGuid,
                  &mWatchdogTimer,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  EfiLibReportStatusCode (
    EFI_PROGRESS_CODE,
    (EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_SW_PC_INIT_END),
    0,
    &gEfiCallerIdGuid,
    NULL
    );

  return Status;
}
