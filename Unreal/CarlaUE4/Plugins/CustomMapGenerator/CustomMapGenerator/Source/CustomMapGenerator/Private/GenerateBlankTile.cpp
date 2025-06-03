// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#include "GenerateBlankTile.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Misc/FileHelper.h"
#include "EditorLevelLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/LevelBounds.h"
#include "Engine/SceneCapture2D.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "StaticMeshAttributes.h"

#include "Traffic/TrafficLightManager.h"
#include "Util/ProceduralCustomMesh.h"
#include "Carla/Game/CarlaStatics.h"

#include <compiler/disable-ue4-macros.h>
#include <carla/opendrive/OpenDriveParser.h>
#include <carla/road/Map.h>
#include <carla/geom/Simplification.h>
#include <carla/road/Deformation.h>
#include <carla/rpc/String.h>
#include <OSM2ODR.h>
#include <compiler/enable-ue4-macros.h>

#include "Engine/Classes/Interfaces/Interface_CollisionDataProvider.h"
#include "Engine/TriggerBox.h"
#include "Engine/AssetManager.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "PhysicsCore/Public/BodySetupEnums.h"
#include "PhysicsEngine/BodySetup.h"
#include "RawMesh.h"
#include "AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "MeshDescription.h"
#include "EditorLevelLibrary.h"
#include "ProceduralMeshConversion.h"
#include "EditorLevelLibrary.h"

#include "ContentBrowserModule.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Math/Vector.h"
#include "GameFramework/Actor.h"

#include "DrawDebugHelpers.h"
#include "Carla/BlueprintLibary/MapGenFunctionLibrary.h"

#include <string>
#include <iostream>
#include "DEM/DEM.hpp"
#include "json/json.hpp"

#if WITH_EDITOR
#include "FileHelpers.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"

DEFINE_LOG_CATEGORY(LogCustomMapGenerator);

UGenerateBlankTile::UGenerateBlankTile()
{
    //MapName = FString("GeneratedResult");
    //BaseLevelName = FString("GeneratedResult");
    OriginTile = FString("148110");
    JsonPath = "/home/richarwa/CarlaIngestion/metadata.json";
    TileHeight = 60960.0;
    TileWidth = 91440.0;
    //GridSectionSize = 30.48;
    GridSectionSize = 60.96;
    SubTilesInHeight = 20;
    SubTilesInWidth = 30;
    DEMCellSize = 2.0;
    DEMConversionFactorToUnreal = 30.48;
    UE_LOG(LogCustomMapGenerator, Display, TEXT("UGenerateBlankTile is called - new3!!!"));
    LoadJSONMetadata();
    UE_LOG(LogCustomMapGenerator, Display, TEXT("UGenerateBlankTile loaded JSON metadata!"));
}

UGenerateBlankTile::~UGenerateBlankTile()
{
    UE_LOG(LogCustomMapGenerator, Display, TEXT("~UGenerateBlankTile is called!!!"));
}

void UGenerateBlankTile::CreateMap()
{
    UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateMap is called!!!"));
    AActor* QueryActor = UGameplayStatics::GetActorOfClass(
        UEditorLevelLibrary::GetEditorWorld(),
        ALargeMapManager::StaticClass() );

    if( QueryActor != nullptr )
    {
        ALargeMapManager* LargeMapManager = Cast<ALargeMapManager>(QueryActor);
        FIntVector NumTilesInXY  = LargeMapManager->GetNumTilesInXY();
        float TileSize = LargeMapManager->GetTileSize();
        FVector Tile0Offset = LargeMapManager->GetTile0Offset();
        FIntVector CurrentTilesInXY = FIntVector(0,0,0);
        ULevel* PersistantLevel = UEditorLevelLibrary::GetEditorWorld()->PersistentLevel;
        BaseLevelName = LargeMapManager->LargeMapTilePath + "/" + LargeMapManager->LargeMapName;
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("Current Tile is %s"), *( CurrentTilesInXY.ToString() ) );
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("NumTilesInXY is %s"), *( NumTilesInXY.ToString() ) );
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("TileSize is %f"), ( TileSize ) );
        UE_LOG(LogCustomMapGenerator, Warning, TEXT("Tile0Offset is %s"), *( Tile0Offset.ToString() ) );
    }
    else {
        UE_LOG(LogCustomMapGenerator, Error, TEXT("Getting large map manager handle failed!"));
    }
    //CreateTerrainMesh(0, FVector2D(0.0, 0.0), 1000.0, 1.0);
    CreateTile(this->OriginTile, FVector(0.0, 0.0, 0.0));
    UE_LOG(LogCustomMapGenerator, Warning, TEXT("Mesh terrain created!"));
}

