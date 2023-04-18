#include "swiotruntime.hpp"
#include <string>
#include "core/logging_categories.h"

using namespace adiscope;

SwiotRuntime::SwiotRuntime():
	m_iioCtx(nullptr)
      ,m_triggerTimer(nullptr)
{}

SwiotRuntime::~SwiotRuntime()
{
	if (m_triggerTimer) {
		m_triggerTimer->stop();
		delete m_triggerTimer;
		m_triggerTimer = nullptr;
	}
}

bool SwiotRuntime::isRuntimeCtx()
{
	bool runtime = false;
	for (const auto &key : m_iioDevices.keys()) {
		if (iio_device_find_attr(m_iioDevices[key], "back")) {
			runtime = true;
			break;
		}
	}
	return runtime;
}

void SwiotRuntime::setContext(iio_context *ctx)
{
	m_iioCtx = ctx;
	createDevicesMap();
	bool isRuntime = isRuntimeCtx();
	if (isRuntime) {
		qInfo(CAT_SWIOT_RUNTIME) <<"runtime context";
		if (m_iioDevices.contains(AD_TRIGGER_NAME)) {
			int retCode = iio_device_set_trigger(m_iioDevices[AD_NAME], m_iioDevices[AD_TRIGGER_NAME]);
			qDebug(CAT_SWIOT_RUNTIME) <<"Trigger has been set: " + QString::number(retCode);
		}
		//		m_triggerTimer = new QTimer();
		//		m_triggerTimer->start(TRIGGER_TIMER_MS);
		//		connect(m_triggerTimer, SIGNAL(timeout()), this, SLOT(onTriggerTimeout()));
	} else {
		qDebug(CAT_SWIOT_RUNTIME) <<"isn't runtime context";
	}
}

//void SwiotRuntime::triggerPing()
//{
//	if (m_iioDevices[AD_NAME]) {
//		const iio_device* triggerDevice = nullptr;
//		int ret = iio_device_get_trigger(m_iioDevices[AD_NAME], &triggerDevice);

//		if (ret < 0 && m_iioDevices.contains(AD_TRIGGER_NAME) && m_iioDevices[AD_TRIGGER_NAME]) {
//			iio_device_set_trigger(m_iioDevices[AD_NAME], m_iioDevices[AD_TRIGGER_NAME]);
//			qDebug(CAT_SWIOT_RUNTIME) << "Trigger has been set";
//		} else {
//			qDebug(CAT_SWIOT_RUNTIME) << QString::number(ret) +" returned";
//		}
//	}
//}

//void SwiotRuntime::onTriggerTimeout()
//{
//	QtConcurrent::run(std::mem_fn(&SwiotRuntime::triggerPing), this);
//}

void SwiotRuntime::createDevicesMap()
{
	if (m_iioCtx) {
		int devicesNumber = iio_context_get_devices_count(m_iioCtx);
		m_iioDevices.clear();
		for (int i = 0; i < devicesNumber; i++) {
			struct iio_device* iioDev = iio_context_get_device(m_iioCtx, i);
			if (iioDev) {
				QString deviceName = QString(iio_device_get_name(iioDev));
				m_iioDevices[deviceName] = iioDev;
			}
		}
	}
}

void SwiotRuntime::onBackBtnPressed()
{
	for (const auto &key : m_iioDevices.keys()) {
		if (iio_device_find_attr(m_iioDevices[key], "back")) {
			iio_device_attr_write_bool(m_iioDevices[key], "back", 1);
		}
	}
	Q_EMIT backBtnPressed();
}