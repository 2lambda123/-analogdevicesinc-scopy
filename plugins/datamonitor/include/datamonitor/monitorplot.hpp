#ifndef MONITORPLOT_H
#define MONITORPLOT_H

#include <QWidget>
#include <QMap>
#include <plotwidget.h>
#include <plotaxis.h>
#include <datamonitormodel.hpp>
#include "scopy-datamonitor_export.h"

namespace scopy {

class TimePlotInfo;
namespace datamonitor {

class MonitorPlotCurve;

class SCOPY_DATAMONITOR_EXPORT MonitorPlot : public QWidget
{
	Q_OBJECT
public:
	explicit MonitorPlot(QWidget *parent = nullptr);

	PlotWidget *plot() const;
	void addMonitor(DataMonitorModel *dataMonitorModel);
	void removeMonitor(QString monitorName);
	void toggleMonitor(bool toggled, QString monitorName);
	bool hasMonitor(QString title);

	void changeCurveStyle(int style);
	void changeCurveThickness(double thickness);

	void updateXAxisIntervalMin(double min);
	void updateXAxisIntervalMax(double max);
	void updateYAxisIntervalMin(double min);
	void updateYAxisIntervalMax(double max);
	void plotYAxisAutoscale();
	double getYAxisIntervalMin();
	double getYAxisIntervalMax();

private:
	PlotWidget *m_plot;
	TimePlotInfo *m_plotInfo;
	QMap<QString, MonitorPlotCurve *> *m_monitorCurves;
	bool m_firstMonitor = true;

	int m_currentCurveStyle = 0;
	double m_currentCurveThickness = 1;
};
} // namespace datamonitor
} // namespace scopy
#endif // MONITORPLOT_H