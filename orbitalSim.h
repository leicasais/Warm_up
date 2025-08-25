/**
 * @brief Orbital simulation
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 */

#ifndef ORBITALSIM_H
#define ORBITALSIM_H

#include "raymath.h"

#define NUM_BODIES 10
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
    OrbitalBody_t (* arr_OrbBodies)[NUM_BODIES]

}OrbitalSim_t;

OrbitalSim *constructOrbitalSim(float timeStep);
void destroyOrbitalSim(OrbitalSim *sim);

void updateOrbitalSim(OrbitalSim *sim);

#endif
