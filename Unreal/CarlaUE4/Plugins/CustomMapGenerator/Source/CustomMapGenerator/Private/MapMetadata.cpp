#include "MapMetadata.h"
#include "CustomMapGenerator.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UMapMetadata::UMapMetadata() {
    this->metadata_path = "";
}

UMapMetadata::~UMapMetadata() {

}

void UMapMetadata::loadMetadata(FString metadata_path) {
    this->original_bounds.Empty();
    this->carla_bounds.Empty();
    this->terrain_data.Empty();

    this->metadata_path = metadata_path;
    FString file_content;
    if (!FFileHelper::LoadFileToString(file_content, *(this->metadata_path))) {
        UE_LOG(LogCustomMapGenerator, Log, TEXT("Failed to load file: %s"), *(this->metadata_path));
        return;
    }
    TSharedPtr<FJsonObject> root_object;
    TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(json_raw);

    if (!FJsonSerializer::Deserialize(reader, root_object) || !root_object.IsValid())
    {
        UE_LOG(LogCustomMapGenerator, Error, TEXT("Failed to parse JSON."));
        return;
    }


    TArray<TSharedPtr<FJsonValue>> original_bounds_json = JsonObject->GetArrayField("original_bounds");
    for (const TSharedPtr<FJsonValue>& value : original_bounds_json)
    {
        if (value->Type == EJson::Number)
        {
            this->original_bounds.Add(value->AsNumber());
        }
        else
        {
            UE_LOG(LogCustomMapGenerator, Warning, TEXT("Non-number value found in 'original_bounds' array"));
        }
    }

    TArray<TSharedPtr<FJsonValue>> carla_bounds_json = JsonObject->GetArrayField("carla_bounds");
    for (const TSharedPtr<FJsonValue>& value : carla_bounds_json)
    {
        if (value->Type == EJson::Number)
        {
            this->carla_bounds.Add(value->AsNumber());
        }
        else
        {
            UE_LOG(LogCustomMapGenerator, Warning, TEXT("Non-number value found in 'carla_bounds' array"));
        }
    }

    TSharedPtr<FJsonObject> terrain = root_object->GetObjectField("terrain");


    for (const auto& tile_entry : terrain->Values)
    {
        FString tile_name = tile_entry.Key;
        TSharedPtr<FJsonObject> tile_props = tile_entry.Value->AsObject();

        if (tile_props.IsValid())
        {
            FTileData tile;

            tile.min_y = tile_props->GetNumberField("min_y");
            tile.min_x = tile_props->GetNumberField("min_y");
            tile.max_y = tile_props->GetNumberField("max_y");
            tile.max_x = tile_props->GetNumberField("max_y");
            tile.min_z = tile_props->GetNumberField("min_z");
            tile.max_z = tile_props->GetNumberField("max_z");
            tile.tile_point_interval = tile_props->GetNumberField("tile_point_interval");
            tile.obj_path = tile_props->GetStringField("obj_path");
            tile.fbx_path = tile_props->GetStringField("fbx_path");
            tile.material = tile_props->GetStringField("material");

            this->terrain_data.Add(TileName, Tile);
        }
    }
}