#ifndef _EC_PTR_TEST_H__
#define _EC_PTR_TEST_H__

class PtrTest {
public:
	bool TestConstructor();
	bool TestDestructor();
	bool TestAssign();
	bool TestNull();
	bool TestOperators();
	bool TestReferenceCycle();
	bool TestWeakReferences();
};

#endif
