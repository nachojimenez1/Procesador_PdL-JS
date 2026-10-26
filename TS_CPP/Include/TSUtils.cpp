#include "TSUtils.h"

using namespace TS_CPP;

FILE* TSUtils::file;
const std::string TSUtils::voidString = std::string("");

bool TSUtils::writeAllTablesToFile(TSManager* tsManager, const std::string& filePath)
{
    bool success = true;

    file = strcmp(filePath.c_str(),"")==0?file:fopen(filePath.c_str(),"w");
    if(file == nullptr)
    {
        tserror("File \"%s\" cannot be opened for write: %s",filePath.c_str(),strerror(errno));
        return false;
    }

    tsdebug("Writing all tables");

    bool gotSomeTables = false;
    for(auto& table: tsManager->_getTablesStorage())
    {
        fprintf(file,"\n\n");
        success &= writeTableToFile(table, filePath);
        gotSomeTables = true;
    }
    fprintf(file,"\n\n");
    if(!gotSomeTables)
    {
        fprintf(file,"(el almacen de tablas esta vacio)\n");
    }

    fclose(file);
    file = nullptr;
    return success;
}

bool TSUtils::writeTableToFile(SymbolTable* table, const std::string& filePath)
{
    bool success = true;
    bool fileAlreadyOpened = file != nullptr;
    if(fileAlreadyOpened)
    {
        tsdebug("File is already opened, appending the Table to the file",NULL);
    }
    else
    {
        file = fopen(filePath.c_str(),"w");
        if(file == nullptr)
        {
            tserror("File \"%s\" cannot be opened for write: %s",filePath.c_str(),strerror(errno));
            return false;
        }
    }
    tsdebug("Writing table with name=\"%s\" to file \"%s\"",table->getTableName().c_str(),filePath.c_str());

    fprintf(file,"CONTENIDO ACTUAL DE LA TABLA %s # %d :\n",table->getTableName().c_str(),table->getId());
    bool gotSomeEntries = false;
    for(auto& entry: table->_getEntriesStorage())
    {
        fprintf(file,"---------------------------------------------------\n");
        success &= writeEntryToFile(entry, filePath);
        gotSomeEntries = true;
    }
    fprintf(file,"---------------------------------------------------\n");
    if(!gotSomeEntries)
    {
        fprintf(file,"(la tabla esta vacia)\n");
    }

    if(!fileAlreadyOpened)
    {
        fclose(file);
        file = nullptr;
    }
    return success;
}

bool TSUtils::writeEntryToFile(Entry* entry, const std::string& filePath)
{
    if(entry->_isDeleted())
    {
        tsdebug("Entry is marked as deleted, skipping it",NULL);
        return true;
    }

    bool success = true;
    bool fileAlreadyOpened = file != nullptr; //file.is_open();
    if(fileAlreadyOpened)
    {
        tsdebug("File is already opened, appending the entry to the file",NULL);
    }
    else
    {
        file = fopen(filePath.c_str(),"w");
        if(file == nullptr)
        {
            tserror("File \"%s\" cannot be opened for write",filePath.c_str());
            return false;
        }
    }

    tsdebug("Writing entry with lexeme=\"%s\" to file \"%s\"",entry->getLexeme().c_str(),filePath.c_str());

    fprintf(file,"* LEXEMA : '%s' \n",entry->getLexeme().c_str());
    fprintf(file,"* ID     : '%d' \n",entry->getId());

    if (entry->_getAttributesStorage().empty())
    {
        fprintf(file,"(no se han definido atributos para este lexema)\n");
    }
    else
    {
        fprintf(file,"ATRIBUTOS :\n");
        for(auto& attribute : entry->_getAttributesStorage())
        {
            success &= writeAttributeToFile(attribute.first,attribute.second,filePath);
        }
    }

    if(!fileAlreadyOpened)
    {
        fclose(file);
        file = nullptr;
    }
    return success;
}

bool TSUtils::writeAttributeToFile(const std::string& attributeName, AttributeValue* value, const std::string& filePath)
{
    bool fileAlreadyOpened = file != nullptr; //file.is_open();
    if(fileAlreadyOpened)
    {
        tsdebug("File is already opened, appending the attribute to the file",NULL);
    }
    else
    {
        file = fopen(filePath.c_str(),"w");
        if(file == nullptr)
        {
            tserror("File \"%s\" cannot be opened for write",filePath.c_str());
            return false;
        }
    }

    std::pair<AttributeValue::AttributeType, AttributeValue::DataUnion> valuePair;

    #if TS_PRINT_MESSAGES_LEVEL >= TS_LEVEL_DEBUG
    if (value->isAValidAttributeValue())
    {
        valuePair = value->getValue();
        if (valuePair.first == AttributeValue::STRING)
        {
            tsdebug("Writing attribute with attributeName=\"%s\" and value=\"%s\" to file \"%s\"",
                    attributeName.c_str(), valuePair.second.strPtr, filePath.c_str());
        }
        else if (valuePair.first == AttributeValue::INT)
        {
            tsdebug("Writing attribute with attributeName=\"%s\" and value=%lld to file \"%s\"", attributeName.c_str(),
                    valuePair.second.integer, filePath.c_str());
        }
        else
        {
            tserror("This line should not be called never. if it does, a bug has been detected with the type of the attribute being printed",
                    NULL);
        }
    }
    #endif

    fprintf(file,"   + %s : ",attributeName.c_str());
    if(!value->isAValidAttributeValue())
    {
        fprintf(file,TS_NULLSTRINGREPLACEMENT "\n");
    }
    else if ((valuePair = value->getValue()).first==AttributeValue::STRING)
    {
        fprintf(file,"'%s'\n",valuePair.second.strPtr);
    }
    else
    {
        fprintf(file,"%lld\n",valuePair.second.integer);
    }

    if(!fileAlreadyOpened)
    {
        #if TS_PRINT_MESSAGES_LEVEL >= TS_LEVEL_INFO
        if (value->isAValidAttributeValue())
        {
            valuePair = value->getValue();
            if (valuePair.first == AttributeValue::STRING)
            {
                tsdebug("Attribute with attributeName=\"%s\" and value=\"%s\" successfully written to file \"%s\"",
                        attributeName.c_str(), valuePair.second.strPtr, filePath.c_str());
            }
            else if (valuePair.first == AttributeValue::INT)
            {
                tsdebug("Attribute with attributeName=\"%s\" and value=%lld successfully written to file \"%s\"",
                        attributeName.c_str(), valuePair.second.integer, filePath.c_str());
            }
            else
            {
                tserror("This line should not be called never. if it does, a bug has been detected with the type of the attribute being printed",
                        NULL);
            }
        }
        #endif
        fclose(file);
        file = nullptr;
    }
    return true;
}

bool TSUtils::writeAllTablesToFileF(TSManager* tsManager, FILE* filePtr)
{
    file = filePtr;
    return writeAllTablesToFile(tsManager, voidString);
}

bool TSUtils::writeTableToFileF(SymbolTable* table, FILE* filePtr)
{
    file = filePtr;
    return writeTableToFile(table,voidString);
}

bool TSUtils::writeEntryToFileF(Entry* entry, FILE* filePtr)
{
    file = filePtr;
    return writeEntryToFile(entry,voidString);
}

bool TSUtils::writeAttributeToFileF(const std::string& attributeName, AttributeValue* value, FILE* filePtr)
{
    file = filePtr;
    return writeAttributeToFile(attributeName,value,voidString);
}
