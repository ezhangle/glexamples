#include "StochasticTransparencyOptions.h"


StochasticTransparencyOptions::StochasticTransparencyOptions()
:   m_transparency{160u}
,   m_optimization{StochasticTransparencyOptimization::AlphaCorrection}
,   m_backFaceCulling{false}
{   
    addProperty<unsigned char>("transparency", this,
        &StochasticTransparencyOptions::transparency, 
        &StochasticTransparencyOptions::setTransparency)->setOptions({
        { "minimum", 0 },
        { "maximum", 255 },
        { "step", 1 }});
    
    addProperty<StochasticTransparencyOptimization>("optimization", this,
        &StochasticTransparencyOptions::optimization,
        &StochasticTransparencyOptions::setOptimization)->setStrings({
        { StochasticTransparencyOptimization::AlphaCorrection, "AlphaCorrection" },
        { StochasticTransparencyOptimization::AlphaCorrectionAndDepthBased, "AlphaCorrectionAndDepthBased" }});
    
    addProperty<bool>("back_face_culling", this,
        &StochasticTransparencyOptions::backFaceCulling, 
        &StochasticTransparencyOptions::setBackFaceCulling);
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
