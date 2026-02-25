/*
 * Eric Ryan Montgomery
 * 11/24/2025
 * For CardGameUI
 * UIDrawer Class to replace functional functions
 */
 
#include "UIDrawer.h"

UIDrawer::UIDrawer() {
	screenDimensions = {(float)GetScreenWidth(), (float)GetScreenHeight()};
	prevScreenDimensions = screenDimensions;
	maxScroll = 0.0f;
	scrollOffset = 0.0f;
	widthSegment = screenDimensions.x/widthBlocks;
	heightSegment = screenDimensions.y/heightBlocks;
	currentFontSize = screenDimensions.y / 22;
	currentFont = LoadFont("fonts/MontserratExtrabold.ttf");
    gridNeedsUpdate = true;
	gridCanvas = { 0 }; // Initialize ID to 0
}

void UIDrawer::Update() {
	// Update screen dimensions
	screenDimensions.x = GetScreenWidth();
	screenDimensions.y = GetScreenHeight();

	if ((screenDimensions.x != prevScreenDimensions.x) || (screenDimensions.y != prevScreenDimensions.y)) {
		prevScreenDimensions.x = screenDimensions.x;
		prevScreenDimensions.y = screenDimensions.y;
	
		widthSegment = screenDimensions.x/widthBlocks;
		heightSegment = screenDimensions.y/heightBlocks;

		//Update FontSize
		currentFontSize = screenDimensions.y / 22;

		gridNeedsUpdate = true;
	}

	//Update Scroll Logic
	this->scrollLogic();

	//Update where the mouse is at
	mousePoint = GetMousePosition(); 
}

void UIDrawer::UpdateButtons(SingleButtonGroup &buttons) {
	buttons.AnimationLogic(this->mousePoint);
}

void UIDrawer::SetBackgroundColor(Color color) {
	ClearBackground(color); 
}

void UIDrawer::scrollLogic() {
	if (this->maxScroll < 0) this->maxScroll = 0;
	if (IsKeyDown(KEY_DOWN) || GetMouseWheelMove() < 0) scrollOffset += this->scrollSpeed;
	if (IsKeyDown(KEY_UP) || GetMouseWheelMove() > 0) scrollOffset -= this->scrollSpeed;
	if (scrollOffset < 0) scrollOffset = 0;
	if (scrollOffset >= this->maxScroll) scrollOffset = this->maxScroll;
}

//Draws grid across screen for debugging purposes
void UIDrawer::DrawGrid() {

    if (gridNeedsUpdate) {
		if (gridCanvas.id != 0) UnloadRenderTexture(gridCanvas); 
		gridCanvas = LoadRenderTexture(screenDimensions.x, screenDimensions.y);
        BeginTextureMode(gridCanvas);
            ClearBackground(BLANK); // Make the canvas transparent
            
            // --- YOUR ORIGINAL LOGIC START ---
            Vector2 linePos = {0, 0};
            for (int i = 0; i < this->widthBlocks + 1; ++i) {
                DrawLineEx({linePos.x, 0}, {linePos.x, this->screenDimensions.y}, 1, BLACK);
                linePos.x = i * this->widthSegment;
            }
            linePos.y = 0; // Reset Y for the second loop
            for (int i = 0; i < this->heightBlocks + 1; ++i) {
                DrawLineEx({0, linePos.y}, {this->screenDimensions.x, linePos.y}, 1, BLACK);
                linePos.y = i * this->heightSegment;
            }
            // --- YOUR ORIGINAL LOGIC END ---
            
        EndTextureMode();
        gridNeedsUpdate = false;
    }

	if (gridCanvas.id != 0) {
		// 2. Draw the baked grid (single draw call instead of 64+ line calls)
		// Source rect has negative height to fix the OpenGL Y-flip
		Rectangle source = { 0, 0, (float)gridCanvas.texture.width, (float)-gridCanvas.texture.height };
		Rectangle dest = { 0, 0, this->screenDimensions.x, this->screenDimensions.y };
		
		DrawTexturePro(gridCanvas.texture, source, dest, {0,0}, 0.0f, WHITE);
	}
}

