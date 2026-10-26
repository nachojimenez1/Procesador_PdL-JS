#ifndef TS_TSUTILS_H
#define TS_TSUTILS_H

#include <fstream>

#include "../TSManager.h"

#include "TSConfig.h"
#include "SymbolTable.h"
#include "Entry.h"

namespace TS_CPP{

/**
 * This class contains a set of useful functions to extend or debug your project
 */
class TSUtils
{
public:
    static bool writeAllTablesToFile(TSManager* tsManager, const std::string& filePath);
    static bool writeTableToFile(SymbolTable* table, const std::string& filePath);
    static bool writeEntryToFile(Entry* entry, const std::string& filePath);
    static bool writeAttributeToFile(const std::string& attributeName, AttributeValue* value, const std::string& filePath);

    static bool writeAllTablesToFileF(TSManager* tsManager, FILE* filePtr);
    static bool writeTableToFileF(SymbolTable* table, FILE* filePtr);
    static bool writeEntryToFileF(Entry* entry, FILE* filePtr);
    static bool writeAttributeToFileF(const std::string& attributeName, AttributeValue* value, FILE* filePtr);
private:
    static FILE* file;
    const static std::string voidString;
};

}

#endif //TS_TSUTILS_H
