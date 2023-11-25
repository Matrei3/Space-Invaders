#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "glm.hpp"
#include <vector>
#include <chrono>
#include <thread>
#include "main.h"
//define window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float GLOBAL_ANIMATION_SPEED = 1.0f;
//define SDL Window related variables
SDL_Window* window = NULL;
SDL_Renderer* windowRenderer = NULL;
SDL_Event currentEvent;
bool quit = false;
int mouseX, mouseY;
Uint64 startTime, endTime, elapsedTime;
bool reload = 0;
struct Rectangle {
    glm::vec2 center;
    int width;
    int height;
    glm::vec2 direction;
    float speed = 1.5f;
    glm::vec3 color = glm::vec3(0, 0, 0);
    SDL_Rect getSDLRect() const {
        SDL_Rect rect;
        rect.x = center.x - width / 2.0f;
        rect.y = center.y - height / 2.0f;
        rect.w = width;
        rect.h = height;

        return rect;
    }
};
Rectangle rectangle,enemy;
std::vector<Rectangle> enemies;
std::vector<Rectangle> bullets;
bool end = false;
bool victory = false;
void initRectangle() {
    rectangle.center.x = WINDOW_WIDTH/2;
    rectangle.center.y = 700;
    rectangle.width = 100;
    rectangle.height = 25;
    rectangle.color = glm::vec3(0, 0, 0);
    //enemies.push_back(rectangle);
    enemy.color = glm::vec3(50, 205, 50);
    enemy.center.x = 100;
    enemy.center.y = 30;
    enemy.width = 100;
    enemy.height = 50;
    enemies.push_back(enemy);
}
void animate() {
    for (auto& enemy : enemies)
    {

        enemy.center += GLOBAL_ANIMATION_SPEED * elapsedTime * enemy.speed * enemy.direction;
    }

}
int direction = 1;
void drawRectangle() {
    SDL_SetRenderDrawColor(windowRenderer, rectangle.color.x, rectangle.color.y, rectangle.color.z, 255);
    SDL_Rect r = rectangle.getSDLRect();
    SDL_RenderFillRect(windowRenderer, &r);
    for (auto& enemy : enemies) {

        
        SDL_SetRenderDrawColor(windowRenderer, enemy.color.x, enemy.color.y, enemy.color.z, 255);
        if (enemy.center.x == 0)
            direction *= -1;
        else if (enemy.center.x == 1280)
            direction *= -1;
        if (!end)
            enemy.center.x += 0.5*direction;
       

        r = enemy.getSDLRect();
        SDL_RenderFillRect(windowRenderer, &r);
    }
    for (auto& bullet : bullets) {
        SDL_SetRenderDrawColor(windowRenderer, bullet.color.x, bullet.color.y, bullet.color.z, 255);
        bullet.center.y -= 0.5;
        SDL_Rect r = bullet.getSDLRect();
        for (auto& enemy : enemies) {
            if (bullet.center.x >= enemy.center.x - enemy.width / 2 and bullet.center.x <= enemy.center.x + enemy.width / 2 and bullet.center.y >= enemy.center.y - enemy.height / 2 and bullet.center.y <= enemy.center.y + enemy.height / 2)
            {
                enemies.pop_back();
                bullets.clear();
                rectangle.color = glm::vec3(255, 165, 0);
                end = true;
                victory = true;
                
            }
        }
        SDL_RenderFillRect(windowRenderer, &r);
    }
}
Uint32 callback(Uint32 interval, void* name) {

    for (auto& enemy : enemies)
    {   
        if(!end)
            enemy.center.y += enemy.width / 2;
        if (enemy.center.y>=675)
        {
            end = true;
            bullets.clear();
            rectangle.color = glm::vec3(255, 0, 0);
            victory = false;
        }
    }
    return 5000;
}
Uint32 callback2(Uint32 interval, void* name) {

    reload = 0;
    return 1000;
}


bool initWindow() {

    bool success = true;

    //Try to initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {

        std::cout << "SDL initialization failed" << std::endl;
        success = false;

    }
    else {

        //Try to create the window
        window = SDL_CreateWindow(
            "Hello SDL2 Window!",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN);

        if (window == NULL) {

            std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
            success = false;

        }
        else {

            // Create a renderer for the current window
            windowRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (windowRenderer == NULL) {

                std::cout << "Failed to create the renderer: " << SDL_GetError() << std::endl;
                success = false;

            }
            else {

                //Set background color
                SDL_SetRenderDrawColor(windowRenderer, 255, 255, 255, 255);

                //Apply background color
                SDL_RenderClear(windowRenderer);
            }
        }
    }

    return success;
}

