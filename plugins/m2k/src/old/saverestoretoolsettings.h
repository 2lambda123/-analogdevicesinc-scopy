#ifndef SAVERESTORETOOLSETTINGS_H
#define SAVERESTORETOOLSETTINGS_H

#include "m2ktool.hpp"

#include <QTemporaryFile>
#include <QtGlobal>

namespace adiscope::m2k {
class SaveRestoreToolSettings
{
	SaveRestoreToolSettings(const SaveRestoreToolSettings &) = delete;
	SaveRestoreToolSettings &operator=(const SaveRestoreToolSettings &) = delete;

	SaveRestoreToolSettings(SaveRestoreToolSettings &&) = delete;
	SaveRestoreToolSettings &operator=(SaveRestoreToolSettings &&) = delete;
public:
	SaveRestoreToolSettings(M2kTool *tool);
	~SaveRestoreToolSettings();

private:
	QTemporaryFile m_temp;
	M2kTool *m_tool;
};
} // namespace adiscope

#endif // SAVERESTORETOOLSETTINGS_H