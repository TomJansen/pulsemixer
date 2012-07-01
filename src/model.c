#include "model.h"

sink_input_t  * sink_inputs = NULL;
int n_sink_inputs = 0;

sink_t * sinks = NULL;
int n_sinks = 0;

sink_input_t * sink_input_by_index(uint32_t index)
{
    int i;
    for(i=0;i<n_sink_inputs;++i) {
        if(sink_inputs[i].index == index)
            return &sink_inputs[i];
    }
    return NULL;
}

sink_input_t * sink_input_by_client_index(uint32_t client_index)
{
    int i;
    for(i=0;i<n_sink_inputs;++i) {
        if(sink_inputs[i].client_index == client_index)
            return &sink_inputs[i];
    }
    return NULL;
}

void sink_input_remove(uint32_t index)
{
    int i, k;
    for(i=0;i<n_sink_inputs;++i) {
        if(sink_inputs[i].index == index) {
            sink_input_t * s = &sink_inputs[i];
            free(s->name);
            free(s->client_name);
            for(k=i+1;k<n_sink_inputs;++k) {
                sink_inputs[k-1] = sink_inputs[k];
            }
            n_sink_inputs--;
            break;
        }
    }

}

void sink_input_add(const pa_sink_input_info * info)
{
    n_sink_inputs++;
    sink_inputs = realloc(sink_inputs, n_sink_inputs*sizeof(sink_input_t));
    sink_input_t * sink_input = &sink_inputs[n_sink_inputs-1];

    sink_input->index = info->index;
    sink_input->name = strdup(info->name);
    sink_input->client_index = info->client;
    sink_input->sink_index = info->sink;
    sink_input->volume = info->volume;
    sink_input->client_name = NULL;
}

sink_t * sink_by_index(uint32_t index)
{
    int i;
    for(i=0;i<n_sinks;++i) {
        if(sinks[i].index == index)
            return &sinks[i];
    }
    return NULL;
}

void sink_remove(uint32_t index)
{
    int i, k;
    for(i=0;i<n_sinks;++i) {
        if(sinks[i].index == index) {
            sink_t * s = &sinks[i];
            free(s->name);
            free(s->description);
            for(k=i+1;k<n_sinks;++k) {
                sinks[k-1] = sinks[k];
            }
            n_sinks--;
            break;
        }
    }
}

void sink_add(const pa_sink_info * info)
{
    n_sinks++;
    sinks = realloc(sinks, n_sinks*sizeof(sink_t));
    sink_t * sink = &sinks[n_sinks-1];

    sink->index = info->index;
    sink->name = strdup(info->name);
    sink->description = strdup(info->description);
    sink->volume = info->volume;
}
