#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "UnrealString.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MapMetadata.generated.h"

USTRUCT(BlueprintType)
struct FCustomMapTileData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float min_y;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float min_x;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float min_z;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float max_y;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float max_x;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float max_z;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    float tile_point_interval;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString obj_path;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString fbx_path;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString material;
};

UCLASS(Blueprintable)
class CUSTOMMAPGENERATOR_API UMapMetadata : public UObject
{
    GENERATED_BODY()
public:
    UMapMetadata();
    ~UMapMetadata();

    UFUNCTION(BlueprintCallable)
    void loadMetadata(FString metadata_path_passed);

    UPROPERTY()
    TMap<FString, FCustomMapTileData> terrain_data;

    UPROPERTY()
    TArray<float> original_bounds;

    UPROPERTY()
    TArray<float> carla_bounds;

    UPROPERTY()
    FString metadata_path;

private:

};


