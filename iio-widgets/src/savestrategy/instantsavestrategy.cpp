#include "savestrategy/instantsavestrategy.h"
#include <gui/stylehelper.h>
#include <QLoggingCategory>
#include <utility>

using namespace scopy;

Q_LOGGING_CATEGORY(CAT_INSTANT_DATA_STRATEGY, "InstantDataSaveStrategy")

InstantSaveStrategy::InstantSaveStrategy(IIOWidgetFactoryRecipe recipe, QWidget *parent)
	: QWidget(parent)
	, m_recipe(recipe)
{}

QWidget *InstantSaveStrategy::ui() { return nullptr; }

bool InstantSaveStrategy::isValid()
{
	if(m_recipe.channel != nullptr && m_recipe.data != "") {
		return true;
	}
	return false;
}

void InstantSaveStrategy::receiveData(QString data)
{
	qCritical(CAT_INSTANT_DATA_STRATEGY) << "Instantly writing" << data << "to" << m_recipe.data;
	Q_EMIT saveData(data);
}

#include "moc_instantsavestrategy.cpp"
