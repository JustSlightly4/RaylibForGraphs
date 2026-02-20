//To Compile on Windows: g++ -Wall -std=c++17 -I./WindowsLibrary UIDrawer.cpp Buttons.cpp main.cpp -o main.exe ./WindowsLibrary/libraylib.a -lopengl32 -lgdi32 -lwinmm
//To Compile on Linux: g++ -Wall -std=c++17 -I./LinuxLibrary UIDrawer.cpp Buttons.cpp main.cpp -o main ./LinuxLibrary/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
#include <memory>
#include "raylib.h"
#include "UIDrawer.h"
using namespace std;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Graph Visualization Project");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    //Textures
    shared_ptr<Texture2D> buttonTexture = make_shared<Texture2D>(LoadTexture("textures/button.png"));

    //Variables
    UIDrawer drawer;
    SingleButtonGroup buttons(buttonTexture);
        buttons.AddButton("Rules", [&](SingleButton &btn){
			//Do nothing
		});
        buttons.AddButton("Settings", [&](SingleButton &btn){
			//Do nothing
		});

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update Variables
        drawer.Update();
        drawer.UpdateButtons(buttons);


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            //Draw the background, FPS, and grid
            drawer.SetBackgroundColor(RAYWHITE); 
            drawer.DrawGrid();
            drawer.DrawFPSOnGrid();

            //Draw Text in the middle of the screen and the buttons at the bottom
            drawer.DrawTextSWrappedOnGrid("This is my graph visualization project!", {drawer.widthBlocks/2-4, drawer.heightBlocks/2-2}, {drawer.widthBlocks/2+4, drawer.heightBlocks/2+2}, 
                {UIDrawer::CENTERX, UIDrawer::CENTERY});
            drawer.DrawButtonRowOnGrid(buttons, {0, drawer.heightBlocks-2}, {8, drawer.heightBlocks});

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}