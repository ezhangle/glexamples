#pragma once

#include <assimp/scene.h>

#include <gloperate/resources/Loader.h>


class AssimpLoader : public gloperate::Loader<aiScene>
{
public:
    bool canLoad(const std::string & ext) const override;

    std::vector<std::string> loadingTypes() const override;

    std::string allLoadingTypes() const override;

    /**
     *  @remarks
     *    Scene must be deleted with `aiReleaseImport(scene)`
     */
    aiScene * load(const std::string & filename, std::function<void(int, int)> progress) const override;
};
