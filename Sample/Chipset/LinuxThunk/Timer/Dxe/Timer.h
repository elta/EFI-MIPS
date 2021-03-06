/*++

Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  Timer.h

Abstract:

  NT Emulation Architectural Protocol Driver as defined in Tiano.
  This Timer module uses an NT Thread to simulate the timer-tick driven
  timer service.

--*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include "Efi2Linux.h"
#include "EfiLinuxLib.h"
#include "EfiDriverLib.h"

//
// Consumed Protocols
//
#include EFI_ARCH_PROTOCOL_CONSUMER (Cpu)

//
// Produced Protocols
//
#include EFI_ARCH_PROTOCOL_PRODUCER (Timer)

//
// Legal timer value range in 100 ns units
//
#define TIMER_MINIMUM_VALUE 0
#define TIMER_MAXIMUM_VALUE (0x100000000 - 1)

//
// Default timer value in 100 ns units (10 ms)
//
#define DEFAULT_TIMER_TICK_DURATION 100000

//
// Function Prototypes
//
EFI_STATUS
EFIAPI
LinuxTimerDriverInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  ImageHandle - TODO: add argument description
  SystemTable - TODO: add argument description

Returns:

  TODO: add return values

--*/
;

EFI_STATUS
EFIAPI
LinuxTimerDriverRegisterHandler (
  IN EFI_TIMER_ARCH_PROTOCOL  *This,
  IN EFI_TIMER_NOTIFY         NotifyFunction
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This            - TODO: add argument description
  NotifyFunction  - TODO: add argument description

Returns:

  TODO: add return values

--*/
;

EFI_STATUS
EFIAPI
LinuxTimerDriverSetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                   TimerPeriod
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This        - TODO: add argument description
  TimerPeriod - TODO: add argument description

Returns:

  TODO: add return values

--*/
;

EFI_STATUS
EFIAPI
LinuxTimerDriverGetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL   *This,
  OUT UINT64                   *TimerPeriod
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This        - TODO: add argument description
  TimerPeriod - TODO: add argument description

Returns:

  TODO: add return values

--*/
;

EFI_STATUS
EFIAPI
LinuxTimerDriverGenerateSoftInterrupt (
  IN EFI_TIMER_ARCH_PROTOCOL  *This
  )
/*++

Routine Description:

  TODO: Add function description

Arguments:

  This  - TODO: add argument description

Returns:

  TODO: add return values

--*/
;

#endif