void shoot() {
    glm::vec2 center;
    SDL_Rect rect = rectangle.getSDLRect();
    Rectangle bullet;
    center.x = rect.x + rectangle.width / 2.0f;
    center.y = rect.y + rectangle.height / 2.0f - 45;
    bullet.center = center;
    bullet.width = 10;
    bullet.height = 10;
    bullet.speed = 2 * rectangle.speed;
    bullet.color = glm::vec3(255, 99, 71);
    bullets.push_back(bullet);

}
void processMovement() {
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_RIGHT])
        if(rectangle.center.x<=WINDOW_WIDTH-50 and !end)
            rectangle.center.x += 0.5;
    if (state[SDL_SCANCODE_LEFT] and !end)
        if (rectangle.center.x >= 50)
            rectangle.center.x -= 0.5;
    if (state[SDL_SCANCODE_SPACE] and !reload and !end)
        shoot(),reload=1;
    if (state[SDL_SCANCODE_R] and end)
        enemies.clear(),initRectangle(),end=false,victory=false;

}


void processEvents() {

    //Check for events in queue
    SDL_PollEvent(&currentEvent);

    //User requests quit
    if (currentEvent.type == SDL_QUIT) {

        quit = true;
    }

    //Mouse event -> pressed button
    if (currentEvent.type == SDL_MOUSEBUTTONDOWN) {

        if (currentEvent.button.button == SDL_BUTTON_LEFT) {

            SDL_GetMouseState(&mouseX, &mouseY);
            std::cout << "Mouse left click => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
        }
        if (currentEvent.button.button == SDL_BUTTON_RIGHT) {

            SDL_GetMouseState(&mouseX, &mouseY);
            std::cout << "Mouse right click => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
        }
    }

    //Mouse event -> mouse movement
    if (currentEvent.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&mouseX, &mouseY);
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK) {

            SDL_GetMouseState(&mouseX, &mouseY);
            std::cout << "Mouse move while left clicking => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
        }
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK) {

            SDL_GetMouseState(&mouseX, &mouseY);
            std::cout << "Mouse move while right clicking => " << "x: " << mouseX << ", y: " << mouseY << std::endl;
        }
    }

    //Keyboard event
    if (currentEvent.type == SDL_KEYDOWN) {

        switch (currentEvent.key.keysym.sym) {

        case SDLK_UP:
            break;

        case SDLK_DOWN:
            break;

        case SDLK_LEFT:
            break;

        case SDLK_RIGHT:
            break;

        case SDLK_r:
            break;

        case SDLK_s:
            break;

        case SDLK_ESCAPE:

            quit = true;
            break;

        default:
            break;
        }
    }
}

void drawFrame() {

    //Clear the background
    SDL_SetRenderDrawColor(windowRenderer, 255, 255, 255, 255);
    SDL_RenderClear(windowRenderer);
    //Draw scene
    //TODO
    drawRectangle();
    //Update window
    SDL_RenderPresent(windowRenderer);
}

void cleanup() {

    //Destroy renderer
    if (windowRenderer) {

        SDL_DestroyRenderer(windowRenderer);
        windowRenderer = NULL;
    }

    //Destroy window
    if (window) {

        SDL_DestroyWindow(window);
        window = NULL;
    }

    //Quit SDL

    SDL_Quit();
}

int main(int argc, char* argv[]) {

    //Initialize window
    if (!initWindow()) {

        std::cout << "Failed to initialize" << std::endl;
        return -1;
    }

    //Game loop

    initRectangle();
    SDL_TimerID timerID = SDL_AddTimer(5000, callback, const_cast<char*>("SDL"));
    SDL_TimerID timerID2 = SDL_AddTimer(0, callback2, const_cast<char*>("SDL"));
    while (!quit) {
        startTime = SDL_GetTicks64();
        processMovement();
        processEvents();
        drawFrame();
       // animate();
        endTime = SDL_GetTicks64();
        elapsedTime = endTime - startTime;
    }
    SDL_RemoveTimer(timerID);
    SDL_RemoveTimer(timerID2);
    cleanup();
    return 0;
}
