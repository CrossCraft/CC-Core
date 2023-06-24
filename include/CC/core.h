#pragma once
#include <CC/world.h>
#include <CC/player.h>
#include <CC/event.h>
#include <CC/eventloop.h>

#ifdef __cplusplus
extern "C" {
#endif

void CC_Core_Init(void);

void CC_Core_SetEventLoop(EventLoop * event_loop);

void CC_Core_Update(double delta_time);

void CC_Core_Term(void);

EventLoop* CC_Core_GetEventLoop(void);

#ifdef __cplusplus
}
#endif
