#include "adcplugin.h"

#include "gui/stylehelper.h"
#include "src/adctimeinstrument.h"

#include <QBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QLoggingCategory>
#include <QPushButton>
#include <QSpacerItem>

#include <iioutil/connectionprovider.h>
#include <pluginbase/preferences.h>
#include <pluginbase/preferenceshelper.h>
#include <widgets/menucollapsesection.h>
#include <widgets/menusectionwidget.h>

#include "timeplotproxy.h"

Q_LOGGING_CATEGORY(CAT_ADCPLUGIN, "ADCPlugin");
using namespace scopy;
using namespace scopy::grutil;
using namespace scopy::adc;

bool ADCPlugin::compatible(QString m_param, QString category)
{
	qDebug(CAT_ADCPLUGIN) << "compatible";

	bool ret = false;
	Connection *conn = ConnectionProvider::GetInstance()->open(m_param);

	if(conn == nullptr)
		return ret;

	for(int i = 0; i < iio_context_get_devices_count(conn->context()); i++) {
		iio_device *dev = iio_context_get_device(conn->context(), i);
		for(int j = 0; j < iio_device_get_channels_count(dev); j++) {
			struct iio_channel *chn = iio_device_get_channel(dev, j);
			if(iio_channel_is_scan_element(chn)) {
				ret = true;
				goto finish;
			}
		}
	}

finish:

	ConnectionProvider::GetInstance()->close(m_param);
	return ret;
}

void ADCPlugin::initPreferences()
{
	Preferences *p = Preferences::GetInstance();
	p->init("adc_plot_xaxis_label_position", QwtAxis::XBottom);
	p->init("adc_plot_yaxis_label_position", QwtAxis::YLeft);
	p->init("adc_plot_yaxis_handle_position", QwtAxis::YLeft);
	p->init("adc_plot_xcursor_position", QwtAxis::XBottom);
	p->init("adc_plot_ycursor_position", QwtAxis::YLeft);
	p->init("adc_plot_show_buffer_previewer", true);
	p->init("adc_default_y_mode", 0);
}

bool ADCPlugin::loadPreferencesPage()
{
	Preferences *p = Preferences::GetInstance();

	m_preferencesPage = new QWidget();
	QVBoxLayout *lay = new QVBoxLayout(m_preferencesPage);

	MenuSectionWidget *generalWidget = new MenuSectionWidget(m_preferencesPage);
	MenuCollapseSection *generalSection =
		new MenuCollapseSection("General", MenuCollapseSection::MHCW_NONE, generalWidget);
	generalWidget->contentLayout()->setSpacing(10);
	generalWidget->contentLayout()->addWidget(generalSection);
	generalSection->contentLayout()->setSpacing(10);
	lay->addWidget(generalWidget);
	lay->setMargin(0);
	lay->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	auto adc_plot_xaxis_label_position = PreferencesHelper::addPreferenceComboList(
		p, "adc_plot_xaxis_label_position", "Plot X-Axis scale position",
		{{"Top", QwtAxis::XTop}, {"Bottom", QwtAxis::XBottom}}, generalSection);
	auto adc_plot_yaxis_label_position = PreferencesHelper::addPreferenceComboList(
		p, "adc_plot_yaxis_label_position", "Plot Y-Axis scale position",
		{{"Left", QwtAxis::YLeft}, {"Right", QwtAxis::YRight}}, generalSection);
	auto adc_plot_yaxis_handle_position = PreferencesHelper::addPreferenceComboList(
		p, "adc_plot_yaxis_handle_position", "Plot channel Y-handle position",
		{{"Left", QwtAxis::YLeft}, {"Right", QwtAxis::YRight}}, generalSection);
	auto adc_plot_xcursor_position = PreferencesHelper::addPreferenceComboList(
		p, "adc_plot_xcursor_position", "Plot X-Cursor position",
		{{"Top", QwtAxis::XTop}, {"Bottom", QwtAxis::XBottom}}, generalSection);
	auto adc_plot_ycursor_position = PreferencesHelper::addPreferenceComboList(
		p, "adc_plot_ycursor_position", "Plot Y-Curosr position",
		{{"Left", QwtAxis::YLeft}, {"Right", QwtAxis::YRight}}, generalSection);
	auto adc_plot_show_buffer_previewer = PreferencesHelper::addPreferenceCheckBox(
		p, "adc_plot_show_buffer_previewer", "Show buffer previewer", m_preferencesPage);

	auto adc_default_y_mode = PreferencesHelper::addPreferenceComboList(
		p, "adc_default_y_mode", "ADC Default Y-Mode", {{"ADC Count", 0}, {"% Full Scale", 1}}, generalSection);

	generalSection->contentLayout()->addWidget(adc_plot_xaxis_label_position);
	generalSection->contentLayout()->addWidget(adc_plot_yaxis_label_position);
	generalSection->contentLayout()->addWidget(adc_plot_yaxis_handle_position);
	generalSection->contentLayout()->addWidget(adc_plot_xcursor_position);
	generalSection->contentLayout()->addWidget(adc_plot_ycursor_position);
	generalSection->contentLayout()->addWidget(adc_plot_show_buffer_previewer);
	generalSection->contentLayout()->addWidget(adc_default_y_mode);
	//	connect(p, &Preferences::preferenceChanged, )

	return true;
}

