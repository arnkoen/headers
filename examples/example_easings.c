#include "../easings.h"
#include <stdio.h>

typedef struct {
    const char* name;
    float (*fn)(float);
} easing_fn;

int main(void) {
    easing_fn easings[] = {
        {"linear", ease_linear},

        {"in_quad", ease_in_quad},
        {"out_quad", ease_out_quad},
        {"in_out_quad", ease_in_out_quad},

        {"in_cubic", ease_in_cubic},
        {"out_cubic", ease_out_cubic},
        {"in_out_cubic", ease_in_out_cubic},

        {"in_quart", ease_in_quart},
        {"out_quart", ease_out_quart},
        {"in_out_quart", ease_in_out_quart},

        {"in_quint", ease_in_quint},
        {"out_quint", ease_out_quint},
        {"in_out_quint", ease_in_out_quint},

        {"in_sine", ease_in_sine},
        {"out_sine", ease_out_sine},
        {"in_out_sine", ease_in_out_sine},

        {"in_expo", ease_in_expo},
        {"out_expo", ease_out_expo},
        {"in_out_expo", ease_in_out_expo},

        {"in_circ", ease_in_circ},
        {"out_circ", ease_out_circ},
        {"in_out_circ", ease_in_out_circ},

        {"in_back", ease_in_back},
        {"out_back", ease_out_back},
        {"in_out_back", ease_in_out_back},

        {"in_elastic", ease_in_elastic},
        {"out_elastic", ease_out_elastic},
        {"in_out_elastic", ease_in_out_elastic},

        {"in_bounce", ease_in_bounce},
        {"out_bounce", ease_out_bounce},
        {"in_out_bounce", ease_in_out_bounce},
    };

    int num_easings = sizeof(easings) / sizeof(easings[0]);

    for (int i = 0; i < num_easings; ++i) {
        printf("Easing: %s\n", easings[i].name);
        printf("   t    value\n");
        printf("---------------\n");
        for (float t = 0.0f; t <= 1.001f; t += 0.1f) {
            float y = easings[i].fn(t);
            printf("%5.2f  %7.4f\n", t, y);
        }
        printf("\n");
    }
    return 0;
}
