#include <gloperate/plugin/plugin_api.h>

#include "screendoor/ScreenDoor.h"
#include "stochastic/Stochastic.h"

#include <glexamples-version.h>

GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PLUGIN(ScreenDoor
    , "ScreenDoor"
    , "Screen-Door Transparency"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

    GLOPERATE_PLUGIN(Stochastic
    , "Stochastic"
    , "Stochastic Transparency"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
