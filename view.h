/**
 * @brief Implements an orbital simulation view
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 */
#ifndef LIBS_H
#define LIBS_H

//shared libraries
#include "raylib.h"
#include "raymath.h"

#endif

#ifndef ORBITALSIMVIEW_H
#define ORBITALSIMVIEW_H

//Macros
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

//libraries
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

//specific libraries
#include <time.h>
#include "orbitalSim.h"

/**
 * The view data
 */
struct View
{
    Camera3D camera;
};

View *constructView(int fps);
void destroyView(View *view);

bool isViewRendering(View *view);
void renderView(View *view, OrbitalSim_t *sim);

#endif
