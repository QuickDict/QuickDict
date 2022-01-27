#include "localdict.h"
#include <QFileInfo>

LocalDict::LocalDict(QObject *parent)
    : DictService(parent)
{}

LocalDict::~LocalDict() {}

void LocalDict::setSource(const QString &source)
{
    if (source == m_dictFileName)
        return;

    m_dictFileName = source;
    m_indexFileName = m_dictFileName + ".index"; // TODO: save index data to cache dir

    if (loaded()) {
        unloadDict();
        unloadIndex();
        setLoaded(false);
    }
    if (enabled() && !m_dictFileName.isEmpty()) {
        if (loadDict() && loadOrBuildIndex())
            setLoaded(true);
    }
    emit sourceChanged(m_dictFileName);
}

void LocalDict::setSorted(bool sorted)
{
    if (m_sorted == sorted)
        return;
    m_sorted = sorted;
    emit sortedChanged(m_sorted);
}

void LocalDict::setLoaded(bool loaded)
{
    if (m_loaded == loaded)
        return;
    m_loaded = loaded;
    emit loadedChanged(m_loaded);
}

bool LocalDict::doSetEnabled(bool enabled)
{
    if (enabled && !m_dictFileName.isEmpty()) {
        if (loadDict()) {
            if (!loadOrBuildIndex()) {
                unloadDict();
                return false;
            }
            setLoaded(true);
            return true;
        } else {
            return false;
        }
    } else if (!enabled && loaded()) {
        unloadDict();
        unloadIndex();
        setLoaded(false);
    }
    return true;
}

bool LocalDict::loadOrBuildIndex()
{
    if (needBuildIndex())
        return buildIndex();
    else
        return loadIndex();
}

bool LocalDict::needBuildIndex()
{
    QFileInfo dictFileInfo(m_dictFileName);
    QFileInfo indexFileInfo(m_indexFileName);
    if (!dictFileInfo.exists()
        || (dictFileInfo.exists() && indexFileInfo.exists()
            && dictFileInfo.lastModified() <= indexFileInfo.lastModified()))
        return false;
    return true;
}
