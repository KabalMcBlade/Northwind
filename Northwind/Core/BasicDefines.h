#pragma once


#ifdef NORTHWIND_EXPORTS
#define NW_DLL __declspec(dllexport)
#else
#define NW_DLL __declspec(dllimport)
#endif 


#if _WIN32 || _WIN64
#if _WIN64
#define NW_x64
#else
#define NW_x86
#endif
#elif __GNUC__
#if __x86_64__ || __ppc64__
#define NW_x64
#else
#define NW_x86
#endif
#else
#define NW_x86
#endif


// defines for easy namespace
#define NW_NAMESPACE_BEGIN namespace Northwind {
#define NW_NAMESPACE_END };

#define NW_USING_NAMESPACE using namespace Northwind; 

#define NW_OPTIMIZATION_OFF __pragma(optimize("",off))
#define NW_OPTIMIZATION_ON __pragma(optimize("",on))

/// forces a function to be in lined
#define NW_INLINE __forceinline

// tells the compiler to never inline a particular function
#define NW_NO_INLINE  __declspec(noinline)

// default memory align
#define NW_MEMORY_ALIGNMENT_SIZE 16

// Memory alignment
#define NW_MEMORY_ALIGNMENT(x)			__declspec(align(x))
#define NW_IS_ALIGNED(ptr, alignment)   ((std::uintptr_t)ptr & (alignment - 1)) == 0
#define NW_MEMORY_ALIGNED				NW_MEMORY_ALIGNMENT(NW_MEMORY_ALIGNMENT_SIZE)


#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif