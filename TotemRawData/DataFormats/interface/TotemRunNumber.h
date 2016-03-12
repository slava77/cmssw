/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors: 
 *   Jan Kašpar (jan.kaspar@gmail.com)
 *
 ****************************************************************************/

#ifndef _TotemRunNumber_h_
#define _TotemRunNumber_h_

#include "DataFormats/Provenance/interface/EventRange.h"

#include <string>

class TotemRunNumber
{
  public:
    unsigned int run, eventBuilderHost, eventBuilderProcess, file;

    TotemRunNumber(unsigned int _run, unsigned int _evbh, unsigned int _evbp, unsigned int _file)
      : run(_run), eventBuilderHost(_evbh), eventBuilderProcess(_evbp), file(_file)
    {
      if (eventBuilderHost < 11 || eventBuilderHost > 15)
      {
        printf("ERROR in TotemRunNumber::TotemRunNumber > invalid eventBuilderHost number (%i), changed to 11.\n", eventBuilderHost);
        eventBuilderHost = 11;
      }

      if (eventBuilderProcess < 1 || eventBuilderProcess > 2)
      {
        printf("ERROR in TotemRunNumber::TotemRunNumber > invalid eventBuilderProcess number (%i), changed to 1.\n", eventBuilderProcess);
        eventBuilderProcess = 1;
      }

      if (file > 9999)
      {
        printf("ERROR in TotemRunNumber::TotemRunNumber > invalid file number (%i), changed to 0.\n", file);
        file = 0;
      }
    }

    TotemRunNumber(edm::RunNumber_t rn)
    {
      file = rn % 10000;
      unsigned int evbID = (rn / 10000) % 10;
      run = rn / 100000;

      if (evbID > 7)
      {
        printf("ERROR in TotemRunNumber::TotemRunNumber > evbId (%i) greater than 7, changed to 0.\n", evbID);
        evbID = 0;
      }

      eventBuilderHost = evbID/2 + 11;
      eventBuilderProcess = (evbID % 2) + 1;
    }

    edm::RunNumber_t ToCMSSWRunNumber()
    {
      unsigned int evbID = (eventBuilderHost-11) * 2 + eventBuilderProcess-1;
      return run*100000 + evbID*10000 + file;
    }

    std::string ToStdString();

    std::string ToLongString();
};

#endif
