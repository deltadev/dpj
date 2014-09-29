#import "DPJGLView.h"

#import <OpenGL/gl3.h>

@interface NSObject (DJGLSupport)
- (void)delegateKeyDown:(NSEvent*)theEvent;

@end

@implementation DPJGLView
@synthesize displayLink = _displayLink;

@synthesize delegate = _delegate;



- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
  CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
  [[self openGLContext] makeCurrentContext];
  
  [_delegate draw];
  
  CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
  CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
  return kCVReturnSuccess;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext)
{
  CVReturn result;
  @autoreleasepool
  { result = [(__bridge DPJGLView*)displayLinkContext getFrameForTime:outputTime]; }
  return result;
}
- (BOOL)acceptsFirstResponder { return YES; }

- (id)initWithFrame:(NSRect)frameRect
{
  NSOpenGLPixelFormatAttribute attrs[] =
  {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFADepthSize, 24,
    NSOpenGLPFAOpenGLProfile,
    NSOpenGLProfileVersion3_2Core,
    0
  };
  NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	
  if (!pf)
    NSLog(@"No OpenGL pixel format");
  
  self = [super initWithFrame:frameRect pixelFormat:pf];
	
  return self;
}
- (void) dealloc { CVDisplayLinkRelease(_displayLink); }


- (void)prepareOpenGL
{
  [[self openGLContext] makeCurrentContext];
  GLint swapInt = 1;
  [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
  
  render_context.reset(new dpj::gl::render_context{});

  CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
	
  @autoreleasepool
  {
    CVDisplayLinkSetOutputCallback(_displayLink, &MyDisplayLinkCallback, (__bridge void*)self);
  }
  
  CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
  render_context->cgl_context = cglContext;
  
  CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, cglContext, cglPixelFormat);
  
  CVDisplayLinkStart(_displayLink);
  
  if (!_delegate)
  {
    NSLog(@" delegate not set. Needed for view didInitGL");
    assert(_delegate);
  }
  else
  {
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    
    [_delegate viewDidInitGL];
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
  }
}

- (void)reshape
{
  NSRect rect = [self bounds];

  CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
  glViewport(0, 0, rect.size.width, rect.size.height);
  CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

#pragma mark - view events

- (void)mouseDragged:(NSEvent*)event
{
  if ([event modifierFlags] & NSControlKeyMask )
  {
    render_context->offset.x += 0.1*[event deltaX];
    render_context->offset.y -= 0.1*[event deltaY];
  }
  else
  {
    render_context->angle.y += 0.5*[event deltaX]; // left-right rotates asround y-axis
    render_context->angle.x += 0.5*[event deltaY]; // up-down around x-axis.
  }
  
}

- (void)scrollWheel:(NSEvent *)theEvent
{
  if ([theEvent modifierFlags] & NSControlKeyMask)
    render_context->offset.z += 0.01*[theEvent scrollingDeltaY];
  else
  {
    float tmp = render_context->view.fov + 0.01*[theEvent scrollingDeltaY];
    if (tmp <= 0)
      tmp = 0;
    else if (tmp >= 180)
      tmp = 180;
    render_context->view.fov = tmp;
  }
}

- (void)keyDown:(NSEvent *)theEvent
{
  id delegate = [NSApp delegate];
  [delegate delegateKeyDown:theEvent];
}
@end
