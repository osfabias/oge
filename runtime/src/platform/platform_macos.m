#include <arm/param.h>
#include <crt_externs.h>
#include <mach/mach_time.h>

#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <copyfile.h>
#include <string.h>
#include <sys/stat.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Foundation/Foundation.h>

#include "oge/defines.h"
// control keys
#include "oge/core/input.h"
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"

static OgeKey translateKey(OgeKey key) {
  // https://boredzo.org/blog/wp-content/uploads/2007/05/IMTx-virtual-keycodes.pdf
  // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
  switch (key) {
    case 0x52:
      return OGE_KEY_NUMPAD0;
    case 0x53:
      return OGE_KEY_NUMPAD1;
    case 0x54:
      return OGE_KEY_NUMPAD2;
    case 0x55:
      return OGE_KEY_NUMPAD3;
    case 0x56:
      return OGE_KEY_NUMPAD4;
    case 0x57:
      return OGE_KEY_NUMPAD5;
    case 0x58:
      return OGE_KEY_NUMPAD6;
    case 0x59:
      return OGE_KEY_NUMPAD7;
    case 0x5B:
      return OGE_KEY_NUMPAD8;
    case 0x5C:
      return OGE_KEY_NUMPAD9;

    case 0x12:
      return OGE_KEY_1;
    case 0x13:
      return OGE_KEY_2;
    case 0x14:
      return OGE_KEY_3;
    case 0x15:
      return OGE_KEY_4;
    case 0x17:
      return OGE_KEY_5;
    case 0x16:
      return OGE_KEY_6;
    case 0x1A:
      return OGE_KEY_7;
    case 0x1C:
      return OGE_KEY_8;
    case 0x19:
      return OGE_KEY_9;
    case 0x1D:
      return OGE_KEY_0;

    case 0x00:
      return OGE_KEY_A;
    case 0x0B:
      return OGE_KEY_B;
    case 0x08:
      return OGE_KEY_C;
    case 0x02:
      return OGE_KEY_D;
    case 0x0E:
      return OGE_KEY_E;
    case 0x03:
      return OGE_KEY_F;
    case 0x05:
      return OGE_KEY_G;
    case 0x04:
      return OGE_KEY_H;
    case 0x22:
      return OGE_KEY_I;
    case 0x26:
      return OGE_KEY_J;
    case 0x28:
      return OGE_KEY_K;
    case 0x25:
      return OGE_KEY_L;
    case 0x2E:
      return OGE_KEY_M;
    case 0x2D:
      return OGE_KEY_N;
    case 0x1F:
      return OGE_KEY_O;
    case 0x23:
      return OGE_KEY_P;
    case 0x0C:
      return OGE_KEY_Q;
    case 0x0F:
      return OGE_KEY_R;
    case 0x01:
      return OGE_KEY_S;
    case 0x11:
      return OGE_KEY_T;
    case 0x20:
      return OGE_KEY_U;
    case 0x09:
      return OGE_KEY_V;
    case 0x0D:
      return OGE_KEY_W;
    case 0x07:
      return OGE_KEY_X;
    case 0x10:
      return OGE_KEY_Y;
    case 0x06:
      return OGE_KEY_Z;

    case 0x27:
      return OGE_KEY_APOSTROPHE;
    case 0x2A:
      return OGE_KEY_BACKSLASH;
    case 0x2B:
      return OGE_KEY_COMMA;
    case 0x18:
      return OGE_KEY_EQUAL; // Equal/Plus
    case 0x32:
      return OGE_KEY_GRAVE;
    case 0x21:
      return OGE_KEY_LBRACKET; 
    case 0x1B:
      return OGE_KEY_MINUS;
    case 0x2F:
      return OGE_KEY_PERIOD;
    case 0x1E:
      return OGE_KEY_RBRACKET;
    case 0x29:
      return OGE_KEY_SEMICOLON;
    case 0x2C:
      return OGE_KEY_SLASH;
    case 0x0A:
      return OGE_KEY_MAX_ENUM; // ?

    case 0x33:
      return OGE_KEY_BACKSPACE;
    case 0x39:
      return OGE_KEY_CAPSLOCK;
    case 0x75:
      return OGE_KEY_DELETE;
    case 0x7D:
      return OGE_KEY_DOWN;
    case 0x77:
      return OGE_KEY_END;
    case 0x24:
      return OGE_KEY_ENTER;
    case 0x35:
      return OGE_KEY_ESCAPE;
    case 0x7A:
      return OGE_KEY_F1;
    case 0x78:
      return OGE_KEY_F2;
    case 0x63:
      return OGE_KEY_F3;
    case 0x76:
      return OGE_KEY_F4;
    case 0x60:
      return OGE_KEY_F5;
    case 0x61:
      return OGE_KEY_F6;
    case 0x62:
      return OGE_KEY_F7;
    case 0x64:
      return OGE_KEY_F8;
    case 0x65:
      return OGE_KEY_F9;
    case 0x6D:
      return OGE_KEY_F10;
    case 0x67:
      return OGE_KEY_F11;
    case 0x6F:
      return OGE_KEY_F12;
    case 0x69:
      return OGE_KEY_PRINT;
    case 0x6B:
      return OGE_KEY_F14;
    case 0x71:
      return OGE_KEY_F15;
    case 0x6A:
      return OGE_KEY_F16;
    case 0x40:
      return OGE_KEY_F17;
    case 0x4F:
      return OGE_KEY_F18;
    case 0x50:
      return OGE_KEY_F19;
    case 0x5A:
      return OGE_KEY_F20;
    case 0x73:
      return OGE_KEY_HOME;
    case 0x72:
      return OGE_KEY_INSERT;
    case 0x7B:
      return OGE_KEY_LEFT;
    case 0x3A:
      return OGE_KEY_LALT;
    case 0x3B:
      return OGE_KEY_LCONTROL;
    case 0x38:
      return OGE_KEY_LSHIFT;
    case 0x37:
      return OGE_KEY_LSUPER;
    case 0x6E:
      return OGE_KEY_MAX_ENUM; // Menu
    case 0x47:
      return OGE_KEY_NUMLOCK;
    case 0x79:
      return OGE_KEY_PAGEDOWN;
    case 0x74:
      return OGE_KEY_PAGEUP;
    case 0x7C:
      return OGE_KEY_RIGHT;
    case 0x3C:
      return OGE_KEY_RSHIFT;
    case 0x36:
      return OGE_KEY_RSUPER;
    case 0x31:
      return OGE_KEY_SPACE;
    case 0x30:
      return OGE_KEY_TAB;
    case 0x7E:
      return OGE_KEY_UP;

    case 0x45:
      return OGE_KEY_ADD;
    case 0x41:
      return OGE_KEY_DECIMAL;
    case 0x4B:
      return OGE_KEY_DIVIDE;
    case 0x4C:
      return OGE_KEY_ENTER;
    case 0x51:
      return OGE_KEY_NUMPAD_EQUAL;
    case 0x43:
      return OGE_KEY_MULTIPLY;
    case 0x4E:
      return OGE_KEY_SUBTRACT;

    default:
      return OGE_KEY_MAX_ENUM;
  }
}

