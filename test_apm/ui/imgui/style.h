#pragma once
#include "imgui.h"
#include "../gui/gui.h"
#include <map>
void InitStyle();

void InitStyle()			//create the Theme
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	//            
	/*
	const ImVec4 bgColor = ImVec4(0.1, 0.1, 0.1, 0.1);
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	*/
	const ImColor bgColor = ImColor(0.0, 0.0, 0.0, 0);

	colors[ImGuiCol_WindowBg] = bgColor;

	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->WindowMinSize = ImVec2(gui::WIDTH, gui::HEIGHT);

	style->FramePadding = ImVec2(8, 6);


	colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
	colors[ImGuiCol_FrameBg] = ImColor(12, 51, 85, 100);
	colors[ImGuiCol_Text] = ImColor(196, 161, 91, 255);
}

void DetectedTextStyle(bool x)
{
	ImGuiStyle* style = &ImGui::GetStyle();
	if (x == true) {   //detected
		style->Colors[ImGuiCol_Text] = ImColor(255, 0, 0, 255);
	}
	if (x == false) {	//undected
		style->Colors[ImGuiCol_Text] = ImColor(0, 255, 0, 255);
	}
}

void ResetTextStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
}
