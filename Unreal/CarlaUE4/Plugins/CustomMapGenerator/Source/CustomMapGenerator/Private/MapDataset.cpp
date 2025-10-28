#include "MapDataset.h"
#include "AssetToolsModule.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxFactory.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetImportTask.h"
#include "Misc/Paths.h"
#include "CustomMapGenerator.h"

UMapDataset::UMapDataset() {
    this->dataset_path = "/workspaces/OpenTwinMap/CarlaCooked/";
}

UMapDataset::~UMapDataset() {

}

FString UMapDataset::getStaticAssetPath(const FString& map_name, const FString& asset_type) {
    return "/Game" + this->getUnrealMapFolderLocal(map_name) + "Static/" + asset_type + "/";
}

FString UMapDataset::getUnrealMapFolderLocal(const FString& map_name) {
    return "/Content/Carla/Maps/" + map_name + "/";
}

FString UMapDataset::getUnrealMapFolderGlobal(const FString& map_name) {
    return FPaths::ProjectDir() + this->getUnrealMapFolderLocal(map_name);
}

FString UMapDataset::getXODRFolderPath(const FString& map_name) {
    return this->getUnrealMapFolderGlobal(map_name) + "OpenDrive/";
}

FString UMapDataset::getXODRMapPath(const FString& map_name) {
  return  this->getXODRFolderPath(map_name) + map_name + ".xodr";
}

void UMapDataset::copyXODR(const FString& map_name) {
    FString original_xodr_path = this->dataset_path + "map.xodr";
    FString destination_xodr_path = this->getXODRMapPath(map_name);
    IPlatformFile& file_manager = FPlatformFileManager::Get().GetPlatformFile();

    if(!file_manager.CopyFile(  *destination_xodr_path, *original_xodr_path,
                              EPlatformFileRead::None,
                              EPlatformFileWrite::None)) {
        UE_LOG(LogCustomMapGenerator, Error, TEXT("Copying XODR from %s to %s failed!"), *(original_xodr_path), *(destination_xodr_path));
    }
    else {
        UE_LOG(LogCustomMapGenerator, Display, TEXT("Copying XODR from %s to %s succeeded!"), *(original_xodr_path), *(destination_xodr_path));
    }
}

void UMapDataset::load() {
    this->map_metadata = NewObject<UMapMetadata>(this, UMapMetadata::StaticClass());
    this->map_metadata->loadMetadata(dataset_path + "metadata.json");
}

TArray<FString> UMapDataset::getAssetKeys(const FString& asset_type) {
    TArray<FString> keys;
    this->map_metadata->asset_data[asset_type].assets.GetKeys(keys);
    return keys;
}

FCustomMapAssetData UMapDataset::getAssetData(const FString& tile_key, const FString& asset_type) {
    return this->map_metadata->asset_data[asset_type].assets[tile_key];
}

TArray<float> UMapDataset::getOrigin() {
    TArray<float> origin;
    origin.Add(this->map_metadata->carla_bounds[0]);
    origin.Add(this->map_metadata->carla_bounds[1]);
    origin.Add(this->map_metadata->carla_bounds[2]);
    return origin;
}