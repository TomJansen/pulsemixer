#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <pulse/pulseaudio.h>

typedef struct {
    uint32_t index;
    char * name;
    char * description;
    pa_cvolume volume;
} sink_t; 

typedef struct {
    uint32_t index;
    char * name;
    uint32_t client_index;
    char * client_name;
    uint32_t sink_index;
    pa_cvolume volume;
} sink_input_t;

extern sink_input_t  * sink_inputs;
extern int n_sink_inputs;

extern sink_t * sinks;
extern int n_sinks;

sink_input_t * sink_input_by_index(uint32_t index);
sink_input_t * sink_input_by_client_index(uint32_t client_index);
sink_t * sink_by_index(uint32_t index);
void sink_input_remove(uint32_t index);
void sink_input_add(const pa_sink_input_info * info);
void sink_remove(uint32_t index);
void sink_add(const pa_sink_info * info);


#endif