void UGenerateBlankTile::LoadJSONMetadata() {
  std::string json_path = std::string(TCHAR_TO_UTF8(*this->JsonPath));
  std::ifstream json_file(json_path);
  this->json_data = nlohmann::json::parse(json_file);
  std::string origin_tile_name = TCHAR_TO_UTF8(*this->OriginTile);
  if (this->json_data.find(origin_tile_name) != this->json_data.end()) {
    nlohmann::json origin_tile = this->json_data[origin_tile_name];
    // X and Y are flipped from DEM to Unreal
    this->OriginX = origin_tile["DEM"]["y"]["min"];
    this->OriginY = origin_tile["DEM"]["x"]["min"];
    this->OriginZ = origin_tile["DEM"]["z"]["min"];
    UE_LOG(LogCustomMapGenerator, Display, TEXT("Origin tile found, Origin X: %f, Origin Y: %f, Origin Z: %f"), this->OriginX, this->OriginY, this->OriginZ);
  }
  else {
    this->OriginX = 1764506.0;
    this->OriginY = 631381.0;
    UE_LOG(LogCustomMapGenerator, Warning, TEXT("Origin tile not found!"));
  }
}

void UGenerateBlankTile::CreateTile(const FString TileIndex, const FVector Offset) {
  std::string TileIndexStr = TCHAR_TO_UTF8(*TileIndex);
  nlohmann::json json_value = this->json_data[TileIndexStr];
  std::string bin_path = json_value["DEM"]["bin_path"];
	double dem_x_min = json_value["DEM"]["x"]["min"];
	double dem_x_max = json_value["DEM"]["x"]["max"];
	double dem_y_min = json_value["DEM"]["y"]["min"];
	double dem_y_max = json_value["DEM"]["y"]["max"];

	std::ifstream bin_file(bin_path, std::ios::binary);
  DEM<double>::Type type = DEM<double>::Type(dem_y_max - dem_y_min, dem_x_max - dem_x_min, dem_y_min, dem_x_min, 2.0, -999999);
  DEM<double> dem = DEM<double>(type, bin_file);

  USceneComponent* root_scene = NewObject<USceneComponent>(this, USceneComponent::StaticClass(), *TileIndex);
  FHitResult hit_result;
  root_scene->SetWorldLocation(Offset, false, &hit_result, ETeleportType::ResetPhysics);
  CreateTerrainMeshForTile(TileIndex, dem, root_scene, json_value);
  this->Tiles.Add(root_scene);
}

void UGenerateBlankTile::CreateTerrainMeshForTile(const FString TileIndex, DEM<double> dem, USceneComponent* root_scene, const nlohmann::json & metadata) {
  USceneComponent* root_terrain = NewObject<USceneComponent>(this, USceneComponent::StaticClass(), *(TileIndex + "_Terrain"));
  root_terrain->AttachToComponent(root_scene, FAttachmentTransformRules::KeepRelativeTransform);
  FHitResult hit_result;
  root_terrain->SetRelativeLocation(FVector(0, 0, 0), false, &hit_result, ETeleportType::ResetPhysics);

  float x_start = (static_cast<float>(metadata["DEM"]["y"]["min"]) - (DEMCellSize / 2.0));
  float y_start = (static_cast<float>(metadata["DEM"]["x"]["min"]) - (DEMCellSize / 2.0));
  float x_height = (static_cast<float>(metadata["DEM"]["y"]["max"]) - static_cast<float>(metadata["DEM"]["y"]["min"])) + DEMCellSize;
  float y_height = (static_cast<float>(metadata["DEM"]["x"]["max"]) - static_cast<float>(metadata["DEM"]["x"]["min"])) + DEMCellSize;
  float x_delta = (x_height / static_cast<float>(SubTilesInHeight));
  float y_delta = (y_height / static_cast<float>(SubTilesInWidth));
  for (int i = 0; i < SubTilesInHeight; i++) {
    for (int j = 0; j < SubTilesInWidth; j++) {
      float x_subtile_start = x_start + (x_delta * i);
      float x_subtile_end = x_start + (x_delta * (i + 1));
      float y_subtile_start = y_start + (y_delta * j);
      float y_subtile_end = y_start + (y_delta * (j + 1));
      FVector2D tile_position_cm = FVector2D(x_delta * i * DEMConversionFactorToUnreal, y_delta * j * DEMConversionFactorToUnreal);
      FVector2D tile_start = FVector2D(x_subtile_start, y_subtile_start);
      FVector2D tile_end = FVector2D(x_subtile_end, y_subtile_end);
      UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateTerrainMesh TileIndex %s, MeshIndex %d, Position: (%f, %f), Start: (%f, %f), End: (%f, %f)"), *TileIndex, (i * SubTilesInWidth) + j, tile_position_cm.X, tile_position_cm.Y, tile_start.X, tile_start.Y, tile_end.X, tile_end.Y);
      CreateTerrainMesh(dem, root_terrain, TileIndex, (i * SubTilesInWidth) + j, tile_position_cm, tile_start, tile_end);
    }
  }
  UEditorLoadingAndSavingUtils::SaveDirtyPackages(true, true);
  UEditorLevelLibrary::SaveCurrentLevel();
}

