#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ansidecl.h"	// added.

typedef int bfd_boolean;
#define FALSE 0
#define TRUE 1
#define ISSPACE(c) (isascii (c) && isspace(c))

//  bfd_arch_arm,       /* Advanced Risc Machines ARM.  */
#define bfd_mach_arm_unknown   0
#define bfd_mach_arm_2         1
#define bfd_mach_arm_2a        2
#define bfd_mach_arm_3         3
#define bfd_mach_arm_3M        4
#define bfd_mach_arm_4         5
#define bfd_mach_arm_4T        6
#define bfd_mach_arm_5         7
#define bfd_mach_arm_5T        8
#define bfd_mach_arm_5TE       9
#define bfd_mach_arm_XScale    10
#define bfd_mach_arm_ep9312    11
#define bfd_mach_arm_iWMMXt    12
#define bfd_mach_arm_iWMMXt2   13


//int floatformat_ieee_single_little=1;
//int floatformat_ieee_double_little=2;


#define STT_LOPROC	13		/* Application-specific semantics */
#define STT_HIPROC	15		/* Application-specific semantics */
/* Additional symbol types for Thumb.  */
#define STT_ARM_TFUNC      STT_LOPROC   /* A Thumb function.  */
#define STT_ARM_16BIT      STT_HIPROC   /* A Thumb label.  */

#define C_EXT		2	/* external symbol		*/
#define C_STAT		3	/* static			*/
#define C_LABEL		6	/* label			*/
/* Storage classes for Thumb symbols */
#define C_THUMBEXT      (128 + C_EXT)		/* 130 */
#define C_THUMBSTAT     (128 + C_STAT)		/* 131 */
#define C_THUMBLABEL    (128 + C_LABEL)		/* 134 */
#define C_THUMBEXTFUNC  (C_THUMBEXT  + 20)	/* 150 */
#define C_THUMBSTATFUNC (C_THUMBSTAT + 20)	/* 151 */

#define CONST_STRNEQ(STR1,STR2) (strncmp ((STR1), (STR2), sizeof (STR2) - 1) == 0)

#ifndef ATTRIBUTE_UNUSED
#define ATTRIBUTE_UNUSED __attribute__ ((__unused__))
#endif /* ATTRIBUTE_UNUSED */


typedef struct bfd_symbol
{
//  flagword flags;

  /* A pointer to the section to which this symbol is
     relative.  This will always be non NULL, there are special
     sections for undefined and absolute symbols.  */
  struct bfd_section *section;

  /* Back end special data.  */
  union
    {
      void *p;
//      bfd_vma i;
    }
  udata;
}
asymbol;

typedef struct
{
  /* The BFD symbol.  */
  asymbol symbol;
  /* ELF symbol information.  */
//  Elf_Internal_Sym internal_elf_sym;
  /* Backend specific information.  */
  union
    {
      unsigned int hppa_arg_reloc;
      void *mips_extr;
      void *any;
    }
  tc_data;

  /* Version information.  This is from an Elf_Internal_Versym
     structure in a SHT_GNU_versym section.  It is zero if there is no
     version information.  */
  unsigned short version;

} elf_symbol_type;

