#pragma once

#include <cstdio>
#include <assert.h>

#include "BasicDefines.h"


#ifdef _DEBUG

#define nwWarning( format, ... ) fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__);
#define nwAssert( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		assert(condition); \
    }
#define nwAssertVoid( condition, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return; \
    }
#define nwAssertValue( condition, return_value, format, ... ) \
    if( !(condition) ) { \
        fprintf (stderr, "%s(%u): " format "\n", __FILE__, __LINE__, __VA_ARGS__); \
		return return_value; \
    }

#else

#define nwWarning( format, ... )
#define nwAssert( condition, format, ... )
#define nwAssertVoid( condition, format, ... )
#define nwAssertValue( condition, return_value, format, ... )

#endif 


#define nwAssertReturnVoid( condition, format, ... )					nwAssertVoid( condition, format, __VA_ARGS__ )
#define nwAssertReturnValue( condition, return_value, format, ...  )	nwAssertValue( condition, return_value, format, __VA_ARGS__ )