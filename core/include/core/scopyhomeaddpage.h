#ifndef SCOPYHOMEADDPAGE_H
#define SCOPYHOMEADDPAGE_H

#include <QWidget>
#include "scopycore_export.h"

namespace Ui {
class ScopyHomeAddPage;
}

namespace adiscope {
class SCOPYCORE_EXPORT ScopyHomeAddPage : public QWidget
{
	Q_OBJECT

public:
	explicit ScopyHomeAddPage(QWidget *parent = nullptr);
	~ScopyHomeAddPage();

Q_SIGNALS:
	void requestAddDevice(QString);
	void requestDevice(QString);

private Q_SLOTS:
	void add();
	void verify();
	void deviceAddedToUi(QString);

private:
	Ui::ScopyHomeAddPage *ui;
	QString pendingUri;
};
}
#endif // SCOPYHOMEADDPAGE_H
