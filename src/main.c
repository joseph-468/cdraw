#include <raylib.h>

int main() {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "My Raylib Window");

    // Main game loop
    while (!WindowShouldClose()) {
        // Update

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello, Raylib!", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();
	return 0;
}