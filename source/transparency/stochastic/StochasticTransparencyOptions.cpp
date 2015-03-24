#include "StochasticTransparencyOptions.h"

#include <glm/common.hpp>

#include <glbinding/gl/enum.h>

#include <globjects/globjects.h>


StochasticTransparencyOptions::StochasticTransparencyOptions()
:   m_transparency{160u}
,   m_optimization{StochasticTransparencyOptimization::AlphaCorrection}
,   m_backFaceCulling{false}
,   m_numSamples{8u}
,   m_numSamplesChanged{true}
{
}

StochasticTransparencyOptions::~StochasticTransparencyOptions() = default;

unsigned char StochasticTransparencyOptions::transparency() const
{
    return m_transparency;
}

void StochasticTransparencyOptions::setTransparency(unsigned char transparency)
{
    m_transparency = transparency;
}

StochasticTransparencyOptimization StochasticTransparencyOptions::optimization() const
{
    return m_optimization;
}

void StochasticTransparencyOptions::setOptimization(StochasticTransparencyOptimization optimization)
{
    m_optimization = optimization;
}

bool StochasticTransparencyOptions::backFaceCulling() const
{
    return m_backFaceCulling;
}

void StochasticTransparencyOptions::setBackFaceCulling(bool b)
{
    m_backFaceCulling = b;
}

uint16_t StochasticTransparencyOptions::numSamples() const
{
    return m_numSamples;
}

void StochasticTransparencyOptions::setNumSamples(uint16_t numSamples)
{
    m_numSamples = numSamples;
    m_numSamplesChanged = true;
}

bool StochasticTransparencyOptions::numSamplesChanged() const
{
    const auto changed = m_numSamplesChanged;
    m_numSamplesChanged = false;
    return changed;
}
