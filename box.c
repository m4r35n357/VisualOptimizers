
#include <stdlib.h>
#include "model.h"

struct Model { real area, min_edge; };

model *model_init () {
    model *m = malloc(sizeof (model));
    m->area = 48.0L; // equality constraint
    m->min_edge = 0.01L;  // inequality constraint
    return m;
}

minima *get_known_minima () {
    return NULL;
}

void cost (int n, point *p, const model *m) { (void)n;
	p->x[0] = p->x[0] >= m->min_edge ? p->x[0] : m->min_edge;
	p->x[1] = p->x[1] >= m->min_edge ? p->x[1] : m->min_edge;
	p->f = m->area / (p->x[0] * p->x[1] * (m->area - 2.0L * p->x[0] * p->x[1]) / (2.0L * (p->x[0] + p->x[1])));
}
