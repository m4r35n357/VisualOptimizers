#ifndef COST_H
#define COST_H

#pragma once

#include "point.h"

//-----------------------------------------------------------------------------
// Cost function interface
//-----------------------------------------------------------------------------

typedef void (*fun_t)(int, point *, const model *);

#endif // COST_H
