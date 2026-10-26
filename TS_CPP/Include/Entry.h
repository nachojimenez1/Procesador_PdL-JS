#ifndef TS_ENTRY_H
#define TS_ENTRY_H

#include "unordered_map"
#include "string"

#include "TSConfig.h"
#include "AttributeValue.h"

namespace TS_CPP{

class SymbolTable;

/**
 * Entry class provides support for each row inside the symbol table
 * It consists of a lexeme, an id and an undefined amount of pairs consisting of attribute name and value (int/string)
 */
class Entry
{
public:
    /**
     * The constructor of an Entry
     * It should not be used directly, but using the method SymbolTable::createNewEntry
     * @param parentTable The parent table of this entry
     * @param lexeme The lexeme of this entry
     * @param id The unique identifier of the entry
     */
    Entry(SymbolTable* parentTable, const std::string& lexeme, int id);

    /**
     * This function retrieves the lexeme of this entry
     * @return A string which holds the lexeme of the entry
     */
    std::string getLexeme() const;

    /**
     * This function retrieves the unique identifier of the entry
     * @return The id of the entry
     */
    int getId() const;

    /**
     * This function search for the attribute with the specified name and returns its value
     * If the attribute does not exist, it returns a special attribute with attributeType=Null and data.str = nullptr (or data.num = 0)
     * @param attributeName The name of the attribute to search for
     * @return The value of the attribute if the attribute exist or NoAttribute (a special attribute with attributeType=NOATRIB and data.str = nullptr (or data.num = 0)) if it doesn't exist
     */
    AttributeValue* getAttribute(const std::string& attributeName);

    /**
     * This function retrieves the displacement of the entry.
     * @return The displacement of the entry
     */
    long long getDisplacement();

    /**
     * This function stores a new attribute with the desired name and value
     * If the attribute already exist, it overwrites it
     * @param attributeName The name of the attribute to store
     * @param value The value of the attribute to store
     */
    void setNumericAttribute(const std::string& attributeName, long long value);

    /**
     * This function stores the size of the entry in the parent symbol table
     * @param size The size of the entry
     */
    void setSize(int size);

    /**
     * This function stores the displacement of the entry in the attribute TS_DISPLACEMENT_ATTR_NAME
     * @param displacement The displacement of the entry
     */
    void _setDisplacement(int displacement);

    /**
     * This function should not be used during normal operation and is intended for advanced users only
     * This function marks the entry as deleted
     */
    void _markAsDeleted();

    /**
     * This function should not be used during normal operation and is intended for advanced users only
     * This function returns true if the entry has been marked as deleted and false otherwise
     * @return true if the entry has been marked as deleted and false otherwise
     */
    bool _isDeleted() const;

    /**
     * This function stores a new attribute with the desired name and value
     * If the attribute already exist, it overwrites it
     * @param attributeName The name of the attribute to store
     * @param value The value of the attribute to store
     */
    void setStringAttribute(const std::string& attributeName, const char* value);

    /**
     * This function deletes the attribute with the specified name from the Entry
     * @param attributeName The name of the attribute to delete
     * @return True if the attribute was successfully deleted or false if the attribute doesn't exist
     */
    bool deleteAttribute(const std::string& attributeName);

    /**
     * This function returns true if the size of the entry has been set and false otherwise
     * @return true if the size of the entry has been set and false otherwise
     */
    bool getHaveSize() const;

    /**
     * This function should not be used during normal operation and is intended for advanced users only
     * This function provides access to the Attribute Storage object
     * @return a reference to the Attribute Storage object
     */
    std::unordered_map<std::string,AttributeValue*>& _getAttributesStorage();

private:

    /// The unique identifier of the entry (it is unique for each TSManager, even 2 entries from different tables doesn't share their Entry::id)
    int id;

    /// A bolean that indicates if the size of the entry has been set
    bool haveSize = false;

    /// A boolean that indicates if the entry has been marked as deleted
    bool isDeleted = false;

    /// The lexeme of the entry
    std::string lexeme;

    /// The structure used to store all the attributes of the entry
    std::unordered_map<std::string,AttributeValue*> attributes;

    SymbolTable* parentTable;
};

}

#endif //TS_ENTRY_H
