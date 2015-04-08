#include "StochasticTransparencyOptions.h"

#include <glm/common.hpp>

#include <glbinding/gl/enum.h>

#include <globjects/globjects.h>

#include "StochasticTransparency.h"


StochasticTransparencyOptions::StochasticTransparencyOptions(StochasticTransparency & painter)
:   m_painter(painter)
,   m_transparency(160u)
,   m_optimization(StochasticTransparencyOptimization::AlphaCorrection)
,   m_backFaceCulling(false)
,   m_numSamples(8u)
,   m_numSamplesChanged(true)
{   
    painter.addProperty<unsigned char>("transparency", this,
        &StochasticTransparencyOptions::transparency, 
        &StochasticTransparencyOptions::setTransparency)->setOptions({
        { "minimum", 0 },
        { "maximum", 255 },
        { "step", 1 }});
    
    painter.addProperty<StochasticTransparencyOptimization>("optimization", this,
        &StochasticTransparencyOptions::optimization,
        &StochasticTransparencyOptions::setOptimization)->setStrings({
        { StochasticTransparencyOptimization::NoOptimization, "NoOptimization" },
        { StochasticTransparencyOptimization::AlphaCorrection, "AlphaCorrection" },
        { StochasticTransparencyOptimization::AlphaCorrectionAndDepthBased, "AlphaCorrectionAndDepthBased" }});
    
    painter.addProperty<bool>("back_face_culling", this,
        &StochasticTransparencyOptions::backFaceCulling, 
        &StochasticTransparencyOptions::setBackFaceCulling);
    
    painter.addProperty<uint16_t>("num_samples", this,
        &StochasticTransparencyOptions::numSamples,
        &StochasticTransparencyOptions::setNumSamples)->setOptions({
        { "minimum", 1u }});
}

StochasticTransparencyOptions::~StochasticTransparencyOptions() = default;

void StochasticTransparencyOptions::initGL()
{
    const auto maxNumSamples = globjects::getInteger(gl::GL_MAX_COLOR_TEXTURE_SAMPLES);
    
    m_painter.property("num_samples")->setOption("maximum", static_cast<uint16_t>(glm::min(8, maxNumSamples)));
}

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
