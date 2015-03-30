#pragma once

#include <cstdint>

#include <reflectionzeug/PropertyGroup.h>


class StochasticTransparency;

enum class StochasticTransparencyOptimization { NoOptimization, AlphaCorrection, AlphaCorrectionAndDepthBased };

class StochasticTransparencyOptions
{
public:
    StochasticTransparencyOptions(StochasticTransparency & painter);
    ~StochasticTransparencyOptions();
    
    void initGL();

    unsigned char transparency() const;
    void setTransparency(unsigned char transparency);
    
    StochasticTransparencyOptimization optimization() const;
    void setOptimization(StochasticTransparencyOptimization optimization);
    
    bool backFaceCulling() const;
    void setBackFaceCulling(bool b);
    
    uint16_t numSamples() const;
    void setNumSamples(uint16_t numSamples);
    
    bool numSamplesChanged() const;

private:
    StochasticTransparency & m_painter;

    unsigned char m_transparency;
    StochasticTransparencyOptimization m_optimization;
    bool m_backFaceCulling;
    uint16_t m_numSamples;
    mutable bool m_numSamplesChanged;
};
