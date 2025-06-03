// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB). This work is licensed under the terms of the MIT license. For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Math/Vector2D.h"
#include "EditorUtilityActor.h"
#include "EditorUtilityWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EditorLevelLibrary.h"
#include "Carla/MapGen/LargeMapManager.h"

#include <compiler/disable-ue4-macros.h>
#include <boost/optional.hpp>
#include <carla/road/Map.h>
#include <compiler/enable-ue4-macros.h>

#include <string>
#include "DEM/DEM.hpp"
#include "json/json.hpp"
#include "GenerateBlankTile.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCustomMapGenerator, Log, All);

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class CUSTOMMAPGENERATOR_API UGenerateBlankTile : public UEditorUtilityWidget
{
GENERATED_BODY()
#if WITH_EDITOR
public:
  UGenerateBlankTile();
  ~UGenerateBlankTile();

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateMap();

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void LoadJSONMetadata();

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateTile(const FString TileIndex, const FVector Offset);

  void CreateTerrainMeshForTile(const FString TileIndex, DEM<double> dem, USceneComponent* root_scene, const nlohmann::json & metadata);

  void CreateTerrainMesh(DEM<double>& dem, USceneComponent* root_terrain, const FString TileIndex, const int MeshIndex, const FVector2D tile_position_cm, const FVector2D tile_start, const FVector2D tile_end);
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File")
  FString JsonPath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File")
  FString MapName;

  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="TileGeneration" )
  FString BaseLevelName;

  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Defaults" )
  FString OriginTile;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defaults")
  UMaterialInstance* DefaultLandscapeMaterial;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defaults")
  float TileHeight;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defaults")
  float TileWidth;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defaults")
  int SubTilesInHeight;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defaults")
  int SubTilesInWidth;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defaults")
  float GridSectionSize;

  UPROPERTY()
  float OriginX;

  UPROPERTY()
  float OriginY;

  UPROPERTY()
  float OriginZ;

  UPROPERTY()
  float DEMCellSize;

  UPROPERTY()
  float DEMConversionFactorToUnreal;

  UPROPERTY()
  TArray<USceneComponent*> Tiles;

  nlohmann::json json_data;

#endif
};
