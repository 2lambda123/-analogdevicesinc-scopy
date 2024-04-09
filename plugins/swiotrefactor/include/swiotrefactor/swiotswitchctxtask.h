/*
 * Copyright (c) 2023 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see https://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SWIOTSWITCHCTXTASK_H
#define SWIOTSWITCHCTXTASK_H

#include <QObject>
#include <QThread>
#include <iioutil/connection.h>

namespace scopy::swiotrefactor {
class SwiotSwitchCtxTask : public QThread
{
	Q_OBJECT
public:
	SwiotSwitchCtxTask(QString uri, bool wasRuntime);
	~SwiotSwitchCtxTask();
	void run() override;

Q_SIGNALS:
	void contextSwitched();
	void contextSwitchFailed();

private Q_SLOTS:
	void onReadModeFinished(scopy::Command *cmd);

private:
	QString m_uri;
	bool m_wasRuntime;
	Connection *m_conn;
};
} // namespace scopy::swiotrefactor

#endif // SWIOTSWITCHCTXTASK_H