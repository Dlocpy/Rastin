#pragma once
#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/string.h>
#include <vector>
#include <map>

#include "ctwinding.h"
#include "Spline.h"

class GridTableModel : public wxGridTableBase
{
public:
	GridTableModel(std::map<wxString, Spline>& limitFactorCurves);

	virtual int GetNumberRows();
	virtual int GetNumberCols();
	virtual bool IsEmptyCell(int row, int col);
	virtual wxString GetValue(int row, int col);
	virtual void SetValue(int row, int col, const wxString& value);

	virtual wxString GetColLabelValue(int col);
	virtual wxString GetRowLabelValue(int row);

	virtual bool AppendRows(size_t numRows = 1) override;
	virtual bool DeleteRows(size_t pos, size_t numRows) override;

	size_t size() const { return m_dataTable.size(); }

	CTWinding* data() { return m_dataTable.data(); }
	const CTWinding* data() const { return m_dataTable.data(); }

	bool saveToFile(const wxString& filename);
	bool loadFromFile(const wxString& filename);
	bool calcRow(int row) const;

private:
	std::vector<CTWinding> m_dataTable;
	std::vector<wxString> m_colLabels;
	std::map<wxString, Spline>& m_curves;
	
	void initData();
	void CalcLimitFactorCurvesRow(int row);
};

