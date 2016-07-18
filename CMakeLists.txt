set(CMAKE_BUILD_TYPE "RelWithDebInfo")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

add_library(cachesim SHARED CacheToolbox.cpp LRUCache.cpp ExclusiveCache.cpp ExclusiveHierarchy.cpp)
install(TARGETS cachesim DESTINATION lib/libcachesim)
install(FILES CacheToolbox.h LRUCache.h ExclusiveCache.h ExclusiveHierarchy.h DESTINATION include/libcachesim)

