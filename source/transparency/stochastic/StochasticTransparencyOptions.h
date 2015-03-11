#pragma once

#include <reflectionzeug/PropertyGroup.h>


enum class StochasticTransparencyOptimization { AlphaCorrection, AlphaCorrectionAndDepthBased };

class StochasticTransparencyOptions : public reflectionzeug::PropertyGroup
{
public:
    StochasticTransparencyOptions();
    virtual ~StochasticTransparencyOptions() override;

    unsigned char transparency() const;
    void setTransparency(unsigned char transparency);
    
    StochasticTransparencyOptimization optimization() const;
    void setOptimization(StochasticTransparencyOptimization optimization);
    
    bool backFaceCulling() const;
    void setBackFaceCulling(bool b);

private:
    unsigned char m_transparency;
    StochasticTransparencyOptimization m_optimization;
    bool m_backFaceCulling;
};
