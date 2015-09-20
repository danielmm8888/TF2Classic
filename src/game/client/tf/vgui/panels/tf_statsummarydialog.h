#ifndef TF_STATSSUMMARYDIALOG_H
#define TF_STATSSUMMARYDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_dialogpanelbase.h"
#include "tf_hud_statpanel.h"
#include "GameEventListener.h"

class CTFStatsSummaryDialog : public CTFDialogPanelBase
{
private:
	DECLARE_CLASS_SIMPLE(CTFStatsSummaryDialog, CTFDialogPanelBase);

public:
	CTFStatsSummaryDialog(vgui::Panel* parent, const char *panelName);
	bool Init();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void OnCommand(const char *command);
	virtual void OnKeyCodePressed(KeyCode code);
	virtual void PerformLayout();
	void SetStats(CUtlVector<ClassStats_t> &vecClassStats);
	void Show();
private:

	enum StatDisplay_t
	{
		SHOW_MAX = 1,
		SHOW_TOTAL,
		SHOW_AVG
	};

	void Reset();
	void SetDefaultSelections();
	void UpdateDialog();
	void UpdateBarCharts();
	void UpdateClassDetails();
	void UpdateControls();
	void InitBarChartComboBox(vgui::ComboBox *pComboBox);
	void SetValueAsClass(const char *pDialogVariable, int iValue, int iPlayerClass);
	void DisplayBarValue(int iChart, int iClass, ClassStats_t &stats, TFStatType_t statType, StatDisplay_t flags, float flMaxValue);
	static float GetDisplayValue(ClassStats_t &stats, TFStatType_t statType, StatDisplay_t statDisplay);
	const char *RenderValue(float flValue, TFStatType_t statType, StatDisplay_t statDisplay);
	static float SafeCalcFraction(float flNumerator, float flDemoninator);
	static int __cdecl CompareClassStats(const ClassStats_t *pStats0, const ClassStats_t *pStats1);
	MESSAGE_FUNC_PARAMS(OnTextChanged, "TextChanged", data);

	static char *FormatSeconds(int seconds);

	vgui::EditablePanel *m_pPlayerData;

	//vgui::EditablePanel *m_pInteractiveHeaders;
	vgui::ComboBox *m_pBarChartComboBoxA;
	vgui::ComboBox *m_pBarChartComboBoxB;
	vgui::ComboBox *m_pClassComboBox;
	
	bool m_bControlsLoaded;							// have we loaded controls yet
	CUtlVector<ClassStats_t> m_aClassStats;			// stats data
	int m_xStartLHBar;								// x min of bars in left hand bar chart
	int m_xStartRHBar;								// x min of bars in right hand bar chart
	int m_iBarMaxWidth;								// width of bars in bar charts
	int m_iBarHeight;								// height of bars in bar charts

	int m_iSelectedClass;							// what class is selected, if any
	int m_iTotalSpawns;								// how many spawns of all classes does this player have
	TFStatType_t m_statBarGraph[2];					// what stat is displayed in the left hand and right hand bar graphs
	StatDisplay_t m_displayBarGraph[2];				// the display type for the left hand and right hand bar graphs
};


#endif // TF_STATSSUMMARY_H