@class ContentView;
@class WindowDelegate;
@class ApplicationDelegate;

typedef struct OgeMacOSHandleInfo {
    CAMetalLayer *pLayer;
} OgeMacOSHandleInfo;
 
struct {
  b8 initialized;
  ApplicationDelegate *pAppDelegate;
  WindowDelegate *pWindowDelegate;
  NSWindow *pWindow;
  ContentView *pView;
  OgeMacOSHandleInfo handle;
  b8 terminateRequsted;
  u8 keyStates[OGE_KEY_MAX_ENUM];
  struct {
    u16 x;
    u16 y;
    i8 wheel;
    u8 buttonStates[OGE_KEY_MAX_ENUM];
  } mouseState;
  f32 deviceRatio;
} s_platformState;

@interface WindowDelegate : NSObject <NSWindowDelegate> {}

@end // WindowDelegate


@implementation WindowDelegate

- (BOOL)windowShouldClose:(id)sender {
  s_platformState.terminateRequsted = OGE_TRUE;
  OGE_INFO("Platform terminate requested.");
  return YES;
}

@end // WindowDelegate

@interface ContentView : NSView <NSTextInputClient> {
  NSWindow                  *pWindow;
  NSTrackingArea            *pRrackingArea;
  NSMutableAttributedString *pMarkedText;
}

- (instancetype)initWithWindow:(NSWindow*)initWindow;

@end // ContentView

