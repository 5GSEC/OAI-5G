#ifndef __ATTACK_EXTERN_H__
#define __ATTACK_EXTERN_H__
 
#undef  _ATTACK_VDEF
#define _ATTACK_VDEF(_type, _var, _def)	extern _type _var;

// See _attack_vars_abs.h for valid attack level option information
#include "_attack_vars_abs.h"

#endif /* ifndef __ATTACK_EXTERN_H__ */
