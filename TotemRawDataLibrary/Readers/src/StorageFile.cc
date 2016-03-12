#include <iostream>
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"
#include "TotemRawDataLibrary/Readers/interface/LocalStorageFile.h"

#ifdef USE_CASTOR
    #include "TotemRawDataLibrary/Readers/interface/RFIOStorageFile.h"
#endif

#ifdef USE_XROOTD
    #include "TotemRawDataLibrary/Readers/interface/XRootStorageFile.h"
#endif

namespace Totem {

    StorageFile* StorageFile::CreateInstance(const std::string &urlPath) {
        if(urlPath.find("/castor") == 0 || urlPath.find("rfio://") == 0) {
            #ifdef USE_CASTOR
                return new RFIOStorageFile(urlPath);
            #else
                std::cout << ">> StorageFile::CreateInstance > RFIO not supported." << std::endl;
                return NULL;
            #endif

        }
        else if(urlPath.find("root://") == 0 || urlPath.find("xroot://") == 0){
            #ifdef USE_XROOTD
                return new XRootStorageFile(urlPath);
            #else
                std::cout << ">> StorageFile::CreateInstance > XRootD not supported." << std::endl;
                return NULL;
            #endif
        }

        return new LocalStorageFile(urlPath);
    }

    std::string StorageFile::GetURLPath() {
        return fileName;
    }



}
