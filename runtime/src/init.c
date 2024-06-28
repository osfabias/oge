#include "oge/oge.h"
#include "oge/core/platform.h"

int ogeInit(const OgeInitInfo *pInitInfo) {
  _OgePlatformInitInfo platformInitInfo = {
    .pApplicationName = pInitInfo->pApplicationName,
    .width  = pInitInfo->width,
    .height = pInitInfo->height,
  };
  _ogePlatformInit(&platformInitInfo);

  return OGE_TRUE;
}

void ogeTerminate() {
  _ogePlatformTerminate();
}
