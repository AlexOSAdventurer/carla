#pragma once

#include <string>
#include <fstream>
#include "CoreMinimal.h"
#include "json/json.hpp"

namespace UCustomMapGeneratorHelpers {

  nlohmann::json LoadJSONMetadata(FString json_path) {
    std::string json_path_str = std::string(TCHAR_TO_UTF8(*json_path));
    std::ifstream json_file(json_path_str);
    return nlohmann::json::parse(json_file);
  }

}