#include "quickdict.h"

QuickDict *QuickDict::_instance = new QuickDict;

QuickDict::QuickDict(QObject *parent)
    : QObject(parent)
{}

QuickDict::~QuickDict() {}