void UGenerateBlankTile::CreateTerrainMesh(DEM<double>& dem, USceneComponent* root_terrain, const FString TileIndex, const int MeshIndex, const FVector2D tile_position_cm, const FVector2D tile_start, const FVector2D tile_end)
{
  UWorld* World = UEditorLevelLibrary::GetEditorWorld();
  // Creation of the procedural mesh
  AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>();
  //MeshActor->AttachToComponent(root_terrain, FAttachmentTransformRules::KeepRelativeTransform);
  MeshActor->SetActorLocation(FVector(tile_position_cm.X, tile_position_cm.Y, 0));
  UStaticMeshComponent* Mesh = MeshActor->GetStaticMeshComponent();

  TArray<FVector> Vertices;
  TArray<int32> Triangles;

  TArray<FVector> Normals;
  TArray<FLinearColor> Colors;
  TArray<FProcMeshTangent> Tangents;
  TArray<FVector2D> UVs;


  int VerticesInX = (((tile_end.X - tile_start.X) * DEMConversionFactorToUnreal) / GridSectionSize) + 1.0f;
  int VerticesInY = (((tile_end.Y - tile_start.Y) * DEMConversionFactorToUnreal) / GridSectionSize) + 1.0f;
  static int StaticMeshIndex = 0;
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Vertices Loop"));
  for (int i = 0; i < VerticesInX; i++) {
    float X = (i * GridSectionSize);
    double dem_X = static_cast<double>(tile_start.X + (X / DEMConversionFactorToUnreal));
    for (int j = 0; j < VerticesInY; j++) {
      float Y = (j * GridSectionSize);
      double dem_Y = static_cast<double>(tile_start.Y + (Y / DEMConversionFactorToUnreal));
      float HeightValue = (static_cast<float>(dem.interpolated_altitude(dem_Y, dem_X)) - this->OriginZ) * DEMConversionFactorToUnreal;
      Vertices.Add(FVector(X, Y, HeightValue));
      UVs.Add(FVector2D(i, j));
    }
  }

  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Triangles Loop"));
  //// Triangles formation. 2 triangles per section.
  for(int i = 0; i < VerticesInX - 1; i++)
  {
    for(int j = 0; j < VerticesInY - 1; j++)
    {
      Triangles.Add(j + (i * VerticesInY));
      Triangles.Add((j + 1) + (i * VerticesInY));
      Triangles.Add(j + ((i + 1) * VerticesInY));

      Triangles.Add((j + 1) + (i * VerticesInY));
      Triangles.Add((j + 1) + ((i + 1) * VerticesInY));
      Triangles.Add(j + ((i + 1) * VerticesInY));
    }
  }

  UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
    Vertices,
    Triangles,
    UVs,
    Normals,
    Tangents
  );

  FProceduralCustomMesh MeshData;
  MeshData.Vertices = Vertices;
  MeshData.Triangles = Triangles;
  MeshData.Normals = Normals;
  MeshData.UV0 = UVs;
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Creating Mesh"));
  UStaticMesh* MeshToSet = UMapGenFunctionLibrary::CreateMesh(MeshData,  Tangents, DefaultLandscapeMaterial, MapName, "Terrain", FName(TEXT("SM_LandscapeMesh" + TileIndex + FString::FromInt(StaticMeshIndex) + FString::FromInt(MeshIndex) )));
  Mesh->SetStaticMesh(MeshToSet);
  MeshActor->SetActorLabel("SM_LandscapeActor" + TileIndex + FString::FromInt(StaticMeshIndex) + FString::FromInt(MeshIndex) );
  MeshActor->Tags.Add(FName("LandscapeToMove"));
  Mesh->CastShadow = false;
  UE_LOG(LogCustomMapGenerator, Warning, TEXT("Adding to Landscapes list"));
  StaticMeshIndex++;
}

#endif