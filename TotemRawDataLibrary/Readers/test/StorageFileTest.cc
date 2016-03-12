#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"
#include <string.h>
#include <stdlib.h>

using namespace Totem;

int testStorageFile(StorageFile *storageFile);
bool testNonExistingFile(StorageFile *storageFile);

// ----- adjust these variables --------
std::string localPathToNonExistingFile = "/afs/cern.ch/user/m/mwojakow/file.test_1";
std::string localPath = "/afs/cern.ch/user/m/mwojakow/file.test";

std::string castorRFIOPathToNonExistingFile = "/castor/cern.ch/user/m/mwojakow/file.test_1";
std::string castorRFIOPath = "/castor/cern.ch/user/m/mwojakow/file.test";

std::string castorXRootPathToNonExistingFile = "root://castorpublic.cern.ch//castor/cern.ch/user/m/mwojakow/file.test_1";
std::string castorXRootPath = "root://castorpublic.cern.ch//castor/cern.ch/user/m/mwojakow/file.test";

std::string eosXRootPathToNonExistingFile = "root://eostotem.cern.ch//eos/totem/user/m/mwojakow/file.test_1";
std::string eosXRootPath = "root://eostotem.cern.ch//eos/totem/user/m/mwojakow/file.test";

char *fileContent = const_cast<char*>("asd\ns\n");
// -------------------------------------

int main() {
    putenv(const_cast<char*>("STAGE_HOST=castorpublic"));

    //local
    if(testNonExistingFile(StorageFile::CreateInstance(localPathToNonExistingFile)) |
            testStorageFile(StorageFile::CreateInstance(localPath))) {
        printf("[LocalStorageFile] test failed\n");
    }
    //castor rfio
    if(testNonExistingFile(StorageFile::CreateInstance(castorRFIOPathToNonExistingFile)) |
            testStorageFile(StorageFile::CreateInstance(castorRFIOPath))) {
        printf("[RFIOStorageFile] test failed\n");
    }
    //castor xroot
    if(testNonExistingFile(
            StorageFile::CreateInstance(castorXRootPathToNonExistingFile)) |
            testStorageFile(StorageFile::CreateInstance(castorXRootPath))) {
        printf("[XRootStorageFile] test failed\n");
    }
    //eos xroot
    if(testNonExistingFile(
            StorageFile::CreateInstance(eosXRootPathToNonExistingFile)) |
            testStorageFile(StorageFile::CreateInstance(eosXRootPath))) {
        printf("[XRootStorageFile] test failed\n");
    }

}

bool testNonExistingFile(StorageFile *storageFile) {
    storageFile->OpenFile();
    bool isOpened = storageFile->IsOpened();
    if(!isOpened) {
        storageFile->PrintError("[Opening non existing file] " + storageFile->GetURLPath());
    }

    delete storageFile;

    return isOpened;
}

int testStorageFile(StorageFile *storageFile) {
    storageFile->OpenFile();

    if (!(storageFile->CheckEOF() == 0 && storageFile->CurrentPosition() == 0)) {
        printf("Problem after opening a file\n");
        delete storageFile;
        return 1;
    }

    size_t fileContentLength = strlen(fileContent);

    char buffer[fileContentLength+1];
    memset(buffer, 0, fileContentLength+1);

    size_t bytes_read = storageFile->ReadData(buffer, sizeof(char), fileContentLength-1);
    if (!(bytes_read == fileContentLength-1 && storageFile->CheckEOF() == 0 && storageFile->CurrentPosition() == (long) fileContentLength-1)) {
        printf("Problem after reading data from a file\n");
        delete storageFile;
        return 2;
    }

    storageFile->Seek(0);

    if (!(storageFile->CurrentPosition() == 0 && storageFile->CheckEOF() == 0)) {
        printf("Problem after seeking\n");
        delete storageFile;
        return 3;
    }

    bytes_read = storageFile->ReadData(buffer, sizeof(char), fileContentLength+10);
    //printf("%d\n", bytes_read);
    //printf("%d\n", storageFile->CheckEOF());
    //printf("%s\n", buffer);

    //todo CHECK! strange castor behaviour
    storageFile->ReadData(NULL, sizeof(char), 1);
    if (!(bytes_read == fileContentLength && storageFile->CheckEOF() != 0 && strcmp(fileContent, buffer) == 0)) {
        printf("Problem after reading to much data from a file\n");
        delete storageFile;
        return 4;
    }

    storageFile->Seek(0);
    if (!(storageFile->CheckEOF() == 0 && storageFile->CurrentPosition() == 0)) {
        printf("Problem after rewinding a file\n");
        delete storageFile;
        return 5;
    }

    delete storageFile;
    return 0;
}