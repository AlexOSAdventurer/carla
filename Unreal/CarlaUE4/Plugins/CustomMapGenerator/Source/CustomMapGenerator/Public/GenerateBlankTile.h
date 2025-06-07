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
#include "GenerateBlankTile.generated.h"

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

  UPROPERTY()
  FVector OriginSurveyFeet;

  UPROPERTY()
  UCustomTerrain* TerrainFactory;

  nlohmann::json JsonData;

#endif
};
