#include "options.h"
#include <stdlib.h>
#include <string.h>

static menu_t menu;

void options_init() {
    // Null out everything
    memset(&menu, 0, sizeof(menu_t));

    //********************************************
    //************ Tracking generator ************
    //********************************************

    category_t *tgen = option_add_category("T. GEN");

    selection_t tgen_modes[] = {
        { "FIXED", OPTION_ID_TGEN_MODE_FIXED },
        { "TRACK", OPTION_ID_TGEN_MODE_TRACK },
        { "OFF", OPTION_ID_TGEN_MODE_OFF }
    };

    option_add_selection(tgen, OPTION_ID_TGEN_MODE, "Mode", tgen_modes, sizeof(tgen_modes)/sizeof(selection_t), OPTION_ID_TGEN_MODE_OFF);
    option_enable(OPTION_ID_TGEN_MODE);

    option_add_frequency(tgen, OPTION_ID_TGEN_FREQ, "Fix freq.", 100e6);
    option_disable(OPTION_ID_TGEN_FREQ);

    option_add_frequency(tgen, OPTION_ID_TGEN_ADJ, "Adj freq.", 10e3);
    option_disable(OPTION_ID_TGEN_ADJ);

    //********************************************
    //************ View category  ****************
    //********************************************

    category_t *view = option_add_category("VIEW");

    selection_t view_fdisp_modes[] = {
        { "CENTER & SPAN", OPTION_ID_VIEW_FDISP_CENTERSPAN },
        { "START & STOP", OPTION_ID_VIEW_FDISP_STARTSTOP },
    };

    option_add_selection(view, OPTION_ID_VIEW_FDISP, "Freq. Display", view_fdisp_modes, sizeof(view_fdisp_modes)/sizeof(selection_t), OPTION_ID_VIEW_FDISP_STARTSTOP);
    option_enable(OPTION_ID_VIEW_FDISP);

    selection_t view_waterfall_modes[] = {
        { "ON", OPTION_ID_VIEW_WATERFALL_ON },
        { "OFF", OPTION_ID_VIEW_WATERFALL_OFF },
    };

    option_add_selection(view, OPTION_ID_VIEW_WATERFALL, "Waterfall", view_waterfall_modes, sizeof(view_waterfall_modes)/sizeof(selection_t), OPTION_ID_VIEW_WATERFALL_OFF);
    option_enable(OPTION_ID_VIEW_WATERFALL);
}

void options_refresh() {
    // Only enable TG frequency if the TG mode is fixed
    if(option_get_selection(OPTION_ID_TGEN_MODE) != OPTION_ID_TGEN_MODE_FIXED) {
        option_disable(OPTION_ID_TGEN_FREQ);
    } else {
        option_enable(OPTION_ID_TGEN_FREQ);
    }

    // Only enable TG adjust if the TG mode is track
    if(option_get_selection(OPTION_ID_TGEN_MODE) != OPTION_ID_TGEN_MODE_TRACK) {
        option_disable(OPTION_ID_TGEN_ADJ);
    } else {
        option_enable(OPTION_ID_TGEN_ADJ);
    }
}


category_t *option_add_category(char *name_v) {

    if( menu.nCategories + 1 == MAX_CATEGORIES ) {
        return 0;
    }

    menu.categories[menu.nCategories].name = name_v;

    ++menu.nCategories;

    return &menu.categories[menu.nCategories-1];
}

menu_t *get_option_menu() {
    return &menu;
}

void option_add_frequency(category_t *target, int id, char *name, double init) {
    option_t *o = &target->options[target->nOptions];
    ++target->nOptions;

    o->name = name;
    o->id = id;
    o->type = OPTION_TYPE_FREQUENCY;
    o->data = malloc(sizeof(double));

    double *d = o->data;
    *d = init;
}

void option_add_selection(category_t *target, int id, char *name, selection_t *values, int nValues, int init) {
    option_t *o = &target->options[target->nOptions];
    ++target->nOptions;

    o->name = name;
    o->id = id;
    o->type = OPTION_TYPE_SELECTION;
    o->data = malloc(sizeof(selection_option_t));

    selection_option_t *s = o->data;
    s->value = init;
    s->nSelections = nValues;
    memcpy(s->selections, values, nValues * sizeof(selection_t));
}

option_t *option_get_generic(int id) {
    for( int i = 0; i != menu.nCategories; ++i ) {
        for( int j = 0; j != menu.categories[i].nOptions; ++j ) {
            option_t *this_option = &menu.categories[i].options[j];
            if( this_option->id == id )
                return this_option;
        }
    }

    return 0;
}

void option_enable(int id) {
    option_t *o = option_get_generic(id);
    o->enable = 1;
}

void option_disable(int id) {
    option_t *o = option_get_generic(id);
    o->enable = 0;
}

double option_get_frequency(int id) {
    option_t *o = option_get_generic(id);
    return *(double*)o->data;
}

int option_get_selection(int id) {
    option_t *o = option_get_generic(id);
    selection_option_t *so = o->data;
    return so->value;
}
