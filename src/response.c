#include "response.h"

#define RESPONSE_IMPLEMENT(LOWER, UPPER, _) \
	SERIALIZE__IMPLEMENT(response_##LOWER, RESPONSE_##UPPER);

RESPONSE__TYPES(RESPONSE_IMPLEMENT, );
