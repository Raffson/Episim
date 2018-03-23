//============================================================================
// Name        : DesignByContract.h
// Author      : Serge Demeyer
// Version     :
// Copyright   : Project Software Engineering - BA1 Informatica - Serge Demeyer - University of Antwerp
// Description : Declarations for design by contract in C++
//
// Modified by Raphael A.
//============================================================================

#include <iostream>

using namespace std;

#define REQUIRE(assertion, what) \
	if (!(assertion)) { cerr << what << endl; assert(assertion); }

#define ENSURE(assertion, what) \
	if (!(assertion)) { cerr << what << endl; assert(assertion); }
