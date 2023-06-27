#ifndef SWIOTADVMENU_H
#define SWIOTADVMENU_H

#include "qboxlayout.h"
#include "qcombobox.h"
#include "qlabel.h"

#include <gui/spinbox_a.hpp>
#include <QWidget>
#include <string>

namespace scopy {
class CustomSwitch;

namespace swiot {

#define SLEW_DISABLE_IDX 0
#define OUTPUT_CHNL "output"
#define INPUT_CHNL "input"

class BufferMenu: public QWidget
{
	Q_OBJECT
public:
	explicit BufferMenu(QWidget* parent = nullptr, QString chnlFunction = "");
	~BufferMenu();

	virtual void init() = 0;
	virtual void connectSignalsToSlots() = 0;
	virtual QString getInfoMessage();
	QVector<QHBoxLayout *> getMenuLayers();
	QMap<QString, QMap<QString, QStringList>> getAttrValues();

	void addMenuLayout(QHBoxLayout *layout);
	void setAttrValues(QMap<QString, QMap<QString, QStringList>> values);
	double convertFromRaw(int rawValue, QString chnlType = OUTPUT_CHNL);
Q_SIGNALS:
	void attrValuesChanged(QString attrName, QString chnlType);

protected:
	QWidget *m_widget;
	QString m_chnlFunction;
	QMap<QString, QMap<QString, QStringList>> m_attrValues;
private:
	QVector<QHBoxLayout *> m_menuLayers;
};

class CurrentInLoopMenu: public BufferMenu
{
	Q_OBJECT
public:
	explicit CurrentInLoopMenu(QWidget* parent = nullptr, QString chnlFunction = "");
	~CurrentInLoopMenu();
	void init();
	void connectSignalsToSlots();
	QString getInfoMessage();
public Q_SLOTS:
	void dacCodeChanged(double value);
private:
	PositionSpinButton *m_dacCodeSpinButton;
	QLabel *m_dacLabel;
};

class DigitalInLoopMenu: public BufferMenu
{
	Q_OBJECT
public:
	explicit DigitalInLoopMenu(QWidget* parent = nullptr, QString chnlFunction = "");
	~DigitalInLoopMenu();
	void init();
	void connectSignalsToSlots();
	QString getInfoMessage();
public Q_SLOTS:
	void dacCodeChanged(double value);
	void thresholdChanged(double value);
private:
	PositionSpinButton *m_dacCodeSpinButton;
	PositionSpinButton *m_thresholdSpinButton;
	QLabel *m_dacLabel;
};

class VoltageOutMenu: public BufferMenu
{
	Q_OBJECT
public:
	explicit VoltageOutMenu(QWidget* parent = nullptr, QString chnlFunction = "");
	~VoltageOutMenu();
	void init();
	void connectSignalsToSlots();
	QString getInfoMessage();
public Q_SLOTS:
	void slewStepIndexChanged(int idx);
	void slewRateIndexChanged(int idx);
	void slewIndexChanged(int idx);
	void dacCodeChanged(double value);
private:
	PositionSpinButton *m_dacCodeSpinButton;
	QLabel *m_dacLabel;
	QComboBox *m_slewOptions;
	QComboBox *m_slewStepOptions;
	QComboBox *m_slewRateOptions;

	void setAvailableOptions(QComboBox *list, QString attrName);
};

class CurrentOutMenu: public BufferMenu
{
	Q_OBJECT
public:
	explicit CurrentOutMenu(QWidget* parent = nullptr, QString chnlFunction = "");
	~CurrentOutMenu();
	void init();
	void connectSignalsToSlots();
	QString getInfoMessage();

public Q_SLOTS:
	void slewStepIndexChanged(int idx);
	void slewRateIndexChanged(int idx);
	void slewIndexChanged(int idx);
	void dacCodeChanged(double value);
private: 
	PositionSpinButton *m_dacCodeSpinButton;
	QComboBox *m_slewOptions;
	QComboBox *m_slewStepOptions;
	QComboBox *m_slewRateOptions;
	QLabel *m_dacLabel;

