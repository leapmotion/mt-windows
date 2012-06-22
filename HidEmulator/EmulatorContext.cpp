#include "StdAfx.h"
#include "EmulatorContext.h"
#include "Emulator.h"

CEmulatorContext::CEmulatorContext(CEmulator* pParent):
	m_pParent(pParent)
{
}

CEmulatorContext::~CEmulatorContext(void)
{
	HID_SYNTEHSIZE_REPORT report;
	memset(&report, 0, sizeof(report));
	report.SynthesizedReportID = REPORTID_RESETTOUCHSTATE;

	// Run through each state in the touchState array:
	for(BYTE i = 0; i < ARRAYSIZE(m_state.touchState); i++)
	{
		auto& cur = m_state.touchState[i];
		if(!cur.TipSwitch)
			// No need to reset this finger:
			continue;

		// The finger needs to have a regenerated state that is equal
		// to the current position, minus whatever keys are assigned at
		// this moment.
		auto& reset = report.ResetReport;
		reset.ResetContactID[reset.ActualInputs] = i;

		if(++reset.ActualInputs == 2)
			// Emulate this report.
		{
			m_pParent->QueueSynthesisToSelf(report);
			reset.ActualInputs = 0;
		}
	}

	// Tail case, if there's anything still unsynthesized
	if(report.ResetReport.ActualInputs)
		m_pParent->QueueSynthesisToSelf(report);

	// Reset mouse events as needed
	if(m_state.mouseState.Button1 || m_state.mouseState.Button2)
	{
		report.SynthesizedReportID = REPORTID_RESETMOUSESTATE;
		m_pParent->QueueSynthesisToSelf(report);
	}
}
