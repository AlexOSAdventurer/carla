#include "MapMetadata.h"
#include "CustomMapGenerator.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Paths.h"

UMapMetadata::UMapMetadata() {
    this->metadata_path = "";
}

UMapMetadata::~UMapMetadata() {

}

void UMapMetadata::loadMetadata(FString metadata_path_passed) {
    this->original_bounds.Empty();
    this->carla_bounds.Empty();
    this->asset_types.Empty();
    this->asset_data.Empty();

    this->metadata_path = metadata_path_passed;
    FString file_content;
    if (!FFileHelper::LoadFileToString(file_content, *(this->metadata_path))) {
        UE_LOG(LogCustomMapGenerator, Log, TEXT("Failed to load file: %s"), *(this->metadata_path));
        return;
    }
    TSharedPtr<FJsonObject> root_object;
    TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(file_content);

    if (!FJsonSerializer::Deserialize(reader, root_object) || !root_object.IsValid())
    {
        UE_LOG(LogCustomMapGenerator, Error, TEXT("Failed to parse JSON."));
        return;
    }


    TArray<TSharedPtr<FJsonValue>> original_bounds_json = root_object->GetArrayField("original_bounds");
    for (const TSharedPtr<FJsonValue>& value : original_bounds_json)
    {
        if (value->Type == EJson::Number)
        {
            this->original_bounds.Add(value->AsNumber() * 100.0);
        }
        else
        {
            UE_LOG(LogCustomMapGenerator, Warning, TEXT("Non-number value found in 'original_bounds' array"));
        }
    }

    TArray<TSharedPtr<FJsonValue>> carla_bounds_json = root_object->GetArrayField("carla_bounds");
    for (const TSharedPtr<FJsonValue>& value : carla_bounds_json)
    {
        if (value->Type == EJson::Number)
        {
            this->carla_bounds.Add(value->AsNumber() * 100.0);
        }
        else
        {
            UE_LOG(LogCustomMapGenerator, Warning, TEXT("Non-number value found in 'carla_bounds' array"));
        }
    }

    TArray<TSharedPtr<FJsonValue>> asset_types_json = root_object->GetArrayField("asset_types");
    for (const TSharedPtr<FJsonValue>& value : asset_types_json)
    {
        if (value->Type == EJson::String)
        {
            this->asset_types.Add(value->AsString());
        }
        else
        {
            UE_LOG(LogCustomMapGenerator, Warning, TEXT("Non-string value found in 'asset_types' array"));
        }
    }

    for (const FString& asset_type : this->asset_types) {
        TSharedPtr<FJsonObject> assets_json = root_object->GetObjectField(asset_type);
        FCustomMapAssetList asset_list;
        asset_list.assets.Empty();
        asset_list.asset_type = asset_type;
        for (const auto& asset_entry : assets_json->Values)
        {
            FString asset_name = asset_entry.Key;
            TSharedPtr<FJsonObject> asset_props = asset_entry.Value->AsObject();

            if (asset_props.IsValid())
            {
                FCustomMapAssetData asset;

                asset.min_y = asset_props->GetNumberField("min_y") * 100.0;
                asset.min_x = asset_props->GetNumberField("min_x") * 100.0;
                asset.max_y = asset_props->GetNumberField("max_y") * 100.0;
                asset.max_x = asset_props->GetNumberField("max_x") * 100.0;
                asset.min_z = asset_props->GetNumberField("min_z") * 100.0;
                asset.max_z = asset_props->GetNumberField("max_z") * 100.0;
                asset.obj_path = asset_props->GetStringField("obj_path");
                asset.fbx_path = asset_props->GetStringField("fbx_path");
                asset.material = asset_props->GetStringField("material");
                asset.name = FPaths::GetBaseFilename(FPaths::GetCleanFilename(asset.fbx_path));

                asset_list.assets.Add(asset_name, asset);
            }
        }
        this->asset_data.Add(asset_type, asset_list);
    }

    TSharedPtr<FJsonObject> terrain = root_object->GetObjectField("terrain");




}