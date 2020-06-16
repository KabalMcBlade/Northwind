#pragma once

#include <cstdio>

#include "BasicDefines.h"


#ifdef _DEBUG

#define udAssert( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
    }
#define udAssertVoid( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return; \
    }
#define udAssertValue( condition, return_value, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return return_value; \
    }

#else

#define udAssert( condition, format, ... )
#define udAssertVoid( condition, format, ... )
#define udAssertValue( condition, return_value, format, ... )

#endif 


#define udAssertReturnVoid( condition, format, ... )					udAssertVoid( condition, format, __VA_ARGS__ )
#define udAssertReturnValue( condition, return_value, format, ...  )	udAssertValue( condition, return_value, format, __VA_ARGS__ )