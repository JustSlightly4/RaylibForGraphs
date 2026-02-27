/*
 * Eric Ryan Montgomery
 * 11/24/2025
 * For CardGameUI
 * UIDrawer Class to replace functional functions
 */
 
#include "UIDrawer.h"

UIDrawer::DebounceFlag::DebounceFlag() {
	flag = false;
	timer = 0.0f;
	delaySeconds = 0.15f;
}

//Triggers a timer to make the flag true after timer is finished
void UIDrawer::DebounceFlag::Trigger() {
	timer = delaySeconds;
	flag = false;
}

// Call this every frame in your Update loop
void UIDrawer::DebounceFlag::Update() {
	if (timer > 0.0f) {
		timer -= GetFrameTime();
		if (timer <= 0.0f) {
			flag = true;
			return;
		}
	}

	if (flag) {
		this->Reset();
	}
}

bool UIDrawer::DebounceFlag::IsReady() {
	return flag;
}

void UIDrawer::DebounceFlag::Reset() {
	timer = 0.0f;
	flag = false;
}

UIDrawer::UIDrawer() {
	screenDimensions = {(float)GetScreenWidth(), (float)GetScreenHeight()};
	prevScreenDimensions = screenDimensions;
	maxScroll = 0.0f;
	scrollOffset = 0.0f;
	widthSegment = screenDimensions.x/widthBlocks;
	heightSegment = screenDimensions.y/heightBlocks;
	currentFontSize = screenDimensions.y / 22;
	//currentFont = LoadFont("fonts/MontserratExtrabold.ttf");
	staticTextures = { 0 }; // Initialize ID to 0
	updateStaticContent.Trigger();
	// 1. Define the character range (e.g., all printable ASCII)
	int glyphCount = 0;
	int *fontChars = LoadCodepoints(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", &glyphCount);

	// 2. Load the font with a specific size and the codepoints
	// This creates the Atlas texture inside the Font object
	currentFont = LoadFontEx("fonts/MontserratExtrabold.ttf", 128, fontChars, glyphCount);
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

		//Triger staticContent to be rebaked
		updateStaticContent.Trigger();
		resetCache.Trigger();
	}

	updateStaticContent.Update();
	resetCache.Update();

	if (resetCache.IsReady()) {
		this->ResetTextCache();
		this->ResetButtonCache();
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

//This function is used to "bake" static content on the screen.
//Given drawing functions, it will merge their results into a single
//texture that the GPU can draw with one call instead of drawing
//everything individual. Basically, for any static largely unchanging
//drawings on the screen use this function to increase performance.
void UIDrawer::DrawStaticTextures(RenderTexture2D &canvas, DebounceFlag &flag, std::function<void()> drawFunc) {
	if (flag.IsReady()) {
		if (canvas.id != 0) UnloadRenderTexture(canvas); 
		canvas = LoadRenderTexture(screenDimensions.x, screenDimensions.y);
		BeginTextureMode(canvas);
			ClearBackground(BLANK);
			drawFunc();
		EndTextureMode();
	}

	if (canvas.id != 0) {
		// 2. Draw the baked grid (single draw call instead of 64+ line calls)
		// Source rect has negative height to fix the OpenGL Y-flip
		Rectangle source = { 0, 0, (float)canvas.texture.width, (float)-canvas.texture.height };
		Rectangle dest = { 0, 0, this->screenDimensions.x, this->screenDimensions.y };
		
		DrawTexturePro(canvas.texture, source, dest, {0,0}, 0.0f, WHITE);
	}
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

constexpr Rectangle UIDrawer::CoordsToRec(Vector2 startCoords, Vector2 endCoords) {
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

//DrawTextS but on a grid
void UIDrawer::DrawTextSOnGridCached(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness) {
	Rectangle dest = CoordsToRec(startCoords, endCoords);

    // 1. Check if we already have this baked
	uint64_t textHash = HashString(text);
    auto it = textCache.find(textHash);
    
    if (it == textCache.end()) {
        // 2. Cache Miss: Bake the text into a texture
        RenderTexture2D canvas = LoadRenderTexture((int)dest.width, (int)dest.height);
        
        BeginTextureMode(canvas);
            ClearBackground(BLANK);
            // Draw text at (0,0) relative to the texture, not the screen!
            DrawTextS(text, CoordsToRec(startCoords, endCoords), 
				this->textColor, this->currentFontSize, orientation, lineThickness);
        EndTextureMode();

        textCache[textHash] = {canvas, 0};
        it = textCache.find(textHash);
    }

    // 3. Draw the cached texture to the screen
    // Source height is negative to flip the texture (Raylib/OpenGL requirement)
    Rectangle source = { 0, 0, (float)it->second.first.texture.width, (float)-it->second.first.texture.height };
    DrawTexturePro(it->second.first.texture, source, dest, {0,0}, 0.0f, WHITE);
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

float UIDrawer::DrawTextSWrappedOnGridCached(std::string_view text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness) {
    Rectangle dest = CoordsToRec(startCoords, endCoords);

    // 1. Check if we already have this baked
	uint64_t textHash = HashString(text);
    auto it = textCache.find(textHash);
    
    if (it == textCache.end()) {
        // 2. Cache Miss: Bake the text into a texture
        RenderTexture2D canvas = LoadRenderTexture((int)dest.width, (int)dest.height);
        
		int textHeight;
        BeginTextureMode(canvas);
            ClearBackground(BLANK);
            // Draw text at (0,0) relative to the texture, not the screen!
            textHeight = DrawTextSWrapped(text, {0, 0, dest.width, dest.height}, this->textColor, 
                             this->currentFontSize, orientation, lineThickness);
        EndTextureMode();

        textCache[textHash] = {canvas, textHeight};
        it = textCache.find(textHash);
    }

    // 3. Draw the cached texture to the screen
    // Source height is negative to flip the texture (Raylib/OpenGL requirement)
    Rectangle source = { 0, 0, (float)it->second.first.texture.width, (float)-it->second.first.texture.height };
    DrawTexturePro(it->second.first.texture, source, dest, {0,0}, 0.0f, WHITE);
	return it->second.second;
}

void UIDrawer::DrawButtonOverlay(SingleButtonGroup &buttons, int index, Rectangle buttonDest) {
	//Draw the overlay to show if the button is hovered over or clicked
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
}

//Draws a single button on the grid
void UIDrawer::DrawButton(SingleButtonGroup &buttons, int index, Rectangle buttonDest) {
	buttons[index].SetBounds(buttonDest);

	//Draw the texture and the text
	DrawTexturePro(*buttons.GetTexture(), this->buttonSource, buttonDest, this->origin, 0.0f, WHITE);
	DrawTextSWrapped(buttons[index].GetLabel(), buttonDest, this->textColor, this->currentFontSize, (Alignment){CENTERX, CENTERY});
}

//Draws a single button on the grid
void UIDrawer::DrawButtonOnGrid(SingleButtonGroup &buttons, int index, Vector2 startCoords, Vector2 endCoords) {
	if (index < 0 || index > buttons.GetSize()-1) return;
	Rectangle buttonDest = CoordsToRec(startCoords, endCoords);
	DrawButton(buttons, index, buttonDest);
	DrawButtonOverlay(buttons, index, buttonDest);
}

//Draws a horizontal row of buttons on the grid
void UIDrawer::DrawButtonRowOnGrid(SingleButtonGroup &buttons, Vector2 startCoords, Vector2 endCoords) {
	int amountOfButtons = buttons.GetSize();
	float buttonWidth = (endCoords.x - startCoords.x)/amountOfButtons;
	float buttonHeight = (endCoords.y - startCoords.y);

	for (int j = 0; j < amountOfButtons; ++j) {
		Rectangle dest = CoordsToRec({startCoords.x + (j * buttonWidth), startCoords.y}, 
		{startCoords.x + (j * buttonWidth) + buttonWidth, startCoords.y + buttonHeight});
		DrawButton(buttons, j, dest);
	}

	for (int j = 0; j < amountOfButtons; ++j) {
		Rectangle dest = CoordsToRec({startCoords.x + (j * buttonWidth), startCoords.y}, 
		{startCoords.x + (j * buttonWidth) + buttonWidth, startCoords.y + buttonHeight});
		DrawButtonOverlay(buttons, j, dest);
	}
}

//Draws a horizontal row of buttons on the grid
void UIDrawer::DrawButtonRowOnGridCached(SingleButtonGroup &buttons, Vector2 startCoords, Vector2 endCoords) {
	int amountOfButtons = buttons.GetSize();
	float buttonWidth = (endCoords.x - startCoords.x)/amountOfButtons;
	float buttonHeight = (endCoords.y - startCoords.y);

	Rectangle dest = CoordsToRec(startCoords, endCoords);

    // 1. Check if we already have this baked
	int id = buttons.GetID();
    auto it = buttonCache.find(id);
    
    if (it == buttonCache.end()) {

        // 2. Cache Miss: Bake the text into a texture
        RenderTexture2D canvas = LoadRenderTexture((int)dest.width, (int)dest.height);
        
        BeginTextureMode(canvas);
            ClearBackground(BLANK);
            // Draw text at (0,0) relative to the texture, not the screen!
            for (int j = 0; j < amountOfButtons; ++j) {
				Rectangle dest = CoordsToRec({0 + (j * buttonWidth), 0}, 
				{0 + (j * buttonWidth) + buttonWidth, 0 + buttonHeight});
				DrawButton(buttons, j, dest);
			}
        EndTextureMode();

        buttonCache[id] = canvas;
        it = buttonCache.find(id);
    }

    // 3. Draw the cached texture to the screen
    // Source height is negative to flip the texture (Raylib/OpenGL requirement)
    Rectangle source = { 0, 0, (float)it->second.texture.width, (float)-it->second.texture.height };
    DrawTexturePro(it->second.texture, source, dest, {0,0}, 0.0f, WHITE);

	//Draw the overlays dynamically
	for (int j = 0; j < amountOfButtons; ++j) {
		Rectangle dest = CoordsToRec({startCoords.x + (j * buttonWidth), startCoords.y}, 
		{startCoords.x + (j * buttonWidth) + buttonWidth, startCoords.y + buttonHeight});
		buttons[j].SetBounds(dest);
		DrawButtonOverlay(buttons, j, dest);
	}
}

void UIDrawer::ResetTextCache() {
	int i = 0;
	for (auto it : textCache) {
		UnloadRenderTexture(it.second.first);
		++i;
	}
	textCache.clear();
	std::cout << "Text Cache has been reset!\n";
	std::cout << i << " textures cleared!\n";
}

void UIDrawer::ResetButtonCache() {
	int i = 0;
	for (auto it : buttonCache) {
		UnloadRenderTexture(it.second);
		++i;
	}
	buttonCache.clear();
	std::cout << "Button Cache has been reset!\n";
	std::cout << i << " textures cleared!\n";
}

constexpr uint64_t UIDrawer::HashString(std::string_view text) {
    uint64_t hash = 14695981039346656037ull; // FNV offset basis
    for (char c : text)
    {
        hash ^= static_cast<uint8_t>(c);
        hash *= 1099511628211ull; // FNV prime
    }
    return hash;
}