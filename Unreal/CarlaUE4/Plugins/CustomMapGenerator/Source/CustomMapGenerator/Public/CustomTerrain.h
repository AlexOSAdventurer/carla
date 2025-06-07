// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "UnrealString.h"
#include "Materials/MaterialInstanceConstant.h"
#include "DEM/DEM.hpp"
#include "json/json.hpp"
#include "CustomTerrain.generated.h"

UCLASS(Blueprintable)
class CUSTOMMAPGENERATOR_API UCustomTerrain : public UObject
{
  GENERATED_BODY()
public:
  UCustomTerrain();
  ~UCustomTerrain();

  void Init(FString MapNamePassed, UMaterialInstance* DefaultLandscapeMaterial, FVector const& origin, FString json_path);

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateTile(const FString TileIndex, const FVector Offset);

  void CreateTerrainMeshForTile(const FString TileIndex, DEM<double> dem, const nlohmann::json & metadata);

  void CreateTerrainMesh(DEM<double>& dem, const FString TileIndex, const int MeshIndex, const FVector2D tile_position_cm, const FVector2D tile_start, const FVector2D tile_end);
  
private:
  UPROPERTY()
  float TileHeight;

  UPROPERTY()
  float TileWidth;

  UPROPERTY()
  int SubTilesInHeight;

  UPROPERTY()
  int SubTilesInWidth;

  UPROPERTY()
  float GridSectionSize;

  UPROPERTY()
  FVector Origin;

  UPROPERTY()
  float DEMCellSize;

  UPROPERTY()
  float DEMConversionFactorToUnreal;

  UPROPERTY()
  UMaterialInstance* DefaultMaterial;

  UPROPERTY()
  FString MapName;

  nlohmann::json TerrainMetadata;
};


