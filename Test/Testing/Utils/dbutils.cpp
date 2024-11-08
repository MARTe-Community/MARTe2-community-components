#include "dbutils.h"
#include "ConfigurationDatabase.h"

#include "TypeDescriptor.h"
#include "TestMacros.h"
#include <stdio.h>

namespace MARTe {
namespace DB {
void print(ConfigurationDatabase db, const char *name, const char *tabs) {
  printf("%s%s:\n", tabs, name);
  for (uint32 i = 0u; i < db.GetNumberOfChildren(); i++) {
    const char *name = db.GetChildName(i);
    StreamString value;
    db.Read(name, value);
    printf("%s  %s: %s\n", tabs, name, value.Buffer());
  }
}

void pprint(ConfigurationDatabase &db, const char *label, int tabs) {
  char *ctabs = new char[2 * tabs + 1];
  memset(ctabs, ' ', 2 * tabs);
  ctabs[tabs] = 0;
  printf("%s%s:\n", ctabs, label);
  for (uint32 i = 0u; i < db.GetNumberOfChildren(); i++) {
    const char *name = db.GetChildName(i);
    StreamString value;
    if (db.Read(name, value)) {
      printf("%s %s: %s\n", ctabs, name, value.Buffer());
    } else if (db.MoveToChild(i)) {
      pprint(db, name, tabs + 1);
      db.MoveToAncestor(1);
    } else {
      printf("%s %s: [error]\n", ctabs, name);
    }
  }
  delete[] ctabs;
}
} // namespace DB

namespace DSDB {
ConfigurationDatabase create() {
  ConfigurationDatabase db;
  db.MoveToRoot();
  db.CreateAbsolute("Signals");
  return db;
}

bool add_signal(ConfigurationDatabase &db, const char *name, const char *type,
                unsigned N, unsigned M) {
  T_ASSERT_TRUE(db.MoveAbsolute("Signals"));
  T_ASSERT_TRUE(db.CreateRelative(name));
  T_ASSERT_TRUE(db.Write("Type", type));
  if (N > 0) {
    T_ASSERT_TRUE(db.Write("NumberOfElements", N));
  }
  if (M > 0) {
    T_ASSERT_TRUE(db.Write("NumberOfDimensions", M));
  }
  T_ASSERT_TRUE(db.MoveToRoot());
  return true;
}

bool append_to_signal(ConfigurationDatabase &db, const char *name,
                      const char *property, AnyType value) {
  T_ASSERT_TRUE(db.MoveAbsolute("Signals"));
  T_ASSERT_TRUE(db.MoveRelative(name));
  T_ASSERT_TRUE(db.Write(property, value));
  T_ASSERT_TRUE(db.MoveToRoot());
  return true;
}
bool set_field(ConfigurationDatabase &db, const char *field, AnyType value) {
  T_ASSERT_TRUE(db.MoveToRoot());
  T_ASSERT_TRUE(db.Write(field, value));
  return true;
}

ConfigurationDatabase make_cdb(ConfigurationDatabase db, bool &ok) {
  ConfigurationDatabase cdb;
  ok = cdb.MoveToRoot();
  ok &= cdb.CreateAbsolute("Signals");

  ok &= db.MoveAbsolute("Signals");
  uint32 size = db.GetNumberOfChildren();
  uint32 dsize = 0;
  uint32 esize = 0;
  ok &= cdb.MoveAbsolute("Signals");
  for (uint32 i = 0; i < size; i++) {
    ok &= db.MoveToChild(i);
    StreamString type;
    db.Read("Type", type);
    TypeDescriptor td =
        TypeDescriptor::GetTypeDescriptorFromTypeName(type.Buffer());
    if (!db.Read("NumberOfElements", esize))
      esize = 1;
    if (!db.Read("NumberOfDimensions", dsize))
      dsize = 1;
    uint32 size = (td.numberOfBits / 8u) * (esize * dsize);
    ok &= cdb.Write(db.GetName(), db);
    ok &= cdb.MoveRelative(db.GetName());
    ok &= cdb.Write("ByteSize", size);
    ok &= cdb.Write("QualifiedName", db.GetName());
    ok &= cdb.MoveToAncestor(1u);
    ok &= db.MoveToAncestor(1u);
  }
  db.MoveToRoot();
  cdb.MoveToRoot();

  return cdb;
}
} // namespace DSDB

namespace GAMDB {
ConfigurationDatabase create() {
  ConfigurationDatabase db;
  db.MoveToRoot();
  db.CreateAbsolute("InputSignals");
  db.CreateAbsolute("OutputSignals");
  return db;
}

bool add_input(ConfigurationDatabase &db, const char *name, const char *type,
               const char *datasource, unsigned N, unsigned M) {
  T_ASSERT_TRUE(db.MoveAbsolute("InputSignals"));
  T_ASSERT_TRUE(db.CreateRelative(name));
  T_ASSERT_TRUE(db.Write("DataSource", datasource));
  T_ASSERT_TRUE(db.Write("QualifiedName", name));
  T_ASSERT_TRUE(db.Write("Type", type));
  T_ASSERT_TRUE(db.Write("NumberOfElements", N));
  T_ASSERT_TRUE(db.Write("NumberOfDimensions", M));
  T_ASSERT_TRUE(db.MoveToRoot());
  return true;
}

bool add_output(ConfigurationDatabase &db, const char *name, const char *type,
                const char *datasource, unsigned N, unsigned M) {
  T_ASSERT_TRUE(db.MoveAbsolute("OutputSignals"));
  T_ASSERT_TRUE(db.CreateRelative(name));
  T_ASSERT_TRUE(db.Write("DataSource", datasource));
  T_ASSERT_TRUE(db.Write("Type", type));
  T_ASSERT_TRUE(db.Write("NumberOfElements", N));
  T_ASSERT_TRUE(db.Write("NumberOfDimensions", M));
  T_ASSERT_TRUE(db.MoveToRoot());
  return true;
}

bool set_parameter(ConfigurationDatabase &db, const char *name,
                   const AnyType &value) {

  T_ASSERT_TRUE(db.MoveToRoot());
  return db.Write(name, value);
}

ConfigurationDatabase make_cdb(ConfigurationDatabase db, bool &ok) {
  ConfigurationDatabase cdb;
  ok = cdb.MoveToRoot();
  ok &= cdb.CreateAbsolute("Signals.InputSignals");
  ok &= cdb.CreateAbsolute("Signals.OutputSignals");

  ok &= db.MoveAbsolute("InputSignals");
  uint32 size = db.GetNumberOfChildren();
  uint32 dsize = 0;
  uint32 esize = 0;
  uint32 bsize = 0;
  ok &= cdb.MoveAbsolute("Signals.InputSignals");
  for (uint32 i = 0; i < size; i++) {
    ok &= db.MoveToChild(i);
    StreamString type;
    db.Read("Type", type);
    TypeDescriptor td =
        TypeDescriptor::GetTypeDescriptorFromTypeName(type.Buffer());
    if (!db.Read("NumberOfElements", esize))
      esize = 1;
    if (!db.Read("NumberOfDimensions", dsize))
      dsize = 1;
    uint32 size = (td.numberOfBits / 8u) * (esize * dsize);
    bsize += size;
    ok &= cdb.Write(db.GetName(), db);
    ok &= cdb.MoveRelative(db.GetName());
    ok &= cdb.Write("ByteSize", size);
    ok &= cdb.Write("QualifiedName", db.GetName());
    ok &= cdb.MoveToAncestor(1u);
    ok &= db.MoveToAncestor(1u);
  }
  ok &= cdb.Write("ByteSize", bsize);
  db.MoveToRoot();
  cdb.MoveToRoot();

  db.MoveAbsolute("OutputSignals");
  size = db.GetNumberOfChildren();
  bsize = 0;
  cdb.MoveAbsolute("Signals.OutputSignals");
  for (uint32 i = 0; i < size; i++) {
    db.MoveToChild(i);
    StreamString type;
    db.Read("Type", type);
    TypeDescriptor td =
        TypeDescriptor::GetTypeDescriptorFromTypeName(type.Buffer());
    if (!db.Read("NumberOfElements", esize))
      esize = 1;
    if (!db.Read("NumberOfDimensions", dsize))
      dsize = 1;
    uint32 size = (td.numberOfBits / 8u) * (esize * dsize);
    bsize += size;
    ok &= cdb.Write(db.GetName(), db);
    ok &= cdb.MoveRelative(db.GetName());
    ok &= cdb.Write("ByteSize", size);
    ok &= cdb.Write("QualifiedName", db.GetName());
    ok &= cdb.MoveToAncestor(1u);
    ok &= db.MoveToAncestor(1u);
  }
  ok &= cdb.Write("ByteSize", bsize);

  if (!ok) {
    fprintf(stderr, "ERROR: Something went wrong here %s:%d\n",
            __PRETTY_FUNCTION__, __LINE__);
  }

  db.MoveToRoot();
  cdb.MoveToRoot();
  return cdb;
}

void print(ConfigurationDatabase db) {
  db.MoveToRoot();
  printf("GAM:\n");
  uint32 input_id = 0;
  uint32 output_id = 0;
  for (uint32 i = 0u; i < db.GetNumberOfChildren(); i++) {
    const char *name = db.GetChildName(i);
    if (strcmp(name, "InputSignals") == 0) {
      input_id = i;
    } else if (strcmp(name, "OutputSignals") == 0) {
      output_id = i;
    } else {
      StreamString value;
      db.Read(name, value);
      printf("  %s: %s\n", name, value.Buffer());
    }
  }
  printf("  InputSignals:\n");
  db.MoveToChild(input_id);
  for (uint32 i = 0u; i < db.GetNumberOfChildren(); i++) {
    db.MoveToChild(i);
    DB::print(db, db.GetName(), "    ");
    db.MoveToAncestor(1u);
  }
  db.MoveToRoot();

  printf("  OutputSignals:\n");
  db.MoveToChild(output_id);
  for (uint32 i = 0u; i < db.GetNumberOfChildren(); i++) {
    db.MoveToChild(i);
    DB::print(db, db.GetName(), "    ");
    db.MoveToAncestor(1u);
  }
  db.MoveToRoot();
}
} // namespace GAMDB
} // namespace MARTe
