#ifndef SCOPY_SWITCHGUISTRATEGY_H
#define SCOPY_SWITCHGUISTRATEGY_H

#include <QWidget>
#include <iio.h>
#include <gui/widgets/menulineedit.h>
#include <gui/widgets/menucollapsesection.h>
#include <gui/customSwitch.h>
#include <gui/spinbox_a.hpp>
#include <gui/widgets/menucombo.h>
#include "guistrategy/guistrategyinterface.h"
#include "attrdata.h"

namespace scopy::attr {
class SwitchAttrUi : public AttrUiStrategyInterface
{
	Q_OBJECT
public:
	/**
	 * @brief This contain a CustomSwitch capable of holding no more than 2 values, the ones specified in
	 * recipe->linkedAttributeValue.
	 * */
	explicit SwitchAttrUi(AttributeFactoryRecipe recipe, QObject *parent = nullptr);
	~SwitchAttrUi();

	/**
	 * @overload AttrUiStrategyInterface::ui()
	 * */
	QWidget *ui() override;

	bool isValid() final;

	void receiveData(QString currentData, QString optionalData) override;

private:
	QWidget *m_ui;
	CustomSwitch *m_menuBigSwitch;
	QStringList *m_optionsList;
};
} // namespace scopy::attr

#endif // SCOPY_SWITCHGUISTRATEGY_H
