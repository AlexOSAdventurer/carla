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

    UMapMetadata* map_metadata;
    FString dataset_path;

private:

};


