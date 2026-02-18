
#include <wx/sizer.h>
#include <wx/valnum.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include "rastin/MainWindow.h"
#include "rastin/ctwinding.h"
#include "rastin/CTWindow.h"
#include "rastin/MyGridCellFloatEditor.h"
#include <fstream>

enum
{
    ID_Open = 1,
    ID_Close = 2,
    ID_Create = 3,
    ID_Save_as = 4,
    ID_Save = 5,
    ID_Window_CT = 6,
    ID_Add_row = 7,
    ID_Delete_row = 8
};
MainWindow::MainWindow()
    : wxFrame(NULL, wxID_ANY, "Rastin"), grid(new wxGrid(this, wxID_ANY)), currentFilePath("Без названия") {

    SetTitle("Rastin - " + currentFilePath);
    InitLimitFactorCurves();
    model = new GridTableModel(limitFactorCurves);

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Create, wxT("&Создать...\tCtrl-H"),
        wxT("Создать новый проект"));
    menuFile->Append(ID_Save, wxT("&Сохранить"), wxT("Сохранение текущего файла"));
    menuFile->Append(ID_Save_as, wxT("&Сохранить как..."), wxT("Сохранение под другим названием"));
    menuFile->Append(ID_Open, wxT("&Открыть..."), wxT("Открыть проект"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, wxT("&Выход"), wxT("Выход из программы"));

    wxMenu* menuModule = new wxMenu;
    menuModule->Append(ID_Window_CT, wxT("&Модуль ТТ..."), wxT("Добавить кривую предельной кратности"));

    wxMenu* menuTable = new wxMenu;
    menuTable->Append(ID_Add_row, wxT("&Добавить строку\tCtrl-+"), wxT("Добавить строку в таблице"));
    menuTable->Append(ID_Delete_row, wxT("&Удалить строку\tCtrl--"), wxT("Удалить строку в таблице"));

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, wxT("&О программе..."), wxT("О программе"));

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&Файл"));
    menuBar->Append(menuModule, wxT("&Модули"));
    menuBar->Append(menuTable, wxT("&Таблица"));
    menuBar->Append(menuHelp, wxT("&Помощь"));

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText(wxT("Добро пожаловать!!!"));

    Bind(wxEVT_MENU, &MainWindow::OnCreate, this, ID_Create);
    Bind(wxEVT_MENU, &MainWindow::OnSave, this, ID_Save);
    Bind(wxEVT_MENU, &MainWindow::OnSaveAs, this, ID_Save_as);
    Bind(wxEVT_MENU, &MainWindow::OnOpen, this, ID_Open);
    Bind(wxEVT_MENU, &MainWindow::OnExit, this, wxID_EXIT);

    Bind(wxEVT_MENU, &MainWindow::OnWindowCT, this, ID_Window_CT);

    Bind(wxEVT_MENU, &MainWindow::OnAddRow, this, ID_Add_row);
    Bind(wxEVT_MENU, &MainWindow::OnDeleteRow, this, ID_Delete_row);

    Bind(wxEVT_MENU, &MainWindow::OnAbout, this, wxID_ABOUT);  
      
    grid->SetTable(model, true);
     
   // grid->ForceRefresh();
    grid->SetGridLineColour(*wxBLACK);
    
    InitTableValidation();
    
    calcButton = new wxButton(this, wxID_ANY, wxT("Расчёт"));
    Bind(wxEVT_BUTTON, &MainWindow::Calc10Persent, this);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(grid, 0, wxALIGN_CENTER | wxALL, 10);
    
    mainSizer->Add(calcButton, 0, wxALIGN_CENTER | wxALL, 10);
    this->SetSizer(mainSizer);
   
    //Layout();

}

void MainWindow::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MainWindow::OnAbout(wxCommandEvent& event) {
    wxMessageBox(wxT("Это программа по расчету трансформаторов тока и трансформаторов напряжения\nВерсия 0.1.0"),
        wxT("О программе"), wxOK | wxICON_INFORMATION);
}


void MainWindow::saveCurves(const std::string& filename) {
    std::ofstream os(filename, std::ios::binary);
    if (!os.is_open()) {
        wxLogError("Не удалось открыть файл для записи: %s", filename);
    }
    try {
        cereal::BinaryOutputArchive archive(os);
        archive(limitFactorCurves);
    }
    catch (const std::exception& e) {
        wxLogError("Ошибка сериализации: %s", e.what());
        os.close();
    }
    os.close();
    if (os.fail()) {
        wxLogError("Ошибка при закрытии файла (возможно, не хватило места на диске)");
    }
}

