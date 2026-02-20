/*
 * Eric Ryan Montgomery
 * 11/24/2025
 * For CardGameUI
 * UIDrawer Class to replace functional functions
 */
 
#include "UIDrawer.h"

UIDrawer::UIDrawer() {
	screenDimensions = {(float)GetScreenWidth(), (float)GetScreenHeight()};
	maxScroll = 0.0f;
	scrollOffset = 0.0f;
	widthSegment = screenDimensions.x/widthBlocks;
	heightSegment = screenDimensions.y/heightBlocks;
	currentFontSize = screenDimensions.y / 22;
	currentFont = LoadFont("fonts/MontserratExtrabold.ttf");
}

void UIDrawer::Update() {
	// Update screen dimensions
	screenDimensions.x = GetScreenWidth();
	screenDimensions.y = GetScreenHeight();
	
	widthSegment = screenDimensions.x/widthBlocks;
	heightSegment = screenDimensions.y/heightBlocks;

	//Update FontSize
	currentFontSize = screenDimensions.y / 22;

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
	//void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color); 
	//Split screen into 32 by 32 squares
	Vector2 linePos = {0, 0};
	for (int i = 0; i < this->widthBlocks + 1; ++i) {
		DrawLineEx({linePos.x, 0}, {linePos.x, this->screenDimensions.y}, 1, BLACK);
		linePos.x = i * this->widthSegment;
	}
	for (int i = 0; i < this->heightBlocks + 1; ++i) {
		DrawLineEx({0, linePos.y}, {this->screenDimensions.x, linePos.y}, 1, BLACK);
		linePos.y = i * this->heightSegment;
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
void UIDrawer::DrawTextS(std::string text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness) {
	
	//Draw Test
	if (orientation.x == CENTERX) { //Centered
		dest.x = (dest.x + (dest.width/2)) - (((MeasureTextEx(this->currentFont, text.c_str(), fontSize, 1.0f)).x)/2);
	} else if (orientation.x == RIGHTX) { //Right
		dest.x = dest.x + dest.width - ((MeasureTextEx(this->currentFont, text.c_str(), fontSize, 1.0f)).x) - lineThickness;
	} else { //Left
		dest.x = dest.x + lineThickness;
	}
	
	if (orientation.y == CENTERY) { //Centered
		dest.y = (dest.y + (dest.height/2)) - (((MeasureTextEx(this->currentFont, text.c_str(), fontSize, 1.0f)).y)/2);
	} else if (orientation.y == DOWNY) { //Down
		dest.y = (dest.y + (dest.height)) - ((MeasureTextEx(this->currentFont, text.c_str(), fontSize, 1.0f)).y) - lineThickness;
	} else { //UP
		dest.y = dest.y + lineThickness;
	}
	
	DrawTextEx(this->currentFont, text.c_str(), (Vector2){dest.x, dest.y}, fontSize, 1.0f, tint); // Draw text using font and additional parameters
}

//DrawTextS but on a grid
void UIDrawer::DrawTextSOnGrid(std::string text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness) {
	DrawTextS(text, CoordsToRec(startCoords, endCoords), 
		this->textColor, this->currentFontSize, orientation, lineThickness);
}

//Draw Text Super Wrapped on the x-axis
float UIDrawer::DrawTextSWrapped(std::string text, Rectangle dest, Color tint, float fontSize, Alignment orientation, int lineThickness) {

	//Splits text up into its individual words
    std::vector<std::string> words;
    std::stringstream ss(text);
    std::string tempWord;
	
	while (ss.peek() == '\n') { //Captures \n characters at the beginning of the text
        ss.get();              // consume this newline
        words.push_back("\n"); // record it
    }
    while (ss >> tempWord) {
        words.push_back(tempWord);
        while (ss.peek() == '\n') { //Captures \n characters after current word
			ss.get();              // consume this newline
			words.push_back("\n"); // record it
		}
    }
    
    //Decides which words need to be on a new line and indexes them
    std::queue<int> newLineQueue;
    float sum = 0;
    float wordWidth = 0;
    /*Add the width of each word in words until it is greater than the dest width
     * then push the index of the word that went over onto the queue to be
     * used later.
     */
    for (size_t i = 0; i < words.size(); ++i) {
		if (words[i] != "\n") {
			wordWidth = MeasureTextEx(this->currentFont, (words[i] + " ").c_str(), fontSize, 1.0f).x;
			sum += wordWidth;
			if (sum >= dest.width && sum > wordWidth) {
				newLineQueue.push(i); //Marking this word as needing to start on a newline
				sum = MeasureTextEx(this->currentFont, (words[i]).c_str(), fontSize, 1.0f).x;
			}
		} else if (i != words.size()-1) {
			words[i] = "";
			newLineQueue.push(i);
			sum = 0;
		} else {
			words[i] = "";
		}
	}
	
	//Pushes words.size() to the end of the queue so the last line isn't forgotten
	newLineQueue.push(words.size());
	
	/*The queue now marks where the new lines should begin
	 * so this sections combines the words into the lines
	 */
	/*Now newLineQueue.size() == number of lines needed*/
	int totalNumLines = newLineQueue.size();
	std::vector<std::string> lines;
	int j = 0;
	for (int i = 0; i < totalNumLines; ++i) {
		lines.push_back("");
		while (j < newLineQueue.front()) {
			if (!lines[i].empty()) lines[i] += " ";
			lines[i] += words[j];
			++j;
		}
		newLineQueue.pop();
	}
	
	//Finally this section puts the words to the screen
	float lineHeight = MeasureTextEx(this->currentFont, "Ay", fontSize, 1.0f).y;
	float blockHeight = lines.size() * lineHeight;

	// Align the entire block vertically once
	if (orientation.y == CENTERY) { //CENTERY
		dest.y = (dest.y + dest.height/2) - blockHeight/2 - lineThickness; //LineThickness will need to be subtracted to counter DrawTextS() UPY
	} else if (orientation.y == DOWNY) { //DOWNY
		dest.y = (dest.y + dest.height) - blockHeight - (lineThickness * 2); ////LineThickness will need to be subtracted twice to counter DrawTextS() UPY
	} else { // UPY
		dest.y = dest.y; //Do not apply lineThickness it will be applied later in DrawTextS() using UPY
	}
	for (size_t i = 0; i < lines.size(); ++i) {
		DrawTextS(lines[i], dest, tint, fontSize, (Alignment){orientation.x, UPY}, lineThickness);
		dest.y += MeasureTextEx(this->currentFont, "Ay", fontSize, 1.0f).y;
	}
	
	return blockHeight;

}

//DrawTextSWrapped but on a grid
float UIDrawer::DrawTextSWrappedOnGrid(std::string text, Vector2 startCoords, Vector2 endCoords, Alignment orientation, int lineThickness) {
	return DrawTextSWrapped(text, {startCoords.x * this->widthSegment, startCoords.y * this->heightSegment, 
		(endCoords.x - startCoords.x) * this->widthSegment, 
		(endCoords.y - startCoords.y) * this->heightSegment}, 
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