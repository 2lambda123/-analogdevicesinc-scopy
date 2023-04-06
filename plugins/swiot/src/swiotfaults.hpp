#ifndef SWIOTFAULTS_HPP
#define SWIOTFAULTS_HPP

#include "ui_swiotfaults.h"

#include "faultspage.hpp"
#include "src/tool/tool_view.hpp"
#include "src/gui/generic_menu.hpp"
#include "src/gui/channel_manager.hpp"

#include <iio.h>

namespace adiscope {
class SwiotFaults : public QWidget {
	Q_OBJECT
public:
	explicit SwiotFaults(struct iio_context *ctx, QWidget *parent = nullptr);
	~SwiotFaults() override;

	void getAd74413rFaultsNumeric();
	void getMax14906FaultsNumeric();

	void pollFaults();

protected Q_SLOTS:
	void runButtonClicked();
	void singleButtonClicked();

private:
        struct iio_context* ctx;

	Ui::SwiotFaults *ui;
	QTimer *timer;
	QThread *thread;

	uint32_t ad74413r_numeric = 0;
	uint32_t max14906_numeric = 0;

	adiscope::gui::ToolView *m_toolView{};
	adiscope::gui::GenericMenu *m_generalSettingsMenu{};
	adiscope::gui::ChannelManager *m_monitorChannelManager{};
	adiscope::gui::FaultsPage *faultsPage{};
	void connectSignalsAndSlots();
	void setupDynamicUi(QWidget *parent);

	gui::GenericMenu *createGeneralSettings(const QString &title, QColor *color);
};
}
#endif // SWIOTFAULTS_HPP
