/**
 * @brief Implements an orbital simulation view
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2022-2023
 */

#include "view.h"


/**
 * @brief Converts a timestamp (number of seconds since 1/1/2022)
 *        to an ISO date ("YYYY-MM-DD")
 *
 * @param timestamp the timestamp
 * @return The ISO date (a raylib string)
 */
const char *getISODate(float timestamp)
{
    // Timestamp epoch: 1/1/2022
    struct tm unichEpochTM = {0, 0, 0, 1, 0, 122};

    // Convert timestamp to UNIX timestamp (number of seconds since 1/1/1970)
    time_t unixEpoch = mktime(&unichEpochTM);
    time_t unixTimestamp = unixEpoch + (time_t)timestamp;

    // Returns ISO date
    struct tm *localTM = localtime(&unixTimestamp);
    return TextFormat("%04d-%02d-%02d",
                      1900 + localTM->tm_year, localTM->tm_mon + 1, localTM->tm_mday);
}

/**
 * @brief Constructs an orbital simulation view
 *
 * @param fps Frames per second for the view
 * @return The view
 */
View *constructView(int fps)
{
    View *view = new View();

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "EDA Orbital Simulation");
    SetTargetFPS(fps);
    DisableCursor();

    view->camera.position = {10.0f, 10.0f, 10.0f};
    view->camera.target = {0.0f, 0.0f, 0.0f};
    view->camera.up = {0.0f, 1.0f, 0.0f};
    view->camera.fovy = 45.0f;
    view->camera.projection = CAMERA_PERSPECTIVE;

    return view;
}

/**
 * @brief Destroys an orbital simulation view
 *
 * @param view The view
 */
void destroyView(View *view)
{
    CloseWindow();

    delete view;
}

/**
 * @brief Should the view still render?
 *
 * @return Should rendering continue?
 */
bool isViewRendering(View *view)
{
    return !WindowShouldClose();
}

/**
 * Renders an orbital simulation
 *
 * @param view The view
 * @param sim The orbital sim
 */
void renderView(View *view, OrbitalSim_t *sim)
{
    const float SCALE_FACTOR = 1e-11f;      // 1×10⁻¹¹ metros
    UpdateCamera(&view->camera, CAMERA_FREE);

    BeginDrawing();

    ClearBackground(BLACK);
    BeginMode3D(view->camera);

    // Fill in your 3D drawing code here:
    
    for (int i = 0; i < (sim->numBodies - NUM_ASTEROIDS); i++)
    {
        Vector3 scaledPos = Vector3Scale(sim->bodies[i].pos, SCALE_FACTOR);    
        float scaledRadius = 0.005f * logf(sim->bodies[i].radius);
        DrawSphere(scaledPos, scaledRadius, sim->bodies[i].color); 
    }
    for (int i = (sim->numBodies - NUM_ASTEROIDS); i < sim->numBodies; i++){
        Vector3 scaledPos = Vector3Scale(sim->bodies[i].pos, SCALE_FACTOR);    
        DrawPoint3D(scaledPos, sim->bodies[i].color);
    }



    DrawGrid(10, 10.0f);
    EndMode3D();

    // 2D drawing code here
    DrawFPS(10, 10);
    
    if (sim) {
        DrawText(getISODate(sim->elapsedTime), 10, 40, 20, RAYWHITE);
        
        // Show simulation info
        DrawText(TextFormat("Bodies: %d", sim->numBodies), 10, 70, 20, WHITE);
        DrawText(TextFormat("Time Step: %.2e s", sim->timeStep), 10, 100, 20, WHITE);

        // === DISTANCIA DE LA CÁMARA AL SOL =====

        if (sim->numBodies > 0) {
            
            Vector3 sunPosReal = sim->bodies[0].pos;                                        // Posición real del Sol (metros)
            Vector3 sunPosScaled = Vector3Scale(sunPosReal, SCALE_FACTOR);                  // Posición escalada del Sol (para visualización)
            float distanceScaled = Vector3Distance(view->camera.position, sunPosScaled);    // Distancia en coordenadas escaladas
            float distanceReal = distanceScaled / SCALE_FACTOR;                             // Distancia real = distancia escalada / factor de escala
            float distanceAU = distanceReal / 1.496e11f;                                    // Convertir a Unidades Astronómicas (1 AU = 1.496e11 m)
            
            // Mostrar las distancias
            DrawText(TextFormat("Distancia Camara-Sol: %.2e m = %.2f AU", distanceReal, distanceAU), 10, 130, 18, WHITE);
            
    }

    EndDrawing();
    }
}
