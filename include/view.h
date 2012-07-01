#ifndef VIEW_H
#define VIEW_H

typedef enum {
    VIEW_EMPTY,
    VIEW_SINK,
    VIEW_SINK_INPUT
} view_entry_type_t;

typedef struct {
    view_entry_type_t type;
    void * ref;
} view_entry_t;

void view_init();
void view_update();
void view_show();
void view_select_norm();
void view_select_next();
void view_select_prev();
view_entry_t view_selected_item();

#endif
