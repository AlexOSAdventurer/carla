// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "UnrealString.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MapDataset.h"
#include "CustomTerrain.generated.h"

UCLASS(Blueprintable)
class CUSTOMMAPGENERATOR_API UCustomTerrain : public UObject
{
  GENERATED_BODY()
public:
  UCustomTerrain();
  ~UCustomTerrain();

  void Init(FString map_name_passed, UMapDataset* map_dataset_passed);

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateTiles();

  UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
  void CreateTile(const FCustomMapAssetData tile_data);

  static const FString asset_type;
  
private:
  UPROPERTY()
  FString map_name;

  UMapDataset* map_dataset;

};


