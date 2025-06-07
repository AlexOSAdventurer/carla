// Copyright (c) 2025 Alex Richardson, Jonathan Sprinkle and Vanderbilt University
#include "CustomMapGenerator.h"
#include "GenerateBlankTile.h"

#include <string>
#include <iostream>
#include "Carla/MapGen/LargeMapManager.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "EditorLevelLibrary.h"
#include "Math/Vector.h"
#include "UnrealString.h"

#include "Helpers.hpp"

#if WITH_EDITOR
#include "FileHelpers.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"

UGenerateBlankTile::UGenerateBlankTile()
{
    OriginTile = FString("148110");
    JsonPath = "/home/richarwa/CarlaIngestion/metadata.json";
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
    UE_LOG(LogCustomMapGenerator, Display, TEXT("CreateMap is called, map name is %s!!!"), *(MapName));
    this->TerrainFactory = NewObject<UCustomTerrain>();
    this->TerrainFactory->Init(MapName, DefaultLandscapeMaterial, this->OriginSurveyFeet, JsonPath);
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
    TerrainFactory->CreateTile(this->OriginTile, FVector(0.0, 0.0, 0.0));
    UE_LOG(LogCustomMapGenerator, Warning, TEXT("Mesh terrain created!"));
}

void UGenerateBlankTile::LoadJSONMetadata() {
  this->JsonData = UCustomMapGeneratorHelpers::LoadJSONMetadata(this->JsonPath);
  std::string origin_tile_name = TCHAR_TO_UTF8(*this->OriginTile);
  if (this->JsonData.find(origin_tile_name) != this->JsonData.end()) {
    nlohmann::json origin_tile = this->JsonData[origin_tile_name];
    // X and Y are flipped from DEM to Unreal
    this->OriginSurveyFeet = FVector(origin_tile["DEM"]["y"]["min"], origin_tile["DEM"]["x"]["min"], origin_tile["DEM"]["z"]["min"]);
    UE_LOG(LogCustomMapGenerator, Display, TEXT("Origin tile found, Origin X: %f, Origin Y: %f, Origin Z: %f"), this->OriginSurveyFeet.X, this->OriginSurveyFeet.Y, this->OriginSurveyFeet.Z);
  }
  else {
    this->OriginSurveyFeet = FVector(631381.0, 1764506.0, 479.64);
    UE_LOG(LogCustomMapGenerator, Warning, TEXT("Origin tile not found!"));
  }
}



#endif