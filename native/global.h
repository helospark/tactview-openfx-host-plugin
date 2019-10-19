#ifdef DEBUG_LOGGING
#define LOG(msg) \
    std::cout << "DEBUG  [openfxNativePlugin] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl
#else
#define LOG(msg) \
    do {} while(0)
#endif


#define LOG_WARN(msg) \
    std::cout << "WARN  [openfxNativePlugin] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl

#define LOG_ERROR(msg) \
    std::cout << "ERROR [openfxNativePlugin] " << __FILE__ << ":" << __LINE__ << " - " << msg << std::endl

#ifdef _WIN32
# define EXPORTED  __declspec( dllexport )
#else
# define EXPORTED
#endif

