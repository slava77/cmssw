/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors: 
 *   Jan Kašpar (jan.kaspar@gmail.com)
 *
 ****************************************************************************/

#include "TotemRawData/DataFormats/interface/TotemRunNumber.h"

using namespace std;

//----------------------------------------------------------------------------------------------------

string TotemRunNumber::ToStdString()
{
  char buf[20];
  sprintf(buf, "%5i.%02i.%i.%04i", run, eventBuilderHost, eventBuilderProcess, file);
  return buf;
}

//----------------------------------------------------------------------------------------------------

string TotemRunNumber::ToLongString()
{
  char buf[100];
  sprintf(buf, "run=%i, evb=%i, proc=%i, file=%i", run, eventBuilderHost, eventBuilderProcess, file);
  return buf;
}
