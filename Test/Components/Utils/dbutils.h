/**
 * @file dbutils.h
 * @brief utilities to configure and print configuration databases for testing
 * purpouses
 **/
#ifndef _DB_UTILS_H__
#define _DB_UTILS_H__

#include "AnyType.h"
#include "ConfigurationDatabase.h"
namespace MARTe {

namespace DB {
/**
 * @brief pretty print a database structure node
 * @param db to print
 * @param name of the database
 * @param tabs to add in pretty print for leveling
 **/
void print(ConfigurationDatabase db, const char *name, const char *tabs = "");

/**
 * @brief pretty print a database structure recursively
 * @param db to print
 * @param label of the node
 * @param tabs to add in pretty print for leveling
 */
void pprint(ConfigurationDatabase &db, const char *label, int tabs = 0);
} // namespace DB

namespace DSDB {
ConfigurationDatabase create();
bool add_signal(ConfigurationDatabase &db, const char *name, const char *type,
                unsigned N = 0, unsigned M = 0);
bool append_to_signal(ConfigurationDatabase &db, const char *name,
                      const char *property, AnyType value);
bool set_field(ConfigurationDatabase &db, const char *field, AnyType value);
ConfigurationDatabase make_cdb(ConfigurationDatabase db, bool &ok);
} // namespace DSDB

namespace GAMDB {
/**
 * @brief crate an empty GAM ConfigurationDatabase for testing
 * @return an empty GAM ConfigurationDatabase
 **/
ConfigurationDatabase create();

/**
 * @brief add an input signal to a GAM configuration
 *
 * This function can be used to add an input signal (both scalar or array)
 * to a GAM.
 *
 * @param[in,out] db of the GAM
 * @param name of the signal
 * @param type of the signal
 * @param datasource name
 * @param N of dimensions
 * @param M of elements per dimension
 * @return true if the input signal is added correctly
 * @return false otherwise
 **/
bool add_input(ConfigurationDatabase &db, const char *name, const char *type,
               const char *datasource, unsigned N = 1, unsigned M = 1);

/**
 * @brief add an output signal to a GAM configuration
 *
 * This function can be used to add an output signal (both scalar or array)
 * to a GAM.
 *
 * @param[in,out] db of the GAM
 * @param name of the signal
 * @param type of the signal
 * @param datasource name
 * @param N of dimensions
 * @param M of elements per dimension
 * @return true if the output signal is added correctly
 * @return false otherwise
 **/
bool add_output(ConfigurationDatabase &db, const char *name, const char *type,
                const char *datasource, unsigned N = 1, unsigned M = 1);

/**
 * @brief set a parameter of a GAM configuration
 *
 * This function can be used to add a parameter
 *
 * @param[in,out] db of the GAM
 * @param name of the parameter
 * @param value of the parameter
 * @return true if the set of the parameter was successful
 * @return false otherwise
 **/
bool set_parameter(ConfigurationDatabase &db, const char *name,
                   const AnyType &value);

/**
 * @brief Pretty print a GAM configuration
 * @param db of the GAM to be printed
 **/
void print(ConfigurationDatabase db);

/**
 * @brief create a ConfigurationDatabase from the initial definition database
 * @param db of the GAM definition
 * @param[out] ok status of the operation
 * @return computed ConfigurationDatabase
 **/
ConfigurationDatabase make_cdb(ConfigurationDatabase db, bool &ok);
} // namespace GAMDB
} // namespace MARTe

#endif
