#pragma once

#define AGL_FILE __FILE__
#define AGL_LINE __LINE__

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define AGL_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define AGL_FUNC_NAME __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define AGL_FUNC_NAME __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define AGL_FUNC_NAME __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define AGL_FUNC_NAME __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define AGL_FUNC_NAME __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define AGL_FUNC_NAME __func__
#else
#define AGL_FUNC_NAME "AGL_FUNC_NAME could not be established!"
#endif

#define AGL_CODE_POINT "Filename: " AGL_FILE "\nFunction: " AGL_FUNC_NAME "\nLine: " AGL_LINE

#define AGL_HALT() __debugbreak()

#ifdef AGL_DEBUG
#define AGL_CORE_ASSERT(condition, message, ...) \
		do { \
			if(!(condition)) \
			{ \
				AGL_HALT(); \
			} \
		} while(false)

#define AGL_ASSERT(condition, message, ...) \
		do { \
			if(!(condition)) \
			{ \
				AGL_HALT(); \
			} \
		} while(false)
#else
#define AGL_CORE_ASSERT(condition, message, ...) \
		do { \
			(void)sizeof(condition); \
		} while(false)

#define AGL_ASSERT(condition, message, ...) \
		do { \
			(void)sizeof(condition); \
		} while(false)
#endif