//Draws dots across screen for debugging purposes
void UIDrawer::DrawGridDots() {
	//void DrawCircleV(Vector2 center, float radius, Color color)
	for (int i = 0; i < this->widthBlocks + 1; ++i) {
		for (int j = 0; j < this->heightBlocks + 1; ++j) {
			DrawCircleV({i * this->widthSegment, j * this->heightSegment}, 3, RED);
		}
	}
}

//Draws the FPS on the top left of the screen
void UIDrawer::DrawFPSOnGrid() {
	std::string FPS = "FPS: " + std::to_string(GetFPS());
	DrawTextSOnGrid(FPS, {0, 0}, {3, 1}, {LEFTX, UPY});
}

inline Rectangle UIDrawer::CoordsToRec(Vector2 startCoords, Vector2 endCoords) {
	return (Rectangle){startCoords.x * this->widthSegment, startCoords.y * this->heightSegment, 
		(endCoords.x - startCoords.x) * this->widthSegment, 
		(endCoords.y - startCoords.y) * this->heightSegment};
}

//Draws a texture on a grid
void UIDrawer::DrawTextureOnGrid(Texture2D &texture, Rectangle source, Vector2 startCoords, Vector2 endCoords, Color tint) {
	DrawTexturePro(texture, source, 
	CoordsToRec(startCoords, endCoords), 
	this->origin, 0.0f, tint);
}

//Draws a rectangle on a grid
void UIDrawer::DrawRectangleOnGrid(Vector2 startCoords, Vector2 endCoords, Color tint) {
	DrawRectangleRec(CoordsToRec(startCoords, endCoords), tint);
}

//Draws Rectangle Lines on a grid
void UIDrawer::DrawRectangleLinesOnGrid(Vector2 startCoords, Vector2 endCoords, Color tint, int lineThickness) {
	DrawRectangleLinesEx(CoordsToRec(startCoords, endCoords), 
		lineThickness, tint);
}

//Draw Test Super
void UIDrawer::DrawTextS(std::string_view text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness) {
	
	// 1. Create a fixed-size buffer on the stack. 
    // 1024 is usually plenty for a single line of UI text.
    char buffer[1024];

    // 2. Ensure we don't overflow our buffer if the text is huge
    size_t length = std::min(text.size(), sizeof(buffer) - 1);

    // 3. Copy the data and null-terminate it
    // .copy() does not add a \0, so we must do it manually.
    text.copy(buffer, length);
    buffer[length] = '\0';

	Vector2 textSize = MeasureTextEx(this->currentFont, buffer, fontSize, 1.0f);

	//Draw Test
	if (orientation.x == CENTERX) { //Centered
		dest.x = (dest.x + (dest.width/2)) - (((textSize).x)/2);
	} else if (orientation.x == RIGHTX) { //Right
		dest.x = dest.x + dest.width - ((textSize).x) - lineThickness;
	} else { //Left
		dest.x = dest.x + lineThickness;
	}
	
	if (orientation.y == CENTERY) { //Centered
		dest.y = (dest.y + (dest.height/2)) - (((textSize).y)/2);
	} else if (orientation.y == DOWNY) { //Down
		dest.y = (dest.y + (dest.height)) - ((textSize).y) - lineThickness;
	} else { //UP
		dest.y = dest.y + lineThickness;
	}
	
	DrawTextEx(this->currentFont, buffer, (Vector2){dest.x, dest.y}, fontSize, 1.0f, tint); // Draw text using font and additional parameters
}

//DrawTextS but on a grid
void UIDrawer::DrawTextSOnGrid(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness) {
	DrawTextS(text, CoordsToRec(startCoords, endCoords), 
		this->textColor, this->currentFontSize, orientation, lineThickness);
}

