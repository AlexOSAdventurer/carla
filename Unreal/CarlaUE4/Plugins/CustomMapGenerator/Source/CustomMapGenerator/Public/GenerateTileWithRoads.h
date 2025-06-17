// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Math/Vector2D.h"
#include "EditorUtilityActor.h"
#include "EditorUtilityWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EditorLevelLibrary.h"
#include "Engine/Classes/Materials/MaterialInterface.h"

#include <compiler/disable-ue4-macros.h>
#include <boost/optional.hpp>
#include <carla/road/Map.h>
#include <compiler/enable-ue4-macros.h>

#include <string>
#include "DEM/DEM.hpp"
#include "json/json.hpp"
#include "CustomTerrain.h"
#include "CustomRoads.h"
#include "GenerateTileWithRoads.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class CUSTOMMAPGENERATOR_API UGenerateTileWithRoads : public UEditorUtilityWidget
{
GENERATED_BODY()
#if WITH_EDITOR
public:
  UGenerateTileWithRoads();
  ~UGenerateTileWithRoads();

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateMap();

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void LoadJSONMetadata();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File")
  FString JsonPath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File")
  FString OpenDrivePath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File")
  FString MapName;

  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="TileGeneration" )
  FString BaseLevelName;

  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Defaults" )
  FString OriginTile;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defaults")
  UMaterialInstance* DefaultLandscapeMaterial;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
  UMaterialInstance* DefaultRoadMaterial;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
  UMaterialInstance* DefaultLaneMarksWhiteMaterial;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
  UMaterialInstance* DefaultLaneMarksYellowMaterial;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults")
  UMaterialInstance* DefaultSidewalksMaterial;

  UPROPERTY()
  FVector OriginSurveyFeet;

  UPROPERTY()
  FVector2D OriginLatLong;

  UPROPERTY()
  UCustomTerrain* TerrainFactory;

  UPROPERTY()
  UCustomRoads* RoadFactory;

  nlohmann::json JsonData;

#endif
};
