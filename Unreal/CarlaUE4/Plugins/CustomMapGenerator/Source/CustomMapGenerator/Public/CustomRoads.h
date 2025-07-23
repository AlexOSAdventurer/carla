// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "UnrealString.h"
#include "Carla/Vehicle/VehicleSpawnPoint.h"
#include "Materials/MaterialInstanceConstant.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/rpc/String.h>
#include <carla/opendrive/OpenDriveParser.h>
#include <carla/geom/Simplification.h>
#include <carla/road/Deformation.h>
#include <carla/rpc/String.h>
#include <boost/optional.hpp>
#include <carla/road/Map.h>
#include <compiler/enable-ue4-macros.h>
#include "Helpers.hpp"
#include "CustomRoads.generated.h"

UCLASS(Blueprintable)
class CUSTOMMAPGENERATOR_API UCustomRoads : public UObject
{
  GENERATED_BODY()
public:
  UCustomRoads();
  ~UCustomRoads();

  void Init(FString MapNamePassed, UMaterialInstance* LandscapePassed, UMaterialInstance* RoadPassed, UMaterialInstance* LaneMarksWhitePassed, UMaterialInstance* LaneMarksYellowPassed, UMaterialInstance* SidewalksPassed, FVector const& origin, FString OpenDrivePath, FString json_path);

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateTile(const FString TileIndex, const FVector Offset, float xDeltaMap, float yDeltaMap);

  UPROPERTY()
  UMaterialInstance* DefaultLandscapeMaterial;

  UPROPERTY()
  UMaterialInstance* DefaultRoadMaterial;
  
  UPROPERTY()
  UMaterialInstance* DefaultLaneMarksWhiteMaterial;

  UPROPERTY()
  UMaterialInstance* DefaultLaneMarksYellowMaterial;

  UPROPERTY()
  UMaterialInstance* DefaultSidewalksMaterial;

private:
  void GenerateRoadMesh(const FString TileIndex, FVector MinLocation, FVector MaxLocation, float xDeltaMap, float yDeltaMap);

  void GenerateLaneMarks(const FString TileIndex, FVector MinLocation, FVector MaxLocation, float xDeltaMap, float yDeltaMap);

  void GenerateSpawnPoints(const FString TileIndex, FVector MinLocation, FVector MaxLocation, float xDeltaMap, float yDeltaMap);

  float DistanceToLaneBorder(FVector &location, int32_t lane_type = 2) const;

  float GetHeight(DEM<double>& dem, FString TileIndex, float X, float Y);

  UPROPERTY()
  float TileHeight;

  UPROPERTY()
  float TileWidth;

  UPROPERTY()
  float DEMCellSize;

  UPROPERTY()
  float DEMConversionFactorToMeters;

  UPROPERTY()
  FVector Origin;

  UPROPERTY()
  FString MapName;

  carla::rpc::OpendriveGenerationParameters opg_parameters;
  boost::optional<carla::road::Map> OpenDriveMap;
  nlohmann::json TerrainMetadata;

};


