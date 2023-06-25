#include <CC/core.h>
#include <CC/entity.h>
#include <CC/eventloop.h>
#include <string.h>
#include <CC/server.h>

static EventLoop * CC_GLOBAL_event_loop = NULL;

EventLoop* CC_Core_GetEventLoop(void) {
    return CC_GLOBAL_event_loop;
}

void CC_Core_Init(void) {
    CC_World_Init(256, 64, 256);

    if(!CC_World_Load()) {
        CC_World_Generate();
    }

    CC_Event_Init();

    CC_Player_Init();
}

void CC_Core_SetEventLoop(EventLoop * event_loop) {
    CC_GLOBAL_event_loop = event_loop;
    CC_EventLoop_SetServer(event_loop, true);

    CC_Server_Register_Handlers(CC_GLOBAL_event_loop);
}

static float CC_GLOBAL_delta_time = 0.0f;

void CC_Core_Update(double delta_time) {
    if(CC_GLOBAL_event_loop != NULL) {
        CC_EventLoop_Update(CC_GLOBAL_event_loop);
    }
    CC_Event_Handle_InBound_Client();

    CC_GLOBAL_delta_time += (float)delta_time;

    CC_Entity_Update(delta_time);

    if(CC_GLOBAL_delta_time >= 0.05f) {
        CC_Player_Update();
        CC_GLOBAL_delta_time = 0.0f;
    }
}

void CC_Core_Term(void) {
    CC_Event_Term();
    CC_Player_Term();
    CC_World_Term();
}
