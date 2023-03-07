#include "qpluginloader.h"
#include <QTest>
#include <QList>
#include <pluginbase/plugin.h>

using namespace adiscope;

class TST_SWIOTPlugin : public QObject
{
	Q_OBJECT
private Q_SLOTS:
	void fileExists();
	void isLibrary();
	void loaded();
	void className();
	void instanceNotNull();
	void multipleInstances();
	void qobjectcast_to_plugin();
	void clone();
	void name();
	void metadata();
};

#define PLUGIN_LOCATION "../../plugins"
#define FILENAME PLUGIN_LOCATION "/libscopyswiot.so"


void TST_SWIOTPlugin::fileExists()
{
	QFile f(FILENAME);
	bool ret;
	ret = f.open(QIODevice::ReadOnly);
	if(ret)
		f.close();
	QVERIFY(ret);
}

void TST_SWIOTPlugin::isLibrary()
{
	QVERIFY(QLibrary::isLibrary(FILENAME));
}

void TST_SWIOTPlugin::className()
{
	QPluginLoader qp(FILENAME,this);
	QVERIFY(qp.metaData().value("className") == "SWIOTPlugin");
}

void TST_SWIOTPlugin::loaded()
{
	QPluginLoader qp(FILENAME,this);
	qp.load();
	QVERIFY(qp.isLoaded());
}

void TST_SWIOTPlugin::instanceNotNull()
{
	QPluginLoader qp(FILENAME,this);
	QVERIFY(qp.instance() != nullptr);
}

void TST_SWIOTPlugin::multipleInstances()
{
	QPluginLoader qp1(FILENAME,this);
	QPluginLoader qp2(FILENAME,this);

	QVERIFY(qp1.instance() == qp2.instance());
}


void TST_SWIOTPlugin::qobjectcast_to_plugin()
{
	QPluginLoader qp(FILENAME,this);
	auto instance = qobject_cast<Plugin*>(qp.instance());
	QVERIFY(instance != nullptr );
}


void TST_SWIOTPlugin::clone()
{
	QPluginLoader qp(FILENAME,this);

	Plugin *p1 = nullptr, *p2 = nullptr;
	auto original = qobject_cast<Plugin*>(qp.instance());
	p1 = original->clone();
	QVERIFY(p1 != nullptr);
	p2 = original->clone();
	QVERIFY(p2 != nullptr);
	QVERIFY(p1 != p2);
}

void TST_SWIOTPlugin::name() {
	QPluginLoader qp(FILENAME,this);

	Plugin *p1 = nullptr, *p2 = nullptr;
	auto original = qobject_cast<Plugin*>(qp.instance());
	p1 = original->clone();
	qDebug()<<p1->name();
}

void TST_SWIOTPlugin::metadata()
{
	QPluginLoader qp(FILENAME,this);

	Plugin *p1 = nullptr, *p2 = nullptr;
	auto original = qobject_cast<Plugin*>(qp.instance());
	original->initMetadata();
	p1 = original->clone();
	qDebug()<<p1->metadata();
	QVERIFY(!p1->metadata().isEmpty());
}


QTEST_MAIN(TST_SWIOTPlugin)

#include "tst_pluginloader.moc"