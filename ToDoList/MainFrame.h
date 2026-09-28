#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <vector>
#include "Task.h"

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:
	void CreateControls();
	void BindEventHandlers();
	void AddSaveTasks();

	void OnAddButtonClick(wxCommandEvent& evt);
	void OnInputEnter(wxCommandEvent& evt);
	void OnListKeyDown(wxKeyEvent& evt);
	void OnClearButtonClicked(wxCommandEvent& evt);
	void OnWindowClosed(wxCloseEvent& evt);

	void AddTaskFromInput();
	void DeleteSelectedTask();
	void moveSelectedTask(int offset);
	void swapTasks(int i, int j);

	void OnTabChanged(wxBookCtrlEvent& evt);
	void RefreshCategories();

	wxPanel* panel;
	wxStaticText* headlineText;
	wxTextCtrl* inputField;
	wxButton* addButton;
	wxCheckListBox* checkListBox;
	wxButton* clearButton;
	wxNotebook* notebook;
	wxTreeCtrl* categoryTree;

	std::vector<Task> GetTasksFromList();

};

