#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <pulse/pulseaudio.h>
#include <sys/select.h>

#include "model.h"
#include "view.h"

typedef enum {
    VOL_UP_FINE,
    VOL_DOWN_FINE,
    VOL_UP,
    VOL_DOWN,
    VOL_NORM,
    VOL_MUTE
} vol_change_t;

void on_client_update(pa_context *c, const pa_client_info * info, int eol, void *userdata)
{
    if(eol) {
        view_update();
    } else {
        sink_input_t * sink_input = sink_input_by_client_index(info->index);
        if(sink_input != NULL) {
            free(sink_input->client_name); 
            sink_input->client_name = strdup(info->name);
        }
    }
}

void on_sink_input_update(pa_context * c, const pa_sink_input_info * info, int eol, void * userdata)
{
    if (eol) {
        view_update();
    } else {
        if(info->has_volume) {
            if(info->index != PA_INVALID_INDEX) {
                sink_input_t * sink_input = sink_input_by_index(info->index);
                if(sink_input == NULL) {
                    sink_input_add(info);
                    if(info->client != PA_INVALID_INDEX)
                        pa_context_get_client_info(c, info->client, on_client_update, NULL);
                } else {
                    sink_input->volume = info->volume;

                    if(sink_input->name == NULL || strcmp(sink_input->name, info->name) != 0) {
                        free(sink_input->name);
                        sink_input->name = strdup(info->name);
                    }
                }
            }
        }
    } 
}

void on_sink_update(pa_context * c, const pa_sink_info * info, int eol, void * userdata)
{
    if(eol) {
        view_update();
    } else {
        if(info->index != PA_INVALID_INDEX) {
            sink_t * sink = sink_by_index(info->index);
            if(sink == NULL) {
                sink_add(info);
            } else {
                sink->volume = info->volume;
            }
        }
    }
}

void on_event(pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata)
{
    if((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SINK_INPUT) {
        /* sink input changed */
        if((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
            if(idx != PA_INVALID_INDEX)
                pa_context_get_sink_input_info(c, idx, on_sink_input_update, NULL);
        }
        /* sink input removed */
        if((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            if(idx != PA_INVALID_INDEX) {
                sink_input_remove(idx);
                view_update();
            }
        }
        /* sink input added */
        if((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_NEW) {
            if(idx != PA_INVALID_INDEX)
                pa_context_get_sink_input_info(c, idx, on_sink_input_update, NULL);
        }
    }
    if((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SINK) {
        /* sink changed */
        if((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE) {
            if(idx != PA_INVALID_INDEX)
                pa_context_get_sink_info_by_index(c, idx, on_sink_update, NULL);
        }
    }
}

pa_volume_t modify_volume(pa_volume_t vol, vol_change_t vol_change)
{
    switch(vol_change) {
        case VOL_DOWN_FINE:
            if (vol < 500)
                return PA_VOLUME_MUTED;
            else
                return vol - 500;
        case VOL_UP_FINE:
            return vol + 500;
        case VOL_DOWN:
            if (vol < 2000)
                return PA_VOLUME_MUTED;
            else
                return vol - 2000;
            break;
        case VOL_UP:
            return vol + 2000;
            break;
        case VOL_MUTE:
            return PA_VOLUME_MUTED;
        default:
            return PA_VOLUME_NORM;
    }
}

void change_sel_vol(pa_context * c, vol_change_t vol_change)
{
    view_entry_t view_entry = view_selected_item();

    if(view_entry.type == VIEW_SINK) {
        sink_t * sink = (sink_t *) view_entry.ref;
        if(sink != NULL) {
            /* averge all channel volumes of local copy */
            pa_volume_t vol;
            vol = pa_cvolume_avg(&sink->volume);
            vol = modify_volume(vol, vol_change);
            pa_cvolume_set(&sink->volume, sink->volume.channels, vol);
            pa_context_set_sink_volume_by_index(c, sink->index, &sink->volume, NULL, NULL);
        }
    }

    if(view_entry.type == VIEW_SINK_INPUT) {
        sink_input_t * sink_input = (sink_input_t *) view_entry.ref;
        if(sink_input != NULL) {
            /* averge all channel volumes of local copy */
            pa_volume_t vol;
            vol = pa_cvolume_avg(&sink_input->volume);
            vol = modify_volume(vol, vol_change);
            pa_cvolume_set(&sink_input->volume, sink_input->volume.channels, vol);
            pa_context_set_sink_input_volume(c, sink_input->index, &sink_input->volume, NULL, NULL);
        }
    }
}

void pa_state_changed(pa_context *c, void *userdata) {
    pa_context_state_t * pa_state = userdata;
    *pa_state = pa_context_get_state(c);
}

int main(int argc, char **argv)
{
    int retval = 0;

    struct timeval timeout, timeout_def;

    pa_mainloop * ml = pa_mainloop_new();
    pa_mainloop_api * api = pa_mainloop_get_api(ml);
    pa_context_state_t pa_state;

    pa_context * context = pa_context_new(api, "pa-mixer");
    pa_context_connect(context, NULL, 0, NULL);
    pa_context_set_state_callback(context, pa_state_changed, &pa_state);

    int running = 1;
    int init_done = 0;

    initscr();
    start_color();
    curs_set(0);
    cbreak();
    noecho();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);

    fd_set read_set, test_set;
    FD_ZERO(&read_set);
    FD_SET(0, &read_set);
    timeout_def.tv_sec = 0;
    timeout_def.tv_usec = 1000;

    view_init();

    while(running) {
        if(pa_state == PA_CONTEXT_UNCONNECTED) {
            pa_mainloop_iterate(ml, 1, NULL);
            continue;
        }
        if(pa_state == PA_CONTEXT_FAILED || pa_state == PA_CONTEXT_TERMINATED) {
            running = 0;
            retval = -1;
        }
        if(pa_state == PA_CONTEXT_READY) {
            if(!init_done) {
                pa_context_set_subscribe_callback(context, on_event, NULL);
                pa_context_subscribe(context, PA_SUBSCRIPTION_MASK_ALL, NULL, NULL);
                pa_context_get_sink_input_info_list(context, on_sink_input_update, context);
                pa_context_get_sink_info_list(context, on_sink_update, context);
            }
            init_done = 1;
        }

        test_set = read_set;
        timeout = timeout_def;
        int r = select(1, &test_set, NULL, NULL, &timeout);
        if(r == 0) {
            /* timeout */
            pa_mainloop_iterate(ml, 0, NULL);
            /*
            wprintw(stdscr, "timeout!");
            refresh();
            */
        }
        if(FD_ISSET(0, &test_set)) {
            /* stdin ready */

            int key = wgetch(stdscr);
            /*
            printf("key %d!\n", key);
            fflush(stdout);
            */
            switch(key) {
                case 'k':
                    view_select_prev();
                    view_show();
                    break;
                case 'j':
                    view_select_next();
                    view_show();
                    break;
                case 'h':
                    change_sel_vol(context, VOL_DOWN);
                    break;
                case 'H':
                    change_sel_vol(context, VOL_DOWN_FINE);
                    break;
                case 'l':
                    change_sel_vol(context, VOL_UP);
                    break;
                case 'L':
                    change_sel_vol(context, VOL_UP_FINE);
                    break;
                case 'n':
                    change_sel_vol(context, VOL_NORM);
                    break;
                case 'm':
                    change_sel_vol(context, VOL_MUTE);
                    break;
            }
        }

    }

    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_mainloop_free(ml);

    return retval;
}