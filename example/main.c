#include "oge/oge.h"

int main() {
  OgeInitInfo initInfo = {
    .pApplicationName = "OGE application",
    .width  = 1280,
    .height = 720,
  };

  if(!ogeInit(&initInfo)) {
    return 1;
  }

  ogeTerminate();
}
