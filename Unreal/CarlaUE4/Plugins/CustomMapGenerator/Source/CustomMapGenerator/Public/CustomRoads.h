// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "UnrealString.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MapDataset.h"
#include "CustomRoads.generated.h"

UCLASS(Blueprintable)
class CUSTOMMAPGENERATOR_API UCustomRoads : public UObject
{
  GENERATED_BODY()
public:
  UCustomRoads();
  ~UCustomRoads();

  void Init(FString map_name_passed, UMapDataset* map_dataset_passed);

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateRoads();

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateRoad(const FCustomMapAssetData road_data, const FString road_path);

  static const FString asset_type;

private:
  UPROPERTY()
  FString map_name;

  UMapDataset* map_dataset;

};


