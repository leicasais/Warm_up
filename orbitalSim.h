/**
 * @brief Orbital simulation
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 */

#ifndef ORBITALSIM_H
#define ORBITALSIM_H

#include "raylib.h"
#include "raymath.h"

/**
 * @brief Orbital body definition
 */
typedef struct 
{
    Vector3 pos;
    Vector3 vel;
    Vector3 acc;
    float mass;
    float radius;
    Color color;

} OrbitalBody_t;

/**
 * @brief Orbital simulation definition
 */
typedef struct 
{
    float timeStep;
    float elapsedTime;
    int numBodies;
    OrbitalBody_t * bodies;

}OrbitalSim_t;



OrbitalSim_t *constructOrbitalSim(float timeStep);
void destroyOrbitalSim(OrbitalSim_t *sim);

void updateOrbitalSim(OrbitalSim_t *sim);

#endif
