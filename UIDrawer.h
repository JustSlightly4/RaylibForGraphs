/*
 * Eric Ryan Montgomery
 * 11/24/2025
 * For CardGameUI
 * UIDrawer Class to replace functional functions
 */

#ifndef UIDRAWERH
#define UIDRAWERH
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <sstream>
#include <functional>
#include <iomanip>
#include <cmath>
#include "raylib.h"
#include "Buttons.h"

 class UIDrawer {
    public:
      //Mutable
      Vector2 screenDimensions;
      float maxScroll;
      float widthSegment;
      float heightSegment;
      float scrollOffset;
      Vector2 mousePoint = GetMousePosition();

      //Font
      Font currentFont;
      int currentFontSize;

      // Constant bases
      static constexpr Vector2 origin = {0, 0};
      static constexpr Rectangle buttonSource = {0, 0, 200, 100};
      static constexpr int heightBlocks = 16;
      static constexpr int widthBlocks = 32;
      static constexpr Color textColor = BLACK;
      static constexpr float scrollSpeed = 50.0f;

      //Alignemnt Variables
      enum AlignmentX {
         CENTERX = 0,
         RIGHTX = 1,
         LEFTX = 2,
      };

      enum AlignmentY {
         CENTERY = 0,
         DOWNY = 1,
         UPY = 2,
      };

      typedef struct ALIGNMENT {
         AlignmentX x;
         AlignmentY y;
         bool reduceTextSize = true;
      } Alignment;

      //Create/Update/Destory Functions
      UIDrawer();
      void Update();
      void UpdateButtons(SingleButtonGroup &buttons);
      void SetBackgroundColor(Color color);

      //Base Drawing Functions
      void DrawGrid();
      void DrawGridDots();
      void DrawFPSOnGrid();
      Rectangle CoordsToRec(Vector2 startCoords, Vector2 endCoords);
      void scrollLogic();
      void DrawTextureOnGrid(Texture2D &texture, Rectangle source, Vector2 startCoords, Vector2 endCoords, Color tint);
      void DrawRectangleOnGrid(Vector2 startCoords, Vector2 endCoords, Color tint);
      void DrawRectangleLinesOnGrid(Vector2 startCoords, Vector2 endCoords, Color tint, int lineThickness);
      void DrawTextS(std::string text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness = 0);
      void DrawTextSOnGrid(std::string text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness = 0);
      float DrawTextSWrapped(std::string text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness = 0);
      float DrawTextSWrappedOnGrid(std::string text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness = 0);

      //Drawing Button Functions
      void DrawButtonOnGrid(SingleButtonGroup &buttons, int index, Vector2 startCoords, Vector2 endCoords);
      void DrawButtonRowOnGrid(SingleButtonGroup &buttons, Vector2 startCoords, Vector2 endCoords);

 };
 #endif