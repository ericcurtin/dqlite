#include "request.h"

#define REQUEST_IMPLEMENT(LOWER, UPPER, _) \
	SERIALIZE_IMPLEMENT(request##LOWER, REQUEST_##UPPER);

REQUEST_TYPES(REQUEST_IMPLEMENT, );

SERIALIZE_IMPLEMENT(requestConnect, REQUEST_CONNECT);
