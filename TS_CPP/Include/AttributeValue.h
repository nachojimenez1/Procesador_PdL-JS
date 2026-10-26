#ifndef PRACTICA_ATTRIBUTEVALUE_H
#define PRACTICA_ATTRIBUTEVALUE_H

#include <cstring>
#include <utility>
#include <cstdlib>

#include "TSConfig.h"

namespace TS_CPP{


/**
 * Class returned by Entry::getAttribute with the fields:
 * @param attributeType An enum containing the type of the attribute (if the attribute does not exist, entryType is Null)
 * @param data An union which contains either the integer stored or the pointer to the c string stored (if the attribute does not exist, data is nullptr (or 0 if viewed as int))
 */
class AttributeValue
{
public:
    /// Enum containing the type of attribute used
    enum AttributeType{NOATRIB = 0,INT,STRING};

    /// Union which stores either a numeric value (.num) or a pointer to a c string (.strPtr)
    union DataUnion{char* strPtr; long long integer;};

    typedef std::pair<AttributeType,DataUnion> AttributeValuePair;

    /**
     * This constructor initializes an AttributeValue object with a c string as a value
     * If the strPtr is null, an Atribute of type NOATRIB is returned instead
     * @param strPtr The pointer to the string that will be copied inside and stored as the value
     */
    explicit AttributeValue(const char* strPtr);

    /**
     * This constructor initializes an AttributeValue object with a long long as a value
     * @param integer The long long number stored as the value
     */
    explicit AttributeValue(long long integer);

    /**
     * This destructor check if the value is a c string, and if it is, it frees the memory pointed by the stored pointer before freeing itself
     */
    ~AttributeValue();

    /**
     * The == operator returns true if both AttributeValues have the same type and its value is the same (either the c string or the integer) or false otherwise
     * @param other The other AttributeValue used to compare
     * @return true if both AttributeValues have the same type and its value is the same (either the c string or the integer) or false otherwise
     */
    bool operator== (const AttributeValue& other);

    /**
     * The != operator returns true if both AttributeValues have different types or its value is different (either the c string or the integer) or false otherwise
     * @param other The other AttributeValue used to compare
     * @return true if both AttributeValues have different types or its value is different (either the c string or the integer) or false otherwise
     */
    bool operator!= (const AttributeValue& other);

    /**
     * This function get the value of the attribute
     * The availability of the string pointed by DataUnion is guaranteed until object destruction
     * @return A std::pair with the first parameter being an Enum AttributeType representing the type of the AttributeValue and the second parameter being a DataUnion to either a string or int (check the first parameter to get its actual type)
     */
    std::pair<AttributeType,DataUnion> getValue();

    /**
     * This function returns true if the data stored inside is valid or false if the data used is invalid (i.e: a null pointer to a string)
     * @return true if the data stored inside is valid or false if the data used is invalid
     */
    bool isAValidAttributeValue();

private:
    /// The type of the data stored in DataUnion
    AttributeType type;

    /// The data stored inside the AttributeValue object, which consist of an union that can be used as a pointer to a c string or as a long long value
    DataUnion data;
};

}

#endif //PRACTICA_ATTRIBUTEVALUE_H
