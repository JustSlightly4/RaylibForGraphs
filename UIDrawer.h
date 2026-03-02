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
#include <functional>
#include <iomanip>
#include <cmath>
#include <string_view>
#include <unordered_map>
#include "raylib.h"
#include "Buttons.h"

class UIDrawer {
   public:
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

      //Class that implements a delayed flag
      //Flag is trigger but only activates after a delay
      class DebounceFlag {
         private:
         bool flag;
         float timer;
         float delaySeconds;

         public:
         DebounceFlag();
         void Trigger();
         void Update();
         bool IsReady();
         void Reset();

      };

      std::unordered_map<uint64_t, std::pair<RenderTexture2D, int>> textCache;
      std::unordered_map<int, RenderTexture2D> buttonCache;
      DebounceFlag resetCache;

      //Mutable
      Vector2 screenDimensions;
      Vector2 prevScreenDimensions;
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

      //Textures and flags for textures
      RenderTexture2D staticTextures;
      DebounceFlag updateStaticContent;

      //Create/Update/Destory Functions
      UIDrawer();
      void Update();
      void UpdateButtons(SingleButtonGroup &buttons);
      void SetBackgroundColor(Color color);

      //Baker Functions
      void DrawStaticTextures(RenderTexture2D &canvas, DebounceFlag &flag, std::function<void()> drawFunc);

      //Base Drawing Functions
      void DrawGrid();
      void DrawGridDots();
      void DrawFPSOnGrid();
      void scrollLogic();
      void DrawTextureOnGrid(Texture2D &texture, Rectangle source, Vector2 startCoords, Vector2 endCoords, Color tint);
      void DrawRectangleOnGrid(Vector2 startCoords, Vector2 endCoords, Color tint);
      void DrawRectangleLinesOnGrid(Vector2 startCoords, Vector2 endCoords, Color tint, int lineThickness);

      //Text Functions
      void DrawTextSOnGrid(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness = 0);
      void DrawTextSOnGridCached(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness = 0);
      float DrawTextSWrappedOnGrid(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness = 0);
      float DrawTextSWrappedOnGridCached(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness = 0);

      //Drawing Button Functions
      void DrawButtonOnGrid(SingleButtonGroup &buttons, int index, Vector2 startCoords, Vector2 endCoords);
      void DrawButtonRowOnGrid(SingleButtonGroup &buttons, Vector2 startCoords, Vector2 endCoords);
      void DrawButtonRowOnGridCached(SingleButtonGroup &buttons, Vector2 startCoords, Vector2 endCoords);

      protected:
      //Private Helper Functions
      constexpr Rectangle CoordsToRec(Vector2 startCoords, Vector2 endCoords);
      constexpr Vector2 CoordToVec(Vector2 coords);
      bool IsMouseWithinDest(const Rectangle &destination);
      void DrawButtonOverlay(SingleButtonGroup &buttons, int index, Rectangle buttonDest);
      void DrawButton(SingleButtonGroup &buttons, int index, Rectangle buttonDest);
      void DrawTextS(std::string_view text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness = 0);
      float DrawTextSWrapped(std::string_view text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness = 0);
      void ResetTextCache();
      void ResetButtonCache();
      constexpr uint64_t HashString(std::string_view text);

 };
 #endif