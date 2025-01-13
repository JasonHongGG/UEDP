//https://github.com/ocornut/imgui/issues/2602
#pragma once
#include "../imgui/imgui.h"
#include <oleidl.h>
#include <Windows.h>
#include <shellapi.h>
#include <string>
#include <functional>
#include "../Utils/Utils.h"

// create a class inheriting from IDropTarget
class DropManager : public IDropTarget
{
public:
	bool DragFlag = false;
	std::string CurDragDropFilePath = "";
	//--- implement the IUnknown parts
	// you could do this the proper way with InterlockedIncrement etc,
	// but I've left out stuff that's not exactly necessary for brevity
	ULONG AddRef() { return 1; }
	ULONG Release() { return 0; }

	// we handle drop targets, let others know
	HRESULT QueryInterface(REFIID riid, void** ppvObject)
	{
		if (riid == IID_IDropTarget)
		{
			*ppvObject = this;	// or static_cast<IUnknown*> if preferred
			// AddRef() if doing things properly
			// but then you should probably handle IID_IUnknown as well;
			return S_OK;
		}

		*ppvObject = NULL;
		return E_NOINTERFACE;
	}


	//--- implement the IDropTarget parts

	// occurs when we drag files into our applications view
	HRESULT DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
	{
		// TODO: check whether we can handle this type of object at all and set *pdwEffect &= DROPEFFECT_NONE if not;

		// do something useful to flag to our application that files have been dragged from the OS into our application
		// trigger MouseDown for button 1 within ImGui
		*pdwEffect &= DROPEFFECT_COPY;
		DragFlag = true;
		return S_OK;
	}

	// occurs when we drag files out from our applications view
	HRESULT DragLeave() { return S_OK; }

	// occurs when we drag the mouse over our applications view whilst carrying files (post Enter, pre Leave)
	HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
	{
		// trigger MouseMove within ImGui, position is within pt.x and pt.y
		// grfKeyState contains flags for control, alt, shift etc
		*pdwEffect &= DROPEFFECT_COPY;
		DragFlag = true;
		return S_OK;
	}

	// occurs when we release the mouse button to finish the drag-drop operation
	HRESULT Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
	{
		// grfKeyState contains flags for control, alt, shift etc
		// render the data into stgm using the data description in fmte
		FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM stgm;

		if (SUCCEEDED(pDataObj->GetData(&fmte, &stgm)))
		{
			HDROP hdrop = (HDROP)stgm.hGlobal; // or reinterpret_cast<HDROP> if preferred
			UINT file_count = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

			// we can drag more than one file at the same time, so we have to loop here
			for (UINT i = 0; i < file_count; i++)
			{
				TCHAR szFile[MAX_PATH];
				UINT cch = DragQueryFile(hdrop, i, szFile, MAX_PATH);
				if (cch > 0 && cch < MAX_PATH)
				{
					wprintf(L"File path: %s\n", szFile);
					CurDragDropFilePath = Utils.UnicodeToANSI(szFile);
					// szFile contains the full path to the file, do something useful with it
					// i.e. add it to a vector or something
				}
			}

			// we have to release the data when we're done with it
			ReleaseStgMedium(&stgm);

			// notify our application somehow that we've finished dragging the files (provide the data somehow)
		}

		// trigger MouseUp for button 1 within ImGui
		*pdwEffect &= DROPEFFECT_COPY;
		DragFlag = false;
		return S_OK;
	}
};


class FileDropManager {
private:
	using CallbackType = std::function<void(std::string)>;
	CallbackType callback;

public:
	DropManager dm;

	void initialize(HWND hwnd) {
		OleInitialize(NULL);
		RegisterDragDrop(hwnd, &dm);
	}

	void uninitialize(HWND hwnd) {
		RevokeDragDrop(hwnd);
		OleUninitialize();
	}


	// 使一定要將檔案拖到特定地點才會觸發
	FileDropManager& DragSoruceEvent() {
		if (dm.DragFlag) {
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern))
			{
				ImGui::SetDragDropPayload("FILES", nullptr, 0);
				//ImGui::BeginTooltip();
				//ImGui::Text("FILES");
				//ImGui::EndTooltip();
				ImGui::EndDragDropSource();
			}
		}
		return *this;
	}

	FileDropManager& SetCallBack(CallbackType cb)
	{
		callback = cb;
		return *this;
	}

	void DragTargetEvent() {
		if (ImGui::BeginDragDropTarget())
		{
			if (ImGui::AcceptDragDropPayload("FILES"))
			{
				//for (const auto& file : draggedFiles)
				//{
				//    // do something with file
				//}
				if (callback) callback(dm.CurDragDropFilePath);
			}
			ImGui::EndDragDropTarget();
		}
	}
};
inline FileDropManager FileDropMgr = FileDropManager();
