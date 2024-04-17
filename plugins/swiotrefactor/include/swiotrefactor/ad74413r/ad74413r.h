/*
 * Copyright (c) 2023 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see https://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef AD74413R_H
#define AD74413R_H

#include "bufferlogic.h"
#include "bufferacquisitionhandler.h"
#include "pluginbase/toolmenuentry.h"
#include "readerthread.h"

#include <iio.h>

#include <gui/mapstackedwidget.h>
#include <gui/widgets/toolbuttons.h>
#include <gui/tooltemplate.h>
#include <gui/widgets/menucontrolbutton.h>
#include <gui/plotaxis.h>

#include <QVector>
#include <QWidget>
#include <spinbox_a.hpp>

#include <iioutil/connection.h>
#define MAX_CURVES_NUMBER 8
#define AD_NAME "ad74413r"
#define SWIOT_DEVICE_NAME "swiot"
#define MAX_SAMPLE_RATE 4800

namespace scopy {

namespace swiotrefactor {
class Ad74413r : public QWidget
{
	Q_OBJECT
public:
	explicit Ad74413r(QString uri = "", ToolMenuEntry *tme = 0, QWidget *parent = nullptr);

	~Ad74413r();

public Q_SLOTS:
	void onRunBtnPressed(bool toggled);
	void onSingleBtnPressed(bool toggled);

	void onReaderThreadFinished();
	void onSingleCaptureFinished();

	void onDiagnosticFunctionUpdated();

	void onActivateRunBtns(bool activate);

	void handleConnectionDestroyed();

	void onSamplingFrequencyUpdated(int channelId, int sampFreq);

Q_SIGNALS:
	void timespanChanged(double val);
	void broadcastReadThreshold();

	void exportBtnClicked(QMap<int, bool> exportConfig);

	void activateExportButton();
	void activateRunBtns(bool activate);

	void backBtnPressed();
private Q_SLOTS:
	void onBackBtnPressed();
	void refreshSampleRate();
	void showPlotLabels(bool b);
	void setupChannel(int chnlIdx, QString function);
	void onBufferRefilled(QMap<int, QVector<double>> bufferData);
	void onChannelBtnChecked(int chnWidgetId, bool en);

private:
	void createDevicesMap(iio_context *ctx);
	void setupConnections();
	void verifyChnlsChanges();
	void initTutorialProperties();
	void initPlotData();
	void setupToolTemplate();
	void initPlot();
	void setupDeviceBtn();
	void setupChannelBtn(MenuControlButton *btn, PlotChannel *ch, QString chnlId, int chnlIdx);
	void setupChannelsMenuControlBtn(MenuControlButton *btn, QString name);
	QPushButton *createBackBtn();
	QWidget *createSettingsMenu(QWidget *parent);
	PlotAxis *createYChnlAxis(QPen pen, QString unitType = "V", int yMin = -1, int yMax = 1);

private:
	ToolMenuEntry *m_tme;
	PositionSpinButton *m_timespanSpin;

	QVector<bool> m_enabledChannels;

	QString m_uri;

	BufferLogic *m_swiotAdLogic;
	ReaderThread *m_readerThread;
	BufferAcquisitionHandler *m_acqHandler;
	CommandQueue *m_cmdQueue;

	//	ExportSettings *m_exportSettings;

	struct iio_context *m_ctx;
	Connection *m_conn;

	////
	/// \brief Added for the new adinstrument
	///
	double m_frequency = 1.0;
	double m_sampleRate = 4800;
	ToolTemplate *m_tool;
	RunBtn *m_runBtn;
	SingleShotBtn *m_singleBtn;
	PrintBtn *m_printBtn;
	QPushButton *m_backBtn;
	PlotWidget *m_plot;
	GearBtn *m_settingsBtn;

	QMap<QString, iio_device *> m_iioDevicesMap;
	CollapsableMenuControlButton *m_devBtn;
	QButtonGroup *m_rightMenuBtnGrp;
	QButtonGroup *m_chnlsBtnGroup;

	MapStackedWidget *m_channelStack;

	int m_currentChannelSelected = 0;
	QMap<int, QVector<double>> m_yValues;
	QVector<double> m_xTime;

	const QString channelsMenuId = "channels";
	/////////////////
};
} // namespace swiotrefactor
} // namespace scopy
#endif // AD74413R_H
