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
#include "CustomTerrain.h"
#include "MapDataset.h"
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

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File")
  FString dataset_path;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="File")
  FString map_name;

  UPROPERTY()
  UCustomTerrain* terrain_factory;

  UPROPERTY()
  UMapDataset* map_dataset;

#endif
};
