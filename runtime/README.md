# Naming
## Type naming
All of the type definitions (except for default C types) that are
used globally across the OGE should be named by the next convention:
```c
typedef <type> OgeXXX <definition> OgeXXX;

// Exmaple from clock.h
typedef struct OgeClock {
  u64 startTime;
  u64 endTime;
  u64 elapsedTime;
} OgeClock; 

// Example from logging.h
typedef enum OgeLogLevel {
  OGE_LOG_LEVEL_TRACE,
  OGE_LOG_LEVEL_INFO,
  OGE_LOG_LEVEL_WARN,
  OGE_LOG_LEVEL_ERROR,
  OGE_LOG_LEVEL_FATAL,
} OgeLogLevel;
```

## Function naming
All of the function that are used globally across the OGE should
be named by the next convention:
```c
<keywords> <return type> ogeXXX(<arguments>);

// Example from engine.h
OGE_API void ogeRun();
```

# Vulkan
## Don't use VkXXX2 structures and vkXXX2 functions.
The new API version brougth the similarity for structures and
added **sType** and **pNext** fields to the structures such as:
VkQueueFamilyProperties2, VkPhysicalDeviceFeatures2 and so on...

These are just bring more unnecessary code to get things done,
so... Why we ever should use it for now?