void MainWindow::loadCurves(const std::string& filename) {
    std::ifstream is(filename, std::ios::binary);
    cereal::BinaryInputArchive archive(is);
    archive(limitFactorCurves);
}

void MainWindow::OnCreate(wxCommandEvent& event) {
    int result = wxMessageBox(wxT("Сохранить изменения?"),
        wxT("Подтверждение"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
    if (result == wxCANCEL)
        return;
    if (result == wxYES) {
        if (currentFilePath == "Без названия") {
            wxFileDialog saveFileDialog(this, wxT("Сохранить проект"), "", "new.tin",
                "TIN files (*.tin)|*.tin", wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);

            if (saveFileDialog.ShowModal() == wxID_CANCEL)
                return;     // user cancel dialog
            currentFilePath = saveFileDialog.GetPath();
        }
        model->saveToFile(currentFilePath);
    }

    currentFilePath = "Без названия";
    SetTitle("Rastin - " + currentFilePath);
    grid->SetTable(nullptr);
    model = new GridTableModel(limitFactorCurves);

    grid->SetTable(model, true);
    grid->SetGridLineColour(*wxBLACK);
    InitTableValidation();
    Fit();
}

void MainWindow::OnSave(wxCommandEvent& event) {
    if (currentFilePath == "Без названия") {
        wxFileDialog saveFileDialog(this, wxT("Сохранить проект"), "", "new.tin",
            "TIN files (*.tin)|*.tin", wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;     // user cancel dialog
        currentFilePath = saveFileDialog.GetPath();
        SetTitle("Rastin - " + wxFileName(currentFilePath).GetName());
    }
    model->saveToFile(currentFilePath);
}

void MainWindow::OnSaveAs(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, wxT("Сохранить проект"), "", "new.tin",
        "TIN files (*.tin)|*.tin", wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // user cancel dialog
    currentFilePath = saveFileDialog.GetPath();
    SetTitle("Rastin - " + wxFileName(currentFilePath).GetName());
    model->saveToFile(currentFilePath);

}

void MainWindow::OnOpen(wxCommandEvent& event) {
    int result = wxMessageBox(wxT("Сохранить изменения в текущем файле?"),
        wxT("Подтверждение"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
    if (result == wxCANCEL)
        return;
    if (result == wxYES) {
        if (currentFilePath == "Без названия") {
            wxFileDialog saveFileDialog(this, wxT("Сохранить проект"), "", "new.tin",
                "TIN files (*.tin)|*.tin", wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);

            if (saveFileDialog.ShowModal() == wxID_CANCEL)
                return;     // user cancel dialog
            currentFilePath = saveFileDialog.GetPath();
        }
        model->saveToFile(currentFilePath);
    }

    wxFileDialog openFileDialog(this, wxT("Открыть файл проекта"), "", "",
            "TIN files (*.tin)|*.tin", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // user cancel dialog
    currentFilePath = openFileDialog.GetPath();
    SetTitle("Rastin - " + wxFileName(currentFilePath).GetName());

    model->loadFromFile(currentFilePath);
    if (grid->GetNumberRows() < model->size()) {
        wxGridTableMessage msg(model, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, model->size() - grid->GetNumberRows());
        grid->ProcessTableMessage(msg);
    }
    if (grid->GetNumberRows() > model->size()) {
        wxGridTableMessage msg(model, wxGRIDTABLE_NOTIFY_ROWS_DELETED, model->size() - 1, grid->GetNumberRows() - model->size());
        grid->ProcessTableMessage(msg);
    }
    this->Fit();
#if 0
    wxGridCellAttr* attrWhite = new wxGridCellAttr();
    attrWhite->SetBackgroundColour(*wxWHITE);
    for (int row = 0; row < model->GetNumberRows(); ++row) {
        grid->SetRowAttr(row, attrWhite);
        attrWhite->IncRef();
        grid->Refresh();
    }
    attrWhite->DecRef();
#endif
    InitTableValidation();

    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        if (!limitFactorCurves.contains(grid->GetCellValue(row, 11))) {
            grid->SetCellValue(row, 11, "<Не выбрано>");
            grid->SetCellValue(row, 12, " ");
            grid->SetCellValue(row, 13, " ");
        }
    }
}

void MainWindow::Calc10Persent(wxCommandEvent& event) {
    
    wxGridCellAttr* attrGreen = new wxGridCellAttr();
    wxGridCellAttr* attrRed = new wxGridCellAttr();
    attrGreen->SetBackgroundColour(*wxGREEN);
    attrRed->SetBackgroundColour(*wxRED);
    for (int row = 0; row < model->GetNumberRows(); ++row) {
        if (model->calcRow(row)) {
            grid->SetRowAttr(row, attrGreen);
            attrGreen->IncRef();
            grid->Refresh();
        }
        else {
            grid->SetRowAttr(row, attrRed);
            attrRed->IncRef();
            grid->Refresh();
        }
    }
    attrGreen->DecRef();
    attrRed->DecRef();
    wxLogMessage(wxT("Расчет окончен!!!"));
}

void MainWindow::OnWindowCT(wxCommandEvent& event) {
    CTWindow* window = new CTWindow(this, limitFactorCurves);
    window->Bind(wxEVT_CLOSE_WINDOW, [window, this](wxCloseEvent& evt) {
        window->Destroy();
        wxArrayString choiceLimitCurves;
        choiceLimitCurves.Add("<Не выбрано>");
        for (auto& curve : limitFactorCurves) {
            choiceLimitCurves.Add(curve.first);
        }
        wxGridCellAttr* attrLimitCurves = new wxGridCellAttr();
        wxGridCellChoiceEditor* editorLimitCurves = new wxGridCellChoiceEditor(choiceLimitCurves);
        attrLimitCurves->SetEditor(editorLimitCurves);
        for (int row = 0; row < model->GetNumberRows(); ++row) {
            grid->SetAttr(row, 11, attrLimitCurves);
            attrLimitCurves->IncRef();
            grid->Refresh();
        }
        attrLimitCurves->DecRef();
        for (int row = 0; row < grid->GetNumberRows(); ++row) {
            if (!limitFactorCurves.contains(grid->GetCellValue(row, 11))) {
                grid->SetCellValue(row, 11, "<Не выбрано>");
                grid->SetCellValue(row, 12, " ");
                grid->SetCellValue(row, 13, " ");
            } 
        }
        });
    window->Center();
    window->ShowModal();
}

void MainWindow::OnAddRow(wxCommandEvent& event) {
    model->AppendRows();
    wxGridTableMessage msg(model, wxGRIDTABLE_NOTIFY_ROWS_INSERTED, model->size() - 1, 1);
    grid->ProcessTableMessage(msg);
    this->Fit();

    int newRow = grid->GetNumberRows() - 1;
    MyGridCellFloatEditor* editor = new MyGridCellFloatEditor;
    for (int col = 0; col < grid->GetNumberCols(); ++col) {
        if (col == 0 || col == 2 || col == 3 || col == 5 || col == 6 || col == 8 || col == 9 || col == 10) {
            grid->SetCellEditor(newRow, col, editor);
            editor->IncRef();
        }
    }
    editor->DecRef();

    grid->SetReadOnly(newRow, 12, true);
    grid->SetReadOnly(newRow, 13, true);

    wxArrayString choiceSecondaryCT{ "1", "5" };

    wxArrayString choiceLimitCurves;
    choiceLimitCurves.Add("<Не выбрано>");
    for (auto& curve : limitFactorCurves) {
        choiceLimitCurves.Add(curve.first);
    }

    wxArrayString choiceCrossSectionArea{ "1.5", "2.5", "4.0", "6.0" };

    wxGridCellAttr* attrSecondaryCT = new wxGridCellAttr();
    wxGridCellAttr* attrLimitCurves = new wxGridCellAttr();
    wxGridCellAttr* attrCrossSectionArea = new wxGridCellAttr();
    wxGridCellChoiceEditor* editorSecondaryCT = new wxGridCellChoiceEditor(choiceSecondaryCT);
    wxGridCellChoiceEditor* editorLimitCurves = new wxGridCellChoiceEditor(choiceLimitCurves);
    wxGridCellChoiceEditor* editorCrossSectionAre = new wxGridCellChoiceEditor(choiceCrossSectionArea);
    attrSecondaryCT->SetEditor(editorSecondaryCT);
    attrLimitCurves->SetEditor(editorLimitCurves);
    attrCrossSectionArea->SetEditor(editorCrossSectionAre);

    grid->SetAttr(newRow, 1, attrSecondaryCT);
    grid->SetAttr(newRow, 11, attrLimitCurves);
    grid->SetAttr(newRow, 7, attrCrossSectionArea);
    
    grid->Refresh();
   }

void MainWindow::OnDeleteRow(wxCommandEvent& event) {

    int rowCount = grid->GetNumberRows();
    if (rowCount == 0) {
        wxMessageBox(wxT("Нет строк для удаления"), wxT("Информация"));
        return;
    }
    int lastRow = rowCount - 1;
    if (model->DeleteRows(lastRow, 1)) {
        wxGridTableMessage msg(model, wxGRIDTABLE_NOTIFY_ROWS_DELETED, lastRow, 1);
        grid->ProcessTableMessage(msg);
        Fit();
    }
}

void MainWindow::InitTableValidation() {
    MyGridCellFloatEditor* editor = new MyGridCellFloatEditor;
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        for (int col = 0; col < grid->GetNumberCols(); ++col) {
            if (col == 0 || col == 2 || col == 3 || col == 5 || col == 6 || col == 8 || col == 9 || col == 10) {
                grid->SetCellEditor(row, col, editor);
                editor->IncRef();
            }
        }
    }
    editor->DecRef();
   // grid->SetColFormatNumber(0);
   // grid->SetColFormatNumber(2);
   // grid->SetColFormatNumber(3);
   // grid->SetColFormatFloat(5, -1, 2);
   // grid->SetColFormatFloat(6, -1, 2);
   // grid->SetColFormatFloat(8, -1, 2);
   // grid->SetColFormatFloat(9, -1, 2);
   // grid->SetColFormatFloat(10, -1, 2);
    

    for (int row = 0; row < model->GetNumberRows(); ++row) {
        grid->SetReadOnly(row, 12, true);
        grid->SetReadOnly(row, 13, true);
    }
    
    wxArrayString choiceSecondaryCT{ "1", "5" };

    wxArrayString choiceLimitCurves;
    choiceLimitCurves.Add("<Не выбрано>");
    for (auto& curve : limitFactorCurves) {
        choiceLimitCurves.Add(curve.first);
    }
   
    wxArrayString choiceCrossSectionArea{ "1.5", "2.5", "4.0", "6.0" };

    wxGridCellAttr* attrSecondaryCT = new wxGridCellAttr();
    wxGridCellAttr* attrLimitCurves = new wxGridCellAttr();
    wxGridCellAttr* attrCrossSectionArea = new wxGridCellAttr();
    wxGridCellChoiceEditor* editorSecondaryCT = new wxGridCellChoiceEditor(choiceSecondaryCT);
    wxGridCellChoiceEditor* editorLimitCurves = new wxGridCellChoiceEditor(choiceLimitCurves);
    wxGridCellChoiceEditor* editorCrossSectionAre = new wxGridCellChoiceEditor(choiceCrossSectionArea);
    attrSecondaryCT->SetEditor(editorSecondaryCT);
    attrLimitCurves->SetEditor(editorLimitCurves);
    attrCrossSectionArea->SetEditor(editorCrossSectionAre);
    for (int row = 0; row < model->GetNumberRows(); ++row) {
        grid->SetAttr(row, 1, attrSecondaryCT);
        grid->SetAttr(row, 11, attrLimitCurves);
        grid->SetAttr(row, 7, attrCrossSectionArea);
        attrSecondaryCT->IncRef();
        attrLimitCurves->IncRef();
        attrCrossSectionArea->IncRef();
        grid->Refresh();
    }
    attrSecondaryCT->DecRef();
    attrLimitCurves->DecRef();
    attrCrossSectionArea->DecRef();
}

void MainWindow::InitLimitFactorCurves() {
   // Spline temp;
   // std::vector<double> x1 = { 5, 7.4, 9, 15, 20, 25, 30 };
   // std::vector<double> y1 = { 33, 25, 20, 11.4, 8.7, 7.5, 6.7 };
   // std::vector<double> x2 = { 5, 7.8, 10, 15, 20, 25, 30 };
   // std::vector<double> y2 = { 35, 24, 19, 10, 7.3, 5.5, 3.3 };
  //  std::vector<double> x3 = { 6, 10 };
  //  std::vector<double> y3 = { 30, 20 };

  //  temp.init(x1, y1);
  // limitFactorCurves.emplace("Не выбрано", temp);
  //  temp.init(x2, y2);
  //  limitFactorCurves.emplace("My_spline2", temp);
  //  temp.init(x3, y3);
  //  limitFactorCurves.emplace("My_spline3", temp);
  //  
    loadCurves("curves.bin");
    
  
}

MainWindow::~MainWindow() {
    saveCurves("curves.bin");
}