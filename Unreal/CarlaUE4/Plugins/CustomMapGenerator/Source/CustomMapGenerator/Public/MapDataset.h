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
    FString getStaticAssetPath(const FString& map_name, const FString& asset_type);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    FString getUnrealMapFolderLocal(const FString& map_name);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    FString getUnrealMapFolderGlobal(const FString& map_name);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    FString getXODRFolderPath(const FString& map_name);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    FString getXODRMapPath(const FString& map_name);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    void copyXODR(const FString& map_name);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    void load();

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    TArray<FString> getAssetKeys(const FString& asset_type);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    FCustomMapAssetData getAssetData(const FString& tile_key, const FString& asset_type);

    UFUNCTION( BlueprintCallable, Category="CustomMapGenerator" )
    TArray<float> getOrigin();

    UPROPERTY()
    UMapMetadata* map_metadata;

    UPROPERTY()
    FString dataset_path;

private:

};


