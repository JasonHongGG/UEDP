#pragma once
#include <cmath>
#include "../../GUI/Config/NotificationConfig.h"
#include "../../GUI/Config/MainConsoleConfig.h"
#include "../../State/EventHandler.h"
#include "../../State/GUIState.h"
#include "../Storage/StorageManager.h"
#include "../../GUI/Interface/ObjectGraphInterface.h"
#include "../../GUI/MyGuiComponent/Operator.h"

namespace ObjectGraphEntry
{
	inline static int IDCounter = 0;
	inline float RadiusDecayRatio = 0.6;

	inline void GetMostUperImportanceObjectVector(int keepCnt);

	inline ImVec2 GetRandomPos();

	inline ImVec2 GetRandomPosInCircle(ImVec2 pos, float radius);

	inline void GenerateUperTreeRecursive(Node* parentNode, ObjectData& object, int radius, int level);

	inline void GenerateUperTree();

	inline void Entry()
	{
		if (ObjectGraphState.GetMostUperImportanceObjectVectorEvent.State == ProcessState::Start) {
			ObjectGraphState.GetMostUperImportanceObjectVectorEvent.State = ProcessState::Processing;
			EventHandler::NotifyEvent(NotificationConfig::NotiyType::Info, "[Execute] Get Most Uper Object Vector");
			GetMostUperImportanceObjectVector(ObjectGraphState.GetMostUperImportanceObjectVectorEvent.Cnt);
			GenerateUperTree();
			ObjectGraphState.GetMostUperImportanceObjectVectorEvent.State = ProcessState::Completed;
		}
	}
}

ImVec2 ObjectGraphEntry::GetRandomPos()
{
	ImVec2 sz = MainConsoleConf.CurentWindowSize / 2;
	ImVec2 addition = sz / 2;
	return { addition.x + float(rand() % int(sz.x)), addition.y + float(rand() % int(sz.y)) };
}

ImVec2 ObjectGraphEntry::GetRandomPosInCircle(ImVec2 pos, float radius) {
	float r = float(rand()) / RAND_MAX * radius;
	float theta = float(rand()) / RAND_MAX * 2 * M_PI;

	float x = pos.x + r * cos(theta);
	float y = pos.y + r * sin(theta);

	return { x, y };
}

void ObjectGraphEntry::GenerateUperTreeRecursive(Node* parentNode, ObjectData& object, int radius, int level)
{
	// 找出 uperCnt 最高的前幾個 uper
	std::vector<std::pair<int, DWORD_PTR>> sortVector;
	for (int i = 0; i < object.Uper.size(); i++) {
		BasicObjectData uper = object.Uper[i];
		ObjectData tempObject = StorageMgr.ObjectDict.Get(uper.Address);
		if (sortVector.size() < ObjectGraphConf.KeepUperNodeNumber) {
			sortVector.push_back(std::make_pair(tempObject.UperCnt, uper.Address));
			// 保持排序
			std::sort(sortVector.begin(), sortVector.end(),
				[](std::pair<int, DWORD_PTR> a, std::pair<int, DWORD_PTR> b) { return a.first > b.first; });
		}
		else {
			for (int i = 0; i < sortVector.size(); i++) {
				if (sortVector[i].first < tempObject.UperCnt) {
					sortVector.insert(sortVector.begin() + i, std::make_pair(tempObject.UperCnt, uper.Address));
					sortVector.pop_back();
					break;
				}
			}
		}
	}

	// 用找到的這接 uper 去建立新的 node
	for (int i = 0; i < sortVector.size() and i < ObjectGraphConf.KeepUperNodeNumber; i++) {
		ObjectData uper = StorageMgr.ObjectDict.Get(sortVector[i].second);
		Node* n = new Node();
		n->ID = IDCounter++;
		n->Name = uper.Name;
		n->Pos = GetRandomPosInCircle(parentNode->Pos, parentNode->Radius);
		n->Radius = radius;
		n->From.push_back(parentNode);
		parentNode->To.push_back(n);

		if (level > 1) {
			StorageMgr.SetObjectUper(uper.Address);
			StorageMgr.ObjectGraphUperProgressBarValue.Set(StorageMgr.ObjectGraphUperProgressBarValue.Get() + 1);
			ObjectData uperList = StorageMgr.ObjectDict.Get(uper.Address);
			GenerateUperTreeRecursive(n, uperList, radius * 0.6, level - 1);
		}

		ObjectGraphConf.AllNodes.push_back(n);
	}
}

void ObjectGraphEntry::GenerateUperTree()
{
	StorageMgr.ObjectGraphUperProgressBarValue.Set(0);
	StorageMgr.ObjectGraphUperProgressBarTotalValue.Set(ObjectGraphState.GetMostUperImportanceObjectVectorEvent.Cnt * ObjectGraphConf.KeepUperNodeNumber); // std::pow(KeepUperNodeNumber, ObjectGraphConf.TreeLevel)
	for (ObjectData objct : ObjectGraphConf.MostUperObjectVector) {
		Node* n = new Node();
		n->ID = IDCounter++;
		n->Name = objct.Name;
		n->Pos = GetRandomPos();

		GenerateUperTreeRecursive(n, objct, n->Radius * RadiusDecayRatio, ObjectGraphConf.TreeLevel);
		ObjectGraphConf.AllNodes.push_back(n);
		ObjectGraphConf.MainNodes.push_back(n);
	}
}

void ObjectGraphEntry::GetMostUperImportanceObjectVector(int keepCnt) {
	StorageMgr.ObjectGraphUperProgressBarValue.Set(0);
	StorageMgr.ObjectGraphUperProgressBarTotalValue.Set(StorageMgr.ObjectDict.Size());
	std::vector<ObjectData> MostUperObjectVector;
	std::vector<std::pair<int, DWORD_PTR>> sortVector;
	if (!StorageMgr.ObjectDict.Empty()) {
		for (auto Obj : StorageMgr.ObjectDict.GetAll()) {
			StorageMgr.ObjectGraphUperProgressBarValue.Set(StorageMgr.ObjectGraphUperProgressBarValue.Get() + 1);

			// 初始階段直接加入
			if (sortVector.size() < keepCnt) {
				sortVector.push_back(std::make_pair(Obj.second.UperCnt, Obj.second.Address));
				// 保持排序
				std::sort(sortVector.begin(), sortVector.end(),
					[](std::pair<int, DWORD_PTR> a, std::pair<int, DWORD_PTR> b) { return a.first > b.first; });
			}
			else {
				for (int i = 0; i < sortVector.size(); i++) {
					if (sortVector[i].first < Obj.second.UperCnt) {
						sortVector.insert(sortVector.begin() + i, std::make_pair(Obj.second.UperCnt, Obj.second.Address));
						sortVector.pop_back();
						break;
					}
				}
			}
		}


		StorageMgr.ObjectGraphUperProgressBarValue.Set(0);
		StorageMgr.ObjectGraphUperProgressBarTotalValue.Set(sortVector.size());
		for (int i = 0; i < sortVector.size(); i++) {
			StorageMgr.ObjectGraphUperProgressBarValue.Set(StorageMgr.ObjectGraphUperProgressBarValue.Get() + 1);
			StorageMgr.SetObjectUper(sortVector[i].second);
			MostUperObjectVector.push_back(StorageMgr.ObjectDict.Get(sortVector[i].second));
		}
	}
	ObjectGraphConf.MostUperObjectVector = MostUperObjectVector;
}