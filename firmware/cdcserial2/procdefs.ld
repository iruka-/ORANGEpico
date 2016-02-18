/*************************************************************************
 * Processor-specific object file.  Contains SFR definitions.
 *************************************************************************/
/*INPUT("processor.o")*/

/*************************************************************************
 * For interrupt vector handling
 *************************************************************************/
PROVIDE(_vector_spacing = 0x00000001);
_ebase_address  = 0x9D001000;

/*************************************************************************
 * Memory Address Equates
 *************************************************************************/
_RESET_ADDR              = 0x9D001000;
_BEV_EXCPT_ADDR          = 0x9D001380;
_DBG_EXCPT_ADDR          = 0x9D001480;
_DBG_CODE_ADDR           = 0xBFC02000;
_GEN_EXCPT_ADDR          = _ebase_address + 0x180;

/*************************************************************************
 * Memory Regions
 *
 * Memory regions without attributes cannot be used for orphaned sections.
 * Only sections specifically assigned to these regions can be allocated
 * into these regions.
 *************************************************************************/
MEMORY
{
  kseg0_program_mem    (rx)  : ORIGIN = 0x9D001a90, LENGTH = 0x3c00
  kseg0_boot_mem             : ORIGIN = 0x9D001810, LENGTH = 0x0380
  exception_mem              : ORIGIN = 0x9D001000, LENGTH = 0x1000
  kseg1_user_mem             : ORIGIN = 0x9D002000, LENGTH = 0x800
	kseg1_boot_mem           : ORIGIN = 0x9D001000, LENGTH = 0xc00
  debug_exec_mem             : ORIGIN = 0xBFC02000, LENGTH = 0xFF0
  config3                    : ORIGIN = 0xBFC00BF0, LENGTH = 0x4
  config2                    : ORIGIN = 0xBFC00BF4, LENGTH = 0x4
  config1                    : ORIGIN = 0xBFC00BF8, LENGTH = 0x4
  config0                    : ORIGIN = 0xBFC00BFC, LENGTH = 0x4
  kseg1_data_mem       (w!x) : ORIGIN = 0xA0000000, LENGTH = 0x2000
  sfrs                       : ORIGIN = 0xBF800000, LENGTH = 0x100000
  bootrom                    : ORIGIN = 0xBFC00000, LENGTH = 0x10
}
SECTIONS
{
  .config_BFC00BF0 : {
    KEEP(*(.config_BFC00BF0))
  } > config3
  .config_BFC00BF4 : {
    KEEP(*(.config_BFC00BF4))
  } > config2
  .config_BFC00BF8 : {
    KEEP(*(.config_BFC00BF8))
  } > config1
  .config_BFC00BFC : {
    KEEP(*(.config_BFC00BFC))
  } > config0

 .extra_prg_mem :
  {
	* (extra_prg_mem)
  } >kseg1_user_mem  

  .bootrom : {
    KEEP(*(.bootrom))
  } > bootrom
 }
