#ifndef VOLTAGECHNLINFO_H
#define VOLTAGECHNLINFO_H

#include "chnlinfo.h"
#define MIN_VOLTAGE_VALUE -5
#define MAX_VOLTAGE_VALUE 5
namespace scopy::swiot{
class VoltageChnlInfo : public ChnlInfo
{
	Q_OBJECT
public:
	explicit VoltageChnlInfo(QString plotUm = "V", QString hwUm = "mV", iio_channel *iioChnl = nullptr);
	~VoltageChnlInfo();

	double convertData(unsigned int data) override;
};
}

#endif // VOLTAGECHNLINFO_H
