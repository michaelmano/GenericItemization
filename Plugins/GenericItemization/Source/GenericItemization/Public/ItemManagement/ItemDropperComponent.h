// Copyright Fissure Entertainment, Pty Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "InstancedStruct.h"
#include "ItemDropperComponent.generated.h"

class AItemDrop;
class UItemInstancer;
class UItemInstancingContextFunction;

/**
 * A Component that sits on an Actor to facilitate the entrypoint to dropping Items for that Actor from a specified DropTable.
 */
UCLASS(ClassGroup = ("Generic Itemization"), Blueprintable, meta = (BlueprintSpawnableComponent))
class GENERICITEMIZATION_API UItemDropperComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UItemDropperComponent();

	/**
	 * Drops Items from this dropper according to the selected ItemDropTable. 
	 * 
	 * @param UserContextData		Arbitrary data that you may want to pack with useful information to pass through during the Item Instancing Process and for access to other external systems.
	 * @param ItemDrops				All of the ItemDrop Actors that were produced for ItemInstances that were generated from the DropTable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintAuthorityOnly, Category = "Generic Itemization")
	bool DropItems(FInstancedStruct UserContextData, TArray<AItemDrop*>& ItemDrops);

protected:
	
	/* The type of Item Drop Actor we will use to represent the Items we will drop within the world. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<AItemDrop> ItemDropClass;
	
	/* The Instancer that generates the Items for this Item Dropper. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Settings")
	UItemInstancer* ItemInstancer;

	/* The DropTable we will use to make Item selections from. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (RowType = "/Script/GenericItemization.ItemDropTableCollectionEntry", DeprecatedProperty, DeprecationMessage = "V1.4 - ItemDropTable has been replaced by the ItemInstancer, please update all usages."), Category = "Generic Itemization")
	FDataTableRowHandle ItemDropTable;

	/* Class that manages generating the ItemInstancingContext for Items that are generated by this dropper. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (DeprecatedProperty, DeprecationMessage = "V1.4 - ContextProviderFunction has been replaced by the ItemInstancer, please update all usages."), Category = "Generic Itemization")
	TSubclassOf<UItemInstancingContextFunction> ContextProviderFunction;

};
