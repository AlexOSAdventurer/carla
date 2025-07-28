#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "UnrealString.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MapMetadata.h"
#include "MapDataset.generated.h"

UCLASS(Blueprintable)
class CUSTOMMAPGENERATOR_API UMapDataset : public UObject
{
    GENERATED_BODY()
public:
    UMapDataset();
    ~UMapDataset();

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    void load();

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    TArray<FString> getTerrainTileKeys();

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    FCustomMapTileData getTerrainData(FString tile_key);

    UPROPERTY()
    UMapMetadata* map_metadata;

    UPROPERTY()
    FString dataset_path;

private:

};