float UIDrawer::DrawTextSWrapped(std::string_view text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness) {
    std::vector<std::string> lines;
    float spaceWidth = MeasureTextEx(this->currentFont, " ", fontSize, 1.0f).x;
    float lineHeight = MeasureTextEx(this->currentFont, "Ay", fontSize, 1.0f).y;
    
    // Ensure we have at least some width to work with
    float maxWidth = (dest.width > lineThickness * 2) ? (dest.width - (lineThickness * 2)) : dest.width;
    
    std::string currentLine;
    float currentLineWidth = 0;

    size_t start = 0;
    while (start < text.length()) {
        size_t end = text.find_first_of(" \n", start);
        if (end == std::string_view::npos) end = text.length();

        std::string word(text.substr(start, end - start));
        float wordWidth = MeasureTextEx(this->currentFont, word.c_str(), fontSize, 1.0f).x;

        // 1. Check for manual newlines first
        bool isManualNewline = (start > 0 && text[start - 1] == '\n');

        // 2. Check if the word triggers a wrap
        // Optimization: Only wrap if currentLine IS NOT empty. 
        // If it's empty and the word is still too wide, we have to draw it anyway (or clip it).
        if (isManualNewline || (currentLineWidth + wordWidth > maxWidth && !currentLine.empty())) {
            lines.push_back(currentLine);
            currentLine = "";
            currentLineWidth = 0;
        }

        // 3. Add the word to the line
        if (!currentLine.empty()) {
            currentLine += " ";
            currentLineWidth += spaceWidth;
        }
        currentLine += word;
        currentLineWidth += wordWidth;

        start = end + 1;
    }

    // Only push the last line if it's not empty to prevent bottom gaps
    if (!currentLine.empty()) lines.push_back(currentLine);

    // Block Alignment Logic
    float blockHeight = lines.size() * lineHeight;
    
    // Y-Offset adjustment: ensure the block doesn't start at a negative coordinate relative to dest
    float startY = dest.y;
    if (orientation.y == CENTERY) {
        startY = (dest.y + dest.height / 2) - (blockHeight / 2);
    } else if (orientation.y == DOWNY) {
        startY = (dest.y + dest.height) - blockHeight - lineThickness;
    } else {
        startY += lineThickness;
    }

    // Drawing
    for (const std::string& line : lines) {
        DrawTextS(line, (Rectangle){dest.x, startY, dest.width, lineHeight}, tint, fontSize, {orientation.x, UPY}, lineThickness);
        startY += lineHeight;
    }

    return blockHeight;
}

//DrawTextSWrapped but on a grid
float UIDrawer::DrawTextSWrappedOnGrid(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness) {
	return DrawTextSWrapped(text, CoordsToRec(startCoords, endCoords), 
		this->textColor, this->currentFontSize, orientation, lineThickness);
}

//Draws a single button on the grid
void UIDrawer::DrawButtonOnGrid(SingleButtonGroup &buttons, int index, Vector2 startCoords, Vector2 endCoords) {
	if (index < 0 || index > buttons.GetSize()-1) return;
	Rectangle buttonDest = CoordsToRec(startCoords, endCoords);
	buttons[index].SetBounds(buttonDest);
	DrawTextureOnGrid(*buttons.GetTexture(), this->buttonSource, startCoords, endCoords, WHITE);
	switch(buttons[index].GetState()) {
		case 1://Hovered GRAY (Color){ 130, 130, 130, 100 }
			DrawRectangleRec(buttonDest, (Color){ 130, 130, 130, 100 });
			break;
		case 2: //Clicked DARKGRAY (Color){ 80, 80, 80, 100 }
			DrawRectangleRec(buttonDest, (Color){ 80, 80, 80, 100 });
			break;
		default: //Neither hover nor clicked
			break;
	}
	DrawTextSWrappedOnGrid(buttons[index].GetLabel(), startCoords, endCoords, (Alignment){CENTERX, CENTERY});
}

//Draws a horizontal row of buttons on the grid
void UIDrawer::DrawButtonRowOnGrid(SingleButtonGroup &buttons, Vector2 startCoords, Vector2 endCoords) {
	int amountOfButtons = buttons.GetSize();
	float buttonWidth = (endCoords.x - startCoords.x)/(min(buttons.GetSize(), amountOfButtons));
	int amountRows = ceil((float)buttons.GetSize() / (float)amountOfButtons);
	float buttonHeight = (endCoords.y - startCoords.y)/amountRows;

	int buttonIndex = 0; //So that we know which button we're indexing
	for (int i = 0; i < amountRows; ++i){ //The row
		for (int j = 0; j < amountOfButtons; ++j) { //the column
			if (buttonIndex >= buttons.GetSize()) return;
			DrawButtonOnGrid(buttons, buttonIndex, 
			{startCoords.x + (j * buttonWidth), startCoords.y + (i * buttonHeight)}, 
			{startCoords.x + (j * buttonWidth) + buttonWidth, startCoords.y + (i * buttonHeight) + buttonHeight});
			++buttonIndex; //Advance the buttonIndex
		}
	}
}