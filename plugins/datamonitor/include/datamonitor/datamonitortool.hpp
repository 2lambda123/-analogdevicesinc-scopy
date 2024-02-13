#ifndef DATAMONITORTOOL_HPP
#define DATAMONITORTOOL_HPP

#include "dataacquisitionmanager.hpp"

#include "iio.h"
#include <QPushButton>
#include <QWidget>
#include <tooltemplate.h>
#include <toolbuttons.h>
#include <QMap>
#include <QScrollArea>

namespace scopy {

class CollapsableMenuControlButton;
class FlexGridLayout;

namespace datamonitor {

class DataMonitorModel;

class DataMonitorTool : public QWidget
{
	Q_OBJECT
public:
	explicit DataMonitorTool(iio_context *ctx, QWidget *parent = nullptr);

	RunBtn *getRunButton() const;

	void initDataMonitor();

private:
	ToolTemplate *tool;
	QPushButton *openLatMenuBtn;
	GearBtn *settingsButton;
	InfoBtn *infoBtn;
	RunBtn *runBtn;
	SingleShotBtn *singleBtn;

	FlexGridLayout *m_flexGridLayout;
	QScrollArea *m_scrollArea;
	void generateMonitor(DataMonitorModel *model, CollapsableMenuControlButton *channelManager);

	QButtonGroup *grp;

	DataAcquisitionManager *dataAcquisitionManager;

	iio_context *ctx;
};
} // namespace datamonitor
} // namespace scopy
#endif // DATAMONITORTOOL_HPP