@implementation ContentView

  - (instancetype)initWithWindow:(NSWindow*)initWindow {
    self = [super init];
    if (self != nil) {
      pWindow = initWindow;
    }

    return self;
  }

  - (BOOL)canBecomeOGE_KEYBOARD_KEYView {
    return YES;
  }

  - (BOOL)acceptsFirstResponder {
    return YES;
  }

  - (BOOL)wantsUpdateLayer {
    return YES;
  }

  - (BOOL)acceptsFirstMouse:(NSEvent *)event {
    return YES;
  }
  
  - (void)mouseDown:(NSEvent *)event {
    s_platformState.mouseState.buttonStates[OGE_MOUSE_BUTTON_LEFT] = 1;
  }

  - (void)mouseDragged:(NSEvent *)event {
    // Equivalent to moving the mouse for now
    [self mouseMoved:event];
  }

  - (void)mouseUp:(NSEvent *)event {
    s_platformState.mouseState.buttonStates[OGE_MOUSE_BUTTON_LEFT] = 0;
  }

  - (void)mouseMoved:(NSEvent *)event {
    const NSPoint pos = [event locationInWindow];

    // Need to invert Y on macOS, since origin is bottom-left.
    // Also need to scale the mouse position by the device pixel
    // ratio so screen lookups are correct.
    NSSize window_size = s_platformState.handle.pLayer.drawableSize;
    s_platformState.mouseState.x = pos.x * s_platformState.handle.pLayer.contentsScale;
    s_platformState.mouseState.y =
      window_size.height - (pos.y * s_platformState.handle.pLayer.contentsScale);
  }

  - (void)rightMouseDown:(NSEvent *)event {
    s_platformState.mouseState.buttonStates[OGE_MOUSE_BUTTON_RIGHT] = 1;
  }

  - (void)rightMouseDragged:(NSEvent *)event  {
    // Equivalent to moving the mouse for now
    [self mouseMoved:event];
  }

  - (void)rightMouseUp:(NSEvent *)event {
    s_platformState.mouseState.buttonStates[OGE_MOUSE_BUTTON_RIGHT] = 0;
  }

  - (void)otherMouseDown:(NSEvent *)event {
    // Interpreted as middle click
    s_platformState.mouseState.buttonStates[OGE_MOUSE_BUTTON_MIDDLE] = 1;
  }

  - (void)otherMouseDragged:(NSEvent *)event {
    // Equivalent to moving the mouse for now
    [self mouseMoved:event];
  }

  - (void)otherMouseUp:(NSEvent *)event {
    // Interpreted as middle click
    s_platformState.mouseState.buttonStates[OGE_MOUSE_BUTTON_MIDDLE] = 0;
  }

  - (void)keyDown:(NSEvent *)event {
    OgeKey key = translateKey((u32)[event keyCode]);
    s_platformState.keyStates[key] = 1;

    // [self interpretKeyEvents:@[event]];
  }

  - (void)keyUp:(NSEvent *)event {
    OgeKey key = translateKey((u32)[event keyCode]);
    s_platformState.keyStates[key] = 0;
  }

  - (void)scrollWheel:(NSEvent *)event {
    s_platformState.mouseState.wheel = ((i8)[event scrollingDeltaY]);
  }

  // NOTE: If these methods not implemented - compiler generates warnings and
  // program crashes on platform initialization
  - (void)insertText:(id)string replacementRange:(NSRange)replacementRange {}

  - (void)setMarkedText:(id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange {}

  - (void)unmarkText {}

  // Defines a constant for empty ranges in NSTextInputClient
  static const NSRange kEmptyRange = { NSNotFound, 0 };

  - (NSRange)selectedRange {return kEmptyRange;}

  - (NSRange)markedRange {return kEmptyRange;}

  - (BOOL)hasMarkedText {return false;}

  - (nullable NSMutableAttributedString *)attributedSubstringForProposedRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {return nil;}

  - (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText {return [NSArray array];}

  - (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {return NSMakeRect(0, 0, 0, 0);}

  - (NSUInteger)characterIndexForPoint:(NSPoint)point {return 0;}
@end // ContentView

@interface ApplicationDelegate : NSObject <NSApplicationDelegate> {}

@end // ApplicationDelegate

@implementation ApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  // Posting an empty event at start
  @autoreleasepool {
    NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];
  } // autoreleasepool

  [NSApp stop:nil];
}

@end // ApplicationDelegate 

b8 ogePlatformInit(const OgePlatformInitInfo *pInitInfo) {
  if (s_platformState.initialized) {
    OGE_WARN("Trying to initialize Cocoa platform while it's already initialized.");
    return OGE_TRUE;
  }

  OGE_TRACE("Initializing Cocoa platform.");

  // TODO: There should be an @autoreleasepool block, but it
  // causes segfault in future on ogePlatformTerminate call

  // Zero platform state struct
  memset(&s_platformState, 0, sizeof(s_platformState));

  [NSApplication sharedApplication];

  // App delegate creation
  s_platformState.pAppDelegate = [[ApplicationDelegate alloc] init];
  if (!s_platformState.pAppDelegate) {
    OGE_ERROR("Failed to initialize NS application delegate.");
    return OGE_FALSE;
  }
  OGE_TRACE("NS application delegate successfully initialized.");
  [NSApp setDelegate:s_platformState.pAppDelegate];

  // Window delegate creation
  s_platformState.pWindowDelegate = [[WindowDelegate alloc] init];
  if (!s_platformState.pWindowDelegate) {
    OGE_ERROR("Failed to initialize NS window delegate.");
    return OGE_FALSE;
  }
  OGE_TRACE("NS window delegate successfully initialized.");

  // Window creation
  s_platformState.pWindow = [[NSWindow alloc]
      initWithContentRect:NSMakeRect(0, 0, pInitInfo->width, pInitInfo->height)
      styleMask:NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable
      backing:NSBackingStoreBuffered
      defer:NO];
  if (!s_platformState.pWindow) {
    OGE_ERROR("Failed to allocate NS window.");
    return OGE_FALSE;
  }
  OGE_TRACE("NS window successfully allocated.");

  // View creation
  s_platformState.pView = [[ContentView alloc] initWithWindow:s_platformState.pWindow];
  [s_platformState.pView setWantsLayer:YES];
  OGE_TRACE("NS content view successfully allocated.");

  // Layer creation
  s_platformState.handle.pLayer = [CAMetalLayer layer];
  OGE_TRACE("CAMetal layer successfully created.");

  // Setting window properties
  [s_platformState.pWindow setLevel:NSNormalWindowLevel];
  [s_platformState.pWindow setContentView:s_platformState.pView];
  [s_platformState.pWindow makeFirstResponder:s_platformState.pView];
  [s_platformState.pWindow setTitle:@(pInitInfo->pApplicationName)];
  [s_platformState.pWindow setDelegate:s_platformState.pWindowDelegate];
  [s_platformState.pWindow setAcceptsMouseMovedEvents:YES];
  [s_platformState.pWindow setRestorable:NO];
  OGE_TRACE("Window properties successully set.");

  if (![[NSRunningApplication currentApplication] isFinishedLaunching]) {
    [NSApp run];
    OGE_TRACE("NS application successfully runned.");
  }

  // Making the app a proper UI app since we're unbundled
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  // Putting window in front on launch
  [NSApp activateIgnoringOtherApps:YES];
  [s_platformState.pWindow makeKeyAndOrderFront:nil];

  // Handle content scaling for various fidelity displays (i.e. Retina)
  s_platformState.handle.pLayer.bounds = s_platformState.pView.bounds;

  // It's important to set the drawableSize to the actual backing pixels. When rendering
  // full-screen, we can skip the macOS compositor if the size matches the display size.
  s_platformState.handle.pLayer.drawableSize = [s_platformState.pView convertSizeToBacking:s_platformState.pView.bounds.size];

  // In its implementation of vkGetPhysicalDeviceSurfaceCapabilitiesKHR, MoltenVK takes into
  // consideration both the size (in points) of the bounds, and the contentsScale of the
  // CAMetalLayer from which the Vulkan surface was created.
  // NOTE: See also https://github.com/KhronosGroup/MoltenVK/issues/428
  s_platformState.handle.pLayer.contentsScale = s_platformState.pView.window.backingScaleFactor;

  // Save off the device pixel ratio.
  s_platformState.deviceRatio = s_platformState.handle.pLayer.contentsScale;
  [s_platformState.pView setLayer:s_platformState.handle.pLayer];

  // This is set to NO by default, but is also important to ensure we can bypass the compositor
  // in full-screen mode
  // NOTE: See "Direct to Display" http://metalkit.org/2017/06/30/introducing-metal-2.html.
  s_platformState.handle.pLayer.opaque = YES;

  OGE_TRACE("Additional preparations finished.");

  // Mark that platform is initialized successfully
  s_platformState.initialized = OGE_TRUE;

  OGE_INFO("Cocoa platform initialized.");

  return OGE_TRUE;
}

void ogePlatformTerminate() {
  if (!s_platformState.initialized) {
    OGE_WARN("Trying to terminated Cocoa platform while it's already terminated.");
    return;
  }

  OGE_TRACE("Terminating Cocoa platform.");

  [s_platformState.pWindow orderOut:nil];
  OGE_TRACE("NS window out order nilled.");

  [s_platformState.pWindow setDelegate:nil];
  [s_platformState.pWindowDelegate release];
  OGE_TRACE("NS window delegate released.");

  [s_platformState.pView release];
  s_platformState.pView = nil;
  OGE_TRACE("NS content view released.");

  [s_platformState.pWindow close];
  s_platformState.pWindow = nil;
  OGE_TRACE("NS window closed.");

  [NSApp setDelegate:nil];
  [s_platformState.pAppDelegate release];
  s_platformState.pAppDelegate = nil;
  OGE_TRACE("NS application delegate released.");

  s_platformState.initialized = OGE_FALSE;

  OGE_INFO("Cocoa platform terminated.");
}

void ogePlatformPumpMessages() {
  OGE_ASSERT(s_platformState.initialized, "Trying to pump platform messages while platfrom isn't initialized");

  @autoreleasepool {
    NSEvent* event;

    while (1) {
      event = [NSApp 
        nextEventMatchingMask:NSEventMaskAny
        untilDate:[NSDate distantPast]
        inMode:NSDefaultRunLoopMode
        dequeue:YES];

      if (!event) {
        break;
      }
      
      [NSApp sendEvent:event];
    }
  } // autoreleasepool
}

b8 ogePlatformTerminateRequested() {
  return s_platformState.terminateRequsted;
}

void* ogePlatformAllocate(u64 size, b8 aligned) {
  return malloc(size);
}

void* ogePlatformReallocate(void *pBlock, u64 size, b8 aligned) {
  return realloc(pBlock, size);
}

void ogePlatformDeallocate(void *pBlock, b8 aligned) {
  free(pBlock);
}

void ogePlatformMemoryCopy(void *pDstBlock, const void *pSrcBlock, u64 size) {
  memcpy(pDstBlock, pSrcBlock, size);
}

void ogePlatformMemorySet(void *pDstBlock, i32 value, u64 size) {
  memset(pDstBlock, value, size);
}

void ogePlatformMemoryMove(void *pDstBlock, const void *pSrcBlock, u64 size) {
  memmove(pDstBlock, pSrcBlock, size);
}

i32 ogePlatformMemoryCompare(const void *pBlock1, const void *pBlock2, u64 size) {
  return memcmp(pBlock1, pBlock2, size);
}

void ogePlatformConsoleWrite(const char *pMessage, u8 color) {
  // trace, info, warn, error, fatal
  const char* clrStrings[] = { "1;30", "1;32", "1;33", "1;31", "0;41", };
  printf("\033[%sm%s\033[0m", clrStrings[color], pMessage);
}

const u8* ogePlatformGetKeyStates() {
  return s_platformState.keyStates;
}

const u8* ogePlatformGetMouseButtonStates() {
  return s_platformState.mouseState.buttonStates;
}

void ogePlatformGetMousePosition(u16 *x, u16 *y) {
  *x = s_platformState.mouseState.x;
  *y = s_platformState.mouseState.y;
}

i8 ogePlatformGetMouseWheel() {
  return s_platformState.mouseState.wheel;
}

u64 ogePlatformGetTime() {
  mach_timebase_info_data_t clockTimebase;
  mach_timebase_info(&clockTimebase);

  u64 machAbsolute = mach_absolute_time();

  u64 nanos = (f64)(machAbsolute * (u64)clockTimebase.numer) / (f64)clockTimebase.denom;
  return nanos * 0.000001f; // to milliseconds
}

void ogePlatformSleep(u64 ms) {
  OGE_TRACE("Sleeping on current thread for %d ms", ms);
  if (ms >= 1000) { sleep(ms / 1000); }
  usleep((ms % 1000) * 1000);
}

