/**
 * @brief Orbital simulation
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 */


#define _USE_MATH_DEFINES // Habilita M_PI #define en Windows

#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"

#include "orbitalSim.h"
#include "ephemerides.h"

#define GRAVITATIONAL_CONSTANT 6.6743E-11F
#define ASTEROIDS_MEAN_RADIUS 4E11F
#define NUM_ASTEROIDS 500
#define MIN_DISTANCE 1000

/**
 * @brief Gets a uniform random value in a range
 *
 * @param min Minimum value
 * @param max Maximum value
 * @return The random value
 */
float getRandomFloat(float min, float max)
{
    return min + (max - min) * rand() / (float)RAND_MAX;
}

/**
 * @brief Configures an asteroid
 *
 * @param body An orbital body
 * @param centerMass The mass of the most massive object in the star system
 */
void configureAsteroid(OrbitalBody_t *body, float centerMass)
{
    // Logit distribution
    float x = getRandomFloat(0.001f, 0.999f); //Evito log(0)
    float l = logf(x) - logf(1 - x) + 1;

    // https://mathworld.wolfram.com/DiskPointPicking.html
    float r = ASTEROIDS_MEAN_RADIUS * sqrtf(fabsf(l));
    float phi = getRandomFloat(0, 2.0F * (float)M_PI);

    // Surprise!
    // phi = 0; // Fijar phi en 0 produce un patrón en espiral similar a Fibonacci !! :o

    // https://en.wikipedia.org/wiki/Circular_orbit#Velocity
    float v = sqrtf(GRAVITATIONAL_CONSTANT * centerMass / r) * getRandomFloat(0.6F, 1.2F);
    float vy = getRandomFloat(-1E2F, 1E2F);

    // Fill in with your own fields:
    body->mass = 1E12F;  // Typical asteroid weight: 1 billion tons
    body->radius = 2E3F; // Typical asteroid radius: 2km
    body->color = GRAY;
    body->pos = (Vector3){r * cosf(phi), 0, r * sinf(phi)};
    body->vel = (Vector3){-v * sinf(phi), vy, v * cosf(phi)};
    body->acc = (Vector3){0, 0, 0};
}

/**
 * @brief Constructs an orbital simulation
 *
 * @param float The time step
 * @return The orbital simulation
 */




OrbitalSim_t *constructOrbitalSim(float timeStep)
{
    OrbitalSim_t *sim = (OrbitalSim_t *)malloc(sizeof(OrbitalSim_t));
    if (!sim) return NULL;
    
    sim->timeStep = timeStep;
    sim->elapsedTime = 0.0f;
    sim->numBodies = SOLARSYSTEM_BODYNUM + NUM_ASTEROIDS;
    sim->bodies = (OrbitalBody_t *)malloc(sim->numBodies * sizeof(OrbitalBody_t)); //reservo memoria para los orbital bodies
    if (!sim->bodies) {
        free(sim);
        return NULL;
    }

    //Inicializo el sistema solar
    for (int i = 0; i < SOLARSYSTEM_BODYNUM; i++) {
        sim->bodies[i].pos = solarSystem[i].position;
        sim->bodies[i].vel = solarSystem[i].velocity;
        sim->bodies[i].acc = (Vector3){0, 0, 0};
        sim->bodies[i].mass = solarSystem[i].mass;
        sim->bodies[i].radius = solarSystem[i].radius;
        sim->bodies[i].color = solarSystem[i].color;
    }

    // Configuro los asteroides
    float centerMass = solarSystem[0].mass; // pongo el Sol como centro
    for (int i = SOLARSYSTEM_BODYNUM; i < sim->numBodies; i++) {
        configureAsteroid(&sim->bodies[i], centerMass);
    }

    return sim; // This should return your orbital sim
}

/**
 * @brief Destroys an orbital simulation
 */
void destroyOrbitalSim(OrbitalSim_t *sim)
{
    if (sim) {
        if (sim->bodies) {
            free(sim->bodies);
        }
        free(sim);
    }
}


/**
 * @brief Calculate gravitational force between two bodies
 */

Vector3 calculateGravitationalForce(const OrbitalBody_t *body1, const OrbitalBody_t *body2) 
{ 
    // Calculo fuerza/m1 para evitar operaciones innecesarias 

    Vector3 r = Vector3Subtract(body2->pos, body1->pos); 
    float distance = Vector3Length(r); 
    
    // Evito dividir por cero
    if (distance < MIN_DISTANCE) { 
        return (Vector3){0, 0, 0}; 
    } 
    
    float invDistance3 = 1.0f / (distance * distance * distance); // 1 / r^3 
    float forceFactor = GRAVITATIONAL_CONSTANT * body2->mass * invDistance3; // 
    
    return Vector3Scale(r, forceFactor); // Fuerza / m1 = G*m2 / r^3 
}

/**
 * @brief Simulates a timestep
 *
 * @param sim The orbital simulation
 */
void updateOrbitalSim(OrbitalSim_t *sim)
{
    if (!sim || !sim->bodies) return;

    //1. Calculo de aceleraciones de los cuerpos
    for (int i = 0; i < sim->numBodies; i++) {
        Vector3 totalAccel = {0, 0, 0};

        for (int j = 0; j < sim->numBodies; j++) {
            if (i != j) {
                Vector3 accel = calculateGravitationalForce(&sim->bodies[i], &sim->bodies[j]);
                totalAccel = Vector3Add(totalAccel, accel);
            }
        } 
        
        sim->bodies[i].acc = totalAccel; // a = F/m
    }

    //2. Update de velocidades y posiciones de forma discreta
    for (int i = 0; i < sim->numBodies; i++) {

        // Update velocidad: v(n+1) = v(n) + a(n) * dt
        Vector3 deltaV = Vector3Scale(sim->bodies[i].acc, sim->timeStep);
        sim->bodies[i].vel = Vector3Add(sim->bodies[i].vel, deltaV);
        
        // Update posicion: x(n+1) = x(n) + v(n+1) * dt
        Vector3 deltaX = Vector3Scale(sim->bodies[i].vel, sim->timeStep);
        sim->bodies[i].pos = Vector3Add(sim->bodies[i].pos, deltaX);
    }

    // Update elapsed time
    sim->elapsedTime += sim->timeStep;
}
