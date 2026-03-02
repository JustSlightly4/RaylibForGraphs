//Manual Instructions:
//***FIRST YOU NEED TO COMPILE RAYLIB WITH: make -C raylib/src PLATFORM=PLATFORM_DESKTOP
//**IF YOU MESS IT UP OR SWITCH PLATFORMS USE: make -C raylib/src clean
//To Compile on Windows: g++ -Wall -std=c++17 -I./raylib/src UIDrawer.cpp Buttons.cpp main.cpp -o main.exe ./raylib/src/libraylib.a -lopengl32 -lgdi32 -lwinmm
//To Compile on Linux: g++ -Wall -std=c++17 -I./raylib/src UIDrawer.cpp Buttons.cpp main.cpp -o main ./raylib/src/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11

//Make Instructions:
//To build raylib and compile main: make
//To delete main: make clean
//To delete main and the compiled raylib library: make clean-all
//#include <GLFW/glfw3.h>

//Web Instructions
//To Compile raylib library: make -C raylib/src PLATFORM=PLATFORM_WEB -B
//To Compile project to WebAssembly:  em++ -Wall -std=c++17 -I./raylib/src UIDrawer.cpp Buttons.cpp main.cpp -o index.html ./raylib/src/libraylib.web.a -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --shell-file ./raylib/src/shell.html -s FORCE_FILESYSTEM=1 --preload-file textures@/textures --preload-file fonts@/fonts
//To Run the WebAssembly project locally: emrun --port 8080 index.html
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

    SetTargetFPS(0);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    {
        //Textures
        shared_ptr<Texture2D> buttonTexture = make_shared<Texture2D>(LoadTexture("textures/button.png"));

        //Variables
        int variable = 0;
        UIDrawer drawer;
        string myText = "This is my graph visualization project! 0";
        SingleButtonGroup buttons(buttonTexture);
            buttons.AddButton("Increase", [&variable, &myText](SingleButton &btn){
                ++variable;
                myText = "This is my graph visualization project! " + to_string(variable);
            });
            buttons.AddButton("Subtract", [&variable, &myText](SingleButton &btn){
                --variable;
                myText = "This is my graph visualization project! " + to_string(variable);
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

                drawer.SetBackgroundColor(RAYWHITE); 

                //The draw static textures
                drawer.DrawStaticTextures(drawer.staticTextures, drawer.updateStaticContent, [&](){
                    drawer.DrawGrid();
                    drawer.DrawRectangleLinesOnGrid({drawer.widthBlocks/2-4, drawer.heightBlocks/2-2}, {drawer.widthBlocks/2+4, drawer.heightBlocks/2+2}, BLACK, 4);
                });

                drawer.DrawFPSOnGrid();

                drawer.DrawTextSWrappedOnGridCached(myText, {drawer.widthBlocks/2-4, drawer.heightBlocks/2-2}, {drawer.widthBlocks/2+4, drawer.heightBlocks/2+2}, 
                    {UIDrawer::CENTERX, UIDrawer::CENTERY}, 4);
                
                drawer.DrawButtonRowOnGridCached(buttons, {0, drawer.heightBlocks-2}, {8, drawer.heightBlocks});

            EndDrawing();
            //----------------------------------------------------------------------------------
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}