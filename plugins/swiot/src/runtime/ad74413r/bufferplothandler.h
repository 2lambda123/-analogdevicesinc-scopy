#ifndef BUFFERPLOTHANDLER_H
#define BUFFERPLOTHANDLER_H

#include <gui/oscilloscope_plot.hpp>
#include <qobject.h>
#include "bufferlogic.h"
#include "linked_button.hpp"
#include <qmutex.h>
#include <QWidget>
#include <deque>

namespace scopy::swiot {
#define DIAG_CHNLS_NUMBER 4
class BufferPlotHandler : public QWidget
{
	Q_OBJECT
public:
	explicit BufferPlotHandler(QWidget *parent = nullptr, int plotChnlsNo = 0, int sampleRate = 0);
	~BufferPlotHandler();

	QColor getCurveColor(int id) const;

	void setSingleCapture(bool en);
	void deleteResidualPlotData();
	void resetPlotParameters();
	void drawPlot();
	void setChnlsUnitOfMeasure(QVector<QString> unitsOfMeasure);
	void setChnlsRangeValues(QVector<std::pair<int, int>> rangeValues);
	void setHandlesName(QMap<int, QString> chnlsId);
	QWidget *getPlotWidget() const;
	bool singleCapture() const;

public Q_SLOTS:
	void onBufferRefilled(QVector<QVector<double>>, int bufferCounter);
	void onBtnExportClicked(QMap<int, bool> exportConfig);
	void onTimespanChanged(double value);
	void onSamplingFreqWritten(int samplingFreq);
	void onChannelWidgetEnabled(int curveId, bool en);
	void onChannelWidgetSelected(int curveId);
	void onPrintBtnClicked();
Q_SIGNALS:
	void singleCaptureFinished();
	void offsetHandleSelected(int hdlIdx, bool selected);
private:
	void initPlot(int plotChnlsNo);
	void initStatusWidget();
	void resetDataPoints();
	void resetDeque();
	void readPreferences();
	void updatePlotTimespan();

	bool eventFilter(QObject *obj, QEvent *event);

	CapturePlot *m_plot;
	QWidget *m_plotWidget;
	QLabel *m_samplesAquiredLabel;
	QLabel *m_plotSamplesNumber;
	LinkedButton* m_btnInfoStatus;

	int m_samplingFreq = 4800;
	double m_timespan = 1;
	int m_plotChnlsNo;

	//all of these will be calculated in functions; for example we will have a spinbox for timespan,
	//and in a slot we will set its value and we will calculate plotSampleRate and the number of necessary buffers
	int m_buffersNumber = 0;
	int m_bufferIndex = 0;
	int m_bufferSize = 0;

	bool m_singleCapture = false;

	std::vector<std::deque<QVector<double>>> m_dataPointsDeque;
	std::vector<double*> m_dataPoints;
	std::vector<bool> m_enabledPlots;
	QMutex *m_lock;


};
}

#endif // BUFFERPLOTHANDLER_H
