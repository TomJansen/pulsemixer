#include "model.h"
#include "view.h"

#define N_VIEW_ENTRIES_MAX 255
#define VIEW_WIDTH 1024

view_entry_t view_entries[255];
int n_view_entries;
int selected;

void view_init()
{
    int i;
    n_view_entries = 0;
    selected = 0;
    for(i=0; i< N_VIEW_ENTRIES_MAX; ++i) {
        view_entries[i].type = VIEW_EMPTY;
        view_entries[i].ref = NULL;
    }
}

void view_update()
{
    int i,j, k;
    k = 0;
    for(i=0;i<n_sinks;++i) {

        view_entries[k].type = VIEW_SINK;
        view_entries[k].ref = &sinks[i];
        k++;

        for(j=0;j<n_sink_inputs;++j) {
            if (sink_inputs[j].sink_index == sinks[i].index) {
                view_entries[k].type = VIEW_SINK_INPUT;
                view_entries[k].ref = &sink_inputs[j];
                k++;
            }
        }
    }

    n_view_entries = k;
    view_select_norm();

    view_show();
}

void view_select_norm()
{
    if (selected >= n_view_entries)
        selected = n_view_entries-1;
    if (selected < 0)
        selected = 0;
}

void view_show()
{
    char line[VIEW_WIDTH+1];
    clear();

    sprintf(line, "Use h,j,k,l,m,n keys, +shift to fine-tune.");

    mvwprintw(stdscr, 0, 0,  line);
    int i;
    for(i=0;i<n_view_entries;++i) {
        view_entry_t view_entry = view_entries[i];
        if(view_entry.type == VIEW_EMPTY) {
            sprintf(line, " ");
        }
        if(view_entry.type == VIEW_SINK) {
            sink_t * sink = (sink_t *) view_entry.ref;
            pa_volume_t volume_avg = pa_cvolume_avg(&sink->volume);
            sprintf(line, "[%7.2f dB] %s", pa_sw_volume_to_dB(volume_avg), sink->description);
        }
        if(view_entry.type == VIEW_SINK_INPUT) {
            sink_input_t * sink_input = (sink_input_t *) view_entry.ref;
            pa_volume_t volume_avg;
            volume_avg = pa_cvolume_avg(&sink_input->volume);
            sprintf(line, "[%7.2f dB] %s : %s", pa_sw_volume_to_dB(volume_avg), sink_input->client_name, sink_input->name);
        }

        // pad string with spaces
        int n = VIEW_WIDTH < COLS ? VIEW_WIDTH : COLS;
        int len,k;
        len = strlen(line);
        for(k=len;k<n;++k) {
            line[k] = ' ';
        }
        line[k] = '\0';
        line[COLS-1] = '\0';

        if(i == selected) {
            attron(COLOR_PAIR(2));
        } else {
            attroff(COLOR_PAIR(2));
        }
        mvwprintw(stdscr, 2+i, 0,  line);
        attroff(COLOR_PAIR(2));
    }
    refresh();
}

void view_select_next()
{
    selected++;
    view_select_norm();
}

void view_select_prev()
{
    selected--;
    view_select_norm();
}

view_entry_t view_selected_item()
{
    return view_entries[selected];
}
