#include "MainFrame.h"
#include "LCS.h"
#include <wx/wx.h>
#include <vector>
#include <string>
#include "Task.h"

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title)
{
	CreateControls();
	BindEventHandlers();
	AddSaveTasks();
}

void MainFrame::CreateControls()
{
	wxFont headlineFont(wxFontInfo(wxSize(0, 36)).Bold());
	wxFont mainFont(wxFontInfo(wxSize(0, 24)));

	panel = new wxPanel(this);
	panel->SetFont(mainFont);

	headlineText = new wxStaticText(panel, wxID_ANY, "To-Do List", wxPoint(0, 22), wxSize(800, -1), wxALIGN_CENTER_HORIZONTAL);
	headlineText->SetFont(headlineFont);

	inputField = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(100, 80), wxSize(495, 35), wxTE_PROCESS_ENTER);
	addButton = new wxButton(panel, wxID_ANY, "Add", wxPoint(600, 80), wxSize(100, 35));

	notebook = new wxNotebook(panel, wxID_ANY, wxPoint(100, 120), wxSize(600, 400));

	checkListBox = new wxCheckListBox(notebook, wxID_ANY);
	notebook->AddPage(checkListBox, "All tasks");

	categoryTree = new wxTreeCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
	notebook->AddPage(categoryTree, "Categories");

	clearButton = new wxButton(panel, wxID_ANY, "Clear", wxPoint(100, 525), wxSize(100, 35));
}

void MainFrame::BindEventHandlers()
{
	addButton->Bind(wxEVT_BUTTON, &MainFrame::OnAddButtonClick, this);
	inputField->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnInputEnter, this);
	checkListBox->Bind(wxEVT_KEY_DOWN, &MainFrame::OnListKeyDown, this);
	clearButton->Bind(wxEVT_BUTTON, &MainFrame::OnClearButtonClicked, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
	notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MainFrame::OnTabChanged, this);
}

void MainFrame::AddSaveTasks()
{
	std::vector<Task> tasks = loadTaskFromFile("tasks.txt");

	for (const Task& task : tasks) {
		int index = checkListBox->GetCount();
		checkListBox->Insert(task.desctiption, index);
		checkListBox->Check(index, task.done);
	}
}

void MainFrame::OnWindowClosed(wxCloseEvent& evt)
{
	saveTaskToFile(GetTasksFromList(), "tasks.txt");
	evt.Skip();
}

// Reads the current tasks back out of the list box
std::vector<Task> MainFrame::GetTasksFromList()
{
	std::vector<Task> tasks;

	for (int i = 0; i < checkListBox->GetCount(); i++) {
		Task task;
		task.desctiption = checkListBox->GetString(i).ToStdString();
		task.done = checkListBox->IsChecked(i);
		tasks.push_back(task);
	}

	return tasks;
}

// Rebuild the categories whenever the Categories tab is opened
void MainFrame::OnTabChanged(wxBookCtrlEvent& evt)
{
	if (evt.GetSelection() == 1) {
		RefreshCategories();
	}

	evt.Skip();
}

void MainFrame::RefreshCategories()
{
	std::vector<Task> tasks = GetTasksFromList();

	categoryTree->DeleteAllItems();
	wxTreeItemId root = categoryTree->AddRoot("Categories"); // hidden by wxTR_HIDE_ROOT

	LCS lcs(tasks);
	std::vector<Category> categories = lcs.findPhrases(2);

	if (categories.empty()) {
		categoryTree->AppendItem(root, "No shared phrases yet");
		return;
	}

	for (const Category& category : categories) {
		wxString heading = wxString::Format("%s (%zu)", category.phrase, category.tasks.size());
		wxTreeItemId categoryItem = categoryTree->AppendItem(root, heading);

		for (int t : category.tasks) {
			categoryTree->AppendItem(categoryItem, tasks[t].desctiption);
		}

	}
}

void MainFrame::OnAddButtonClick(wxCommandEvent& evt)
{
	AddTaskFromInput();
}

void MainFrame::OnInputEnter(wxCommandEvent& evt)
{
	AddTaskFromInput();
}

void MainFrame::OnListKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode()) {
	case WXK_DELETE:
		DeleteSelectedTask();
		break;

	case WXK_BACK:
		DeleteSelectedTask();
		break;
	case WXK_UP:
		moveSelectedTask(-1);
		break;
	case WXK_DOWN:
		moveSelectedTask(+1);
			break;
	}
}

void MainFrame::OnClearButtonClicked(wxCommandEvent& evt)
{
	if (checkListBox->IsEmpty()) {
		return;
	}

	wxMessageDialog dialog(this, "Are you sure you want to clear all tasks?", "Clear", wxYES_NO | wxCANCEL);
	int result = dialog.ShowModal();

	if (result == wxID_YES) {
		checkListBox->Clear();
	}
}


void MainFrame::AddTaskFromInput()
{
	wxString description = inputField->GetValue();

	if (!description.IsEmpty()) {
		checkListBox->Insert(description, checkListBox->GetCount());
		inputField->Clear();
	}

	inputField->SetFocus();
}

void MainFrame::DeleteSelectedTask()
{
	int selectedIndex = checkListBox->GetSelection();

	if (selectedIndex == wxNOT_FOUND) {
		return;
	}

	checkListBox->Delete(selectedIndex);
}

void MainFrame::moveSelectedTask(int offset)
{
	int selectedIndex = checkListBox->GetSelection();

	if (selectedIndex == wxNOT_FOUND) {
		return;
	}

	int newIndex = selectedIndex + offset;

	if (newIndex >= 0 && newIndex < checkListBox->GetCount()) {
		swapTasks(selectedIndex, newIndex);
		checkListBox->SetSelection(newIndex, true);
	}
}

void MainFrame::swapTasks(int i, int j)
{
	Task taskI{ checkListBox->GetString(i).ToStdString(), checkListBox->IsChecked(i) };
	Task taskJ{ checkListBox->GetString(j).ToStdString(), checkListBox->IsChecked(j) };

	checkListBox->SetString(i, taskJ.desctiption);
	checkListBox->Check(i, taskJ.done);

	checkListBox->SetString(j, taskI.desctiption);
	checkListBox->Check(j, taskI.done);
}


