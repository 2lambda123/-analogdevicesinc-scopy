#include "swiotdigitalchannel.h"

adiscope::DigitalChannel::DigitalChannel(const QString& deviceName, const QString& deviceType, QWidget *parent) :
        gui::GenericMenu(parent),
        ui(new Ui::DigitalChannel),
        m_deviceName(deviceName),
        m_deviceType(deviceType) {

        this->ui->setupUi(this);
        initInteractiveMenu();

        this->ui->m_channelName->setText(deviceName);
        this->ui->m_channelType->setText(deviceType);

        this->ui->customSwitch->setOn(QLabel("high"));
        this->ui->customSwitch->setOff(QLabel("low"));

        if (deviceType == "input") {
                this->ui->customSwitch->setVisible(false);
        }

        this->ui->scaleChannel->setOrientation(Qt::Horizontal);
        this->ui->scaleChannel->setScalePosition(QwtThermo::LeadingScale);
        this->ui->scaleChannel->setOriginMode(QwtThermo::OriginCustom);
        this->ui->scaleChannel->setMinimumSize(100, 50);
        this->ui->scaleChannel->setAutoScaler(true);
        this->ui->scaleChannel->setScale(0, 1);
        this->ui->scaleChannel->setAutoScaler(false);
        this->ui->scaleChannel->setScaleStepSize(1);
        this->ui->scaleChannel->setOrientation(Qt::Horizontal);
        this->ui->scaleChannel->setScalePosition(QwtThermo::LeadingScale);
        this->ui->scaleChannel->setOriginMode(QwtThermo::OriginCustom);
        this->ui->scaleChannel->setStyleSheet("font-size:16px;");

        this->ui->sismograph->setPlotAxisXTitle(""); // clear title
        this->ui->sismograph->setAxisScale(0, 0, 1, 1); // y axis
        this->ui->sismograph->setAutoscale(false);
        this->ui->sismograph->setColor(Qt::red);
        this->ui->sismograph->updateYScale(30, 0);

        this->connectSignalsAndSlots();
}

adiscope::DigitalChannel::~DigitalChannel() {
        delete ui;
};

void adiscope::DigitalChannel::connectSignalsAndSlots() {

}

Ui::DigitalChannel *adiscope::DigitalChannel::getUi() const {
        return ui;
}

void adiscope::DigitalChannel::updateTimeScale(double newMin, double newMax) {
        this->ui->sismograph->updateYScale(newMax, newMin);
}

void DigitalChannel::addDataSample(double value) {
        this->ui->sismograph->plot(value); // FIXME: gets called 4 times for some reason
        this->ui->scaleChannel->setValue(value);
        this->ui->LcdNumber->display((int)(value)); // FIXME: does not change the displayed value
}

const std::vector<std::string> &DigitalChannel::getConfigModes() const {
        return m_configModes;
}

void DigitalChannel::setConfigModes(const std::vector<std::string> &configModes) {
        m_configModes = configModes;

        this->ui->configModes->clear();
        for (const std::string& item : this->m_configModes) {
                this->ui->configModes->addItem(QString::fromStdString(item));
        }
}

const QString &DigitalChannel::getSelectedConfigMode() const {
        return m_selectedConfigMode;
}

void DigitalChannel::setSelectedConfigMode(const QString &selectedConfigMode) {
        m_selectedConfigMode = selectedConfigMode;

        int idx = this->ui->configModes->findText(selectedConfigMode);
        this->ui->configModes->setCurrentIndex(idx);
        qDebug() << "The channel " << this->m_deviceName << " read selected config mode " << selectedConfigMode;
}
