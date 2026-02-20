/*
 * Eric Ryan Montgomery
 * 09/21/2025
 * For CardGameUI
 * Button Classes/Functions are written here
 */
 
#include "Buttons.h"

//Start SingleButton Class---------------------------------------
SingleButton::SingleButton(string label, function<void(SingleButton &currButton)> givenFunction) {
	onClick = givenFunction;
	this->label = label;
	buttonState = 0;
	functionality = true;
	tint = WHITE;
	pendingAction = false;
}

void SingleButton::Click() {
	if (onClick) onClick(*this);
}

void SingleButton::SetButtonState(int state) {
	if (state < 0 || state > 2) return;
	buttonState = state;
}

void SingleButton::SetBounds(Rectangle dest) {
	bounds = dest;
}

Rectangle SingleButton::GetBounds() {
	return bounds;
}

int SingleButton::GetState() {
	return buttonState;
}

Color SingleButton::GetColor() {
	return tint;
}

void SingleButton::SetLabel(string label) {
	this->label = label;
}

string SingleButton::GetLabel() {
	return label;
}

void SingleButton::SetFunctionality(bool b) {
	functionality = b;
	if (functionality == false) {
		SetButtonState(2);
	}
}

bool SingleButton::GetFunctionality() {
	return functionality;
}

void SingleButton::SetPendingAction(bool b){
	pendingAction = b;
}
bool SingleButton::GetPendingAction() {
	return pendingAction;
}
//End SingleButton Class-----------------------------------------



//Start SingleButtonGroup Class----------------------------------
SingleButtonGroup::SingleButtonGroup(shared_ptr<Texture2D>& texture) : buttonTexture(texture) {
	size = 0;
}

void SingleButtonGroup::AddButton(string label, function<void(SingleButton&)> givenFunction) {
	buttons.push_back(SingleButton(label, givenFunction));
	buttonsMap[label] = size;
	++size;
}

void SingleButtonGroup::ClearAllButtons() {
	buttons.clear();
	buttonsMap.clear();
	size = 0;
}

//Return size of button array
int SingleButtonGroup::GetSize() {
	return size;
}

//Overload [] operator with integer
SingleButton& SingleButtonGroup::operator[](size_t index) {
    if (index < 0 || index >= buttons.size())
        throw std::out_of_range("Button index out of range");
    return buttons[index];
}

//Overload [] operator with string
SingleButton& SingleButtonGroup::operator[](const string& label) {
    auto it = buttonsMap.find(label);
    if (it == buttonsMap.end()) {
        throw std::out_of_range("Button label not found: " + label);
    }
    return buttons[it->second];
}

//Provides the animation logic for the button group
void SingleButtonGroup::AnimationLogic(Vector2 &mousePoint) {

	for (int i = 0; i < size; ++i) {
		if (buttons[i].GetPendingAction() == true && !IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			buttons[i].SetPendingAction(false);
			buttons[i].Click();
		}
	}

	//Allows buttons to have animations and functionality
	for (int i = 0; i < size; ++i) {
		if (buttons[i].GetFunctionality() == true) { //Check to make sure the button is allowed
			if (CheckCollisionPointRec(mousePoint, buttons[i].GetBounds())) //Check to see if the mouse is over the button
			{
				//Sets Color and activates button if clicked
				if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {//Clicked
					buttons[i].SetButtonState(2); //Clicked, Dark Gray
					buttons[i].SetPendingAction(true);
				} else {
					buttons[i].SetButtonState(1); //Hovered, Light Gray
				}
			}
			else buttons[i].SetButtonState(0); //Not hovered nor clicked, White
		}
	}
}

//Sets Button Functionality on a range on start(inclusive) to end(non-inclusive);
void SingleButtonGroup::SetFunctionality(bool b, int start, int end) {
	if (size == 0 || start > end || start < 0 || end > size - 1) {
		cout << "Indexing failure in function SetFunctionality!" << endl;
		return;
	}
	for (int i = start; i < end+1; ++i) {
		buttons[i].SetFunctionality(b);
	}
}

shared_ptr<Texture2D>& SingleButtonGroup::GetTexture() {
	return buttonTexture; // Doesn't increase reference count
}
//End SingleButtonGroup Class----------------------------------