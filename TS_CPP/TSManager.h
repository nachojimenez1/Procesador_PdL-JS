#ifndef TS_TSMANAGER_H
#define TS_TSMANAGER_H

#include "string"
#include "list"
#include "unordered_map"
#include "cstring"

#include "TSConfig.h"
#include "SymbolTable.h"


/**
 * This library provides an easy to use & efficient implementation of a Symbol table
 * This library is developed by Victor Arzoz Consuegra (@vacmg) under the MIT License
 * You should have received a copy of the License with this software,
 * if that is not the case, you can get a copy from https://opensource.org/license/mit/
 */
namespace TS_CPP {

/**
 *
 * TSManager class provides functionality to create, delete and manage Symbol Tables
 * More than one instance can be used in the same program,
 * but for most cases, one manager is enough to support an arbitrary number of tables
 *
 */
class TSManager
{
public:

    /**
     * The constructor of a TSManager
     * @param firstID The first table ID to be used (next tables will use the next greater int as their ID in a sequence)
     */
    explicit TSManager(int firstID);

    /**
     * The constructor of a TSManager
     * The first table ID to be used is 1
     */
    TSManager();

    /**
     * The destructor of a TSManager
     * It deletes all of its tables before deleting itself
     */
    ~TSManager();

    /**
     * This function creates a new symbol table stored inside the manager class with a unique id
     * @param tableName The name of the table (opctional)
     * @return A pointer to the new symbol table or nullptr if it doesn't exist
     */
    SymbolTable* createNewTable(const std::string& tableName);

    /**
     * This function creates a new symbol table stored inside the manager class with a unique id
     * @return A pointer to the new symbol table or nullptr if it doesn't exist
     */
    SymbolTable* createNewTable();

    /**
     * This function returns the symbol table referenced by the id
     * @param id The identifier of the symbol table
     * @return A pointer to the symbol table referenced by the id or nullptr if it doesn't exist
     */
    SymbolTable* getTable(int id);

    /**
     * This function search and return the table referenced by the table name
     * @param tableName The name of the table to search for
     * @return A pointer to the symbol table referenced by the table name or nullptr if it doesn't exist
     */
    SymbolTable* searchTable(const std::string& tableName);

    /**
     * This function deletes the table referenced by the id
     * @param id The identifier of the symbol table
     * @return true if the table was successfully deleted and false otherwise
     */
    bool deleteTable(int id);

    /**
     * This function search and return the entry with the selected identifier no matter in which table is located
     * @param id The identifier of the symbol table
     * @param tableId The identifier of the table where the entry is located (output parameter, optional)
     * @return A pointer to the Entry referenced by the id or nullptr if it doesn't exist
     */
    Entry* searchAllTablesForEntryWithId(int id, int* tableId = nullptr);

    /**
     * This function should not be used during normal operation and is intended for advanced users only
     * This function provides access to the Symbol Tables Storage object
     * @return a reference to the Symbol Tables Storage object
     */
    std::list<SymbolTable*>& _getTablesStorage();

private:

    /// Table ID Counter
    int lastTableID;

    /// Entry ID Counter
    int lastEntryID;

    /// Ordered linked list of tables (by creation time (first one is the older one))
    std::list<SymbolTable*> symbolTableList;
};

}

#include "TSUtils.h"

#endif //TS_TSMANAGER_H
