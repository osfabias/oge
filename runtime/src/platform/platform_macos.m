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
#include "oge/core/logging.h"
#include "oge/core/platform.h"
#include "oge/core/assertion.h"

@class ContentView;
@class WindowDelegate;
@class ApplicationDelegate;

typedef struct OgeMacOSHandleInfo {
    CAMetalLayer *pLayer;
} OgeMacOSHandleInfo;
 
struct {
  b8                  initialized;
  ApplicationDelegate *pAppDelegate;
  WindowDelegate      *pWindowDelegate;
  NSWindow            *pWindow;
  ContentView         *pView;
  OgeMacOSHandleInfo  handle;
  b8                  terminateRequsted;
  f32                 deviceRatio;
} ogePlatformState;

@interface WindowDelegate : NSObject <NSWindowDelegate> {}

@end // WindowDelegate


@implementation WindowDelegate

- (BOOL)windowShouldClose:(id)sender {
  ogePlatformState.terminateRequsted = OGE_TRUE;
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

  - (BOOL)canBecomeKeyView {
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

  // If these methods not implemented - compiler generates warnings and
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
  if (ogePlatformState.initialized) {
    OGE_WARN("Trying to initialize Cocoa platform while it's already initialized.");
    return OGE_TRUE;
  }

  OGE_TRACE("Initializing Cocoa platform.");

  // TODO: There should be an @autoreleasepool block, but it
  // causes segfault in future on ogePlatformTerminate call

  // Zero platform state struct
  memset(&ogePlatformState, 0, sizeof(ogePlatformState));

  [NSApplication sharedApplication];

  // App delegate creation
  ogePlatformState.pAppDelegate = [[ApplicationDelegate alloc] init];
  if (!ogePlatformState.pAppDelegate) {
    OGE_ERROR("Failed to initialize NS application delegate.");
    return OGE_FALSE;
  }
  OGE_TRACE("NS application delegate successfully initialized.");
  [NSApp setDelegate:ogePlatformState.pAppDelegate];

  // Window delegate creation
  ogePlatformState.pWindowDelegate = [[WindowDelegate alloc] init];
  if (!ogePlatformState.pWindowDelegate) {
    OGE_ERROR("Failed to initialize NS window delegate.");
    return OGE_FALSE;
  }
  OGE_TRACE("NS window delegate successfully initialized.");

  // Window creation
  ogePlatformState.pWindow = [[NSWindow alloc]
      initWithContentRect:NSMakeRect(0, 0, pInitInfo->width, pInitInfo->height)
      styleMask:NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable
      backing:NSBackingStoreBuffered
      defer:NO];
  if (!ogePlatformState.pWindow) {
    OGE_ERROR("Failed to allocate NS window.");
    return OGE_FALSE;
  }
  OGE_TRACE("NS window successfully allocated.");

  // View creation
  ogePlatformState.pView = [[ContentView alloc] initWithWindow:ogePlatformState.pWindow];
  [ogePlatformState.pView setWantsLayer:YES];
  OGE_TRACE("NS content view successfully allocated.");

  // Layer creation
  ogePlatformState.handle.pLayer = [CAMetalLayer layer];
  OGE_TRACE("CAMetal layer successfully created.");

  // Setting window properties
  [ogePlatformState.pWindow setLevel:NSNormalWindowLevel];
  [ogePlatformState.pWindow setContentView:ogePlatformState.pView];
  [ogePlatformState.pWindow makeFirstResponder:ogePlatformState.pView];
  [ogePlatformState.pWindow setTitle:@(pInitInfo->pApplicationName)];
  [ogePlatformState.pWindow setDelegate:ogePlatformState.pWindowDelegate];
  [ogePlatformState.pWindow setAcceptsMouseMovedEvents:YES];
  [ogePlatformState.pWindow setRestorable:NO];
  OGE_TRACE("Window properties successully set.");

  if (![[NSRunningApplication currentApplication] isFinishedLaunching]) {
    [NSApp run];
    OGE_TRACE("NS application successfully runned.");
  }

  // Making the app a proper UI app since we're unbundled
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  // Putting window in front on launch
  [NSApp activateIgnoringOtherApps:YES];
  [ogePlatformState.pWindow makeKeyAndOrderFront:nil];

  // Handle content scaling for various fidelity displays (i.e. Retina)
  ogePlatformState.handle.pLayer.bounds = ogePlatformState.pView.bounds;

  // It's important to set the drawableSize to the actual backing pixels. When rendering
  // full-screen, we can skip the macOS compositor if the size matches the display size.
  ogePlatformState.handle.pLayer.drawableSize = [ogePlatformState.pView convertSizeToBacking:ogePlatformState.pView.bounds.size];

  // In its implementation of vkGetPhysicalDeviceSurfaceCapabilitiesKHR, MoltenVK takes into
  // consideration both the size (in points) of the bounds, and the contentsScale of the
  // CAMetalLayer from which the Vulkan surface was created.
  // NOTE: See also https://github.com/KhronosGroup/MoltenVK/issues/428
  ogePlatformState.handle.pLayer.contentsScale = ogePlatformState.pView.window.backingScaleFactor;

  // Save off the device pixel ratio.
  ogePlatformState.deviceRatio = ogePlatformState.handle.pLayer.contentsScale;
  [ogePlatformState.pView setLayer:ogePlatformState.handle.pLayer];

  // This is set to NO by default, but is also important to ensure we can bypass the compositor
  // in full-screen mode
  // NOTE: See "Direct to Display" http://metalkit.org/2017/06/30/introducing-metal-2.html.
  ogePlatformState.handle.pLayer.opaque = YES;

  OGE_TRACE("Additional preparations finished.");

  // Mark that platform is initialized successfully
  ogePlatformState.initialized = OGE_TRUE;

  OGE_INFO("Cocoa platform initialized.");
  
  return OGE_TRUE;
}

void ogePlatformTerminate() {
  if (!ogePlatformState.initialized) {
    OGE_WARN("Trying to terminated Cocoa platform while it's already terminated.");
    return;
  }

  OGE_TRACE("Terminating Cocoa platform.");

  [ogePlatformState.pWindow orderOut:nil];
  OGE_TRACE("NS window out order nilled.");

  [ogePlatformState.pWindow setDelegate:nil];
  [ogePlatformState.pWindowDelegate release];
  OGE_TRACE("NS window delegate released.");

  [ogePlatformState.pView release];
  ogePlatformState.pView = nil;
  OGE_TRACE("NS content view released.");

  [ogePlatformState.pWindow close];
  ogePlatformState.pWindow = nil;
  OGE_TRACE("NS window closed.");

  [NSApp setDelegate:nil];
  [ogePlatformState.pAppDelegate release];
  ogePlatformState.pAppDelegate = nil;
  OGE_TRACE("NS application delegate released.");

  ogePlatformState.initialized = OGE_FALSE;

  OGE_INFO("Cocoa platform terminated.");
}

void ogePlatformPumpMessages() {
  OGE_ASSERT(ogePlatformState.initialized, "Trying to pump platform messages while platfrom isn't initialized");

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
  return ogePlatformState.terminateRequsted;
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

f64 ogePlatformGetTime() {
  mach_timebase_info_data_t clockTimebase;
  mach_timebase_info(&clockTimebase);

  u64 machAbsolute = mach_absolute_time();

  u64 nanos = (f64)(machAbsolute * (u64)clockTimebase.numer) / (f64)clockTimebase.denom;
  return nanos / 1.0e9; // Convert to seconds
}

void ogePlatformSleep(u64 ms) {
  OGE_TRACE("Sleeping on current thread for %d ms", ms);
  if (ms >= 1000) { sleep(ms / 1000); }
  usleep((ms % 1000) * 1000);
}

