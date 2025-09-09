/**
 * @brief Orbital simulation
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 */
#ifndef LIBS_H
#define LIBS_H

//shared libraries
#include "raylib.h"

#endif

#ifndef ORBITALSIM_H
#define ORBITALSIM_H

//specific libraries
#include "raymath.h"
#include <cmath>

//Macros
#define _USE_MATH_DEFINES // Habilita M_PI #define en Windows
#define GRAVITATIONAL_CONSTANT 6.6743E-11F
#define ASTEROIDS_MEAN_RADIUS 4E11F
#define NUM_ASTEROIDS 500
#define MIN_DISTANCE 1000


/**
 * @brief Orbital body definition
 */

using presicion = double; // cambia el tipo de variable de los paraemtros que modifican a la fisica del programa

typedef struct 
{
    Vector3 pos;
    Vector3 vel;
    Vector3 acc;
    presicion mass;
    presicion radius;
    Color color;

} OrbitalBody_t;

/**
 * @brief Orbital simulation definition
 */
typedef struct 
{
    presicion timeStep;
    presicion elapsedTime;
    int numBodies;
    OrbitalBody_t * bodies;

}OrbitalSim_t;



OrbitalSim_t *constructOrbitalSim(float timeStep);
void destroyOrbitalSim(OrbitalSim_t *sim);

void updateOrbitalSim(OrbitalSim_t *sim);

#endif
