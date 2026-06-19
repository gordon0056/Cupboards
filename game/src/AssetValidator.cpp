#include "AssetValidator.hpp"

namespace game
{

AssetValidator& AssetValidator::instance()
{
    static AssetValidator inst;
    return inst;
}

void AssetValidator::addAsset(const std::string& path,
                               const std::string& description,
                               bool critical)
{
    assets_.push_back({path, description, critical, false});
}

bool AssetValidator::validate()
{
    bool allCriticalFound = true;

    for (auto& asset : assets_)
    {
        asset.found = std::filesystem::exists(asset.path);
        if (!asset.found)
        {
            if (asset.critical)
            {
                allCriticalFound = false;
                std::cerr << "[AssetValidator] CRITICAL: Missing "
                          << asset.description << " (" << asset.path << ")" << std::endl;
            }
            else
            {
                std::cout << "[AssetValidator] WARNING: Missing optional "
                          << asset.description << " (" << asset.path << ")" << std::endl;
            }
        }
    }

    std::cout << "[AssetValidator] Validation complete. "
              << getFoundCount() << "/" << assets_.size() << " found." << std::endl;
    return allCriticalFound;
}

size_t AssetValidator::getFoundCount() const
{
    size_t count = 0;
    for (const auto& asset : assets_)
        if (asset.found) count++;
    return count;
}

size_t AssetValidator::getMissingCriticalCount() const
{
    size_t count = 0;
    for (const auto& asset : assets_)
        if (!asset.found && asset.critical) count++;
    return count;
}

std::vector<std::string> AssetValidator::scanDirectory(
    const std::string& dirPath, const std::string& extension)
{
    std::vector<std::string> files;
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath))
        {
            if (entry.is_regular_file() && entry.path().extension() == extension)
                files.push_back(entry.path().string());
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "[AssetValidator] Failed to scan " << dirPath << ": " << e.what() << std::endl;
    }
    return files;
}

}
