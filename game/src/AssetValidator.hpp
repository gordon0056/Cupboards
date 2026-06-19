#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace game
{

struct AssetCheck
{
    std::string path;
    std::string description;
    bool critical;
    bool found;
};

class AssetValidator
{
public:
    static AssetValidator& instance();

    void addAsset(const std::string& path, const std::string& description,
                  bool critical = false);
    bool validate();

    size_t getFoundCount() const;
    size_t getMissingCriticalCount() const;

    static std::vector<std::string> scanDirectory(
        const std::string& dirPath, const std::string& extension);

private:
    AssetValidator() = default;
    ~AssetValidator() = default;
    AssetValidator(const AssetValidator&) = delete;
    AssetValidator& operator=(const AssetValidator&) = delete;

    std::vector<AssetCheck> assets_;
};

}
