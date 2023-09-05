/*
 * event.cpp
 *
 *  Created on: Aug 27, 2023
 *      Author: gns2.lee
 */

#include "event.hpp"

using namespace eda;

static bool is_init = false;
static bool is_log = true;
static qbuffer_t event_q = {};
static event::event_node_t event_node;
static std::array<event_t, EVENT_Q_MAX> event_buf;

error_t event::Init()
{
    is_init = false;
    is_log = true;
    event_q = {};
    for (int i = 0; i < EVENT_Q_MAX; i++)
    {
        event_buf[i] = {};
    }
    event_node = {};
    event_node.count = 0;
    for (int i = 0; i < EVENT_NODE_MAX; i++)
    {
        event_node.event_func[i] = NULL;
    }
    is_init = qbufferCreateBySize(&event_q, (uint8_t *)event_buf.data(), sizeof(event_t), EVENT_Q_MAX);

    return ERROR_SUCCESS;
}

bool event::NodeAdd(bool (*func)(event_t *p_event))
{
    if (event_node.count < EVENT_NODE_MAX)
    {
        event_node.event_func[event_node.count] = func;
        event_node.count++;
        return true;
    }

    return false;
}

bool event::Availble(void)
{
    if (is_init != true)
        return false;

    if (qbufferAvailable(&event_q) > 0)
        return true;
    else
        return false;
}

bool event::Get(event_t *p_event)
{
    bool ret;

    if (is_init != true)
        return false;

    ret = qbufferRead(&event_q, (uint8_t *)p_event, 1);

    return ret;
}

bool event::Put(event_t *p_event)
{
    bool ret;

    if (is_init != true)
        return false;
    qbuffer_t event_q1{};
    ret = qbufferWrite(&event_q1, (uint8_t *)p_event, 1);

    return ret;
}

bool event::Update(void)
{
    if (is_init != true)
        return false;

    while (Availble())
    {
        event_t evt;

        if (Get(&evt) == true)
        {
            if (is_log == true)
            {
                logPrintf("[  ] Event %s:%d\n", event_str[evt.code], evt.data);
            }

            for (int i = 0; i < event_node.count; i++)
            {
                if (event_node.event_func[i] != NULL)
                {
                    event_node.event_func[i](&evt);
                }
            }
        }
    }

    return true;
}