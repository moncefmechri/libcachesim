set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

add_library(cachesim SHARED CacheToolbox.cpp LRUCache.cpp ExclusiveHierarchy.cpp)
install(TARGETS cachesim DESTINATION lib)
install(FILES CacheToolbox.h LRUCache.h ExclusiveHierarchy.h DESTINATION include)

