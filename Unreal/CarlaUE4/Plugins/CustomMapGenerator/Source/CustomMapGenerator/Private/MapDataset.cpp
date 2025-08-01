#include "MapDataset.h"
#include "AssetToolsModule.h"
#include "Factories/FbxImportUI.h"
#include "Factories/FbxFactory.h"
#include "EditorAssetLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetImportTask.h"
#include "Misc/Paths.h"
#include "CustomMapGenerator.h"

UAssetImportTask* _generateFBXImportTask(const FString& dataset_path, const FString& fbx_path, const FString& destination_path, const FString& asset_name, UFbxFactory* factory) {
    FString final_fbx_path = dataset_path + fbx_path;
    UAssetImportTask* import_task = NewObject<UAssetImportTask>();
    import_task->Filename = final_fbx_path;
    import_task->DestinationPath = destination_path;       // E.g. "/Game/Carla/Maps/MyTown/Meshes"
    import_task->DestinationName = asset_name;             // E.g. "MyTerrainTile_01"
    import_task->bAutomated = true;
    import_task->bSave = true;
    import_task->bReplaceExisting = true;
    import_task->Factory = factory;
    return import_task;
}

UMapDataset::UMapDataset() {
    this->dataset_path = "/home/richarwa/CarlaIngestion/CarlaCooked/";
}

UMapDataset::~UMapDataset() {

}

FString UMapDataset::getStaticAssetPath(const FString& map_name, const FString& asset_type) {
    return "/Game" + this->getUnrealMapFolderLocal(map_name) + "Static/" + asset_type + "/";
}


TMap<FString, FString> UMapDataset::importAssetMeshes(const FString& map_name, const FString& asset_type) {
    TMap<FString, FString> basename_to_unreal_path;
    basename_to_unreal_path.Empty();

    FString mesh_path = this->getStaticAssetPath(map_name, asset_type);

    UFbxFactory* factory = NewObject<UFbxFactory>();
    factory->ImportUI->MeshTypeToImport = FBXIT_StaticMesh;
    factory->ImportUI->bImportAsSkeletal = false;
    factory->ImportUI->bImportMaterials = false;
    factory->ImportUI->bImportTextures = false;
    factory->ImportUI->bImportAnimations = false;
    factory->AddToRoot();  // Prevent GC

    TArray<UAssetImportTask*> tasks;

    for (FString& key : this->getAssetKeys(asset_type)) {
        FCustomMapAssetData asset_data = this->getAssetData(key, asset_type);
        UAssetImportTask* import_task = _generateFBXImportTask(this->dataset_path, asset_data.fbx_path, mesh_path, asset_data.name, factory);
        basename_to_unreal_path.Add(asset_data.name, FString::Printf(TEXT("%s%s_%s.%s_%s"), *mesh_path, *(asset_data.name), *(asset_data.name), *(asset_data.name), *(asset_data.name)));
        tasks.Add(import_task);
    }

    FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
    AssetToolsModule.Get().ImportAssetTasks(tasks);

    // Add materials
    for (FString& key : this->getAssetKeys(asset_type)) {
        FCustomMapAssetData asset_data = this->getAssetData(key, asset_type);

        UMaterialInterface* material = Cast<UMaterialInterface>(
            StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *(asset_data.material))
        );
        if (!material) {
            UE_LOG(LogCustomMapGenerator, Display, TEXT("Material lookup failed %s"), *(asset_data.material));
            continue;
        }

        FAssetRegistryModule& unreal_asset_registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        FAssetData unreal_asset_data = unreal_asset_registry.Get().GetAssetByObjectPath(*(basename_to_unreal_path[asset_data.name]));
        UStaticMesh* imported_mesh = Cast<UStaticMesh>(unreal_asset_data.GetAsset());
        if (!imported_mesh) {
            UE_LOG(LogCustomMapGenerator, Display, TEXT("Mesh lookup failed %s %s"), *(basename_to_unreal_path[asset_data.name]), *(asset_data.name));
            continue;
        }
        imported_mesh->SetMaterial(0, material);
        imported_mesh->MarkPackageDirty();
        imported_mesh->PostEditChange();
    }

    return basename_to_unreal_path;
}

FString UMapDataset::getUnrealMapFolderLocal(const FString& map_name) {
    return "/CustomMaps/" + map_name + "/";
}

FString UMapDataset::getUnrealMapFolderGlobal(const FString& map_name) {
    return FPaths::ProjectDir() + "/Content" + this->getUnrealMapFolderLocal(map_name);
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