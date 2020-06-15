#pragma once


#ifdef UNDEAD_EXPORTS
#define UD_DLL __declspec(dllexport)
#else
#define UD_DLL __declspec(dllimport)
#endif 


#if _WIN32 || _WIN64
#if _WIN64
#define UD_x64
#else
#define UD_x86
#endif
#elif __GNUC__
#if __x86_64__ || __ppc64__
#define UD_x64
#else
#define UD_x86
#endif
#else
#define UD_x86
#endif


// defines for easy namespace
#define UD_NAMESPACE_BEGIN namespace Undead {
#define UD_NAMESPACE_END };  

#define UD_USING_NAMESPACE using namespace Undead; 

#define UD_OPTIMIZATION_OFF __pragma(optimize("",off))
#define UD_OPTIMIZATION_ON __pragma(optimize("",on))

/// forces a function to be in lined
#define UD_INLINE __forceinline

// tells the compiler to never inline a particular function
#define UD_NO_INLINE  __declspec(noinline)

