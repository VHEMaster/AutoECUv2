/*
******************************************************************************
**
**  File        : LinkerScript.ld
**
**  Author      : STM32CubeIDE
**
**  Abstract    : Linker script for STM32H7 series
**                      1024Kbytes FLASH
**                       800Kbytes RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Distribution: The file is distributed as is without any warranty
**                of any kind.
**
*****************************************************************************
** @attention
**
** Copyright (c) 2023 STMicroelectronics.
** All rights reserved.
**
** This software is licensed under terms that can be found in the LICENSE file
** in the root directory of this software component.
** If no LICENSE file comes with this software, it is provided AS-IS.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM); /* end of "DTCMRAM" Ram type memory */

_Min_Heap_Size = 0x2000; /* required amount of heap  */
_Min_Stack_Size = 0x4000; /* required amount of stack */

/* Memories definition */
MEMORY
{
  RAM_D1 (xrw)   : ORIGIN = 0x24000000, LENGTH =  512K
  FLASH  (rx)    : ORIGIN = 0x08000000, LENGTH = 1024K    /* Memory is divided. Actual start is 0x08000000 and actual length is 2048K */
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
}

/* Sections */
SECTIONS
{
  /* The startup code into "FLASH" Rom type memory */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup code */
    . = ALIGN(4);
  } >FLASH

  /* Initialized data sections into "RAM" Ram type memory */
  _siitcm_func = LOADADDR(.itcm_func);
  . = ALIGN(4);
  .itcm_func :
  {
    . = ALIGN(4);
    _sitcm_func = .;        /* create a global symbol at data start */
    __itcm_func_start__ = _sitcm_func;
    *(.itcm_func)        /* .itcm_func sections */
    *(.itcm_func*)       /* .itcm_func* sections */
    
    *(.text.HAL_ADC_St*)  	/* .text.HAL_* sections */
    *(.text.HAL_DMA_St*)  	/* .text.HAL_* sections */
    *(.text.HAL_SPI_T*)   	/* .text.HAL_* sections */
    *(.text.HAL_SPI_R*)   	/* .text.HAL_* sections */
    *(.text.HAL_TIM_S*)   	/* .text.HAL_* sections */
    *(.text.HAL_HRTIM_S*) 	/* .text.HAL_* sections */
    *(.text.HAL_UART_T*)  	/* .text.HAL_* sections */
    *(.text.HAL_UART_R*)  	/* .text.HAL_* sections */
    *(.text.HAL_USART_T*) 	/* .text.HAL_* sections */
    *(.text.HAL_USART_R*) 	/* .text.HAL_* sections */

    . = ALIGN(4);
    _eitcm_func = .;        /* define a global symbol at data end */
    __itcm_func_end__ = _eitcm_func;

  } >ITCMRAM AT> FLASH
  
  /* The program code and other data into "FLASH" Rom type memory */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >FLASH

  /* Constant data into "FLASH" Rom type memory */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >FLASH

  .ARM.extab (READONLY) : {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } >FLASH

  .ARM (READONLY) : {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >FLASH

  .preinit_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .init_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
  } >FLASH

  .fini_array (READONLY) :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
    . = ALIGN(4);
  } >FLASH

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections into "RAM" Ram type memory */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.RamFunc)        /* .RamFunc sections */
    *(.RamFunc*)       /* .RamFunc* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >DTCMRAM AT> FLASH

  /* Uninitialized data section into "RAM" Ram type memory */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >DTCMRAM

  /* User_heap section, used to check that there is enough "RAM" Ram  type memory left */
  ._user_heap :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = ALIGN(8);
  } >RAM_D1

  /* User_stack section, used to check that there is enough "RAM" Ram  type memory left */
  ._user_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >DTCMRAM
  
  /* Uninitialized data section into "RAM" Ram type memory */
  . = ALIGN(4);
  .dmabss :
  {
    /* This is used by the startup in order to initialize the .dmabss section */
    _sdmabss = .;         /* define a global symbol at dmabss start */
    __dmabss_start__ = _sdmabss;
    *(.dmabss)
    *(.dmabss*)

    . = ALIGN(4);
    _edmabss = .;         /* define a global symbol at dmabss end */
    __dmabss_end__ = _edmabss;
  } >RAM_D1

  /* Uninitialized data section into "RAM" Ram type memory */
  . = ALIGN(4);
  .rambss :
  {
    /* This is used by the startup in order to initialize the .rambss section */
    _srambss = .;         /* define a global symbol at rambss start */
    __rambss_start__ = _srambss;
    *(.rambss)
    *(.rambss*)

    . = ALIGN(4);
    _erambss = .;         /* define a global symbol at rambss end */
    __rambss_end__ = _erambss;
  } >RAM_D1

  /* Used by the startup to initialize data */
  _siramdata = LOADADDR(.ramdata);

  /* Initialized data sections into "RAM" Ram type memory */
  .ramdata :
  {
    . = ALIGN(4);
    _sramdata = .;        /* create a global symbol at data start */
    *(.ramdata)           /* .data sections */
    *(.ramdata*)          /* .data* sections */
    *(.RamDataFunc)        /* .RamFunc sections */
    *(.RamDataFunc*)       /* .RamFunc* sections */

    . = ALIGN(4);
    _eramdata = .;        /* define a global symbol at data end */
  } >RAM_D1 AT> FLASH

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
