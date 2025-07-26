#include "MapDataset.h"
#include "CustomMapGenerator.h"

UMapDataset::UMapDataset() {
    this->dataset_path = "/home/richarwa/CarlaIngestion/CarlaCooked/";
    this->map_metadata = NewObject<UMapMetadata>(this, UMapMetadata::StaticClass());
    this->map_metadata->loadMetadata(dataset_path + "metadata.json");
}

UMapDataset::~UMapDataset() {

}
