#pragma once
#include <string>
#include <vector>
#include "../../imgui/imgui.h"
#define M_PI 3.14159265358979323846

struct Node
{
	int ID;
	float Radius = 30.f;
	ImVec2 Pos;
	ImVec2 Velocity = { 0.f, 0.f };
	std::string Name;
	std::vector<Node*> To;	// �Q���~��
	std::vector<Node*> From; // �~�ө�� //�a�i
};