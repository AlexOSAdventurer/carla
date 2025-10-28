#pragma once

#include "CoreMinimal.h"
#include "Math/Vector.h"
#include "UnrealString.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MapMetadata.generated.h"

USTRUCT(BlueprintType)
struct FCustomMapAssetData
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
    FString obj_path;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString fbx_path;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString unreal_path;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString material;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString name;
};

USTRUCT(BlueprintType)
struct FCustomMapAssetList
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    TMap<FString, FCustomMapAssetData> assets;

    UPROPERTY(BlueprintReadOnly, EditAnywhere)
    FString asset_type;
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
    TArray<FString> asset_types;

    UPROPERTY()
    TMap<FString, FCustomMapAssetList> asset_data;

    UPROPERTY()
    TArray<float> original_bounds;

    UPROPERTY()
    TArray<float> carla_bounds;

    UPROPERTY()
    FString metadata_path;

private:

};