	void setAvailableOptions(QComboBox *list, QString attrName);
};

class DiagnosticMenu: public BufferMenu
{
	Q_OBJECT
public:
	explicit DiagnosticMenu(QWidget* parent = nullptr, QString chnlFunction = "");
	~DiagnosticMenu();
	void init();
	void connectSignalsToSlots();
public Q_SLOTS:
	void diagIndextChanged(int idx);
private:
	QComboBox *m_diagOptions;

	void setAvailableOptions(QComboBox *list, QString attrName);
};

class DigitalInMenu : public BufferMenu
{
	Q_OBJECT
public:
	explicit DigitalInMenu(QWidget* parent = nullptr, QString chnlFunction = "");
	~DigitalInMenu();
	void init();
	void connectSignalsToSlots();
public Q_SLOTS:
	void thresholdChanged(double value);
private:
	PositionSpinButton *m_thresholdSpinButton;

};

class WithoutAdvSettings: public BufferMenu
{
	Q_OBJECT
public:
	explicit WithoutAdvSettings(QWidget* parent = nullptr, QString chnlFunction = "");
	~WithoutAdvSettings();
	void init();
	void connectSignalsToSlots();
};

class BufferMenuBuilder
{

public:

	enum MenuType {
		CURRENT_IN_LOOP		= 0,
		DIGITAL_IN		= 1,
		DIGITAL_IN_LOOP		= 2,
		VOLTAGE_OUT		= 3,
		CURRENT_OUT		= 4,
		VOLTAGE_IN		= 5,
		CURRENT_IN_EXT		= 6,
		CURRENT_IN_LOOP_HART	= 7,
		CURRENT_IN_EXT_HART	= 8,
		RESISTANCE		= 9,
		DIAGNOSTIC		= 10,
		HIGH_Z			= 11
	};

	static int decodeFunctionName(QString function)
	{
		if (function.compare("voltage_in") == 0) {
			return VOLTAGE_IN;
		} else if (function.compare("current_in_ext") == 0) {
			return CURRENT_IN_EXT;
		} else if (function.compare("current_in_loop_hart") == 0) {
			return CURRENT_IN_LOOP_HART;
		} else if (function.compare("current_in_ext_hart") == 0) {
			return CURRENT_IN_EXT_HART;
		} else if (function.compare("resistance") == 0) {
			return RESISTANCE;
		} else if (function.compare("current_in_loop") == 0) {
			return CURRENT_IN_LOOP;
		} else if (function.compare("digital_input") == 0) {
			return DIGITAL_IN;
		} else if (function.compare("digital_input_loop") == 0) {
			return DIGITAL_IN_LOOP;
		} else if (function.compare("voltage_out") == 0) {
			return VOLTAGE_OUT;
		} else if (function.compare("current_out") == 0) {
			return CURRENT_OUT;
		} else if (function.compare("diagnostic") == 0) {
			return DIAGNOSTIC;
		} else if (function.compare("high_z") == 0) {
			return HIGH_Z;
		} else {
			return -1;
		}
	}

	static BufferMenu* newAdvMenu(QWidget *widget, QString function)
	{
		int menu_type = decodeFunctionName(function);
		switch (menu_type) {
		case CURRENT_IN_LOOP:
			return new CurrentInLoopMenu(widget, function);
		case DIGITAL_IN_LOOP:
			return new DigitalInLoopMenu(widget, function);
		case VOLTAGE_OUT:
			return new VoltageOutMenu(widget, function);
		case CURRENT_OUT:
			return new CurrentOutMenu(widget, function);
		case DIGITAL_IN:
			return new DigitalInMenu(widget, function);
		case DIAGNOSTIC:
			return new DiagnosticMenu(widget, function);
		default:
			return new WithoutAdvSettings(widget, function);
		}
	}

};

}
}


#endif // SWIOTADVMENU_H
