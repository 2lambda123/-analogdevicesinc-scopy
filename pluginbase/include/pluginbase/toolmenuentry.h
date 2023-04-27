#ifndef TOOLMENUENTRY_H
#define TOOLMENUENTRY_H

#include <QObject>
#include <QString>
#include <QUuid>
#include "scopypluginbase_export.h"

namespace scopy {
/**
 * @brief The ToolMenuEntry class
 * Class used by plugin and scopy to populate the tool menu
 * SCOPY_NEW_TOOLMENUENTRY macro can be used to instantiate ToolMenuEntry objects
 */
class SCOPYPLUGINBASE_EXPORT ToolMenuEntry : public QObject {
	Q_OBJECT
public:
	ToolMenuEntry(QString id, QString name, QString icon, QString pluginName, QObject *parent = nullptr) :
		QObject(parent), m_id(id), m_uuid(QUuid::createUuid().toString()),
		m_name(name), m_icon(icon), m_pluginName(pluginName), m_visible(true),
		m_enabled(false), m_running(false), m_runBtnVisible(false),
		m_attached(true), m_tool(nullptr) {}

	ToolMenuEntry(const ToolMenuEntry &other) {
		m_id = other.m_id;
		m_uuid = other.m_uuid;
		m_name = other.m_name;
		m_icon = other.m_icon;
		m_visible = other.m_visible;
		m_enabled = other.m_enabled;
		m_running = other.m_running;
		m_runBtnVisible = other.m_runBtnVisible;
		m_attached = other.m_attached;
		m_pluginName = other.m_pluginName;
		m_tool = other.m_tool;
	}

	~ToolMenuEntry() {}
	inline QString id() const { return m_id; }
	inline QString uuid() const { return m_uuid; }
	inline QString name() const { return m_name; }
	inline QString icon() const { return m_icon; }
	inline QString pluginName() const { return m_pluginName; }
	inline bool visible() const { return m_visible; }
	inline bool enabled() const { return m_enabled; }
	inline bool running() const { return m_running; }
	inline bool attached() const { return m_attached; }
	inline bool runEnabled() const 	{ return m_runEnabled;	}
	inline bool runBtnVisible() const { return m_runBtnVisible; }
	inline QWidget *tool() const { return m_tool; }

	static ToolMenuEntry *findToolMenuEntryByName(QList<ToolMenuEntry*> list, QString id);
	static ToolMenuEntry *findToolMenuEntryById(QList<ToolMenuEntry*> list, QString id);
	static ToolMenuEntry *findToolMenuEntryByTool(QList<ToolMenuEntry*> list, QWidget *w);
	/**
	 * @brief setName
	 * @param newName
	 * Set tool menu entry name
	 */
	void setName(const QString &newName);
	/**
	 * @brief setIcon
	 * @param newIcon
	 * Set tool menu entry icon
	 */
	void setIcon(const QString &newIcon);

public Q_SLOTS:
	/**
	 * @brief setVisible
	 * @param newVisible
	 * Makes the menu entry visible
	 */
	void setVisible(bool newVisible);
	/**
	 * @brief setEnabled
	 * @param newEnabled
	 * Enables the toolmenuentry - makes the menu entry clickable
	 */
	void setEnabled(bool newEnabled);
	/**
	 * @brief setRunning
	 * @param newRunning
	 * Sets the tool in run state
	 */	
	void setRunning(bool newRunning);

	/**
	 * @brief setAttached
	 * @param attach
	 * Attaches the tool to the main window
	 */
	void setAttached(bool attach);

	/**
	 * @brief setRunEnabled
	 * @param newRunEnabled
	 * Set tool run button state
	 */
	void setRunEnabled(bool newRunEnabled);

	/**
	 * @brief setRunBtnVisible
	 * @param newRunBtnVisible
	 * Set run button visible on/off
	 */
	void setRunBtnVisible(bool newRunBtnVisible);

	/**
	 * @brief setTool
	 * @param newTool
	 * links a widget to the tool menu entry. Remove tool from the menu entry by setting newtool to nullptr
	 */
	QWidget* setTool(QWidget *newTool);

Q_SIGNALS:
	/**
	 * @brief updateToolEntry
	 * signal is emitted automatically when modifying anything about the tool entry
	 */
	void updateToolEntry();
	/**
	 * @brief updateTool
	 * signal is emitted automatically when changing a tool linked to the tool entry
	 */
	void updateToolAttached(bool);
	void updateTool(QWidget*);

	/**
	 * @brief requestRun
	 * Signal is emitted when the run button is toggled
	 */
	void runToggled(bool);

	/**
	 * @brief requestRun
	 * Signal is emitted when the run button is clicked from Scopy UI
	 */
	void runClicked(bool);

private:
	QString m_id;
	QString m_uuid;
	QString m_name;
	QString m_icon;
	QString m_pluginName;
	bool m_visible;
	bool m_enabled;
	bool m_running;
	bool m_runEnabled;
	bool m_runBtnVisible;
	bool m_attached;
	QWidget* m_tool;
};

inline ToolMenuEntry *ToolMenuEntry::findToolMenuEntryByName(QList<ToolMenuEntry *> list, QString name)
{
	for(auto &&tme : list) {
		if(tme->name()==name) {
			return tme;
		}
	}
	return nullptr;
}

inline ToolMenuEntry *ToolMenuEntry::findToolMenuEntryById(QList<ToolMenuEntry *> list, QString id)
{
	for(auto &&tme : list) {
		if(tme->id()==id) {
			return tme;
		}
	}
	return nullptr;
}

inline ToolMenuEntry *ToolMenuEntry::findToolMenuEntryByTool(QList<ToolMenuEntry *> list, QWidget *w)
{
	for(auto &&tme : list) {
		if(tme->tool()==w) {
			return tme;
		}
	}
	return nullptr;
}

inline void ToolMenuEntry::setName(const QString &newName)
{
	m_name = newName;
	Q_EMIT updateToolEntry();
}

inline void ToolMenuEntry::setIcon(const QString &newIcon)
{
	m_icon = newIcon;
	Q_EMIT updateToolEntry();
}

inline void ToolMenuEntry::setVisible(bool newVisible)
{
	m_visible = newVisible;
	Q_EMIT updateToolEntry();
}

inline void ToolMenuEntry::setEnabled(bool newEnabled)
{
	m_enabled = newEnabled;
	Q_EMIT updateToolEntry();
}

inline void ToolMenuEntry::setRunning(bool newRunning)
{
	m_running = newRunning;
	Q_EMIT updateToolEntry();
}

inline void ToolMenuEntry::setRunBtnVisible(bool newRunBtnVisible)
{
	m_runBtnVisible = newRunBtnVisible;
	Q_EMIT updateToolEntry();
}

inline QWidget* ToolMenuEntry::setTool(QWidget *newTool)
{
	QWidget* oldTool;
	oldTool = m_tool;
	m_tool = newTool;
	if(oldTool != m_tool) {
		Q_EMIT updateTool(oldTool);
	}
	return oldTool;
}


inline void ToolMenuEntry::setAttached(bool attach)
{
	bool oldAttach = m_attached;
	m_attached = attach;
	if(oldAttach != m_attached && m_tool) {
		Q_EMIT updateToolAttached(oldAttach);
	}
}


inline void ToolMenuEntry::setRunEnabled(bool newRunEnabled)
{
	m_runEnabled = newRunEnabled;
	Q_EMIT updateToolEntry();
}

}

#endif // TOOLMENUENTRY_H