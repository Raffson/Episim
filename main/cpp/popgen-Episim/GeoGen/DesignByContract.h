//============================================================================
// Name        : DesignByContract.h
// Author      : Serge Demeyer
// Version     :
// Copyright   : Project Software Engineering - BA1 Informatica - Serge Demeyer - University of Antwerp
// Description : Declarations for design by contract in C++
//
// Modified by Raphael Assa to a point where it is unrecognizable...
// Took things straight from <assert.h> since the original DesignByContract.h would cause trouble...
//============================================================================

#include <sys/cdefs.h>
#ifdef __cplusplus
#include <stdlib.h>
#endif // __cplusplus

// Question to the professor on tuesday...
// Does it make sense to use design by contract in this context?
// i.e. in scientific simulating software...
// I believe it does, however I would like to verify this...

//#ifndef NDEBUG
// If NDEBUG is not defined, we abort...
// (I'm assuming NDEBUG is why we couldn't get our assertions to work...)

//Commenting out the ifndef to abort no matter what...
#define REQUIRE(assertion, what)                                                                                       \
        if (!(assertion))                                                                                              \
        ((void)printf("%s:%u: failed pre-condition `%s'\n", __FILE__, __LINE__, what), abort())

#define ENSURE(assertion, what)                                                                                        \
        if (!(assertion))                                                                                              \
        ((void)printf("%s:%u: failed post-condition `%s'\n", __FILE__, __LINE__, what), abort())

/*
#else
// If NDEBUG is defined, we may not want to abort...
// Could be useful to see if a crash occurs when a pre-condition or post-condition is not met..
// If we would want to abort no matter what, then the section in #ifndef NDEBUG will suffice,
// but obvisouly we would need to comment out the the rest in such a case...
#define REQUIRE(assertion, what)                                                                                       \
        if (!(assertion))                                                                                              \
        ((void)printf("%s:%u: failed pre-condition `%s'\n", __FILE__, __LINE__, what))

#define ENSURE(assertion, what)                                                                                        \
        if (!(assertion))                                                                                              \
        ((void)printf("%s:%u: failed post-condition `%s'\n", __FILE__, __LINE__, what))

#endif // NDEBUG
*/
