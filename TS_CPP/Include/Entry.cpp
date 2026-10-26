#include "TSConfig.h"
#include "Entry.h"
#include "SymbolTable.h"

using namespace TS_CPP;

Entry::Entry(SymbolTable* parentTable, const std::string& lexeme, int id)
{
    this->parentTable = parentTable;
    this->lexeme = lexeme;
    this->id = id;
    this->haveSize = false;
    this->isDeleted = false;
}

std::string Entry::getLexeme() const
{
    return lexeme;
}

int Entry::getId() const
{
    return id;
}

AttributeValue* Entry::getAttribute(const std::string& attributeName)
{
    if(attributes.find(attributeName) == attributes.end())
    {
        tsinfo("Attribute of entry %d (%s) with name=%s not found",id,lexeme.c_str(),attributeName.c_str());
        return nullptr;
    }
    else
    {
        tsdebug("Attribute of entry %d (%s) with name=%s found",id,lexeme.c_str(),attributeName.c_str());
        return attributes[attributeName];
    }
}

void Entry::setNumericAttribute(const std::string &attributeName, long long value)
{
    if(attributes.find(attributeName) == attributes.end())
    {
        tsinfo("Setting new attribute of entry %d (%s) with name=%s",id,lexeme.c_str(),attributeName.c_str());
    }
    else
    {
        tsinfo("Attribute of entry %d (%s) with name=%s already exist. Overwriting previous value...",id,lexeme.c_str(),attributeName.c_str());
        delete attributes[attributeName];
    }

    attributes[attributeName] = new AttributeValue(value);
}

void Entry::setStringAttribute(const std::string &attributeName, const char *value)
{
    if(attributes.find(attributeName) == attributes.end())
    {
        tsinfo("Setting new attribute of entry %d (%s) with name=%s & value=%s",id,lexeme.c_str(),attributeName.c_str(),value);
    }
    else
    {
        tsinfo("Attribute of entry %d (%s) with name=%s already exist. Overwriting previous value with the new one (%s)",id,lexeme.c_str(),attributeName.c_str(),value);
        delete attributes[attributeName];
    }

    attributes[attributeName] = new AttributeValue(value);
}

bool Entry::deleteAttribute(const std::string &attributeName)
{
    if(attributes.find(attributeName) == attributes.end()) // if entry does not exist
    {
        tserror("Attribute of entry %d (%s) with name=%s does not exist",id,lexeme.c_str(), attributeName.c_str());
        return false;
    }
    else
    {
        attributes.erase(attributeName);
        tsinfo("Attribute of entry %d (%s) with name=%s successfully deleted",id,lexeme.c_str(), attributeName.c_str());
        return true;
    }
}

std::unordered_map<std::string, AttributeValue*> &Entry::_getAttributesStorage()
{
    return attributes;
}

void Entry::_setDisplacement(int displacement)
{
    setNumericAttribute(TS_DISPLACEMENT_ATTR_NAME,displacement);
}

void Entry::setSize(int size)
{
    setNumericAttribute(TS_SIZE_ATTR_NAME,size);
    parentTable->_addTableEntriesSize(size);
    this->haveSize = true;
}

long long Entry::getDisplacement()
{
    return getAttribute(TS_DISPLACEMENT_ATTR_NAME)->getValue().second.integer;
}

bool Entry::getHaveSize() const
{
    return this->haveSize;
}

void Entry::_markAsDeleted()
{
    this->isDeleted = true;
}

bool Entry::_isDeleted() const
{
    return this->isDeleted;
}
