#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <QtCore>
#include <soloutils>

const int wgThumbLimits[] = { 120, 150, 180, 200, 250, 300 };

const QMap<HUString, QVariant> wgDefaultUserOptions =
{
  { "thumbsize", 2 }, // index into wgThumbLimits
  { "showtoc", true }, // show/hide table of contents for EVERY page
};

const int wgMaxTocLevel = 2;

#endif // GLOBALVARS_H
