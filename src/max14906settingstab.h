#ifndef MAX14906SETTINGSTAB_H
#define MAX14906SETTINGSTAB_H

#include <QWidget>
#include "gui/spinbox_a.hpp"

using namespace adiscope;

namespace Ui {
class Max14906SettingsTab;
}

class Max14906SettingsTab : public QWidget
{
	Q_OBJECT

public:
	explicit Max14906SettingsTab(QWidget *parent = nullptr);
	~Max14906SettingsTab() override;

        PositionSpinButton* getSpinButton();

private:
	Ui::Max14906SettingsTab *ui;
        PositionSpinButton *max_spin_button{};
};

#endif // MAX14906SETTINGSTAB_H
