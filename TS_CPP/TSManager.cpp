#include "TSManager.h"

using namespace TS_CPP;

TSManager::TSManager()
{
    lastTableID = 0;
    lastEntryID = 0;
    tsinfo("Created TSManager with lastTableID=%d, lastEntryID=%d",lastTableID,lastEntryID);
}

TSManager::TSManager(int firstID)
{
    lastTableID = firstID-1;
    lastEntryID = 0;
    tsinfo("Created TSManager with lastTableID=%d, lastEntryID=%d",lastTableID,lastEntryID);
}

SymbolTable* TSManager::createNewTable()
{
    auto* table = new SymbolTable(++lastTableID, &lastEntryID);
    symbolTableList.push_back(table);
    tsinfo("Created new Table (0x%llx) with id=%d",(unsigned long long)table,table->getId());
    return table;
}

SymbolTable* TSManager::createNewTable(const ::std::string& tableName)
{
    auto* table = new SymbolTable(++lastTableID, &lastEntryID, tableName);
    symbolTableList.push_back(table);
    tsinfo("Created new Table (0x%llx) with id=%d, name=%s",(unsigned long long)table,table->getId(),table->getTableName().c_str());
    return table;
}

SymbolTable *TSManager::getTable(const int id)
{
    SymbolTable* table = nullptr;

    auto iter = symbolTableList.begin();
    while (table == nullptr && iter!=symbolTableList.end() && (*iter)->getId()<=id)
    {
        if((*iter)->getId() == id)
        {
            table = *iter;
        }
        iter++;
    }
#if TS_PRINT_MESSAGES_LEVEL == TS_LEVEL_DEBUG
    if(table == nullptr)
    {
        tsinfo("Table with id=%d does not exist", id);
    }
    else
    {
        tsinfo("Table with id=%d found at 0%llx", id, (unsigned long long)table);
    }
#endif
    return table;
}

SymbolTable *TSManager::searchTable(const ::std::string& tableName)
{
    SymbolTable* table = nullptr;

    auto iter = symbolTableList.begin();
    while (table == nullptr && iter!=symbolTableList.end())
    {
        if((*iter)->getTableName() == tableName)
        {
            table = *iter;
        }
        iter++;
    }
#if TS_PRINT_MESSAGES_LEVEL >= TS_LEVEL_INFO
    if(table == nullptr)
    {
        tsinfo("Table with name=%s does not exist", tableName.c_str());
    }
    else
    {
        tsinfo("Table with name=%s found at 0%llx", tableName.c_str(), (unsigned long long)table);
    }
#endif
    return table;
}

bool TSManager::deleteTable(int id)
{
    SymbolTable* table = getTable(id);
    if(table == nullptr)
    {
        tserror("Table with id=%d does not exist", id);
        return false;
    }
    symbolTableList.remove(table);
    delete table;
    tsinfo("Table with id=%d successfully deleted", id);
    return true;
}

TSManager::~TSManager()
{
    for(auto table : symbolTableList)
    {
        delete table;
    }
}

Entry *TSManager::searchAllTablesForEntryWithId(int id, int *tableId)
{
    Entry* entry = nullptr;
    for(auto table : symbolTableList)
    {
        entry = table->getEntry(id);
        if(entry!= nullptr)
        {
            if(tableId!= nullptr)
            {
                *tableId = table->getId();
            }
            break;
        }
    }
    return entry;
}

::std::list<SymbolTable *> &TSManager::_getTablesStorage()
{
    return symbolTableList;
}
