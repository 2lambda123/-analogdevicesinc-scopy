#include "registercontroller.hpp"
#include "qtextspinbox.hpp"

#include <QVBoxLayout>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdebug.h>
#include <QPushButton>
#include <QSpinBox>
#include <QAbstractSpinBox>
#include <titlespinbox.hpp>
#include <utils.h>

#include "utils.hpp"
#include "regmapstylehelper.hpp"

using namespace scopy;
using namespace regmap;

RegisterController::RegisterController(QWidget *parent)
    : QWidget{parent}
{
	mainLayout = new QHBoxLayout(this);
	Utils::removeLayoutMargins(mainLayout);
	setLayout(mainLayout);

	QWidget *readWidget = new QWidget(this);
	readWidget->setFixedHeight(72);
	mainLayout->addWidget(readWidget);
	QHBoxLayout *readWidgetLayout = new QHBoxLayout(readWidget);
	Utils::removeLayoutMargins(readWidgetLayout);
	readWidget->setLayout(readWidgetLayout);
	readWidgetLayout->setMargin(16);

	QVBoxLayout *readWidgetLeftLayout = new QVBoxLayout();
	Utils::removeLayoutMargins(readWidgetLeftLayout);
	adrPck = new TitleSpinBox("Address 1: ", readWidget);
	adrPck->setMaximumHeight(30);
	readWidgetLeftLayout->addWidget(adrPck);

	addressPicker =adrPck->getSpinBox();

	addressPicker->setDisplayIntegerBase(16);
	addressPicker->setMinimum(0);
	addressPicker->setMaximum(INT_MAX);
	addressPicker->setPrefix("0x");
	addressPicker->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	QObject::connect(addressPicker, &QSpinBox::textChanged, this, [=](QString address){
		bool ok;
		Q_EMIT	registerAddressChanged(address.toInt(&ok,16));
	});
	readWidgetLayout->addLayout(readWidgetLeftLayout,3);

	readButton = new QPushButton("Read", readWidget);
	//request read
	QObject::connect( readButton, &QPushButton::clicked, this , [=](){
		bool ok;
		Q_EMIT requestRead(addressPicker->text().toInt(&ok,16));
	});

	readWidgetLayout->addWidget(readButton, 1, Qt::AlignRight);

	QWidget *writeWidget = new QWidget(this);
	writeWidget->setFixedHeight(72);
	mainLayout->addWidget(writeWidget);
	writeWidgetLayout = new QHBoxLayout(writeWidget);
	Utils::removeLayoutMargins(writeWidgetLayout);
	writeWidget->setLayout(writeWidgetLayout);
	writeWidgetLayout->setMargin(16);

	QVBoxLayout *writeWidgetLeftLayout = new QVBoxLayout();
	Utils::removeLayoutMargins(writeWidgetLeftLayout);
	writeWidgetLeftLayout->setSpacing(0);
	valueLabel = new QLabel("Value: ", writeWidget);
	writeWidgetLeftLayout->addWidget(valueLabel);

	regValue = new QLineEdit(writeWidget);
	regValue->setText("N/R");
	regValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	QObject::connect(regValue, &QLineEdit::textChanged, this, &RegisterController::valueChanged);

	writeWidgetLeftLayout->addWidget(regValue);
	writeWidgetLayout->addLayout(writeWidgetLeftLayout, 3);

	writeButton = new QPushButton("Write", writeWidget);
	//request write on register
	QObject::connect( writeButton, &QPushButton::clicked, this, [=](){
		bool ok;
		Q_EMIT requestWrite(addressPicker->value(), regValue->text().toInt(&ok,16));
	});

	writeWidgetLayout->addWidget(writeButton, 1, Qt::AlignRight);

    applyStyle();
}

RegisterController::~RegisterController()
{

    delete regValue;
	delete addressPicker;
	delete mainLayout;
	if (detailedRegisterToggle) {
		delete detailedRegisterToggle;
	}
}

void RegisterController::registerChanged(uint32_t address)
{
    if (!addressChanged) {
        addressPicker->setValue(address);
    } else {
        addressChanged = false;
    }
}

void RegisterController::registerValueChanged(QString value)
{
	regValue->setText(value);
}

QString RegisterController::getAddress()
{
	return addressPicker->text();
}

void RegisterController::setHasMap(bool hasMap)
{
	this->hasMap = hasMap;

	if (hasMap) {
		detailedRegisterToggle = new QPushButton(this);
		detailedRegisterToggle->setCheckable(true);
		QIcon detailedRegisterToggleIcon;
		detailedRegisterToggleIcon.addPixmap(Util::ChangeSVGColor(":/gui/icons/scopy-default/icons/setup3_unchecked.svg","white",1));
		detailedRegisterToggle->setIcon(detailedRegisterToggleIcon);
		detailedRegisterToggle->setChecked(true);
		QObject::connect(detailedRegisterToggle,
						 &QPushButton::toggled,
						 this,
						 &RegisterController::toggleDetailedMenu);
		writeWidgetLayout->addWidget(detailedRegisterToggle, 0.5, Qt::AlignRight);
		RegmapStyleHelper::smallBlueButton(detailedRegisterToggle);
		detailedRegisterToggle->setFixedSize(30,30);
	}
}

void RegisterController::applyStyle()
{
	setFixedHeight(72);

	readButton->setStyleSheet(RegmapStyleHelper::BlueButton(nullptr));
	writeButton->setStyleSheet(RegmapStyleHelper::BlueButton(nullptr));
	valueLabel->setStyleSheet(RegmapStyleHelper::grayLabel(nullptr));
	regValue->setStyleSheet(RegmapStyleHelper::lineEditStyle(nullptr));

	setStyleSheet( RegmapStyleHelper::regmapControllerStyle(nullptr));
}
