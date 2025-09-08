/**
 * @brief Orbital simulation
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 */

#include "orbitalSim.h"
#include "ephemerides.h"
#define SYSTEM_TOGGLE        // Si está definido, se simulará el sistema solar. En caso de estarlo, se simulará Alfa Centauri

/**
 * @brief Gets a uniform random value in a range
 *
 * @param min Minimum value
 * @param max Maximum value
 * @return The random value
 */
presicion getRandomPresicion(presicion min, presicion max)
{
    return min + (max - min) * rand() / presicion{RAND_MAX};
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
    presicion x = getRandomPresicion(presicion{0.001}, presicion{0.999}); //Evito log(0)
    presicion l = std::log(x) - std::log(presicion{1} - x) + presicion{1};
    presicion r = ASTEROIDS_MEAN_RADIUS * std::sqrt(std::abs(l));
    presicion phi = getRandomPresicion(presicion{0}, presicion{2.0} * presicion{M_PI});

    // Surprise!
    // phi = 0; // Fijar phi en 0 produce un patrón en espiral similar a Fibonacci !! :o

    presicion v  = std::sqrt(GRAVITATIONAL_CONSTANT * centerMass / r)
             * getRandomPresicion(presicion{0.6}, presicion{1.2});
    presicion vy = getRandomPresicion(presicion{-1E2}, presicion{1E2});

    body->mass = presicion{1E12};  // Typical asteroid weight: 1 billion tons
    body->radius = presicion{2E3}; // Typical asteroid radius: 2km
    body->color = GRAY;
    
    //Casteo a float por que rylib usa float para el render
    body->pos = Vector3{static_cast<float>(r * std::cos(phi)),0.0f
                , static_cast<float>(r * std::sin(phi))};
    body->vel = Vector3{ static_cast<float>(-v * std::sin(phi)), static_cast<float>(vy),
                static_cast<float>( v * std::cos(phi))};
    body->acc = (Vector3){0.0f, 0.0f, 0.0f};
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
    sim->elapsedTime = presicion{0.0};

    #ifdef SYSTEM_TOGGLE
    
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
        sim->bodies[i].acc = (Vector3){0.0f, 0.0f, 0.0f};
        sim->bodies[i].mass = solarSystem[i].mass;
        sim->bodies[i].radius = solarSystem[i].radius;
        sim->bodies[i].color = solarSystem[i].color;
    }

    // Configuro los asteroides
    float centerMass = solarSystem[0].mass; // pongo el Sol como centro
    for (int i = SOLARSYSTEM_BODYNUM; i < sim->numBodies; i++) {
        configureAsteroid(&sim->bodies[i], centerMass);
    }

    #else

    sim->numBodies = ALPHACENTAURISYSTEM_BODYNUM + NUM_ASTEROIDS;
    sim->bodies = (OrbitalBody_t *)malloc(sim->numBodies * sizeof(OrbitalBody_t));
    if (!sim->bodies) {
        free(sim);
        return NULL;
    }

    // Inicializo el sistema Alpha Centauri
    for (int i = 0; i < ALPHACENTAURISYSTEM_BODYNUM; i++) {
        sim->bodies[i].pos = alphaCentauriSystem[i].position;
        sim->bodies[i].vel = alphaCentauriSystem[i].velocity;
        sim->bodies[i].acc = Vector3{0.0f, 0.0f, 0.0f};
        sim->bodies[i].mass = alphaCentauriSystem[i].mass;
        sim->bodies[i].radius = alphaCentauriSystem[i].radius;
        sim->bodies[i].color = alphaCentauriSystem[i].color;
    }

    // Configuro los asteroides
    float centerMass = alphaCentauriSystem[0].mass; // pongo el Sol como centro
    for (int i = ALPHACENTAURISYSTEM_BODYNUM; i < sim->numBodies; i++) {
    configureAsteroid(&sim->bodies[i], centerMass);
    }

    #endif
    
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
    presicion distance = Vector3Length(r); 
    
    // Evito dividir por cero
    if (distance < MIN_DISTANCE) { 
        return (Vector3){0, 0, 0}; 
    } 
    
    presicion invDistance3 = presicion{1.0} / (distance * distance * distance); 
    presicion forceFactor = GRAVITATIONAL_CONSTANT * body2->mass * invDistance3;
    
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

    // Calculo de aceleraciones de los cuerpos
    for (int i = 0; i < sim->numBodies; i++) {
        Vector3 totalAccel = {0, 0, 0};

        for (int j = 0; j < (sim->numBodies -NUM_ASTEROIDS); j++) {
            if (i != j) {
                Vector3 accel = calculateGravitationalForce(&sim->bodies[i], &sim->bodies[j]);
                totalAccel = Vector3Add(totalAccel, accel);
            }
        } 
        
        sim->bodies[i].acc = totalAccel; // a = F/m
    }

    // Update de velocidades y posiciones de forma discreta
    for (int i = 0; i < sim->numBodies; i++) {

        //velocidad: v(n+1) = v(n) + a(n) * dt
        Vector3 deltaV = Vector3Scale(sim->bodies[i].acc, sim->timeStep);
        sim->bodies[i].vel = Vector3Add(sim->bodies[i].vel, deltaV);
        
        //posicion: x(n+1) = x(n) + v(n+1) * dt
        Vector3 deltaX = Vector3Scale(sim->bodies[i].vel, sim->timeStep);
        sim->bodies[i].pos = Vector3Add(sim->bodies[i].pos, deltaX);
    }

    // Update elapsed time
    sim->elapsedTime += sim->timeStep;
}
