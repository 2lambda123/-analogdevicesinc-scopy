#ifndef PLOTAUTOSCALER_H
#define PLOTAUTOSCALER_H

#include "scopy-gui_export.h"

#include <QObject>
#include <QWidget>

#include <plotchannel.h>
#include <spinbox_a.hpp>

namespace scopy::gui {

class SCOPY_GUI_EXPORT PlotAutoscaler : public QObject
{
	Q_OBJECT
public:
	PlotAutoscaler(QObject *parent = nullptr);
	~PlotAutoscaler();

Q_SIGNALS:
	void newMin(double);
	void newMax(double);

public Q_SLOTS:
	void start();
	void stop();
	void autoscale();
	bool xAxisMode() const;
	void setXAxisMode(bool newXAxis);
	double tolerance() const;
	void setTolerance(double newTolerance);
	void addChannels(PlotChannel *);
	void removeChannels(PlotChannel *);

private:
	QTimer *m_autoScaleTimer;
	QList<PlotChannel *> m_channels;
	bool m_xAxisMode;
	double m_tolerance;
};
} // namespace scopy::gui
#endif // PLOTAUTOSCALER_H
