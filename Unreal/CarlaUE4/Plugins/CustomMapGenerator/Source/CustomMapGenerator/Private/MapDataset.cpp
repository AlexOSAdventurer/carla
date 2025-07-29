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

FString UMapDataset::getStaticTerrainPath(const FString& map_name) {
    return "/Game/CustomMaps/" + map_name + "/Static/Terrain/";
}


TMap<FString, FString> UMapDataset::importTerrainMeshes(const FString& map_name) {
    TMap<FString, FString> basename_to_unreal_path;
    basename_to_unreal_path.Empty();

    FString mesh_path = this->getStaticTerrainPath(map_name);

    UFbxFactory* factory = NewObject<UFbxFactory>();
    factory->ImportUI->MeshTypeToImport = FBXIT_StaticMesh;
    factory->ImportUI->bImportAsSkeletal = false;
    factory->ImportUI->bImportMaterials = false;
    factory->ImportUI->bImportTextures = false;
    factory->ImportUI->bImportAnimations = false;
    factory->AddToRoot();  // Prevent GC

    TArray<UAssetImportTask*> tasks;

    for (FString& key : this->getTerrainTileKeys()) {
        FCustomMapTileData tile_data = this->getTerrainData(key);
        UAssetImportTask* import_task = _generateFBXImportTask(this->dataset_path, tile_data.fbx_path, mesh_path, tile_data.name, factory);
        basename_to_unreal_path.Add(tile_data.name, FString::Printf(TEXT("%s%s_%s.%s_%s"), *mesh_path, *(tile_data.name), *(tile_data.name), *(tile_data.name), *(tile_data.name)));
        tasks.Add(import_task);
    }

    FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
    AssetToolsModule.Get().ImportAssetTasks(tasks);

    // Add materials
    for (FString& key : this->getTerrainTileKeys()) {
        FCustomMapTileData tile_data = this->getTerrainData(key);

        UMaterialInterface* material = Cast<UMaterialInterface>(
            StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *(tile_data.material))
        );
        if (!material) {
            UE_LOG(LogCustomMapGenerator, Display, TEXT("Material lookup failed %s"), *(tile_data.material));
            continue;
        }

        FAssetRegistryModule& asset_registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        FAssetData asset_data = asset_registry.Get().GetAssetByObjectPath(*(basename_to_unreal_path[tile_data.name]));
        UStaticMesh* imported_mesh = Cast<UStaticMesh>(asset_data.GetAsset());
        if (!imported_mesh) {
            UE_LOG(LogCustomMapGenerator, Display, TEXT("Mesh lookup failed %s %s"), *(basename_to_unreal_path[tile_data.name]), *(tile_data.name));
            continue;
        }
        imported_mesh->SetMaterial(0, material);
        imported_mesh->MarkPackageDirty();
        imported_mesh->PostEditChange();
    }

    return basename_to_unreal_path;
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

TArray<float> UMapDataset::getOrigin() {
    TArray<float> origin;
    origin.Add(this->map_metadata->carla_bounds[0]);
    origin.Add(this->map_metadata->carla_bounds[1]);
    origin.Add(this->map_metadata->carla_bounds[2]);
    return origin;
}