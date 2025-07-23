#pragma once
#ifdef WITH_EDITOR

#include <string>
#include <fstream>
#include "CoreMinimal.h"
#include "json/json.hpp"
#include "DEM/DEM.hpp"


namespace UCustomMapGeneratorHelpers {

  static nlohmann::json LoadJSONMetadata(FString json_path) {
    std::string json_path_str = std::string(TCHAR_TO_UTF8(*json_path));
    std::ifstream json_file(json_path_str);
    return nlohmann::json::parse(json_file);
  }

  static const nlohmann::json& GetDEMMeta(const nlohmann::json& json_data, const FString TileIndex) {
    std::string TileIndexStr = TCHAR_TO_UTF8(*TileIndex);
    const nlohmann::json& json_value = json_data[TileIndexStr]["DEM"];
    return json_value;
  }

  static DEM<double> ReadDEMFromTile(const nlohmann::json& json_data, const FString TileIndex) {
    const nlohmann::json& dem_meta = GetDEMMeta(json_data, TileIndex);
    const std::string bin_path = dem_meta["bin_path"];
    double dem_x_min = dem_meta["x"]["min"];
    double dem_x_max = dem_meta["x"]["max"];
    double dem_y_min = dem_meta["y"]["min"];
    double dem_y_max = dem_meta["y"]["max"];

    std::ifstream bin_file(bin_path, std::ios::binary);
    DEM<double>::Type type = DEM<double>::Type(dem_y_max - dem_y_min, dem_x_max - dem_x_min, dem_y_min, dem_x_min, 2.0, -999999);
    DEM<double> dem = DEM<double>(type, bin_file);
    return dem;
  }

}

#endif