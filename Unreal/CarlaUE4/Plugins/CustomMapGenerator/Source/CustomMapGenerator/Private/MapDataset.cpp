#include "MapDataset.h"
#include "CustomMapGenerator.h"

UMapDataset::UMapDataset() {
    this->dataset_path = "/home/richarwa/CarlaIngestion/CarlaCooked/";
}

UMapDataset::~UMapDataset() {

}

void UMapDataset::load() {
    this->map_metadata = NewObject<UMapMetadata>(this, UMapMetadata::StaticClass());
    this->map_metadata->loadMetadata(dataset_path + "metadata.json");
}

TArray<FString> UMapDataset::getTerrainTileKeys() {
    TArray<FString> keys;
    this->map_metadata->terrain_data.GetKeys(keys);
    return keys;
}

FCustomMapTileData UMapDataset::getTerrainData(FString tile_key) {
    return this->map_metadata->terrain_data[tile_key];
}