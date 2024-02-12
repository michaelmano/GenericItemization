// Copyright Fissure Entertainment, Pty Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InstancedStruct.h"
#include "GenericItemizationInstanceTypes.h"
#include "ItemInventoryComponent.generated.h"

class AItemDrop;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FItemInventoryComponentItemTakenSignature, UItemInventoryComponent*, ItemInventoryComponent, const FInstancedStruct&, Item, const FInstancedStruct&, UserContextData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FItemInventoryComponentItemRemovedSignature, UItemInventoryComponent*, ItemInventoryComponent, const FInstancedStruct&, Item, const FInstancedStruct&, UserContextData);

/**
 * A Component that sits on an Actor that owns and manages actual instances of Items.
 */
UCLASS(ClassGroup = ("Generic Itemization"), Blueprintable, meta = (BlueprintSpawnableComponent))
class GENERICITEMIZATION_API UItemInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	friend struct FFastItemInstancesContainer;
	
	UItemInventoryComponent();

	virtual void PreNetReceive() override;
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

	/* Called when the Inventory received a new ItemInstance to manage. */
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Item Taken"))
	FItemInventoryComponentItemTakenSignature OnItemTakenDelegate;

	/* Called when the Inventory dropped an ItemInstance that it was managing. */
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Item Removed"))
	FItemInventoryComponentItemRemovedSignature OnItemRemovedDelegate;

	/**
	 * Checks if the given Item can be taken by the Inventory Component.
	 * 
	 * @param Item				The Item that is to be checked if it can be taken.
	 * @param UserContextData	Additional Data that might be needed in order to make an appropriate determination.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Generic Itemization")
	bool CanTakeItem(const FInstancedStruct& Item, FInstancedStruct UserContextData);

	/**
	 * Takes an ItemInstance and thereafter manages it with this Inventory Component.
	 * This function will call Reset on the Item, it first makes a copy of it to be managed by the
	 *
	 * @param Item				The ItemInstance that this Inventory will take.
	 * @param UserContextData	Additional Data that might provide needed context around the taking of the ItemInstance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Generic Itemization")
	virtual bool TakeItem(UPARAM(Ref) FInstancedStruct& Item, FInstancedStruct UserContextData);

	/**
	 * Takes an ItemDrop Actor's ItemInstance and thereafter manages it with this Inventory Component.
	 *
	 * @param ItemDrop			The ItemDrop Actor that is representing the ItemInstance that this Inventory will take.
	 * @param UserContextData	Additional Data that might provide needed context around the taking of the ItemInstance.
	 * @param bDestroyItemDrop	True if the ItemDrop Actor will have its Destroy() function called on it after a successful pickup.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Generic Itemization")
	virtual bool TakeItemDrop(AItemDrop* ItemDrop, FInstancedStruct UserContextData, bool bDestroyItemDrop = true);

	/**
	 * Drops the ItemInstance with the ItemToDrop Id and passes out the ItemDrop that was created to represent it in the world.
	 *
	 * @param ItemToDrop		The Id of the ItemInstance that is going to be dropped.
	 * @param OutItemDrop		The ItemDrop that was created to represent the ItemInstance in the world.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, BlueprintCallable, Category = "Generic Itemization")
	bool DropItem(FGuid ItemToDrop, AItemDrop*& OutItemDrop);

	/**
	 * Releases the ItemInstance with the ItemToRelease Id and passes it out such that it is no longer managed by this Inventory Component.
	 *
	 * @param ItemToRelease		The Id of the ItemInstance that is going to be released.
	 * @param OutItem			The ItemInstance that will be released. Its lifetime is no longer managed by this Inventory Component.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Generic Itemization")
	bool ReleaseItem(FGuid ItemToRelease, FInstancedStruct& OutItem);

	/* Returns a copy of all of the Items this Inventory currently contains. */
	UFUNCTION(BlueprintCallable, Category = "Generic Itemization")
	TArray<FInstancedStruct> GetItems();

	/* Returns a copy of all of the Items this Inventory currently contains including their contexts. */
	UFUNCTION(BlueprintCallable, Category = "Generic Itemization")
	TArray<FFastItemInstance> GetItemsWithContext();

	/* Gets a copy of the ItemInstance with the given ItemId. */
	UFUNCTION(BlueprintCallable, Category = "Generic Itemization")
	FInstancedStruct GetItem(FGuid ItemId, bool& bSuccessful);

	/* Gets a copy of the ItemInstances UserContextData with the given ItemId. */
	UFUNCTION(BlueprintCallable, Category = "Generic Itemization")
	FInstancedStruct GetItemContextData(FGuid ItemId, bool& bSuccessful);

	/* Returns the number of Items in the Inventory. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Generic Itemization")
	int32 GetNumItems() const;

	/* Returns true if this Component's Owner Actor has authority. */
	bool HasAuthority() const;

protected:

	/* The type of Item Drop Actor we will use to represent the Items we will drop within the world. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Generic Itemization")
	TSubclassOf<AItemDrop> ItemDropClass;

	/* Container for all of the ItemInstances that this Inventory is managing. */
	UPROPERTY(Replicated)
	FFastItemInstancesContainer ItemInstances;

	/* Cached value of whether our owner is a simulated Actor. */
	UPROPERTY()
	bool bCachedIsNetSimulated;
	
	/* Caches the flags that indicate whether this component has network authority. */
	void CacheIsNetSimulated();

	/* Called when the Inventory received a new ItemInstance to manage. */
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "On Added Item"))
	void K2_OnAddedItem(const FInstancedStruct& Item, const FInstancedStruct& UserContextData);

	/* Called when the Inventory dropped an ItemInstance that it was managing. */
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "On Removed Item"))
	void K2_OnRemovedItem(const FInstancedStruct& Item, const FInstancedStruct& UserContextData);

private:

	/* Called natively by the FFastItemInstancesContainer to notify the Inventory of an Item being Added. */
	void OnAddedItemInstance(const FFastItemInstance& FastItemInstance);

	/* Called natively by the FFastItemInstancesContainer to notify the Inventory of an Item being Removed. */
	void OnRemovedItemInstance(const FFastItemInstance& FastItemInstance);

};
