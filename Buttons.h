/*
 * Eric Ryan Montgomery
 * 09/21/2025
 * For CardGameUI
 * Button Classes/Functions are written here
 */
#ifndef BUTTONSH
#define BUTTONSH
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <memory>
#include "raylib.h"
using namespace std;

class SingleButton {
	public:
		SingleButton(string label, function<void(SingleButton &currButton)> givenFunction); //Constructor
		void Click(); //Performs the buttons given lamda function
		void SetButtonState(int state); //Set the buttons state
		void SetBounds(Rectangle dest); //Set the buttons bounds
		Rectangle GetBounds(); //Get the buttons bounds
		int GetState(); //Get the current state of the button
		Color GetColor(); //Gets the current color of the button
		void SetLabel(string label); //Sets the label for the button
		string GetLabel(); //Gets the label for the button
		void SetFunctionality(bool b); //Turns the button on or off
		bool GetFunctionality(); //Get whether the button is currently on or off
		void SetPendingAction(bool b);
		bool GetPendingAction();
	protected:
		std::function<void(SingleButton &currButton)> onClick;  //Custom Function for each button
		int buttonState; // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
		bool functionality; //If true the button can be clicked
		bool pendingAction;
		Rectangle bounds;
		Color tint;
		string label;
};

class SingleButtonGroup {
	public:
		SingleButtonGroup(shared_ptr<Texture2D>& texture);
		void AddButton(string label, function<void(SingleButton&)> givenFunction);
		void ClearAllButtons();
		int GetSize();
		SingleButton& operator[](size_t index);
		SingleButton& operator[](const string& label);
		void AnimationLogic(Vector2 &mousePoint);
		void SetFunctionality(bool b, int start, int end); 
		auto begin() { return buttons.begin(); }
		auto end() { return buttons.end(); }
		shared_ptr<Texture2D>& GetTexture();
	protected:
		std::shared_ptr<Texture2D> buttonTexture;
		int size;
		vector<SingleButton> buttons;
		unordered_map<string, int> buttonsMap;
};

#endif