bool ADCPlugin::loadIcon()
{
	SCOPY_PLUGIN_ICON(":/gui/icons/scopy-default/icons/tool_oscilloscope.svg");
	return true;
}

bool ADCPlugin::loadPage()
{
	m_page = new QWidget();
	QVBoxLayout *lay = new QVBoxLayout(m_page);
	return true;
}

void ADCPlugin::loadToolList()
{
	m_toolList.append(
		SCOPY_NEW_TOOLMENUENTRY("time", "Time", ":/gui/icons/scopy-default/icons/tool_oscilloscope.svg"));
}

bool iio_is_buffer_capable(struct iio_device *dev) {
	for(int j = 0; j < iio_device_get_channels_count(dev); j++) {
		struct iio_channel *chn = iio_device_get_channel(dev, j);
		if(!iio_channel_is_output(chn) && iio_channel_is_scan_element(chn)) {
			return true;
		}
	}
	return false;
}

void ADCPlugin::createGRIIOTreeNode(GRTopBlockNode* ctxNode, iio_context *ctx)
{
	int devCount = iio_context_get_devices_count(ctx);
	qDebug(CAT_ADCPLUGIN) << " Found " << devCount << "devices";
	for(int i = 0; i < devCount; i++) {
		iio_device *dev = iio_context_get_device(ctx, i);
		QString dev_name = QString::fromLocal8Bit(iio_device_get_name(dev));

		qDebug(CAT_ADCPLUGIN) << "Looking for scanelements in " << dev_name;
		/*if(dev_name == "m2k-logic-analyzer-rx")
			continue;*/

		QStringList channelList;

		GRIIODeviceSource *gr_dev = new GRIIODeviceSource(ctx, dev_name, dev_name, 0x400, ctxNode);
		GRIIODeviceSourceNode *d = new GRIIODeviceSourceNode(ctxNode, gr_dev, gr_dev);

		if(iio_is_buffer_capable(dev)) { // at least one scan element
			ctxNode->addTreeChild(d);
			ctxNode->src()->registerIIODeviceSource(gr_dev);
		} else {
			continue;
		}

		for(int j = 0; j < iio_device_get_channels_count(dev); j++) {
			struct iio_channel *chn = iio_device_get_channel(dev, j);
			QString chn_name = QString::fromLocal8Bit(iio_channel_get_id(chn));
			qDebug(CAT_ADCPLUGIN) << "Verify if " << chn_name << "is scan element";
			if(chn_name == "timestamp" /*|| chn_name == "accel_z" || chn_name =="accel_y"*/)
				continue;
			if(!iio_channel_is_output(chn) && iio_channel_is_scan_element(chn)) {

				GRIIOFloatChannelSrc *ch = new GRIIOFloatChannelSrc(gr_dev, chn_name, d);
				GRIIOFloatChannelNode *c = new GRIIOFloatChannelNode(ctxNode, ch, d);
				d->addTreeChild(c);
			}
		}
	}
}

bool ADCPlugin::onConnect()
{
	Connection *conn = ConnectionProvider::GetInstance()->open(m_param);
	if(conn == nullptr)
		return false;
	m_ctx = conn->context();
	m_toolList[0]->setEnabled(true);
	m_toolList[0]->setRunBtnVisible(true);

	// create gnuradio flow out of channels
	// pass channels to ADC instrument - figure out channel model (sample rate/ size/ etc)
	AcqTreeNode *root = new AcqTreeNode("root",this);
	GRTopBlock *top = new GRTopBlock("ctx", this);
	GRTopBlockNode *ctxNode = new GRTopBlockNode(top, nullptr);
	root->addTreeChild(ctxNode);
	auto timeProxy = new TimePlotProxy("time0",root,this);
	time = new ADCTimeInstrument(timeProxy);
	connect(root,&AcqTreeNode::newChild,timeProxy,&TimePlotProxy::addChannel);
	createGRIIOTreeNode(ctxNode, m_ctx);
	// root->treeChildren()[0]->addTreeChild(new AcqTreeNode("other"));



	m_toolList[0]->setTool(time);

	return true;
}

bool ADCPlugin::onDisconnect()
{
	qDebug(CAT_ADCPLUGIN) << "disconnect";
	if(m_ctx)
		ConnectionProvider::GetInstance()->close(m_param);
	for(auto &tool : m_toolList) {
		tool->setEnabled(false);
		tool->setRunBtnVisible(false);
		QWidget *w = tool->tool();
		if(w) {
			tool->setTool(nullptr);
			delete(w);
		}
	}
	return true;
}

void ADCPlugin::saveSettings(QSettings &s) {}

void ADCPlugin::loadSettings(QSettings &s) {}

QString ADCPlugin::about()
{
	QString content = "ADC plugin";
	return content;
}

void ADCPlugin::initMetadata()
{
	loadMetadata(
		R"plugin(
	{
	   "priority":10,
	   "category":[
		  "iio",
		  "adc"
	   ]
	}
)plugin");
}

QString ADCPlugin::version() { return "0.1"; }
