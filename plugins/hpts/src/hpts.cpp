#include "hpts.h"
#include <QHBoxLayout>
#include <QTimer>
#include <QDebug>
#include <QComboBox>
#include <gui/plotaxis.h>
#include <QwtPlotLayout>
#include <gui/stylehelper.h>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(CAT_HPTS_TOOL, "HPTS_TOOL");
using namespace scopy::HockeyPuckTempSensor;
using namespace scopy::gui;
HockeyPuckTempSensor::HockeyPuckTempSensor(iio_context *ctx, QWidget *parent) : m_ctx(ctx), QWidget(parent) {

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QHBoxLayout *layout = new QHBoxLayout(this);
	setLayout(layout);

	       // tool template configuration
	m_tool = new ToolTemplate(this);
	m_tool->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_tool->centralContainer()->setVisible(true);
	m_tool->topContainerMenuControl()->setVisible(false);

	m_dev = iio_context_find_device(m_ctx, HPTS_DEV_NAME);
	if(m_dev == nullptr) {
		qWarning(CAT_HPTS_TOOL) << "Device " << HPTS_DEV_NAME <<" not found";
	}
	m_ch = iio_device_find_channel(m_dev,HPTS_CH_NAME,false);

	if(m_ch == nullptr) {
		qWarning(CAT_HPTS_TOOL) << "Channel " << HPTS_CH_NAME <<" not found";
	}

	initData();
	setupUiElements();
	m_tool->addWidgetToCentralContainerHelper(tempWidget);
	layout->addWidget(m_tool);
	tim = new QTimer(this);
	connect(tim, &QTimer::timeout, this, &HockeyPuckTempSensor::refresh);
	tim->start(1000);
}

void HockeyPuckTempSensor::initData(){
	datax.clear();
	datay.clear();
	deque.clear();
	datax.reserve(HPTS_NR_SAMPLES);
	datay.reserve(HPTS_NR_SAMPLES);

	for(int i = 0; i < HPTS_NR_SAMPLES; i++){
		deque.push_back(i);
		datay.push_back(i);
		datax.push_back(i);
	}
}

void HockeyPuckTempSensor::setMin(double val){
	plot->yAxis()->setMin(val - HPTS_HIST);
}

void HockeyPuckTempSensor::setMax(double val){
	plot->yAxis()->setMax(val + HPTS_HIST);
}

HockeyPuckTempSensor::~HockeyPuckTempSensor() {}

void HockeyPuckTempSensor::refresh() {
	if(changedUnit) {
		if(unit == "Celsius" ) {
			setCelsius();
		} else {
			setKelvin();
		}
	}
	changedUnit = false;

	deque.pop_back();
	double val = readData();
	deque.push_front(val);
	datay.clear();

	for(int i=0;i<deque.size();i++){
		datay.push_back(deque[i]);
	}

	sevenseg->display(deque.front());

	ch->curve()->setSamples(datax.data(),datay.data(),HPTS_NR_SAMPLES);
	pa->autoscale();
	plot->replot();
}

void HockeyPuckTempSensor::updateUnit(QString txt){
	initData();
	// hack
	changedUnit = true;
	unit = txt;
	qInfo()<<txt;
}

void HockeyPuckTempSensor::setupUiElements()
{
	tempWidget = new QWidget(this);
	QVBoxLayout *vlay = new QVBoxLayout(tempWidget);
	plot = new PlotWidget(this);
	plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	plot->plot()->plotLayout()->setAlignCanvasToScales(false);
	QHBoxLayout *hlay = new QHBoxLayout(tempWidget);
	QVBoxLayout *v1lay = new QVBoxLayout(tempWidget);
	name = new QLabel("Temperature");
//	QLabel *unitOfMeasurement = new QLabel("Degrees");
	unitOfMeasurement = new QComboBox(this);
	unitOfMeasurement->addItem("Celsius");
	unitOfMeasurement->addItem("Kelvin");
	unit = "Celsius";
	connect(unitOfMeasurement, &QComboBox::currentTextChanged, this, &HockeyPuckTempSensor::updateUnit);
	sevenseg = new LcdNumber(this);
	applyStylesheet(HPTS_COLOR);

	vlay->addWidget(plot);
	vlay->addLayout(hlay);
	hlay->addLayout(v1lay);
	v1lay->addWidget(name);
	v1lay->setAlignment(Qt::AlignLeft);
	v1lay->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Maximum, QSizePolicy::Minimum));
	v1lay->addWidget(unitOfMeasurement);
	hlay->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Maximum));
	hlay->addWidget(sevenseg);

	// plot setup
	pa = new PlotAutoscaler(this);
	ch = new PlotChannel(QString("temp"), QPen(HPTS_COLOR), plot, plot->xAxis(), plot->yAxis(), this);
	plot->addedChannel(ch);
	plot->xAxis()->setInterval(HPTS_NR_SAMPLES - 1, 0 );
	plot->xAxis()->setVisible(true);
	plot->yAxis()->setVisible(true);
	ch->setEnabled(true);

	pa->addChannels(ch);
	connect(pa, &PlotAutoscaler::newMin, this, &HockeyPuckTempSensor::setMin);
	connect(pa, &PlotAutoscaler::newMax, this, &HockeyPuckTempSensor::setMax);
	ch->curve()->setSamples(datax.data(),datay.data(),HPTS_NR_SAMPLES);
	pa->autoscale();

}

void HockeyPuckTempSensor::applyStylesheet(QString lcdColor)
{
	QString style = QString(R"css(
						QWidget {
								background-color: &&childWidgetBackground&& ;
								height: 60px;
						}

						scopy--LcdNumber {
							background-color: transparent ;
							color : &&lcdColor&& ;
							border : 0px ;
						}
						)css");

	style.replace("&&childWidgetBackground&&", StyleHelper::getColor("UIElementBackground"));
	style.replace("&&hoverBackground&&", StyleHelper::getColor("LabelText"));
	style.replace("&&lcdColor&&", lcdColor);

	StyleHelper::MenuLargeLabel(name);
	// StyleHelper::MenuLargeLabel(unitOfMeasurement);
	StyleHelper::MenuComboBox(unitOfMeasurement);
	unitOfMeasurement->setFixedWidth(100);
	unitOfMeasurement->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	sevenseg->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	sevenseg->setFixedSize(200,100);
	sevenseg->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);
	sevenseg->setDigitCount(9);

	sevenseg->setStyleSheet(style);
}


float HockeyPuckTempSensor::readData() {

	char buffer[100];
	iio_channel_attr_read(m_ch, HPTS_ATTR_NAME, buffer, 100);
	float val = atof(buffer);
	return val;
}

void HockeyPuckTempSensor::setCelsius() {
	iio_channel_attr_write(m_ch, HPTS_UNIT_ATTR_NAME, "celsius");
}

void HockeyPuckTempSensor::setKelvin() {
	iio_channel_attr_write(m_ch, HPTS_UNIT_ATTR_NAME, "kelvin");
}

